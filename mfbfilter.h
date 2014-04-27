#ifndef MFBFILTER_H
#define MFBFILTER_H

#include <QtCore>
#include <complex>
#include "filter.h"

class MFBfilter : public Filter
{
protected:
    void calcHighPass();
    void calcLowPass();
    void createHighPassSchematic(QString &s);
    void createLowPassSchematic(QString &s);

public:
    MFBfilter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_=1.0);

};

#endif // MFBFILTER_H
