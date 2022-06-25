#ifndef UHD_RX_H_
#define UHD_RX_H_
#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <condition_variable>
#include <mutex>
#include "ringbuf.h"
#include "debug_buf.h"

class uhd_rx
{
public:
    
    void set_rf_freq(double freq)
    {
        tune_request.target_freq = freq;
        usrp->set_rx_freq(tune_request);
        m_rx_freq = usrp->get_rx_freq();
        std::cout << boost::format("Actual RX Freq: %f MHz...") % (usrp->get_rx_freq() / 1e6) << std::endl << std::endl;
    }
    double get_rf_freq(void){return m_rx_freq;}

    void set_if_bandwidth(double bw)
    {
        usrp->set_rx_bandwidth(bw);
        m_rx_bw = usrp->get_rx_bandwidth();
        std::cout << boost::format("Actual RX Bandwidth: %f MHz...") % (usrp->get_rx_bandwidth() / 1e6) << std::endl << std::endl;
    }
    double get_if_bandwidth(void){return m_rx_bw;}

    void set_sample_rate(double rate)
    {
        usrp->set_rx_rate(rate);
        m_rate = usrp->get_rx_rate();
        std::cout << boost::format("Actual RX Rate: %f Msps...") % ( usrp->get_rx_rate() / 1e6) << std::endl << std::endl;
    }
    double get_sample_rate(void){return m_rate;}

    void set_rx_gain(double gain)
    {
        usrp->set_rx_gain(gain);
        m_rx_gain = usrp->get_rx_gain();
        std::cout << boost::format("Actual RX Gain: %f dB...") % usrp->get_rx_gain() << std::endl << std::endl;
    }
    double get_rx_gain(void){return m_rx_gain;}


    uhd_rx(std::string &device
          ,std::string &subdev
          ,std::string &ref
          ,unsigned pkt_size = 8192
          ,double rate = 1e6
          ,double freq = 21.225e6
          ,double gain = 6
          ,double bw = 1e6
          );
    ~uhd_rx();
    void work();
    void stop();
    unsigned recv_pkt(std::complex<float> *buf);

private:
    double m_rx_freq;
    double m_rx_bw;
    double m_rate;
    double m_rx_gain;
    unsigned m_pkt_size;
    unsigned m_samps_per_buff;
    
    uhd::usrp::multi_usrp::sptr usrp;
    uhd::rx_streamer::sptr m_rx_stream;
    uhd::rx_metadata_t md;
    uhd::stream_cmd_t stream_cmd;
    uhd::tune_request_t tune_request;
    uhd::stream_args_t stream_args;

    std::complex<float> *m_dev_buf;
    unsigned buf_wr_idx;
    
    std::vector<std::complex<float>*> buff_ptrs;

    bool m_stop_rx;
    std::complex<float> *m_usr_buf;
    std::mutex rx_mutex;
    std::condition_variable rx_condvar;
    bool data_rdy;
    
};

#endif
