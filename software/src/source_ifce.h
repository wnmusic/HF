#pragma once

class source_ifce
{
public:
    virtual unsigned read(void* buf, int num_samples) = 0;
    virtual double get_sample_rate(void) = 0;
    virtual unsigned get_block_size(void) = 0;
};
