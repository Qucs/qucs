#ifndef SALLENKEY_H
#define SALLENKEY_H

#include <QtCore>
#include <complex>
#include <math.h>
#include "filter.h"

struct RC_elements {
    float R1;
    float R2;
    float R3;
    float R4;
    float C1;
    float C2;
};

class SallenKey : public Filter
{

private:

    QVector<RC_elements> Stages;

    void calcSallenKeyHPF();
    void calcSallenKeyLPF();

public:
    SallenKey(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_=1.0);

    QString* createSchematic();
    void calcFilter();
};

#endif // SALLENKEY_H
