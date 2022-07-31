#pragma once

class source_ifce
{
public:
    virtual unsigned read(void* buf, int num_samples) = 0;
    virtual double get_source_rate(void) = 0;
};
