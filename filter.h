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

struct FilterParam {
    float Ap;
    float As;
    float Fc;
    float Fs;
    float Rp;
    float Kv;
};

class Filter
{

public:
    enum FType {HighPass, LowPass, BandPass};
    enum FilterFunc {Butterworth, Chebyshev, Cauer, Bessel, InvChebyshev};

protected:
    QVector< std::complex<float> > Poles;
    QVector< std::complex<float> > Zeros;
    QVector<RC_elements> Stages;

    Filter::FType ftype;
    Filter::FilterFunc ffunc;
    int Nfil;
    float Fc,Kv,Fs,Ap,As,Rp;
    int Nr,Nc,Nopamp; // total number of R,C, opamp

    int Nr1,Nc1,Nop1; // number of R,C, opamp per stage

    void calcButterworth();
    void calcChebyshev();
    void calcCauer();

    void createFirstOrderComponentsHPF(QString &s,RC_elements stage, int dx);
    void createFirstOrderComponentsLPF(QString &s,RC_elements stage, int dx);
    void createFirstOrderWires(QString &s, int dx, int y);
    float autoscaleCapacitor(float C, QString &suffix);
    virtual void calcHighPass();
    virtual void calcLowPass();
    virtual void createHighPassSchematic(QString &s);
    virtual void createLowPassSchematic(QString &s);

public:


    Filter(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par);
    virtual ~Filter();

    void calcFirstOrder();

    void createPartList(QStringList &lst);
    void createPolesZerosList(QStringList &lst);

    virtual void createSchematic(QString &s);


    virtual void calcFilter();

};

#endif // FILTER_H
