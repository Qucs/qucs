#ifndef FILTER_H
#define FILTER_H

#include <QtCore>
#include <math.h>
#include <complex>

class Filter
{

public:
    enum FType {HighPass, LowPass, BandPass};

protected:
    QVector< std::complex<float> > Poles;

    Filter::FType ftype;
    int Nfil;
    float Fc,Kv;

public:


    Filter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_=1.0);
    ~Filter();

    virtual QString* createSchematic()
    {

    }

    virtual void calcFilter()
    {

    }

};

#endif // FILTER_H
