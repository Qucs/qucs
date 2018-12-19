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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "matchsettingsdialog.h"
#include <QApplication>

/*
   This is a pop-up window for configuring the matching network settings
*/

MatchSettingsDialog::MatchSettingsDialog(
    struct ImplementationParams ImplDetails, QWidget *parent, int topology)
    : QDialog(parent) {
  setWindowTitle(tr("Matching circuit settings"));
  QGridLayout *MatchSettingslayout = new QGridLayout();

  // Order
  Order_Label = new QLabel(tr("Order"));
  Order_Spinbox = new QSpinBox();
  Order_Spinbox->setMinimum(2);
  Order_Spinbox->setValue(ImplDetails.order - 1);
  MatchSettingslayout->addWidget(Order_Label, 0, 0);
  MatchSettingslayout->addWidget(Order_Spinbox, 0, 1);

  // Maximum ripple
  maxRipple_Label = new QLabel(tr("Max. ripple"));
  maxRipple_Spinbox = new QDoubleSpinBox();
  maxRipple_Spinbox->setMinimum(0.05);
  maxRipple_Spinbox->setMaximum(1);
  maxRipple_Spinbox->setSingleStep(0.05);
  maxRipple_Spinbox->setValue(ImplDetails.gamma_MAX);
  MatchSettingslayout->addWidget(maxRipple_Label, 1, 0);
  MatchSettingslayout->addWidget(maxRipple_Spinbox, 1, 1);

  // Stub type
  Stub_Type_Label = new QLabel(tr("Stub type"));
  Stub_Type_Combo = new QComboBox();
  Stub_Type_Combo->addItem(tr("Open"));
  Stub_Type_Combo->addItem(tr("Short"));
  Stub_Type_Combo->setCurrentIndex(ImplDetails.open_short);
  MatchSettingslayout->addWidget(Stub_Type_Label, 2, 0);
  MatchSettingslayout->addWidget(Stub_Type_Combo, 2, 1);

  // Stub implementation (balanced or unbalanced)
  Stub_Implementation_Label = new QLabel(tr("Stub implementation"));
  Stub_Implementation_Combo = new QComboBox();
  Stub_Implementation_Combo->addItem(tr("Unbalanced"));
  Stub_Implementation_Combo->addItem(tr("Balanced"));
  Stub_Implementation_Combo->setCurrentIndex(ImplDetails.BalancedStubs);
  MatchSettingslayout->addWidget(Stub_Implementation_Label, 3, 0);
  MatchSettingslayout->addWidget(Stub_Implementation_Combo, 3, 1);

  // Weighting type
  Weighting_Type_Label = new QLabel(tr("Weighting"));
  Weighting_Type_Combo = new QComboBox();
  Weighting_Type_Combo->addItem(tr("Chebyshev"));
  Weighting_Type_Combo->addItem(tr("Binomial"));
  Weighting_Type_Combo->setCurrentIndex(ImplDetails.weighting_type);
  MatchSettingslayout->addWidget(Weighting_Type_Label, 4, 0);
  MatchSettingslayout->addWidget(Weighting_Type_Combo, 4, 1);

  // Network response
  Network_Response_Label = new QLabel(tr("Network response"));
  Network_Response_Combo = new QComboBox();
  Network_Response_Combo->addItem(tr("Lowpass"));
  Network_Response_Combo->addItem(tr("Highpass"));
  Network_Response_Combo->setCurrentIndex(ImplDetails.network_response);
  MatchSettingslayout->addWidget(Network_Response_Label, 5, 0);
  MatchSettingslayout->addWidget(Network_Response_Combo, 5, 1);

  // Quality factor
  QualityFactor_Label = new QLabel(tr("Quality factor"));
  Quality_Factor_Spinbox = new QDoubleSpinBox();
  Quality_Factor_Spinbox->setMinimum(1);
  Quality_Factor_Spinbox->setValue(ImplDetails.Q);
  Quality_Factor_Spinbox->setDecimals(1);
  Quality_Factor_Spinbox->setSingleStep(0.5);
  MatchSettingslayout->addWidget(QualityFactor_Label, 6, 0);
  MatchSettingslayout->addWidget(Quality_Factor_Spinbox, 6, 1);

  // Capacitor Q
  CapacitorQ_Label = new QLabel(tr("Capacitor Q"));
  CapacitorQ_Spinbox = new QDoubleSpinBox();
  CapacitorQ_Spinbox->setMinimum(0.5);
  CapacitorQ_Spinbox->setMaximum(1000);
  CapacitorQ_Spinbox->setValue(ImplDetails.CAPQ);
  CapacitorQ_Spinbox->setSingleStep(0.5);
  CapacitorQ_Spinbox->setDecimals(1);
  MatchSettingslayout->addWidget(CapacitorQ_Label, 7, 0);
  MatchSettingslayout->addWidget(CapacitorQ_Spinbox, 7, 1);

  // Inductor Q
  InductorQ_Label = new QLabel(tr("Inductor Q"));
  InductorQ_Spinbox = new QDoubleSpinBox();
  InductorQ_Spinbox->setMinimum(0.5);
  InductorQ_Spinbox->setMaximum(1000);
  InductorQ_Spinbox->setValue(ImplDetails.INDQ);
  InductorQ_Spinbox->setSingleStep(0.5);
  InductorQ_Spinbox->setDecimals(1);
  MatchSettingslayout->addWidget(InductorQ_Label, 8, 0);
  MatchSettingslayout->addWidget(InductorQ_Spinbox, 8, 1);

  // L2 inductor for double tapped resonator
  L2_Double_Tapped_Resonator_Label = new QLabel(tr("Fixed inductor"));
  L2_Double_Tapped_Resonator_SpinBox = new QDoubleSpinBox();
  L2_Double_Tapped_Resonator_SpinBox->setMinimum(0.1);
  L2_Double_Tapped_Resonator_SpinBox->setMaximum(1000);
  L2_Double_Tapped_Resonator_SpinBox->setSingleStep(0.5);
  double val = ImplDetails.L2;
  int suffix = 0;
  if (val < 1) {
    if (val > 1e-3)
      val *= 1e3, suffix = 1;
    else if (val > 1e-6)
      val *= 1e6, suffix = 2;
    else if (val > 1e-9)
      val *= 1e9, suffix = 3;
    else if (val > 1e-12)
      val *= 1e12, suffix = 4;
    else
      val *= 1e12, suffix = 4;
  }
  L2_Double_Tapped_Resonator_SpinBox->setValue(val);
  L2_Double_Tapped_Resonator_SpinBox->setDecimals(1);
  L2_Double_Tapped_Resonator_Scale_Combo = new QComboBox();
  L2_Double_Tapped_Resonator_Scale_Combo->addItem("H");
  L2_Double_Tapped_Resonator_Scale_Combo->addItem("mH");
  L2_Double_Tapped_Resonator_Scale_Combo->addItem("uH");
  L2_Double_Tapped_Resonator_Scale_Combo->addItem("nH");
  L2_Double_Tapped_Resonator_Scale_Combo->addItem("pH");
  L2_Double_Tapped_Resonator_Scale_Combo->setCurrentIndex(suffix);
  MatchSettingslayout->addWidget(L2_Double_Tapped_Resonator_Label, 9, 0);
  MatchSettingslayout->addWidget(L2_Double_Tapped_Resonator_SpinBox, 9, 1);
  MatchSettingslayout->addWidget(L2_Double_Tapped_Resonator_Scale_Combo, 9, 2);

  // Coupling coefficient (coupled inductors)
  k_Transformer_Label = new QLabel(tr("Coupling coefficient (k)"));
  k_Transformer_Spinbox = new QDoubleSpinBox();
  k_Transformer_Spinbox->setMinimum(0.01);
  k_Transformer_Spinbox->setMaximum(0.99);
  k_Transformer_Spinbox->setSingleStep(0.01);
  k_Transformer_Spinbox->setValue(ImplDetails.k);
  MatchSettingslayout->addWidget(k_Transformer_Label, 10, 0);
  MatchSettingslayout->addWidget(k_Transformer_Spinbox, 10, 1);

  // Use coupled inductor or the uncoupled equivalent circuit
  coupled_L_Label = new QLabel("Mode");
  coupled_L_Combo = new QComboBox();
  coupled_L_Combo->addItem(tr("Coupled inductors"));
  coupled_L_Combo->addItem(tr("Uncoupled inductors (Tee-type)"));
  coupled_L_Combo->addItem(
      QString(tr("Uncoupled inductors (%1-type)").arg(QChar(0xC0, 0x03))));
  coupled_L_Combo->setCurrentIndex(ImplDetails.coupled_L_Equivalent);
  MatchSettingslayout->addWidget(coupled_L_Label, 11, 0);
  MatchSettingslayout->addWidget(coupled_L_Combo, 11, 1);

  // BW (double-tuned transformer)
  BW_Label = new QLabel("BW");
  BW_Spinbox = new QSpinBox();
  BW_Spinbox->setMinimum(1);
  BW_Spinbox->setMaximum(1000);
  val = ImplDetails.BW;
  suffix = 0;
  if (val > 1e9)
    val *= 1e-9, suffix = 3;
  else if (val > 1e6)
    val *= 1e-6, suffix = 2;
  else if (val > 1e3)
    val *= 1e-3, suffix = 1;

  BW_Spinbox->setValue(val);
  BW_Scale_Combo = new QComboBox();
  QStringList freq_units;
  freq_units.append("Hz");
  freq_units.append("kHz");
  freq_units.append("MHz");
  freq_units.append("GHz");
  BW_Scale_Combo->addItems(freq_units);
  BW_Scale_Combo->setCurrentIndex(suffix);
  MatchSettingslayout->addWidget(BW_Label, 12, 0);
  MatchSettingslayout->addWidget(BW_Spinbox, 12, 1);
  MatchSettingslayout->addWidget(BW_Scale_Combo, 12, 2);

  // Quarter wavelength transmission line lumped equivalent
  Lumped_QW_Label =
      new QLabel(QString("%1/4 line implementation").arg(QChar(0xBB, 0x03)));
  Lumped_QW_Combo = new QComboBox();
  Lumped_QW_Combo->addItem("Transmission line");
  Lumped_QW_Combo->addItem(
      QString(tr("%1-type equivalent").arg(QChar(0xC0, 0x03))));
  Lumped_QW_Combo->addItem(tr("T-type equivalent"));
  Lumped_QW_Combo->setCurrentIndex(ImplDetails.use_l4_lumped_equivalent);
  MatchSettingslayout->addWidget(Lumped_QW_Label, 13, 0);
  MatchSettingslayout->addWidget(Lumped_QW_Combo, 13, 1);

  // lambda/8 transmission line lumped equivalent
  Lumped_L8_Label =
      new QLabel(QString("%1/8 line implementation").arg(QChar(0xBB, 0x03)));
  Lumped_L8_Combo = new QComboBox();
  Lumped_L8_Combo->addItem("Transmission line");
  Lumped_L8_Combo->addItem(
      QString(tr("%1-type equivalent").arg(QChar(0xC0, 0x03))));
  Lumped_L8_Combo->addItem(tr("T-type equivalent"));
  Lumped_L8_Combo->setCurrentIndex(ImplDetails.use_l8_lumped_equivalent);
  MatchSettingslayout->addWidget(Lumped_L8_Label, 14, 0);
  MatchSettingslayout->addWidget(Lumped_L8_Combo, 14, 1);

  // Arbitrary-length transmission line lumped equivalent
  Lumped_TL_Label = new QLabel(tr("Transmission line implementation"));
  Lumped_TL_Combo = new QComboBox();
  Lumped_TL_Combo->addItem("Transmission line");
  Lumped_TL_Combo->addItem(
      QString(tr("%1-type equivalent").arg(QChar(0xC0, 0x03))));
  Lumped_TL_Combo->addItem(tr("T-type equivalent"));
  Lumped_TL_Combo->setCurrentIndex(ImplDetails.use_TL_lumped_equivalent);
  MatchSettingslayout->addWidget(Lumped_TL_Label, 15, 0);
  MatchSettingslayout->addWidget(Lumped_TL_Combo, 15, 1);

  // Default settings
  Order_Label->setVisible(false);
  Order_Spinbox->setVisible(false);
  maxRipple_Label->setVisible(false);
  maxRipple_Spinbox->setVisible(false);
  Stub_Type_Label->setVisible(false);
  Stub_Type_Combo->setVisible(false);
  Stub_Implementation_Label->setVisible(false);
  Stub_Implementation_Combo->setVisible(false);
  Weighting_Type_Label->setVisible(false);
  Weighting_Type_Combo->setVisible(false);
  Network_Response_Label->setVisible(false);
  Network_Response_Combo->setVisible(false);
  QualityFactor_Label->setVisible(false);
  Quality_Factor_Spinbox->setVisible(false);
  CapacitorQ_Label->setVisible(false);
  CapacitorQ_Spinbox->setVisible(false);
  InductorQ_Label->setVisible(false);
  InductorQ_Spinbox->setVisible(false);
  L2_Double_Tapped_Resonator_Label->setVisible(false);
  L2_Double_Tapped_Resonator_Scale_Combo->setVisible(false);
  L2_Double_Tapped_Resonator_SpinBox->setVisible(false);
  k_Transformer_Label->setVisible(false);
  k_Transformer_Spinbox->setVisible(false);
  coupled_L_Label->setVisible(false);
  coupled_L_Combo->setVisible(false);
  BW_Label->setVisible(false);
  BW_Spinbox->setVisible(false);
  BW_Scale_Combo->setVisible(false);
  Lumped_QW_Label->setVisible(false);
  Lumped_QW_Combo->setVisible(false);
  Lumped_L8_Label->setVisible(false);
  Lumped_L8_Combo->setVisible(false);
  Lumped_TL_Label->setVisible(false);
  Lumped_TL_Combo->setVisible(false);

  switch (topology) {
  case SINGLESTUB:
  case DOUBLESTUB:
    Lumped_TL_Label->setVisible(true);
    Lumped_TL_Combo->setVisible(true);
    Stub_Type_Label->setVisible(true);
    Stub_Type_Combo->setVisible(true);
    Stub_Implementation_Label->setVisible(true);
    Stub_Implementation_Combo->setVisible(true);
    break;

  case MULTISTAGEL4:
    Order_Label->setVisible(true);
    Order_Spinbox->setVisible(true);
    maxRipple_Label->setVisible(true);
    maxRipple_Spinbox->setVisible(true);
    Weighting_Type_Label->setVisible(true);
    Weighting_Type_Combo->setVisible(true);
    Lumped_QW_Label->setVisible(true);
    Lumped_QW_Combo->setVisible(true);
    break;

  case CASCADEDLSECTIONS:
    CapacitorQ_Label->setVisible(true);
    CapacitorQ_Spinbox->setVisible(true);
    InductorQ_Label->setVisible(true);
    InductorQ_Spinbox->setVisible(true);
    Order_Label->setVisible(true);
    Order_Spinbox->setVisible(true);
    Network_Response_Label->setVisible(true);
    Network_Response_Combo->setVisible(true);
    break;

  case L8L4:
    Lumped_L8_Label->setVisible(true);
    Lumped_L8_Combo->setVisible(true);
  case QUARTER_WAVE_LINE:
    Lumped_QW_Label->setVisible(true);
    Lumped_QW_Combo->setVisible(true);
    break;

  case TEE_TYPE:
  case PI_TYPE:
    Network_Response_Label->setVisible(true);
    Network_Response_Combo->setVisible(true);
    QualityFactor_Label->setVisible(true);
    Quality_Factor_Spinbox->setVisible(true);
  case LSECTION:
    InductorQ_Label->setVisible(true);
    InductorQ_Spinbox->setVisible(true);
    CapacitorQ_Label->setVisible(true);
    CapacitorQ_Spinbox->setVisible(true);
    break;

  case SERIES_DOUBLE_TUNED_TRANSFORMER:
  case PARALLEL_DOUBLE_TUNED_TRANSFORMER:
    CapacitorQ_Label->setVisible(true);
    CapacitorQ_Spinbox->setVisible(true);
    coupled_L_Label->setVisible(true);
    coupled_L_Combo->setVisible(true);
    maxRipple_Label->setVisible(true);
    maxRipple_Spinbox->setVisible(true);
    BW_Label->setVisible(true);
    BW_Spinbox->setVisible(true);
    BW_Scale_Combo->setVisible(true);
    break;

  case SINGLE_TUNED_TRANSFORMER:
    k_Transformer_Label->setVisible(true);
    k_Transformer_Spinbox->setVisible(true);
    CapacitorQ_Label->setVisible(true);
    CapacitorQ_Spinbox->setVisible(true);
    coupled_L_Label->setVisible(true);
    coupled_L_Combo->setVisible(true);
    break;

  case DOUBLE_TAPPED:
    L2_Double_Tapped_Resonator_Label->setVisible(true);
    L2_Double_Tapped_Resonator_Scale_Combo->setVisible(true);
    L2_Double_Tapped_Resonator_SpinBox->setVisible(true);

  case TAPPED_L:
  case TAPPED_C:
    CapacitorQ_Label->setVisible(true);
    CapacitorQ_Spinbox->setVisible(true);
    InductorQ_Label->setVisible(true);
    InductorQ_Spinbox->setVisible(true);
    QualityFactor_Label->setVisible(true);
    Quality_Factor_Spinbox->setVisible(true);
    break;
  }

  OK_Button = new QPushButton(tr("OK"));
  Cancel_Button = new QPushButton(tr("Cancel"));
  MatchSettingslayout->addWidget(OK_Button, 16, 0);
  MatchSettingslayout->addWidget(Cancel_Button, 16, 1);
  connect(OK_Button, SIGNAL(clicked()), SLOT(slot_save_settings()));
  connect(Cancel_Button, SIGNAL(clicked()), SLOT(slot_cancel_settings()));

  setLayout(MatchSettingslayout);
  OK_Button->setFocus();
}

