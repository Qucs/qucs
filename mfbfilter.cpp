#include "mfbfilter.h"

MFBfilter::MFBfilter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_)
    : Filter(poles_,type_,Fcutoff,Kv_)
{
    Nr1 = 3;
    Nc1 = 2;
    Nop1 = 1;
}

void MFBfilter::createSchematic(QString &s)
{

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
