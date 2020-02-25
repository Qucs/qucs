/***************************************************************************
                                matchsettingsdialog.h
                                ----------
    copyright            :  QUCS team
    author                :  2018 Andres Martinez-Mera

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATCHSETTINGSDIALOG_H
#define MATCHSETTINGSDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include "matchsubstratedialog.h"
#include <QDialog>
#include <QGridLayout>

#define LSECTION 0
#define SINGLESTUB 1
#define DOUBLESTUB 2
#define MULTISTAGEL4 3
#define CASCADEDLSECTIONS 4
#define QUARTER_WAVE_LINE 5
#define L8L4 6
#define PI_TYPE 7
#define TEE_TYPE 8
#define TAPPED_C 9
#define TAPPED_L 10
#define DOUBLE_TAPPED 11
#define SINGLE_TUNED_TRANSFORMER 12
#define PARALLEL_DOUBLE_TUNED_TRANSFORMER 13
#define SERIES_DOUBLE_TUNED_TRANSFORMER 14

#define CHEBYSHEV_WEIGHTING 0
#define BINOMIAL_WEIGHTING 1

#define CALCULATE_INPUT 0
#define CALCULATE_OUTPUT 1

enum NETWORK_TYPE { TWO_PORT_INPUT, TWO_PORT_OUTPUT, SINGLE_PORT };
enum RESPONSE_TYPE { LOWPASS, HIGHPASS };

struct ImplementationParams {
  bool BalancedStubs = false, open_short = true; // Stub implementation
  int order = 3;                                 // Number of sections
  double gamma_MAX = 0.1; // Maximum ripple for the impedance transformer
  int network_type = 0;   // Network topology
  double Q = 5; // Q of the overall matching network. Only for Pi/Tee matching
  RESPONSE_TYPE network_response =
      LOWPASS; // Response type for Pi/Tee matching networks
  int weighting_type =
      0; // Weighting of the impedance transformer: Chebyshev or binomial
  double CAPQ = 1000; // Capacitor quality factor, Q = X/R = 1/(w·C·R)
  double INDQ =
      1000; // Inductor quality factor, Q = X/R = (w·L) / Rnetwork_response
  double L2 = 5e-9; // L2 parameter for the double tapped resonator
  double k = 0.95;  // Coupling coefficient of the transformer
  int coupled_L_Equivalent =
      0; // Use coupled inductor or its uncoupled equivalent
  double BW =
      20e6; // Bandwidth for the double-tuned transformer matching method
  int use_l4_lumped_equivalent = 0, use_l8_lumped_equivalent = 0,
      use_TL_lumped_equivalent = 0;
  // Use the lumped lambda/4, lambda/8, etc. transmission line lumped equivalent
  // 0 -> Use transmission lines
  // 1 -> Pi quarter-wave line equivalent
  // 2 -> Tee quarter-wave line equivalent
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
  MatchSettingsDialog(struct ImplementationParams Impl, QWidget *parent = 0,
                      int topology = LSECTION);
  ~MatchSettingsDialog();
  struct ImplementationParams GetOptions();

private:
  QLabel *Order_Label, *Network_Response_Label, *QualityFactor_Label,
      *maxRipple_Label, *Weighting_Type_Label, *Stub_Type_Label,
      *Stub_Implementation_Label, *CapacitorQ_Label, *InductorQ_Label,
      *L2_Double_Tapped_Resonator_Label, *k_Transformer_Label, *coupled_L_Label,
      *BW_Label, *f2_Label, *Lumped_QW_Label, *Lumped_L8_Label,
      *Lumped_TL_Label;
  QComboBox *Network_Response_Combo, *Stub_Type_Combo, *Weighting_Type_Combo,
      *Stub_Implementation_Combo, *L2_Double_Tapped_Resonator_Scale_Combo,
      *coupled_L_Combo, *BW_Scale_Combo, *f2_Scale_Combo, *Lumped_QW_Combo,
      *Lumped_L8_Combo, *Lumped_TL_Combo;
  QSpinBox *Order_Spinbox, *BW_Spinbox, *f2_Spinbox;
  QDoubleSpinBox *Quality_Factor_Spinbox, *maxRipple_Spinbox,
      *CapacitorQ_Spinbox, *InductorQ_Spinbox,
      *L2_Double_Tapped_Resonator_SpinBox, *k_Transformer_Spinbox;
  QPushButton *OK_Button, *Cancel_Button;
  struct ImplementationParams params;

  double getScale(QString);

public slots:
  void slot_save_settings();
  void slot_cancel_settings();
};
#endif
