#ifndef SALLENKEY_H
#define SALLENKEY_H

#include <QtCore>
#include <complex>
#include <math.h>
#include "filter.h"

class SallenKey : public Filter
{

private:

    std::vector<float> C1;
    std::vector<float> C2;
    std::vector<float> R1;
    std::vector<float> R2;
    std::vector<float> R3;
    std::vector<float> R4;

    void calcSallenKeyHPF();
    void calcSallenKeyLPF();

public:
    SallenKey(QVector< std::complex<float> > poles_, bool HighPass_ = false);

    QString* createSchematic();
    void calcFilter();
};

#endif // SALLENKEY_H
