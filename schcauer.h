#ifndef SCHCAUER_H
#define SCHCAUER_H

#include <QtCore>
#include <complex>
#include <math.h>
#include "filter.h"

class SchCauer : public Filter // Cauer 2-order section
{

protected:

    void calcHighPass();
    void calcLowPass();
    void createHighPassSchematic(QString &s);
    void createLowPassSchematic(QString &s);

public:
    SchCauer(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par);
};

#endif // SCHCAUER_H
