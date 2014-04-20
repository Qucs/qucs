#ifndef FILTER_H
#define FILTER_H

#include <QtCore>
#include <math.h>
#include <complex>

struct RC_elements {
    int   N;
    float R1;
    float R2;
    float R3;
    float R4;
    float C1;
    float C2;
};

class Filter
{

public:
    enum FType {HighPass, LowPass, BandPass};

protected:
    QVector< std::complex<float> > Poles;
    QVector<RC_elements> Stages;

    Filter::FType ftype;
    int Nfil;
    float Fc,Kv;

public:


    Filter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_=1.0);
    virtual ~Filter();

    void calcFirstOrder();

    void createPartList(QStringList &lst);

    virtual QString* createSchematic()
    {

    }

    virtual void calcFilter()
    {

    }

};

#endif // FILTER_H
