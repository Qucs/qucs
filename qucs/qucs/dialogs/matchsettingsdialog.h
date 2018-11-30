/***************************************************************************
                          matchsettingsdialog.h
                             ---------------
    begin                : Nov 2018
    copyright            : (C) 2018 by Andres Martinez-Mera, The Qucs Team
    email                : andresmartinezmera@gmail.com

-----------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATCHSETTINGSDIALOG_H
#define MATCHSETTINGSDIALOG_H

#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>

#include <QDialog>
#include <QGridLayout>
#include "matchsubstratedialog.h"

#define LSECTION           0
#define SINGLESTUB         1
#define DOUBLESTUB         2
#define MULTISTAGEL4       3
#define CASCADEDLSECTIONS  4
#define L8L4               5
#define PI_TYPE            6

#define CHEBYSHEV_WEIGHTING 0
#define BINOMIAL_WEIGHTING  1

#define CALCULATE_INPUT  0
#define CALCULATE_OUTPUT 1

enum NETWORK_TYPE{TWO_PORT_INPUT, TWO_PORT_OUTPUT, SINGLE_PORT};
enum RESPONSE_TYPE{LOWPASS, HIGHPASS};

struct ImplementationParams {
    bool BalancedStubs=false, open_short=true;//Stub implementation
    int order=3;//Number of section
    double gamma_MAX=0.05;//Maximum ripple for the impedance transformer
    int network_type = 0;//Network topology
    double Q = 5;//Q of the overall matching network. Only for Pi/Tee matching
    RESPONSE_TYPE network_response = LOWPASS;//Response type for Pi/Tee matching networks
    int weighting_type = 0;//Weighting of the impedance transformer: Chebyshev or binomial
    double CAPQ = 1000;//Capacitor quality factor, Q = X/R = 1/(w·C·R)
    double INDQ = 1000;//Inductor quality factor, Q = X/R = (w·L) / R
};


struct NetworkParams {
  double S11real, S11imag, S12real, S12imag, S21real, S21imag, S22real, S22imag;
  double Z1, Z2, freq;
  tSubstrate Substrate;
  NETWORK_TYPE network;
  double DetReal, DetImag;
  struct ImplementationParams InputNetwork, OutputNetwork;
  bool micro_syn, SP_block;
  double r_real, r_imag;
};

class MatchSettingsDialog : public QDialog {
  Q_OBJECT
public:
  MatchSettingsDialog(QWidget *parent = 0, int topology=LSECTION);
  ~MatchSettingsDialog();
  struct ImplementationParams GetOptions();

private:
  QLabel *Order_Label, *Network_Response_Label, *QualityFactor_Label, *maxRipple_Label, *Weighting_Type_Label,
         *Stub_Type_Label, *Stub_Implementation_Label, *CapacitorQ_Label, *InductorQ_Label;
  QComboBox *Network_Response_Combo, *Stub_Type_Combo, *Weighting_Type_Combo, *Stub_Implementation_Combo;
  QSpinBox *Order_Spinbox;
  QDoubleSpinBox *Quality_Factor_Spinbox, *maxRipple_Spinbox, *CapacitorQ_Spinbox, *InductorQ_Spinbox;
  QPushButton *OK_Button, *Cancel_Button;
  struct ImplementationParams params;

public slots:
  void slot_save_settings();
  void slot_cancel_settings();  
};
#endif
