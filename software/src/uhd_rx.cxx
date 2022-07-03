#include "uhd_rx.h"
#include <boost/program_options.hpp>
#include <boost/format.hpp>
//#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>

uhd_rx::uhd_rx(std::string &device
              ,std::string &subdev
              ,std::string &ref
              ,unsigned buf_size
              ,double rate
              ,double freq
              ,double gain
              ,double bw
              ):
    stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS),
    tune_request(freq),
    stream_args("fc32")
{

    usrp = uhd::usrp::multi_usrp::make(device);
    
    usrp->set_clock_source(ref);
    
    usrp->set_rx_subdev_spec(subdev);

    //set the sample rate
    if (rate <= 0.0) {
        std::cerr << "Please specify a valid sample rate" << std::endl;
    }
    set_sample_rate(rate);
    set_rf_freq(freq);
    set_rx_gain(gain);
    set_if_bandwidth(bw);

    m_rx_stream = usrp->get_rx_stream(stream_args);
    // allocate buffers to receive with samples (one buffer per channel)
    //m_samps_per_buff = m_rx_stream->get_max_num_samps();
    m_samps_per_buff = (int)floor(rate * 0.02);
    m_dev_buf = new std::complex<float> [m_samps_per_buff];
    std::cout <<"num samples per transfer: " << m_samps_per_buff << std::endl;
    buff_ptrs.push_back(m_dev_buf);

    m_ringbuf = new ring_buffer<std::complex<float>>(buf_size);
    m_stop_rx = true;
}

uhd_rx::~uhd_rx()
{
    delete[] m_dev_buf;
    delete m_ringbuf;
}

void
uhd_rx::start(void* ctx)
{
    uhd_rx *rx = static_cast<uhd_rx*>(ctx);
    rx->work();
}


void
uhd_rx::work()
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


unsigned uhd_rx::read(void *ptr, int num_samples)
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

double uhd_rx::get_sample_rate(void)
{
    return m_rate;
}
