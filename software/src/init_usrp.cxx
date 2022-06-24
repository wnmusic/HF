#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>

int UHD_SAFE_MAIN(int argc, char *argv[]) {
    uhd::set_thread_priority_safe();

    std::string device_args("addr=192.168.1.21");
    std::string subdev("A:A");
    std::string ref("internal");

    double rate(1e6);
    double freq(21.2e6);
    double gain(10);
    double bw(1e6);

    bool verbose = false;

    //create a usrp device
    std::cout << std::endl;
    std::cout << boost::format("Creating the usrp device with: %s...") % device_args << std::endl;
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(device_args);

    // Lock mboard clocks
    std::cout << boost::format("Lock mboard clocks: %f") % ref << std::endl;
    usrp->set_clock_source(ref);
    
    //always select the subdevice first, the channel mapping affects the other settings
    std::cout << boost::format("subdev set to: %f") % subdev << std::endl;
    usrp->set_rx_subdev_spec(subdev);
    std::cout << boost::format("Using Device: %s") % usrp->get_pp_string() << std::endl;

    //set the sample rate
    if (rate <= 0.0) {
        std::cerr << "Please specify a valid sample rate" << std::endl;
        return ~0;
    }

    // set sample rate
    std::cout << boost::format("Setting RX Rate: %f Msps...") % (rate / 1e6) << std::endl;
    usrp->set_rx_rate(rate);
    std::cout << boost::format("Actual RX Rate: %f Msps...") % (usrp->get_rx_rate() / 1e6) << std::endl << std::endl;

    // set freq
    std::cout << boost::format("Setting RX Freq: %f MHz...") % (freq / 1e6) << std::endl;
    uhd::tune_request_t tune_request(freq);
    usrp->set_rx_freq(tune_request);
    std::cout << boost::format("Actual RX Freq: %f MHz...") % (usrp->get_rx_freq() / 1e6) << std::endl << std::endl;

    // set the rf gain
    std::cout << boost::format("Setting RX Gain: %f dB...") % gain << std::endl;
    usrp->set_rx_gain(gain);
    std::cout << boost::format("Actual RX Gain: %f dB...") % usrp->get_rx_gain() << std::endl << std::endl;

    // set the IF filter bandwidth
    std::cout << boost::format("Setting RX Bandwidth: %f MHz...") % (bw / 1e6) << std::endl;
    usrp->set_rx_bandwidth(bw);
    std::cout << boost::format("Actual RX Bandwidth: %f MHz...") % (usrp->get_rx_bandwidth() / 1e6) << std::endl << std::endl;

    uhd::stream_args_t stream_args("fc32"); // complex floats
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

    // meta-data will be filled in by recv()
    uhd::rx_metadata_t md;

    // allocate buffers to receive with samples (one buffer per channel)
    const size_t samps_per_buff = rx_stream->get_max_num_samps();
    std::vector<std::complex<float>> rx_buf(samps_per_buff);

    std::cout <<"num samples per transfer: " << samps_per_buff << std::endl;

    std::vector<std::complex<float>*> buff_ptrs;
    buff_ptrs.push_back(&rx_buf.front());

    double timeout = 0.1;

    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    stream_cmd.stream_now = true;
    rx_stream->issue_stream_cmd(stream_cmd); // tells all channels to stream

    size_t total_samples;
    
    while(total_samples < 10000000)
    {

        size_t num_rx_samps = rx_stream->recv(buff_ptrs, samps_per_buff, md, timeout);

      // handle the error code
        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT)
            break;
        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            throw std::runtime_error(
                str(boost::format("Receiver error %s") % md.strerror()));
        }

        if (verbose)
            std::cout << boost::format(
                             "Received packet: %u samples, %u full secs, %f frac secs")
                             % num_rx_samps % md.time_spec.get_full_secs()
                             % md.time_spec.get_frac_secs()
                      << std::endl;

        total_samples += num_rx_samps;
    }

    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd); 

                                                    
    std::cout << std::endl << "Done! with " << total_samples << " samples streamed! " << std::endl << std::endl;                                                    
    return EXIT_SUCCESS;
}


