#ifndef UHD_FE_H_
#define UHD_FE_H_
#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <condition_variable>
#include <mutex>
#include "ringbuf.h"
#include "debug_buf.h"
#include "source_ifce.h"
#include "sink_ifce.h"

enum{
    GPIO_TX_SEL_7M = 1,
    GPIO_TX_SEL_14M = 2,
    GPIO_TX_SEL_21M = 3,    
    GPIO_TX_SEL_28M = 4,
};

enum{
    FADE_STATE_NONE = 0,
    FADE_STATE_IN,
    FADE_STATE_OUT
};

class uhd_fe: public source_ifce, public sink_ifce
{
public:
    
    void set_rf_freq(double freq)
    {
        tune_request.target_freq = freq;
        usrp->set_rx_freq(tune_request);
        usrp->set_tx_freq(tune_request);
        m_rx_freq = usrp->get_rx_freq();
        std::cout << boost::format("Actual RX Freq: %f MHz...") % (usrp->get_rx_freq() / 1e6) << std::endl << std::endl;
    }
    double get_rf_freq(void){return m_rx_freq;}

    void set_rx_bandwidth(double bw)
    {
        usrp->set_rx_bandwidth(bw);
        m_rx_bw = usrp->get_rx_bandwidth();
        std::cout << boost::format("Actual RX Bandwidth: %f MHz...") % (m_rx_bw / 1e6) << std::endl << std::endl;
    }
    double get_rx_bandwidth(void){return m_rx_bw;}


    void set_tx_bandwidth(double bw){
        usrp->set_tx_bandwidth(bw);        
        m_tx_bw = usrp->get_tx_bandwidth();
        std::cout << boost::format("Actual TX Bandwidth: %f MHz...") % (m_tx_bw / 1e6) << std::endl << std::endl;        
    }

    void set_rx_sample_rate(double rate)
    {
        usrp->set_rx_rate(rate);
        m_rx_rate = usrp->get_rx_rate();
        std::cout << boost::format("Actual RX Rate: %f Msps...") % ( m_rx_rate / 1e6) << std::endl << std::endl;
    }

    void set_tx_sample_rate(double rate)
    {
        usrp->set_tx_rate(rate);
        m_tx_rate = usrp->get_tx_rate();
        std::cout << boost::format("Actual TX Rate: %f Msps...") % ( m_tx_rate / 1e6) << std::endl << std::endl;
    }
    

    void set_rx_gain(double gain)
    {
        usrp->set_rx_gain(gain);
        m_rx_gain = usrp->get_rx_gain();
        std::cout << boost::format("Actual RX Gain: %f dB...") % m_rx_gain << std::endl << std::endl;
    }
    double get_rx_gain(void){return m_rx_gain;}

    void set_tx_gain(double gain)
    {
        usrp->set_tx_gain(gain);
        m_tx_gain = usrp->get_tx_gain();
        std::cout << boost::format("Actual TX Gain: %f dB...") % m_tx_gain << std::endl << std::endl;
    }
    double get_tx_gain(void){return m_tx_gain;}
    
    double get_normlized_rx_gain(void){
        return usrp->get_normalized_rx_gain();
    }

    uhd_fe(std::string &device
          ,std::string &subdev
          ,std::string &ref
          ,unsigned buf_size = 8192*16
          ,double freq = 21.225e6
          ,double rx_rate = 1e6
          ,double tx_rate = 1e6
          ,double rx_gain = 6
          ,double tx_gain = 6
          ,double rx_bw = 1e6
          ,double tx_bw = 1e6
          );
    ~uhd_fe();
    
    void work();
    static  void start(void* ctx);
    
    void stop(){
        m_stop_rx = true;
    }
    virtual unsigned read(void* buf, int num_samples);
    virtual unsigned write(void* buf, int num_samples);
    virtual double get_source_rate(void){
        return m_rx_rate;
    }
    virtual double get_sink_rate(void){
        return m_tx_rate;
    }

    void set_ptt(bool on){
        b_ptt_on_async = on;
    }

    void set_gpio(int gpio){
        gpio = gpio > 4 ? 4 : (gpio < 1 ? 1 : gpio);
        usrp->set_gpio_attr("RXA", "OUT", (0x01<<gpio), gpio_mask);
    }

private:
    double m_rx_freq;
    double m_rx_bw;
    double m_tx_bw;
    double m_rx_rate;
    double m_tx_rate;    
    double m_rx_gain;
    double m_tx_gain;
    unsigned m_samps_per_buff;
    
    uhd::usrp::multi_usrp::sptr usrp;
    uhd::rx_streamer::sptr m_rx_stream;
    uhd::tx_streamer::sptr m_tx_stream;
    uhd::rx_metadata_t md;
    uhd::tx_metadata_t tx_md;    
    uhd::stream_cmd_t stream_cmd;
    uhd::tune_request_t tune_request;
    uhd::stream_args_t stream_args;

    std::complex<float> *m_dev_buf;
    std::vector<std::complex<float>*> buff_ptrs;
    
    ring_buffer<std::complex<float>> *m_ringbuf;

    bool m_stop_rx;
    std::mutex rx_mutex;
    std::condition_variable rx_condvar;

    bool b_ptt_on_async;
    bool b_ptt_on;
    int fading_block_cnt;
    int fade_state;

    static const uint32_t FADE_BLOCKS = 10;

    static const uint32_t atr_mask = 1;
    /* current wiring: RXA
       GPIO1 - 28M
       GPIO2 - 21M
       GPIO3 - 7M
       GPIO4 - 14M
    */
    static const uint32_t gpio_mask = 0x1E;
};

#endif
