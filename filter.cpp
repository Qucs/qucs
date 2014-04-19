#include "filter.h"

Filter::Filter(QVector< std::complex<float> > poles_, bool HighPass_)
{
    Poles = poles_;
    HighPass = HighPass_;
}
