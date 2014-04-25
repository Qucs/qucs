#ifndef MFBFILTER_H
#define MFBFILTER_H

#include <QtCore>
#include <complex>
#include "filter.h"

class MFBfilter : public Filter
{
private:
    void calcMFB_HPF();
    void calcMFB_LPF();

public:
    MFBfilter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_=1.0);

    void createSchematic(QString &s);
    void calcFilter();
};

#endif // MFBFILTER_H
