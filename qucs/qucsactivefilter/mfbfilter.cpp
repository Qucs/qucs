/***************************************************************************
                               mfbfilter.cpp
                              ----------------
    begin                : Wed Apr 10 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mfbfilter.h"

MFBfilter::MFBfilter(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par)
    : Filter(ffunc_,type_,par)
{
    switch (ftype) {
    case Filter::LowPass :
        Nr1=3;
        Nc1=2;
        break;
    case Filter::HighPass :
        Nr1=2;
        Nc1=3;
        break;
    default:
        break;
    }

    //Nr1 = 3;
    //Nc1 = 2;
    Nop1 = 1;
}

void MFBfilter::createLowPassSchematic(QString &s)
{
    //int const N_R=4; // number of resisitors in 2-order Sallen-Key stage
    //int const N_C=2; // number of capacitors in 2-order Sallen-Key stage
    RC_elements stage;
    int dx = 0;
    int N2ord = order/2; // number of 2-nd order stages
    int N1stOrd = order%2; // number of 1-st order stages

    s += "<Qucs Schematic 0.0.17>\n";
    s += "<Components>\n";
    s += QString("<.AC AC1 1 300 440 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"%1 kHz\" 1 \"501\" 1 \"no\" 0>\n").arg((10.0*Fc)/1000.0);
    s += "<.DC DC1 1 60 440 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    s += "<Eqn Eqn1 1 610 450 -30 14 0 0 \"K=dB(out.v/in.v)\" 1 \"yes\" 0>\n";
    s += QString("<Vac V1 1 %1 330 18 -26 0 1 \"1 V\" 1 \"1 kHz\" 0 \"0\" 0 \"0\" 0>\n").arg(70+dx);
    s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(70+dx);
    for (int i=1; i<=N2ord; i++) {
        stage = Sections.at(i-1);
        qDebug()<<stage.N;
        QString suffix1, suffix2;
        float C1 = autoscaleCapacitor(stage.C1,suffix1);
        float C2 = autoscaleCapacitor(stage.C2,suffix2);
        s += QString("<GND * 1 %1 380 0 0 0 0>\n").arg(200+dx);
        s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(360+dx);
        s += QString("<OpAmp OP%1 1 %2 270 -26 -70 1 0 \"1e6\" 1 \"15 V\" 0>\n").arg(1+(i-1)*Nop1).arg(390+dx);
        s += QString("<C C%1 1 %2 350 17 -26 0 1 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(2+(i-1)*Nc1).arg(200+dx).arg(C2,0,'f',3).arg(suffix2);
        s += QString("<C C%1 1 %2 180 17 -26 0 1 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(1+(i-1)*Nc1).arg(320+dx).arg(C1,0,'f',3).arg(suffix1);
        s += QString("<R R%1 1 %2 180 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(1+(i-1)*Nr1).arg(200+dx).arg(stage.R1,0,'f',3);
        s += QString("<R R%1 1 %2 250 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(2+(i-1)*Nr1).arg(150+dx).arg(stage.R2,0,'f',3);
        s += QString("<R R%1 1 %2 250 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(3+(i-1)*Nr1).arg(250+dx).arg(stage.R3,0,'f',3);
        dx += 510;
    }

    if (N1stOrd!=0) {
        createFirstOrderComponentsLPF(s,Sections.last(),dx+10);
    }

    s += "</Components>\n";
    s += "<Wires>\n";
    dx = 0;
    s += "<70 250 120 250 \"in\" 120 220 22 \"\">\n";
    s += "<70 250 70 300 \"\" 0 0 0 \"\">\n";
    for (int i=1; i<=N2ord; i++) {
        if (i!=1) {
            s += QString("<%1 250 %2 270 \"\" 0 0 0 \"\">\n").arg(120+dx).arg(120+dx);
            s += QString("<%1 270 %2 270 \"\" 0 0 0 \"\">\n").arg(dx-40).arg(120+dx);
        }
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(180+dx).arg(200+dx);
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(220+dx);
        s += QString("<%1 210 %2 250 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(200+dx);
        s += QString("<%1 250 %2 320 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(200+dx);
        s += QString("<%1 290 %2 350 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(360+dx);
        s += QString("<%1 130 %2 150 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(200+dx);
        s += QString("<%1 130 %2 130 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(320+dx);
        s += QString("<%1 130 %2 150 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(280+dx).arg(320+dx);
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(360+dx);
        s += QString("<%1 210 %2 250 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        s += QString("<%1 130 %2 130 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(470+dx);
        s += QString("<%1 270 %2 270 \"\" 0 0 0 \"\">\n").arg(430+dx).arg(470+dx);
        if ((2*i)==order) {
            s += QString("<%1 130 %2 270 \"out\" %3 170 70 \"\">\n").arg(470+dx).arg(470+dx).arg(500+dx);
        } else {
            s += QString("<%1 130 %2 270 \"\" 0 0 0 \"\">\n").arg(470+dx).arg(470+dx);
        }
        dx += 510;
    }

    if (N1stOrd!=0) {
            createFirstOrderWires(s,dx+10,270);
    }

    s += "</Wires>\n";
}

void MFBfilter::createHighPassSchematic(QString &s)
{
    RC_elements stage;
    int dx = 0;
    int N2ord = order/2; // number of 2-nd order stages
    int N1stOrd = order%2; // number of 1-st order stages

    s += "<Qucs Schematic 0.0.17>\n";
    s += "<Components>\n";
    s += QString("<.AC AC1 1 300 440 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"%1 kHz\" 1 \"501\" 1 \"no\" 0>\n").arg((10.0*Fc)/1000.0);
    s += "<.DC DC1 1 60 440 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    s += "<Eqn Eqn1 1 610 450 -30 14 0 0 \"K=dB(out.v/in.v)\" 1 \"yes\" 0>\n";
    s += QString("<Vac V1 1 %1 330 18 -26 0 1 \"1 V\" 1 \"1 kHz\" 0 \"0\" 0 \"0\" 0>\n").arg(70+dx);
    s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(70+dx);
    for (int i=1; i<=N2ord; i++) {
        stage = Sections.at(i-1);
        qDebug()<<stage.N;
        QString suffix1, suffix2;
        float C1 = autoscaleCapacitor(stage.C1,suffix1);
        float C2 = autoscaleCapacitor(stage.C2,suffix2);
        s += QString("<GND * 1 %1 380 0 0 0 0>\n").arg(200+dx);
        s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(360+dx);
        s += QString("<OpAmp OP%1 1 %2 270 -26 -70 1 0 \"1e6\" 1 \"15 V\" 0>\n").arg(1+(i-1)*Nop1).arg(390+dx);
        s += QString("<R R%1 1 %2 350 17 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(1+(i-1)*Nr1).arg(200+dx).arg(stage.R1,0,'f',3);
        s += QString("<R R%1 1 %2 180 17 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(2+(i-1)*Nr1).arg(320+dx).arg(stage.R2,0,'f',3);
        s += QString("<C C%1 1 %2 180 15 -26 0 1 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(1+(i-1)*Nc1).arg(200+dx).arg(C1,0,'f',3).arg(suffix1);
        s += QString("<C C%1 1 %2 250 -26 15 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(2+(i-1)*Nc1).arg(150+dx).arg(C2,0,'f',3).arg(suffix2);
        s += QString("<C C%1 1 %2 250 -26 15 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(3+(i-1)*Nc1).arg(250+dx).arg(C1,0,'f',3).arg(suffix1);
        dx += 510;
    }

    if (N1stOrd!=0) {
        createFirstOrderComponentsHPF(s,Sections.last(),dx+10);
    }

    s += "</Components>\n";
    s += "<Wires>\n";
    dx = 0;
    s += "<70 250 120 250 \"in\" 120 220 22 \"\">\n";
    s += "<70 250 70 300 \"\" 0 0 0 \"\">\n";
    for (int i=1; i<=N2ord; i++) {
        if (i!=1) {
            s += QString("<%1 250 %2 270 \"\" 0 0 0 \"\">\n").arg(120+dx).arg(120+dx);
            s += QString("<%1 270 %2 270 \"\" 0 0 0 \"\">\n").arg(dx-40).arg(120+dx);
        }
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(180+dx).arg(200+dx);
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(220+dx);
        s += QString("<%1 210 %2 250 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(200+dx);
        s += QString("<%1 250 %2 320 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(200+dx);
        s += QString("<%1 290 %2 350 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(360+dx);
        s += QString("<%1 130 %2 150 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(200+dx);
        s += QString("<%1 130 %2 130 \"\" 0 0 0 \"\">\n").arg(200+dx).arg(320+dx);
        s += QString("<%1 130 %2 150 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(280+dx).arg(320+dx);
        s += QString("<%1 250 %2 250 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(360+dx);
        s += QString("<%1 210 %2 250 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        s += QString("<%1 130 %2 130 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(470+dx);
        s += QString("<%1 270 %2 270 \"\" 0 0 0 \"\">\n").arg(430+dx).arg(470+dx);
        if ((2*i)==order) {
            s += QString("<%1 130 %2 270 \"out\" %3 170 70 \"\">\n").arg(470+dx).arg(470+dx).arg(500+dx);
        } else {
            s += QString("<%1 130 %2 270 \"\" 0 0 0 \"\">\n").arg(470+dx).arg(470+dx);
        }
        dx += 510;
    }

    if (N1stOrd!=0) {
            createFirstOrderWires(s,dx+10,270);
    }

    s += "</Wires>\n";
}

void MFBfilter::calcHighPass()
{
    float R1,R2,C1,C2;
    float Wc = 2*M_PI*Fc;

    for (int k=1; k <= order/2; k++) {
        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;

        C1 = 10.0/Fc;
        C2 = C1/Kv;
        R1 = B/((2*C1+C2)*Wc);
        R2 = ((2*C1 + C2)*C)/(B*C1*C2*Wc);

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 0;
        curr_stage.R4 = 0;
        curr_stage.R5 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = C2;
        Sections.append(curr_stage);
    }

    this->calcFirstOrder();

}

void MFBfilter::calcLowPass()
{
    float R1,R2,R3,C1,C2;
    float Wc = 2*M_PI*Fc;

    for (int k=1; k <= order/2; k++) {
        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;

        C2 = (10.0/Fc);
        C1 = (B*B*C2)/(4*C*(Kv+1));
        R2 = (2*(Kv+1))/(Wc*(B*C2+sqrt(B*B*C2*C2-4*C*C1*C2*(Kv+1))));
        R1 = R2/Kv;
        R3 = 1.0/(C*C1*C2*Wc*Wc*R2);

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 0;
        curr_stage.R5 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = C2;
        Sections.append(curr_stage);
    }

    this->calcFirstOrder();
}
