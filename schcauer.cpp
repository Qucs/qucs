#include "schcauer.h"

SchCauer::SchCauer(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par) :
    Filter(ffunc_, type_, par)
{
    Nr = 5;
    Nopamp = 3;
    Nc = 2;
}

void SchCauer::calcLowPass()
{
    float R1,R2,R3,R4,R5,C1,C2;
    float Wc = 2*M_PI*Fc;
    float Nst = order/2 + order%2;
    float Kv1 = pow(Kv,1.0/Nst);
    //qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        float A = im*im;

        qDebug()<<A<<B<<C;

        C1 = 10.0/Fc;
        C2 = C1;
        R5 = 1.0/(Wc*C1);
        R1 = (B*R5)/(Kv1*C);
        R2 = R5/B;
        R3 = (B*R5)/C;
        R4 = (Kv1*C*R5)/A;

        RC_elements curr_sec;
        curr_sec.N = k;
        curr_sec.R1 = 1000*R1;
        curr_sec.R2  = 1000*R2;
        curr_sec.R3 = 1000*R3;
        curr_sec.R4 = 1000*R4;
        curr_sec.R5 = 1000*R5;
        curr_sec.C1 = C1;
        curr_sec.C2 = C2;
        Sections.append(curr_sec);

    }

    this->calcFirstOrder();
}


void SchCauer::calcHighPass()
{
    float R1,R2,R3,R4,R5,C1,C2;
    float Wc = 2*M_PI*Fc;
    float Nst = order/2 + order%2;
    float Kv1 = pow(Kv,1.0/Nst);
    //qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        float A = im*im;

        qDebug()<<A<<B<<C;

        C1 = 10.0/Fc;
        C2 = C1;
        R5 = 1.0/(Wc*C1);
        R1 = (B*A*R5)/(Kv1*C);
        R2 = (C*R5)/B;
        R3 = B*R5;
        R4 = Kv1*R5;

        RC_elements curr_sec;
        curr_sec.N = k;
        curr_sec.R1 = 1000*R1;
        curr_sec.R2  = 1000*R2;
        curr_sec.R3 = 1000*R3;
        curr_sec.R4 = 1000*R4;
        curr_sec.R5 = 1000*R5;
        curr_sec.C1 = C1;
        curr_sec.C2 = C2;
        Sections.append(curr_sec);

    }

    this->calcFirstOrder();
}


void SchCauer::createLowPassSchematic(QString &s)
{

}

void SchCauer::createHighPassSchematic(QString &s)
{
    createLowPassSchematic(s);
}
