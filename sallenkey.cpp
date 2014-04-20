#include "sallenkey.h"
#include <iostream>

SallenKey::SallenKey(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_) :
    Filter(poles_, type_, Fcutoff, Kv_)
{

}

QString* SallenKey::createSchematic()
{
    RC_elements stage;
    int dx = 0;
    QString *s = new QString("<Qucs Schematic 0.0.17>\n");
    *s += "<Components>\n";
    *s += QString("<.AC AC1 1 30 410 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"%1 kHz\" 1 \"501\" 1 \"no\" 0>\n").arg((10.0*Fc)/1000.0);
    *s += "<.DC DC1 1 280 410 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    *s += "<Eqn Eqn1 1 270 540 -30 14 0 0 \"K=(out.v/in.v)\" 1 \"yes\" 0>\n";
    foreach (stage,Stages) {
        *s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(stage.N).arg(370+dx);
        *s += QString("<R R%1 1 %2 340 15 -26 0 1 \"34.45k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>").arg(2+stage.N).arg(320+dx);
        *s += QString("<C C2 1 %1 190 -26 -45 1 0 \"2000p\" 1 \"\" 0 \"neutral\" 0>").arg(200+dx);
        *s += QString("<C C1 1 %1 190 -26 17 0 0 \"2000p\" 1 \"\" 0 \"neutral\" 0>").arg(100+dx);
        *s += QString("<R R2 1 %1 240 -75 -26 1 1 \"41.59k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>").arg(270+dx);
        *s += QString("<R R1 1 330 70 -26 15 0 0 \"6.09k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>").arg(330+dx);
        *s += QString("<R R4 1 %1 260 -26 15 1 2 \"1m\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>").arg(410+dx);
        dx += 510;
    }
    *s += "</Components>\n";
    *s += "<Wires>\n";
    dx = 0;
    foreach (stage,Stages) {

        dx += 510;
    }
    *s += "</Wires>\n";

    return s;
}


void SallenKey::calcFilter()
{
    Stages.clear();

    switch (ftype) {
    case Filter::LowPass : calcSallenKeyLPF();
        break;
    case Filter::HighPass : calcSallenKeyHPF();
        break;
    default:
        break;
    }
}


void SallenKey::calcSallenKeyLPF()
{
    float R1,R2,R3,R4,C1,C2;
    float Wc = 2*M_PI*Fc;

    for (int k=1; k <= Nfil/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;


        C2 = 10 / Fc;
        C1 = (B*B+4*C*(Kv-1))*C2/(4*C);
        R1 = 2/(Wc*(B*C2+sqrt((B*B + 4*C*(Kv-1))*(C2*C2)-4*C*C1*C2)));
        R2 = 1/(C*C1*C2*R1*Wc*Wc);

        if (Kv != 1.0) {
            R3 = Kv*(R1 + R2)/(Kv - 1);
            R4 = Kv*(R1 + R2);
        } else {
            R3 = 999;
            R4 = 0;
        }

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 1000*R4;
        curr_stage.C1 = C1;
        curr_stage.C2 = C2;
        Stages.append(curr_stage);
    }

    this->calcFirstOrder();
}


void SallenKey::calcSallenKeyHPF()
{
    float R1,R2,R3,R4,C1;
    float Wc = 2*M_PI*Fc;


    for (int k=1; k <= Nfil/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;

        C1 = 10 / Fc;

        R2 = 4*C/(Wc*C1*(B+sqrt(B*B+8*C*(Kv-1))));

        R1 = C/(Wc*Wc*C1*C1*R2);

        if (Kv != 1.0) {
            R3 = Kv*R2/(Kv - 1);
            R4 = Kv*R2;
        } else {
            R3 = 999;
            R4 = 0;
        }

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 1000*R4;
        curr_stage.C1 = C1;
        curr_stage.C2 = 0;
        Stages.append(curr_stage);
    }
    calcFirstOrder();
}
