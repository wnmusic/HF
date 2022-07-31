#pragma once
#include "source_ifce.h"
#include <stdio.h>
#include <exception>

template <class T> 
class rawfile_source : public source_ifce
{
public:    
    rawfile_source(const char* fname, double sample_rate){
        m_file = fopen(fname, "rb");
        if (!m_file){
            throw std::runtime_error("Could not open file");
        }
        m_rate = sample_rate;
    }
    ~rawfile_source(){
        if(m_file){
            fclose(m_file);
        }
    }

    virtual double get_source_rate(void) {return m_rate;}
    virtual unsigned read(void* buf, int num_samples){
        return fread(buf, sizeof(T), num_samples, m_file);
    }
private:
    FILE *m_file;
    double m_rate;
};
