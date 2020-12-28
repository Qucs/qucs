/***************************************************************************
                             qucsactivefilter.h
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

#ifndef FILTERSINTEZ_H
#define FILTERSINTEZ_H

#include <QtGui>
#include <QtSvg>
#include <QMainWindow>
#include <complex>
#include "filter.h"

struct tQucsSettings {
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString Language;
  bool showConsole;
};

extern struct tQucsSettings QucsSettings;

class QucsActiveFilter : public QMainWindow
{
    Q_OBJECT

private:

    enum lstTopology { topoMFB = 0, topoSallenKey = 1, topoCauer = 2};
    enum lstFiltFunc { funcButterworth = 0, funcChebyshev = 1, funcInvChebyshev = 2,
                       funcCauer = 3, funcBessel = 4, funcLegendre = 5, funcUser = 6};
    enum lstFiltType { tLowPass =0, tHiPass=1, tBandPass=2, tBandStop=3};

    QLabel *lblInputData{nullptr};
    QLabel *lblA1{nullptr};
    QLabel *lblA2{nullptr};
    QLabel *lblF1{nullptr};
    QLabel *lblF2{nullptr};
    QLabel *lblTyp{nullptr};
    QLabel *lblKv{nullptr};
    QLabel *lblRpl1{nullptr};
    QLabel *lblRpl2{nullptr};
    QLabel *lblResp{nullptr};

    QLabel *lblAFR;
    QLabel *lblTopology;
    QLabel *lblOrder;

    QLineEdit *edtA1{nullptr}; // passband attenuation A1
    QLineEdit *edtA2{nullptr}; // stopband attenuation A2
    QLineEdit *edtF1{nullptr}; // passband cutoff frequency F1
    QLineEdit *edtF2{nullptr}; // stopband cutoff frequency F2
    QLineEdit *edtKv{nullptr};
    QLineEdit *edtPassbRpl{nullptr};
    QLineEdit *edtStopbRpl{nullptr};
    QLineEdit *edtOrder{nullptr};

    QComboBox *cbxFilterFunc{nullptr};
    QPushButton *btnCalcSchematic{nullptr};
    QPushButton *btnDefineTransferFunc{nullptr};

    QGroupBox *gpbCons{nullptr};
    QAction *viewConsole{nullptr};
    QPlainTextEdit *txtResult{nullptr};

    QSvgWidget *imgAFR{nullptr};
    QLabel *lblSch{nullptr};

    /*QRadioButton *btnLowPass;
    QRadioButton *btnHighPass;
    QRadioButton *btnBandPass;
    QRadioButton *btnBandStop;*/

    QComboBox *cbxFilterType{nullptr};
    QComboBox *cbxResponse{nullptr};

    QPushButton *btnElements{nullptr};
    //QPushButton *btnPassive;

    QVBoxLayout *top1{nullptr};
    
    QSvgWidget *sch_pic{nullptr};

    QWidget *zenter{nullptr};

    void errorMessage(QString s);

    QVector< std::complex<float> > Poles;

    double  Fc;
    int Nfil;

    QVector<long double> coeffB;
    QVector<long double> coeffA;

    Filter::FType ftyp;

private slots:

    void slotUpdateSchematic();
    void slotUpdateResponse();
    void slotCalcSchematic();
    void slotSwitchParameters();
    void slotSetLabels();
    void slotDefineTransferFunc();
    void slotHelpAbout();
    void slotHelpAboutQt();
    void slotHelpIntro();
    void slotViewConsole(bool toggle);
    void fixSize();

public:
    QucsActiveFilter(QWidget *parent = 0);
    ~QucsActiveFilter();
};

#endif // FILTERSINTEZ_H
