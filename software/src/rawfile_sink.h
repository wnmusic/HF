#pragma once
#include "sink_ifce.h"
#include <stdio.h>

template<class T> 
class rawfile_sink : public sink_ifce
{
public:
    rawfile_sink(const char* fname, double rate = 1.0)
        :m_rate(rate)
    {
        m_file = fopen(fname, "wb");
    }
    ~rawfile_sink(){
        if(m_file) fclose(m_file);
    }
    virtual unsigned write(void* buf, int num_samples){
        return fwrite(buf, sizeof(T), num_samples, m_file);
    }
    virtual double get_sink_rate(void) { return m_rate;}
private:
    FILE *m_file;
    double m_rate;
};
