#include "filter.h"

Filter::Filter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_)
{
    Poles = poles_;
    Nfil = Poles.count();
    ftype = type_;
    Fc = Fcutoff;
    Kv = Kv_;
}

Filter::~Filter()
{

}
