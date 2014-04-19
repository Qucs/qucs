#ifndef FILTER_H
#define FILTER_H

#include <QtCore>
#include <math.h>
#include <complex>

class Filter
{

private:
    QVector< std::complex<float> > Poles;

    bool HighPass;

public:


    Filter(QVector< std::complex<float> > poles_, bool HighPass_ = false);

    virtual QString* createSchematic()
    {

    }

    virtual void calcFilter()
    {

    }

};

#endif // FILTER_H
