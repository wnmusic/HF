#pragma once
#include "source_ifce.h"
#include <stdio.h>

class complex_rawfile_source : public source_ifce
{
public:    
    complex_rawfile_source(const char* fname){
        m_file = fopen(fname, "rb");
        m_pos = 0;
    }
    ~complex_rawfile_source(){
        if(m_file){
            fclose(m_file);
        }
    }

    virtual double get_sample_rate(void) {return 1.0;}
    virtual double get_block_size(void){return m_blksize;}
    virtual unsigned read(void* buf, int num_samples){
        return fread(buf, sizeof(std::complex<float>), m_blksize, m_file);
    }
private:
    unsigned m_pos;
    static const unsigned m_blksize = 1024;
    FILE *m_file;
    
}
