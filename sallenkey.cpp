#include "sallenkey.h"

SallenKey::SallenKey(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_) :
    Filter(poles_, type_, Fcutoff, Kv_)
{

}

QString* SallenKey::createSchematic()
{

}


void SallenKey::calcFilter()
{
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
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 1000*R4;
        curr_stage.C1 = C1;
        curr_stage.C2 = C2;
        Stages.append(curr_stage);


    }

    if (Nfil%2 != 0) {
        int k = Nfil/2 + 1;
        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float C = re*re + im*im;
        C1 = 10/Fc;
        R1 = 1.0/(Wc*C*C1);

        if (Kv != 1.0) {
            R2 = Kv*R1/(Kv - 1);
            R3 = Kv*R1;
        } else {
            R2 = 999;
            R3 = 0;
        }
        RC_elements curr_stage;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2 = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = 0;
        Stages.append(curr_stage);


    }


}


void SallenKey::calcSallenKeyHPF()
{

}
