#ifndef SALLENKEY_H
#define SALLENKEY_H

#include <QtCore>
#include <complex>
#include <math.h>
#include "filter.h"


class SallenKey : public Filter
{

private:

    void calcSallenKeyHPF();
    void calcSallenKeyLPF();

public:
    SallenKey(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_=1.0);

    QString* createSchematic();
    void calcFilter();
};

#endif // SALLENKEY_H
