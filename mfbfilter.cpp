#include "mfbfilter.h"

MFBfilter::MFBfilter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_)
    : Filter(poles_,type_,Fcutoff,Kv_)
{
}

QString* MFBfilter::createSchematic()
{

}

void MFBfilter::calcFilter()
{

}
