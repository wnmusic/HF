#pragma once

class sink_ifce
{
public:
    virtual unsigned write(void* buf, int num_samples) = 0;
    virtual double get_sink_rate(void) = 0;
};
