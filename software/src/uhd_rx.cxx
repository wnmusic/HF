#include "uhd_rx.h"
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>

uhd_rx::uhd_rx(std::string &device
              ,std::string &subdev
              ,std::string &ref
              ,unsigned pkt_size
              ,double rate
              ,double freq
              ,double gain
              ,double bw
              ):
    stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE),
    tune_request(freq),
    stream_args("fc32"),
    m_pkt_size(pkt_size)
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
    m_samps_per_buff = m_rx_stream->get_max_num_samps();

    unsigned m_buf_size = 2*(m_samps_per_buff > m_pkt_size ? m_samps_per_buff : m_pkt_size);

    m_dev_buf = new std::complex<float> [m_buf_size];
    std::cout <<"num samples per transfer: " << m_samps_per_buff << std::endl;
    buff_ptrs.push_back(m_dev_buf);

    buf_wr_idx = 0;
    m_stop_rx = true;
}

uhd_rx::~uhd_rx()
{
    delete[] m_dev_buf;
}

void uhd_rx::stop()
{
    m_stop_rx = true;
}

void
uhd_rx::work()
{
    static const double rx_timeout = 0.1;
    stream_cmd.stream_now = true;
    m_rx_stream->issue_stream_cmd(stream_cmd); // tells all channels to stream
    m_stop_rx = false;
    m_usr_buf = NULL;
    while(!m_stop_rx)
    {
        size_t num_rx_samps = m_rx_stream->recv(buff_ptrs, m_samps_per_buff, md, rx_timeout);
        // handle the error code
        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT)
            break;
        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            throw std::runtime_error(
                str(boost::format("Receiver error %s") % md.strerror()));
        }

        buf_wr_idx += num_rx_samps;

        if(buf_wr_idx >= m_pkt_size){
            std::lock_guard<std::mutex> guard(rx_mutex);          
            if (m_usr_buf){
                memcpy(m_usr_buf, m_dev_buf, sizeof(std::complex<float>) * m_pkt_size);
                data_rdy = true;
                rx_condvar.notify_one();
            }else{
                std::cerr << "O";
            }
            
            buf_wr_idx -= m_pkt_size;
            if (buf_wr_idx > 0){
                memcpy(m_dev_buf, &m_dev_buf[m_pkt_size], buf_wr_idx * sizeof(std::complex<float>));
            }
        }
        
        buff_ptrs[0] = &m_dev_buf[buf_wr_idx];
    }

    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    m_rx_stream->issue_stream_cmd(stream_cmd); 

    return;
}


unsigned
uhd_rx::recv_pkt(std::complex<float> *buf)
{

    std::unique_lock<std::mutex> lock(rx_mutex);
    data_rdy = false;
    m_usr_buf = buf;
    while (!data_rdy){
        rx_condvar.wait(lock);
    }
    m_usr_buf = NULL;
    return m_pkt_size;
}
