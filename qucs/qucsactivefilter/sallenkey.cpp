/***************************************************************************
                               sallenkey.cpp
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


#include "sallenkey.h"
#include <iostream>

SallenKey::SallenKey(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par) :
    Filter(ffunc_, type_, par)
{
    Nr1 = 4;
    Nc1 = 2;
    Nop1 = 1;
}


void SallenKey::calcLowPass()
{
    float R1,R2,R3,R4,C1,C2;
    float Wc = 2*M_PI*Fc;
    float Nst = order/2 + order%2;
    float Kv1 = pow(Kv,1.0/Nst);
    qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;


        C2 = 10 / Fc;
        C1 = (B*B+4*C*(Kv1-1))*C2/(4*C);
        R1 = 2/(Wc*(B*C2+sqrt((B*B + 4*C*(Kv1-1))*C2*C2-4*C*C1*C2)));
        R2 = 1/(C*C1*C2*R1*Wc*Wc);

        if (Kv != 1.0) {
            R3 = Kv1*(R1 + R2)/(Kv1 - 1);
            R4 = Kv1*(R1 + R2);
        } else {
            R3 = 1;
            R4 = 0;
        }

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 1000*R4;
        curr_stage.R5 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = C2;
        Sections.append(curr_stage);
    }

    this->calcFirstOrder();
}


void SallenKey::calcHighPass()
{
    float R1,R2,R3,R4,C1;
    float Wc = 2*M_PI*Fc;

    float Nst = order/2 + order%2;
    float Kv1 = pow(Kv,1.0/Nst);

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;

        qDebug()<<B<<C;

        C1 = 10 / Fc;

        R2 = 4*C/(Wc*C1*(B+sqrt(B*B+8*C*(Kv1-1))));

        R1 = C/(Wc*Wc*C1*C1*R2);

        if (Kv != 1.0) {
            R3 = Kv1*R2/(Kv1 - 1);
            R4 = Kv1*R2;
        } else {
            R3 = 1;
            R4 = 0;
        }

        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2  = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 1000*R4;
        curr_stage.R5 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = C1;
        Sections.append(curr_stage);
    }
    calcFirstOrder();
}

void SallenKey::createHighPassSchematic(QString &s)
{
    int const N_R=4; // number of resisitors in 2-order Sallen-Key stage
    int const N_C=2; // number of capacitors in 2-order Sallen-Key stage
    RC_elements stage;
    int dx = 0;
    int N2ord = order/2; // number of 2-nd order stages
    int N1stOrd = order%2; // number of 1-st order stages

    s += "<Qucs Schematic 0.0.17>\n";
    s += "<Components>\n";
    s += QString("<.AC AC1 1 30 410 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"%1 kHz\" 1 \"501\" 1 \"no\" 0>\n").arg((10.0*Fc)/1000.0);
    s += "<.DC DC1 1 280 410 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    s += "<Eqn Eqn1 1 270 540 -30 14 0 0 \"K=(out.v/in.v)\" 1 \"yes\" 0>\n";
    s += QString("<Vac V1 1 %1 260 18 -26 0 1 \"1 V\" 1 \"1 kHz\" 0 \"0\" 0 \"0\" 0>\n").arg(20+dx);
    s += QString("<GND * 1 %1 290 0 0 0 0>\n").arg(20+dx);
    for (int i=1; i<=N2ord; i++) {
        stage = Sections.at(i-1);
        qDebug()<<stage.N;
        QString suffix1, suffix2;
        float C1 = autoscaleCapacitor(stage.C1,suffix1);
        float C2 = autoscaleCapacitor(stage.C2,suffix2);
        //qDebug()<<C1<<suffix1<<" "<<C2<<suffix2<<stage.C1<<stage.C2;
        s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(stage.N).arg(370+dx);
        s += QString("<GND * 1 %1 270 0 0 0 0>\n").arg(270+dx);
        s += QString("<GND * 1 %1 370 0 0 0 0>\n").arg(320+dx);
        s += QString("<R R%1 1 %2 340 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(3+(i-1)*N_R).arg(320+dx).arg(stage.R3,0,'f',3);
        s += QString("<C C%1 1 %2 190 -26 -45 1 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(2+(i-1)*N_C).arg(200+dx).arg(C2,0,'f',3).arg(suffix2);
        s += QString("<C C%1 1 %2 190 -26 17 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(1+(i-1)*N_C).arg(100+dx).arg(C1,0,'f',3).arg(suffix1);
        s += QString("<R R%1 1 %2 240 -75 -26 1 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(2+(i-1)*N_R).arg(270+dx).arg(stage.R2,0,'f',3);
        s += QString("<R R%1 1 %2 70 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(1+(i-1)*N_R).arg(330+dx).arg(stage.R1,0,'f',3);
        s += QString("<R R%1 1 %2 260 -26 15 1 2 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(4+(i-1)*N_R).arg(410+dx).arg(stage.R4,0,'f',3);
        dx += 510;
    }

    if (N1stOrd!=0) {
        qDebug()<<Sections.last().N;
        createFirstOrderComponentsHPF(s,Sections.last(),dx);
    }

    s += "</Components>\n";
    s += "<Wires>\n";
    dx = 0;
    s += QString("<%1 190 %2 230 \"\" 0 0 0 \"\">\n").arg(20+dx).arg(20+dx);
    s += QString("<%1 190 %2 190 \"in\" %3 160 18 \"\">\n").arg(20+dx).arg(70+dx).arg(70+dx);
    for (int i=1; i<=N2ord; i++) {
        if (i!=1) {
            s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(70+dx);
            s += QString("<%1 190 %2 160 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-20);
            s += QString("<%1 160 %2 160 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-50);
        }
        s += QString("<%1 70 %2 70 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(460+dx);
        if ((2*i)==order) {
            s += QString("<%1 70 %2 160 \"out\" %3 90 51 \"\">\n").arg(460+dx).arg(460+dx).arg(490+dx);
        } else {
            s += QString("<%1 70 %2 160 \"\" 0 0 0 \"\">\n").arg(460+dx).arg(460+dx);
        }
        s += QString("<%1 160 %2 160 \"\" 0 0 0 \"\">\n").arg(410+dx).arg(460+dx);
        s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(440+dx).arg(460+dx);
        s += QString("<%1 160 %2 260 \"\" 0 0 0 \"\">\n").arg(460+dx).arg(460+dx);
        s += QString("<%1 190 %2 210 \"\" 0 0 0 \"\">\n").arg(270+dx).arg(270+dx);
        s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(230+dx).arg(270+dx);
        s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(130+dx).arg(150+dx);
        s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(150+dx).arg(170+dx);
        s += QString("<%1 70 %2 190 \"\" 0 0 0 \"\">\n").arg(150+dx).arg(150+dx);
        s += QString("<%1 70 %2 70 \"\" 0 0 0 \"\">\n").arg(150+dx).arg(300+dx);
        s += QString("<%1 140 %2 190 \"\" 0 0 0 \"\">\n").arg(270+dx).arg(270+dx);
        s += QString("<%1 140 %2 140 \"\" 0 0 0 \"\">\n").arg(270+dx).arg(340+dx);
        s += QString("<%1 180 %2 260 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        s += QString("<%1 180 %2 180 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(340+dx);
        s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(380+dx);
        s += QString("<%1 260 %2 310 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        dx += 510;
    }

    if (N1stOrd!=0) {
        createFirstOrderWires(s,dx,160);
    }

    s += "</Wires>\n";
}

void SallenKey::createLowPassSchematic(QString &s)
{
    int const N_R=4; // number of resisitors in 2-order Sallen-Key stage
    int const N_C=2; // number of capacitors in 2-order Sallen-Key stage
    RC_elements stage;
    int dx = 0;
    int N2ord = order/2; // number of 2-nd order stages
    int N1stOrd = order%2; // number of 1-st order stages

    s += "<Qucs Schematic 0.0.17>\n";
    s += "<Components>\n";
    s += QString("<.AC AC1 1 30 410 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"%1 kHz\" 1 \"501\" 1 \"no\" 0>\n").arg((10.0*Fc)/1000.0);
    s += "<.DC DC1 1 280 410 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    s += "<Eqn Eqn1 1 270 540 -30 14 0 0 \"K=(out.v/in.v)\" 1 \"yes\" 0>\n";
    s += QString("<Vac V1 1 %1 260 18 -26 0 1 \"1 V\" 1 \"1 kHz\" 0 \"0\" 0 \"0\" 0>\n").arg(20+dx);
    s += QString("<GND * 1 %1 290 0 0 0 0>\n").arg(20+dx);
    for (int i=1; i<=N2ord; i++) {
        stage = Sections.at(i-1);
        qDebug()<<stage.N;
        QString suffix1, suffix2;
        float C1 = autoscaleCapacitor(stage.C1,suffix1);
        float C2 = autoscaleCapacitor(stage.C2,suffix2);
        s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(stage.N).arg(370+dx);
        s += QString("<GND * 1 %1 270 0 0 0 0>\n").arg(270+dx);
        s += QString("<GND * 1 %1 370 0 0 0 0>\n").arg(320+dx);
        s += QString("<C C%1 1 %2 70 -26 15 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(2+(i-1)*N_C).arg(330+dx).arg(C2,0,'f',3).arg(suffix2);
        s += QString("<C C%1 1 %2 240 -75 -26 1 1 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(1+(i-1)*N_C).arg(270+dx).arg(C1,0,'f',3).arg(suffix1);
        s += QString("<R R%1 1 %2 190 -26 -45 1 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(2+(i-1)*N_R).arg(200+dx).arg(stage.R2,0,'f',3);
        s += QString("<R R%1 1 %2 190 -26 17 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(1+(i-1)*N_R).arg(100+dx).arg(stage.R1,0,'f',3);
        s += QString("<R R%1 1 %2 340 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(3+(i-1)*N_R).arg(320+dx).arg(stage.R3,0,'f',3);
        s += QString("<R R%1 1 %2 260 -26 15 1 2 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(4+(i-1)*N_R).arg(410+dx).arg(stage.R4,0,'f',3);
        dx += 510;
    }

    if (N1stOrd!=0) {
        createFirstOrderComponentsLPF(s,Sections.last(),dx);
    }

    s += "</Components>\n";
    s += "<Wires>\n";
    dx = 0;
    s += QString("<%1 190 %2 230 \"\" 0 0 0 \"\">\n").arg(20+dx).arg(20+dx);
    s += QString("<%1 190 %2 190 \"in\" %3 160 18 \"\">\n").arg(20+dx).arg(70+dx).arg(70+dx);
    for (int i=1; i<=N2ord; i++) {
        if (i!=1) {
            s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(70+dx);
            s += QString("<%1 190 %2 160 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-20);
            s += QString("<%1 160 %2 160 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-50);
        }
        s += QString("<%1 70 %2 70 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(460+dx);
        if ((2*i)==order) {
            s += QString("<%1 70 %2 160 \"out\" %3 90 51 \"\">\n").arg(460+dx).arg(460+dx).arg(490+dx);
        } else {
            s += QString("<%1 70 %2 160 \"\" 0 0 0 \"\">\n").arg(460+dx).arg(460+dx);
        }
        s += QString("<%1 160 %2 160 \"\" 0 0 0 \"\">\n").arg(410+dx).arg(460+dx);
        s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(440+dx).arg(460+dx);
        s += QString("<%1 160 %2 260 \"\" 0 0 0 \"\">\n").arg(460+dx).arg(460+dx);
        s += QString("<%1 190 %2 210 \"\" 0 0 0 \"\">\n").arg(270+dx).arg(270+dx);
        s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(230+dx).arg(270+dx);
        s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(130+dx).arg(150+dx);
        s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(150+dx).arg(170+dx);
        s += QString("<%1 70 %2 190 \"\" 0 0 0 \"\">\n").arg(150+dx).arg(150+dx);
        s += QString("<%1 70 %2 70 \"\" 0 0 0 \"\">\n").arg(150+dx).arg(300+dx);
        s += QString("<%1 140 %2 190 \"\" 0 0 0 \"\">\n").arg(270+dx).arg(270+dx);
        s += QString("<%1 140 %2 140 \"\" 0 0 0 \"\">\n").arg(270+dx).arg(340+dx);
        s += QString("<%1 180 %2 260 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        s += QString("<%1 180 %2 180 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(340+dx);
        s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(380+dx);
        s += QString("<%1 260 %2 310 \"\" 0 0 0 \"\">\n").arg(320+dx).arg(320+dx);
        dx += 510;
    }

    if (N1stOrd!=0) {
        createFirstOrderWires(s,dx,160);
    }

    s += "</Wires>\n";
}
