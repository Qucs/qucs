/***************************************************************************
                              matchdialog.h
                             ---------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de

-----------------------------------------------------------------------------
    Update (2017)        : New impedance matching techniques: Single matching,
                           double stub matching, real to real broadband transformers, 
                           cascaded L-sections and lambda/8 + lambda/4 matching

                          Andres Martinez-Mera <andresmartinezmera@gmail.com>
                          Claudio Girardi      <claudio.girardi@virgilio.it>
------------------------------------------------------------------------------

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

#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <cmath>

class Element;
class QLabel;
class QLineEdit;
class QComboBox;
class QGroupBox;
class QRadioButton;
class QHBoxLayout;
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

#define LSECTION           0
#define SINGLESTUB         1
#define DOUBLESTUB         2
#define MULTISTAGEL4       3
#define CASCADEDLSECTIONS  4
#define L8L4               5
#define PI_TYPE            6

enum NETWORK_TYPE{TWO_PORT_INPUT, TWO_PORT_OUTPUT, SINGLE_PORT};
enum RESPONSE_TYPE{LOWPASS, HIGHPASS};

struct NetworkParams {
  double S11real, S11imag, S12real, S12imag, S21real, S21imag, S22real, S22imag;
  double Z1, Z2, freq;
  bool BalancedStubs, micro_syn, SP_block, open_short;
  int order;
  double gamma_MAX;
  double r_real, r_imag;
  tSubstrate Substrate;
  NETWORK_TYPE network;
  double DetReal, DetImag;
  int input_network_type, output_network_type;
  double Q;
  RESPONSE_TYPE network_response;
};

static const double Z_FIELD = 376.73031346958504364963;
static const double SPEED_OF_LIGHT = 299792458.0;

/*! coth function */
static inline double coth(const double x) {
  return (1.0 + 2.0 / (exp(2.0 * (x)) - 1.0));
}

/*! sech function */
static inline double sech(const double x) {
  return (2.0 / (exp(x) + exp(-(x))));
}

/* sec function*/
static inline double sec(const double x) { return 1 / cos(x); }

/* sec function*/
static inline double asec(const double x) { return acos(1 / x); }

class MatchDialog : public QDialog {
  Q_OBJECT
public:
  MatchDialog(QWidget *parent = 0);
  ~MatchDialog();

  static void c2p(double &, double &);
  static void p2c(double &, double &);
  static void r2z(double &, double &, double);
  static void z2r(double &, double &, double);

  bool calcMatchingCircuit(struct NetworkParams);
  enum SchematicCode {
    SNG_PORT,
    SNG_PORT_SPAR_SIM,
    DBL_PORT,
    DBL_PORT_SPAR_SIM
  };
  //-------------------------------------------------------------------------------------------------------
  // These functions calculate the specified matching network and and generate
  // the circuit description code
  QString getImageFrom_XB(bool, double, double);
  QString calcMatchingLC(struct NetworkParams);
  QString calcMatchingCascadedLCSections(struct NetworkParams);
  QString calcSingleStub(struct NetworkParams);
  QString calcDoubleStub(struct NetworkParams);
  QString calcMatchingLambda8Lambda4(struct NetworkParams);
  QString calcBinomialLines(struct NetworkParams);
  QString calcChebyLines(struct NetworkParams);
  QString calcPiParameters(struct NetworkParams);
  QString calcPiType(struct NetworkParams);
  //--------------------------------------------------------------------------------------------------------

  QString calcBiMatch(struct NetworkParams);
  bool calc2PortMatch(struct NetworkParams);

  void SchematicParser(QString, int &, double, tSubstrate,
                       bool); // This function convert the circuit description
                              // code into a Qucs schematic

  void getMicrostrip(double, double, tSubstrate *, double &, double &);
  void setFrequency(double);
  void setTwoPortMatch(bool on) {
    TwoCheck->setChecked(on);
    TwoCheck->setEnabled(false);
  }

public slots:
  void slotButtCreate();
  void slotImpedanceChanged(const QString &);
  void slotReflectionChanged(const QString &);
  void slotSetTwoPort(bool);
  void slotChangeMode(int);
  void setS11LineEdits(double, double);
  void setS12LineEdits(double, double);
  void setS21LineEdits(double, double);
  void setS22LineEdits(double, double);
  void slotChangeMode_TopoCombo();
  void slotSetMicrostripCheck();
  void slotChebyCheck();

private:
  QHBoxLayout *all; // the mother of all widgets
  QVBoxLayout *MethodLayout;
  QDoubleValidator *DoubleVal;
  QLabel *Port1Label, *Port2Label, *Ohm1Label, *Ohm2Label, *FormatLabel,
      *FrequencyLabel, *TopoLabel, *TopoLabel_Output, *OrderLabel, *RelPermLabel, *S11Label,
      *S11sLabel, *S11uLabel, *S21Label, *S21sLabel, *S21uLabel, *S12Label,
      *S12sLabel, *S12uLabel, *S22Label, *S22sLabel, *S22uLabel, *SubsHScale,
      *ThicknessScale, *minWScale, *maxWScale, *ResistivityLabel, *QualityFactorLabel,
      *maxRippleLabel, *thicknessLabel, *subsHLabel, *minWLabel, *maxWLabel, *NetworkResponseLabel,
      *tanDLabel, *WeightingLabel, *InputLabel, *OutputLabel;
  QComboBox *FormatCombo, *UnitCombo, *TopoCombo, *RelPermCombo, *TopoCombo_Output, *NetworkResponseCombo;

  QLineEdit *Ref1Edit, *Ref2Edit, *FrequencyEdit, *OrderEdit, *ResistivityEdit,
      *MaxRippleEdit, *SubHeightEdit, *thicknessEdit, *minWEdit, *maxWEdit, *QualityFactorEdit,
      *tanDEdit, *S11magEdit, *S11degEdit, *S21magEdit, *S21degEdit,
      *S12magEdit, *S12degEdit, *S22magEdit, *S22degEdit;

  QCheckBox *TwoCheck, *MicrostripCheck, *AddSPBlock, *BalancedCheck;
  QRadioButton *OpenRadioButton, *ShortRadioButton, *BinRadio, *ChebyRadio;
  QGroupBox *SubstrateBox, *Weighting_groupBox, *MethodBox;

  double tmpS21mag, tmpS21deg;

  void set2PortWidgetsVisible(bool);
  QString flipLadderCode(QString laddercode);
};

#endif
