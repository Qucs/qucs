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

void Filter::createSchematic(QString &s)
{
    switch (ftype) {
    case Filter::HighPass : createHighPassSchematic(s);
        break;
    case Filter::LowPass : createLowPassSchematic(s);
        break;
    default: break;
    }

    QFile sch("filter.sch");
    sch.open(QFile::WriteOnly);
    QTextStream out(&sch);
    out<<s;
    sch.close();
}

void Filter::createHighPassSchematic(QString &s)
{

}

void Filter::createLowPassSchematic(QString &s)
{

}

void Filter::calcFilter()
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

    Nr = Nr1*(Nfil/2);
    Nc = Nc1*(Nfil/2);
    Nopamp = Nop1*Nfil/2;
}


void Filter::calcHighPass()
{

}

void Filter::calcLowPass()
{

}

void Filter::calcFirstOrder()
{
    if (Nfil%2 != 0) {

        float R2,R3;

        int k = Nfil/2 + 1;
        float Wc = 2*M_PI*Fc;
        float re = Poles.at(k-1).real();
        //float im = Poles.at(k-1).imag();
        //float C = re*re + im*im;
        float C = -re;
        float C1 = 10/Fc;
        float R1 = 1.0/(Wc*C*C1);
        qDebug()<<C;

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

void Filter::createFirstOrderComponentsHPF(QString &s,RC_elements stage,int dx)
{
    QString suf;
    float C1 = autoscaleCapacitor(stage.C1,suf);
    s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(Nopamp+1).arg(270+dx);
    s += QString("<GND * 1 %1 270 0 0 0 0>\n").arg(170+dx);
    s += QString("<GND * 1 %1 370 0 0 0 0>\n").arg(220+dx);
    s += QString("<R R%1 1 %2 340 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+2).arg(220+dx).arg(stage.R2,0,'f',3);
    s += QString("<R R%1 1 %2 240 -75 -26 1 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+1).arg(170+dx).arg(stage.R1,0,'f',3);
    s += QString("<R R%1 1 %2 260 -26 15 1 2 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+3).arg(310+dx).arg(stage.R3,0,'f',3);
    s += QString("<C C%1 1 %2 190 -26 -45 1 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(Nc+1).arg(100+dx).arg(C1,0,'f',3).arg(suf);
}


void Filter::createFirstOrderComponentsLPF(QString &s,RC_elements stage,int dx)
{
    QString suf;
    float C1 = autoscaleCapacitor(stage.C1,suf);
    s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(Nopamp+1).arg(270+dx);
    s += QString("<GND * 1 %1 270 0 0 0 0>\n").arg(170+dx);
    s += QString("<GND * 1 %1 370 0 0 0 0>\n").arg(220+dx);
    s += QString("<R R%1 1 %2 340 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+2).arg(220+dx).arg(stage.R2,0,'f',3);
    s += QString("<R R%1 1 %2 190 -75 -52 1 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+1).arg(100+dx).arg(stage.R1,0,'f',3);
    s += QString("<R R%1 1 %2 260 -26 15 1 2 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+3).arg(310+dx).arg(stage.R3,0,'f',3);
    s += QString("<C C%1 1 %2 240 26 -45 1 1 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(Nc+1).arg(170+dx).arg(C1,0,'f',3).arg(suf);
}

void Filter::createFirstOrderWires(QString &s, int dx)
{
    s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(70+dx);
    s += QString("<%1 190 %2 160 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-20);
    s += QString("<%1 160 %2 160 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-50);

    s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(130+dx).arg(170+dx);
    s += QString("<%1 160 %2 160 \"out\" %3 130 39 \"\">\n").arg(310+dx).arg(360+dx).arg(380+dx);
    s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(340+dx).arg(360+dx);
    s += QString("<%1 160 %2 260 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(360+dx);
    s += QString("<%1 190 %2 210 \"\" 0 0 0 \"\">\n").arg(170+dx).arg(170+dx);
    s += QString("<%1 140 %2 190 \"\" 0 0 0 \"\">\n").arg(170+dx).arg(170+dx);
    s += QString("<%1 140 %2 140 \"\" 0 0 0 \"\">\n").arg(170+dx).arg(240+dx);
    s += QString("<%1 180 %2 260 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
    s += QString("<%1 180 %2 180 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(240+dx);
    s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(280+dx);
    s += QString("<%1 260 %2 310 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
}


float Filter::autoscaleCapacitor(float C, QString &suffix)
{
    float C1 = C*1e-6;

    if (C1>=1e-7) {
        suffix = "uF";
        C1 *= 1e6;
    }

    if ((C1<1e-7)&&(C1>=1e-8)) {
        suffix = "nF";
        C1 *= 1e9;
    }

    if (C1<1e-8) {
        suffix = "pF";
        C1 *= 1e12;
    }
    return C1;
}
