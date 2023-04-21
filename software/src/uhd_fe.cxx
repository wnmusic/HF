#include "uhd_fe.h"
#include <boost/thread/thread.hpp>
#include <iostream>

uhd_fe::uhd_fe(std::string &device
              ,std::string &subdev
              ,std::string &ref
              ,unsigned buf_size
              ,double freq
              ,double rx_rate
              ,double tx_rate
              ,double rx_gain
              ,double tx_gain
              ,double rx_bw
              ,double tx_bw
              ):
    stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS),
    tune_request(freq),
    stream_args("fc32")
{

    usrp = uhd::usrp::multi_usrp::make(device);
    
    usrp->set_clock_source(ref);
    
    usrp->set_rx_subdev_spec(subdev);
    usrp->set_tx_subdev_spec(subdev);

    //set the sample rate
    if (rx_rate <= 0.0 || tx_rate <= 0.0) {
        std::cerr << "Please specify a valid sample rate" << std::endl;
    }
    
    set_rx_sample_rate(rx_rate);
    set_tx_sample_rate(tx_rate);
    set_rf_freq(freq);
    set_rx_gain(rx_gain);
    set_tx_gain(tx_gain);    
    set_rx_bandwidth(rx_bw);
    set_tx_bandwidth(tx_bw);

    m_rx_stream = usrp->get_rx_stream(stream_args);
    m_tx_stream = usrp->get_tx_stream(stream_args);

    m_samps_per_buff = (int)floor(m_rx_rate * 0.02);
    m_dev_buf = new std::complex<float> [m_samps_per_buff];
    std::cout <<"num samples per transfer: " << m_samps_per_buff << std::endl;
    buff_ptrs.push_back(m_dev_buf);

    m_ringbuf = new ring_buffer<std::complex<float>>(buf_size);
    m_stop_rx = true;
    b_ptt_on_async = false;
    b_ptt_on = false;
    fade_state = FADE_STATE_NONE;
    fading_block_cnt = 0;

    //setting up GPIO 0 for TX
    usrp->set_gpio_attr("RXA", "CTRL", 1, atr_mask | gpio_mask);
    usrp->set_gpio_attr("RXA", "DDR",  1|(gpio_mask), atr_mask | gpio_mask);
    usrp->set_gpio_attr("RXA", "ATR_0X", 0, atr_mask);
    usrp->set_gpio_attr("RXA", "ATR_RX", 0, atr_mask);
    usrp->set_gpio_attr("RXA", "ATR_TX", 1, atr_mask);
    usrp->set_gpio_attr("RXA", "ATR_XX", 1, atr_mask);

    set_gpio(GPIO_TX_SEL_21M);
    
}

uhd_fe::~uhd_fe()
{
    delete[] m_dev_buf;
    delete m_ringbuf;
}

void
uhd_fe::start(void* ctx)
{
    uhd_fe *rx = static_cast<uhd_fe*>(ctx);
    rx->work();
}


void
uhd_fe::work()
{
    static const double rx_timeout = 0.1;
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;
    stream_cmd.stream_now = false;
    stream_cmd.time_spec = usrp->get_time_now() + uhd::time_spec_t(0.5);

    
    m_rx_stream->issue_stream_cmd(stream_cmd); // tells all channels to stream
    m_stop_rx = false;
    while(!m_stop_rx)
    {
        boost::this_thread::disable_interruption disable_interrupt;        
        size_t num_rx_samps = m_rx_stream->recv(buff_ptrs, m_samps_per_buff, md, rx_timeout, true);
        boost::this_thread::restore_interruption restore_interrupt(disable_interrupt);
        // handle the error code
        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            std::cerr<< md.strerror() << std::endl;
            continue;
        }
        if (num_rx_samps > 0)
        {
            std::lock_guard<std::mutex> guard(rx_mutex);          
            if (!m_ringbuf->write(m_dev_buf, num_rx_samps)){
                std::cerr << "O";
            }
            rx_condvar.notify_one();
        }
    }
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    m_rx_stream->issue_stream_cmd(stream_cmd); 
    rx_condvar.notify_one();
    
    return;
}


unsigned uhd_fe::read(void *ptr, int num_samples)
{

    std::complex<float> *buf = (std::complex<float>*)ptr;

    std::unique_lock<std::mutex> lock(rx_mutex);
    while (m_ringbuf->get_count() < num_samples){
        rx_condvar.wait(lock);
        
        if (m_stop_rx) break;
    }

    m_ringbuf->read(buf, num_samples);
    return m_stop_rx ? 0 : num_samples;
}

unsigned uhd_fe::write(void *ptr, int num_samples)
{

    bool b_xmit = false;
    if (b_ptt_on_async && !b_ptt_on){

        tx_md.start_of_burst = true;
        tx_md.has_time_spec  = false;
        tx_md.end_of_burst   = false;
        b_ptt_on = true;
        fade_state = FADE_STATE_IN;
    }
    else if (b_ptt_on && !b_ptt_on_async){
        tx_md.start_of_burst = false;
        tx_md.end_of_burst   = true;
        tx_md.has_time_spec  = false;
        b_ptt_on = false;
        fade_state = FADE_STATE_OUT;

    }
    else if (b_ptt_on){
        tx_md.start_of_burst = false;
        tx_md.has_time_spec  = false;
        tx_md.end_of_burst   = false;
    }

    switch(fade_state){
    case FADE_STATE_IN:
        if (fading_block_cnt++ == FADE_BLOCKS){
            fade_state = FADE_STATE_NONE;
            fading_block_cnt = 0;
        }
        break;
    case FADE_STATE_OUT:
        if (fading_block_cnt++ == FADE_BLOCKS){
            fade_state = FADE_STATE_NONE;
            fading_block_cnt = 0;
        }
        break;
    case FADE_STATE_NONE:
    default:
        fading_block_cnt = 0;
        break;
    }
    
    if (fade_state == FADE_STATE_IN || fade_state == FADE_STATE_OUT)
    {
        memset(ptr, 0, sizeof(std::complex<float>) * num_samples);
        return m_tx_stream->send(ptr, num_samples, tx_md);
    }
    else if (b_ptt_on)
    {
        return m_tx_stream->send(ptr, num_samples, tx_md);
    }
    
    /* not really write to hadware */
    return num_samples;
}


