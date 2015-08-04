/***************************************************************************
                              matchdialog.h
                             ---------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATCHDIALOG_H
#define MATCHDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <math.h>

class Element;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QRadioButton;
class QVBoxLayout;
class QDoubleValidator;

struct tSubstrate {
  double er;
  double height;
  double thickness;
  double tand;
  double resistivity;
  double roughness;
  double minWidth, maxWidth;
};

static const double Z_FIELD = 376.73031346958504364963;
/*! coth function */
static inline double coth(const double x) {
  return (1.0 + 2.0 / (exp(2.0*(x)) - 1.0));
}

/*! sech function */
static inline double sech(const double x) {
  return  (2.0 / (exp(x) + exp(-(x))));
}

class MatchDialog : public QDialog {
   Q_OBJECT
public:
  MatchDialog(QWidget *parent=0);
 ~MatchDialog();

  static void c2p(double&, double&);
  static void p2c(double&, double&);
  static void r2z(double&, double&, double);
  static void z2r(double&, double&, double);
  static QString calcMatching(double, double, double, double);
  static bool calcMatchingCircuitLC(double, double, double, double);
  bool calcMatchingCircuitSingleStub(double, double, double, double, bool, bool);
  bool calcMatchingCircuitDoubleStub(double, double, double, double, bool, bool);
  bool calcMatchingCircuitLambda4(double, double, double, double, int, bool);
  static QString calcBiMatch(double, double, double, double, double, double,double, double);
  static bool calc2PortMatch(double, double, double, double, double, double, double, double, double);
  bool calcMatchingCircuitSingleStub2Ports(double S11real, double S11imag,
                                                  double S22real, double S22imag, double DetReal, double DetImag,
                                                  double Z1, double Z2, double Freq, bool open_short, bool micro_syn);
  bool calcMatchingCircuitDoubleStub2Ports(double S11real, double S11imag,
                                                  double S22real, double S22imag, double DetReal, double DetImag,
                                                  double Z1, double Z2, double Freq, double open_short, bool micro_syn);
  bool calcMatchingCircuitLambda42Ports(double S11real, double S11imag,
                                               double S22real, double S22imag, double DetReal, double DetImag,
                                               double Z1, double Z2, double Freq, int order, bool micro_syn);
  void getMicrostrip(double Z0, double freq, tSubstrate *substrate, double &width, double &er_eff);


  void setFrequency(double);

  QLineEdit *Ref1Edit, *Ref2Edit, *FrequencyEdit, *OrderEdit,
            *S11magEdit,*S11degEdit, *S21magEdit,*S21degEdit,*ResistivityEdit,
            *S12magEdit,*S12degEdit, *S22magEdit,*S22degEdit, *SubHeightEdit, *thicknessEdit, *minWEdit, *maxWEdit, *tanDEdit;
  QCheckBox *TwoCheck, *MicrostripCheck;
  QRadioButton *OpenRadioButton, *ShortRadioButton;
  QGroupBox *SubstrateBox;

public slots:
  void slotButtCreate();
  void slotImpedanceChanged(const QString&);
  void slotReflexionChanged(const QString&);
  void slotSetTwoPort(bool);
  void slotChangeMode(int);
  void slotChangeMode_TopoCombo(int);
  void slotSetMicrostripCheck(bool on);

private:
  QHBoxLayout *all;   // the mother of all widgets
  QDoubleValidator  *DoubleVal;
  QLabel      *Port1Label, *Port2Label, *Ohm1Label, *Ohm2Label,
              *FormatLabel, *FrequencyLabel, *TopoLabel, *OrderLabel, *RelPermLabel,
              *S11Label, *S11sLabel, *S11uLabel,
              *S21Label, *S21sLabel, *S21uLabel,
              *S12Label, *S12sLabel, *S12uLabel, *SubsHScale, *ThicknessScale, *minWScale, *maxWScale, *ResistivityLabel,
              *S22Label, *S22sLabel, *S22uLabel, *thicknessLabel, *subsHLabel, *minWLabel, *maxWLabel, *tanDLabel;
  QComboBox   *FormatCombo, *UnitCombo, *TopoCombo, *RelPermCombo;
};

#endif
