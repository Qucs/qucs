/***************************************************************************
                               MatchSettingsDialog.cpp
                              -----------------
    begin                : Nov 2018
    copyright            : (C) 2018 by Andres Martinez-Mera, The Qucs Team
    email                : andresmartinezmera@gmail.com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

MatchSettingsDialog::MatchSettingsDialog(QWidget *parent, int topology) : QDialog(parent) {
  setWindowTitle(tr("Matching circuit settings"));
  QGridLayout *MatchSettingslayout = new QGridLayout();

  //Order
  Order_Label = new QLabel(tr("Order"));
  Order_Spinbox = new QSpinBox();
  Order_Spinbox->setValue(3);
  MatchSettingslayout->addWidget(Order_Label, 0, 0);
  MatchSettingslayout->addWidget(Order_Spinbox, 0,1);

  //Maximum ripple
  maxRipple_Label = new QLabel(tr("Max. ripple"));
  maxRipple_Spinbox = new QDoubleSpinBox();
  maxRipple_Spinbox->setMinimum(0.05);
  MatchSettingslayout->addWidget(maxRipple_Label, 1, 0);
  MatchSettingslayout->addWidget(maxRipple_Spinbox, 1, 1);

  //Stub type
  Stub_Type_Label = new QLabel(tr("Stub type"));
  Stub_Type_Combo = new QComboBox();
  Stub_Type_Combo->addItem(tr("Open"));
  Stub_Type_Combo->addItem(tr("Short"));
  MatchSettingslayout->addWidget(Stub_Type_Label, 2, 0);
  MatchSettingslayout->addWidget(Stub_Type_Combo, 2, 1);

  //Stub implementation (balanced or unbalanced)
  Stub_Implementation_Label = new QLabel(tr("Stub implementation"));
  Stub_Implementation_Combo = new QComboBox();
  Stub_Implementation_Combo->addItem(tr("Unbalanced"));
  Stub_Implementation_Combo->addItem(tr("Balanced"));
  MatchSettingslayout->addWidget(Stub_Implementation_Label, 3, 0);
  MatchSettingslayout->addWidget(Stub_Implementation_Combo, 3, 1);

  //Weighting type
  Weighting_Type_Label = new QLabel(tr("Weighting"));
  Weighting_Type_Combo = new QComboBox();
  Weighting_Type_Combo->addItem(tr("Chebyshev"));
  Weighting_Type_Combo->addItem(tr("Binomial"));
  MatchSettingslayout->addWidget(Weighting_Type_Label, 4, 0);
  MatchSettingslayout->addWidget(Weighting_Type_Combo, 4, 1);

  //Network response
  Network_Response_Label = new QLabel(tr("Network response"));
  Network_Response_Combo = new QComboBox();
  Network_Response_Combo->addItem(tr("Lowpass"));
  Network_Response_Combo->addItem(tr("Highpass"));
  MatchSettingslayout->addWidget(Network_Response_Label, 5, 0);
  MatchSettingslayout->addWidget(Network_Response_Combo, 5, 1);

  //Quality factor
  QualityFactor_Label = new QLabel(tr("Quality factor"));
  Quality_Factor_Spinbox = new QDoubleSpinBox();
  Quality_Factor_Spinbox->setMinimum(1);
  Quality_Factor_Spinbox->setValue(5);
  Quality_Factor_Spinbox->setDecimals(1);
  Quality_Factor_Spinbox->setSingleStep(0.5);
  MatchSettingslayout->addWidget(QualityFactor_Label, 6, 0);
  MatchSettingslayout->addWidget(Quality_Factor_Spinbox, 6, 1);

  //Capacitor Q
  CapacitorQ_Label =  new QLabel(tr("Capacitor Q"));
  CapacitorQ_Spinbox = new QDoubleSpinBox();
  CapacitorQ_Spinbox->setMinimum(0.5);
  CapacitorQ_Spinbox->setMaximum(1000);
  CapacitorQ_Spinbox->setValue(1000);
  CapacitorQ_Spinbox->setSingleStep(0.5);
  CapacitorQ_Spinbox->setDecimals(1);
  MatchSettingslayout->addWidget(CapacitorQ_Label, 7, 0);
  MatchSettingslayout->addWidget(CapacitorQ_Spinbox, 7, 1);

  //Inductor Q
  InductorQ_Label =  new QLabel(tr("Inductor Q"));
  InductorQ_Spinbox = new QDoubleSpinBox();
  InductorQ_Spinbox->setMinimum(0.5);
  InductorQ_Spinbox->setMaximum(1000);
  InductorQ_Spinbox->setValue(1000);
  InductorQ_Spinbox->setSingleStep(0.5);
  InductorQ_Spinbox->setDecimals(1);
  MatchSettingslayout->addWidget(InductorQ_Label, 8, 0);
  MatchSettingslayout->addWidget(InductorQ_Spinbox, 8, 1);


  //Default settings
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

  switch (topology) {
  case LSECTION:
       CapacitorQ_Label->setVisible(true);
       CapacitorQ_Spinbox->setVisible(true);
       InductorQ_Label->setVisible(true);
       InductorQ_Spinbox->setVisible(true);
       break;

  case SINGLESTUB:
  case DOUBLESTUB:
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
       break;

  case CASCADEDLSECTIONS:
       CapacitorQ_Label->setVisible(true);
       CapacitorQ_Spinbox->setVisible(true);
       InductorQ_Label->setVisible(true);
       InductorQ_Spinbox->setVisible(true);
       Order_Label->setVisible(true);
       Order_Spinbox->setVisible(true);
       break;
  case L8L4:
       break;

  case TEE_TYPE:
  case PI_TYPE:
       CapacitorQ_Label->setVisible(true);
       CapacitorQ_Spinbox->setVisible(true);
       InductorQ_Label->setVisible(true);
       InductorQ_Spinbox->setVisible(true);
       Network_Response_Label->setVisible(true);
       Network_Response_Combo->setVisible(true);
       QualityFactor_Label->setVisible(true);
       Quality_Factor_Spinbox->setVisible(true);
       break;

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
  MatchSettingslayout->addWidget(OK_Button, 9, 0);
  MatchSettingslayout->addWidget(Cancel_Button, 9, 1);
  connect(OK_Button, SIGNAL(clicked()), SLOT(slot_save_settings()));
  connect(Cancel_Button, SIGNAL(clicked()), SLOT(slot_cancel_settings()));

  setLayout(MatchSettingslayout);
  OK_Button->setFocus();
}

MatchSettingsDialog::~MatchSettingsDialog() {
}
void MatchSettingsDialog::slot_save_settings(){
    params.BalancedStubs = Stub_Implementation_Combo->currentIndex() == 0;
    params.open_short = Stub_Type_Combo->currentIndex() == 0; // Open stub or short circuit stub configuration
    params.order = Order_Spinbox->value() + 1; // Order of the multisection lambda/4 matching
    params.gamma_MAX = maxRipple_Spinbox->value(); // Maximum ripple (Chebyshev weighting only)
    params.Q = Quality_Factor_Spinbox->value(); // Quality factor of the overall network
    params.CAPQ = CapacitorQ_Spinbox->value();// Capacitor quality factor
    params.INDQ = InductorQ_Spinbox->value();// Inductor quality factor
    switch (Network_Response_Combo->currentIndex()) {
    case 0:
        params.network_response = LOWPASS;
        break;
    case 1:
        params.network_response = HIGHPASS;
        break;
    }

    params.weighting_type = Weighting_Type_Combo->currentIndex() == 0; //Chebyshev or binomial
    accept();

}
void MatchSettingsDialog::slot_cancel_settings(){
    params.order = -1;//Indicates that the main window must not update the settings
    accept();
}

struct ImplementationParams MatchSettingsDialog::GetOptions()
{
    return params;
}
