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
    MFBfilter(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par);

};

#endif // MFBFILTER_H