MatchSettingsDialog::~MatchSettingsDialog() {}
void MatchSettingsDialog::slot_save_settings() {
  params.BalancedStubs = Stub_Implementation_Combo->currentIndex() != 0;
  params.open_short = Stub_Type_Combo->currentIndex() ==
                      0; // Open stub or short circuit stub configuration
  params.order =
      Order_Spinbox->value() + 1; // Order of the multisection lambda/4 matching
  params.gamma_MAX =
      maxRipple_Spinbox->value(); // Maximum ripple (Chebyshev weighting only)
  params.Q =
      Quality_Factor_Spinbox->value(); // Quality factor of the overall network
  params.CAPQ = CapacitorQ_Spinbox->value(); // Capacitor quality factor
  params.INDQ = InductorQ_Spinbox->value();  // Inductor quality factor
  switch (Network_Response_Combo->currentIndex()) {
  case 0:
    params.network_response = LOWPASS;
    break;
  case 1:
    params.network_response = HIGHPASS;
    break;
  }

  params.weighting_type =
      Weighting_Type_Combo->currentIndex() == 0; // Chebyshev or binomial
  params.L2 = L2_Double_Tapped_Resonator_SpinBox->value() *
              getScale(L2_Double_Tapped_Resonator_Scale_Combo->currentText());
  params.k = k_Transformer_Spinbox->value();
  params.coupled_L_Equivalent = coupled_L_Combo->currentIndex();
  params.BW = BW_Spinbox->value() * getScale(BW_Scale_Combo->currentText());
  params.use_l4_lumped_equivalent = Lumped_QW_Combo->currentIndex();
  params.use_l8_lumped_equivalent = Lumped_L8_Combo->currentIndex();
  params.use_TL_lumped_equivalent = Lumped_TL_Combo->currentIndex();
  accept();
}

// This function calculates the scale factor for a given index
double MatchSettingsDialog::getScale(QString text) {
  // Get first character
  QChar preffix = text.at(0);
  if (preffix == 'm')
    return 1e-3;
  else if (preffix == 'u')
    return 1e-6;
  else if (preffix == 'n')
    return 1e-9;
  else if (preffix == 'p')
    return 1e-12;
  else if (preffix == 'f')
    return 1e-15;
  else if (preffix == 'k')
    return 1e3;
  else if (preffix == 'M')
    return 1e6;
  else if (preffix == 'G')
    return 1e9;
  else if (preffix == 'T')
    return 1e12;
  else
    return 1;
}

void MatchSettingsDialog::slot_cancel_settings() {
  params.order =
      -1; // Indicates that the main window must not update the settings
  accept();
}

struct ImplementationParams MatchSettingsDialog::GetOptions() {
  return params;
}
