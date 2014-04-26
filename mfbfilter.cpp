#include "mfbfilter.h"

MFBfilter::MFBfilter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_)
    : Filter(poles_,type_,Fcutoff,Kv_)
{
}

void MFBfilter::createSchematic(QString &s)
{

}

void MFBfilter::calcFilter()
{
    Stages.clear();

    switch (ftype) {
    case Filter::LowPass : calcLowPass();
        break;
    case Filter::HighPass : calcHighPass();
        break;
    default:
        break;
    }

    Nr = 3*(Nfil/2);
    Nc = 2*(Nfil/2);
    Nopamp = Nfil/2;
}

void MFBfilter::calcHighPass()
{

}

void MFBfilter::calcLowPass()
{
    float R1,R2,R3,C1,C2;
    float Wc = 2*M_PI*Fc;

    for (int k=1; k <= Nfil/2; k++) {
        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;

        C2 = (10.0/Fc)*1e-6;
        C1 = (B*B*C2)/(4*C*(Kv+1));
        R2 = (2*(Kv+1))/(Wc*(B*C2+sqrt(B*B*C2*C2-4*C*C1*C2*(Kv+1))));
        R1 = R2/Kv;
        R3 = 1.0/(C*C1*C2*Wc*Wc*R2);

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = R1;
        curr_stage.R2  = R2;
        curr_stage.R3 = R3;
        curr_stage.R4 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = C2;
        Stages.append(curr_stage);
    }
}
