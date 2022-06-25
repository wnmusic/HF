#ifndef DEBUG_BUF_H
#define DEBUG_BUF_H

#include <mutex>

template <class T> 
class debug_buf{

public:
    debug_buf(unsigned num_ele): m_size(num_ele){
        m_buf = new T[num_ele];
        rdy_to_write = true;
        curr_nb = 0;
    }
    ~debug_buf(){
        delete []m_buf;
    }
    
    bool write(void* data, unsigned nb, unsigned rate){
        std::lock_guard<std::mutex> guard(mutex);
        if (rdy_to_write){
            nb = nb > m_size-curr_nb ? m_size-curr_nb : nb;
            memcpy(&m_buf[curr_nb], data, sizeof(T)*nb);
            curr_nb += nb;
            sample_rate = rate;
            if (curr_nb >= m_size){
                rdy_to_write = false;
            }
            return true;
        }
        return false;
    }
    
    bool read(void* data, unsigned &nb, unsigned &rate){
        std::lock_guard<std::mutex> guard(mutex);
        if (rdy_to_write == true){
            return false;
        }
        memcpy(data, m_buf, sizeof(T)*m_size);
        curr_nb = 0;
        rdy_to_write = true;
        
        rate = sample_rate;
        nb = m_size;
        return true;
    }

    unsigned get_size(){
        return m_size;
    }

private:
    T* m_buf;
    bool rdy_to_write = true;
    unsigned m_size = 0;
    unsigned curr_nb = 0;
    std::mutex mutex;
    unsigned sample_rate;
};


#endif
