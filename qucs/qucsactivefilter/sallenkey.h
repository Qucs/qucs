#ifndef SALLENKEY_H
#define SALLENKEY_H

#include <QtCore>
#include <complex>
#include <math.h>
#include "filter.h"


class SallenKey : public Filter
{

protected:

    void calcHighPass();
    void calcLowPass();
    void createHighPassSchematic(QString &s);
    void createLowPassSchematic(QString &s);

public:
    SallenKey(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par);

    //void createSchematic(QString &s);

};

#endif // SALLENKEY_H
