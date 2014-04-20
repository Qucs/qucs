#include "filter.h"

Filter::Filter(QVector< std::complex<float> > poles_, Filter::FType type_, float Fcutoff, float Kv_)
{
    Poles = poles_;
    Nfil = Poles.count();
    ftype = type_;
    Fc = Fcutoff;
    Kv = Kv_;
}

Filter::~Filter()
{

}

void Filter::calcFirstOrder()
{
    if (Nfil%2 != 0) {

        float R2,R3;

        int k = Nfil/2 + 1;
        float Wc = 2*M_PI*Fc;
        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float C = re*re + im*im;
        float C1 = 10/Fc;
        float R1 = 1.0/(Wc*C*C1);


        if (Kv != 1.0) {
            R2 = Kv*R1/(Kv - 1);
            R3 = Kv*R1;
        } else {
            R2 = 999;
            R3 = 0;
        }
        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2 = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = 0;
        Stages.append(curr_stage);

    }

}

void Filter::createPartList(QStringList &lst)
{
    lst<<"N C1(uF) C2(uF) R1(kOhm) R2(kOhm) R3(kOhm) R4(kOhm)";
    RC_elements stage;

    foreach (stage,Stages) {
        lst<<QString::number(stage.N)+"  "+QString::number(stage.C1)+"  "+QString::number(stage.C2)+
             "  "+QString::number(stage.R1)+"  "+QString::number(stage.R2)+"  "+QString::number(stage.R3)+
             "  "+QString::number(stage.R4);
    }
}
