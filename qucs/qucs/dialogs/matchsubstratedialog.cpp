/***************************************************************************
                                matchsubstratedialog.cpp
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

#include "matchsubstratedialog.h"
#include <QApplication>

/*
   This is a pop-up window to enter the substrate parameters
*/

MatchSubstrateDialog::MatchSubstrateDialog(struct tSubstrate subs,
                                           QWidget *parent)
    : QDialog(parent) {
  setWindowTitle(tr("Substrate parameters"));
  QGridLayout *SubstrateParameterslayout = new QGridLayout();
  double val;
  int index = 0;

  QStringList units;
  units.append("mm");
  units.append("mil");
  units.append("um");

  // Relative permittivity
  Relative_Permittivity_Label = new QLabel(tr("Relative permittivity"));
  Relative_Permittivity_Combo = new QComboBox();
  Relative_Permittivity_Combo->setEditable(true);
  const char **p = List_er;
  while (*(++p))
    Relative_Permittivity_Combo->addItem(
        *p); // The dielectric coeff combobox is filled with
             // the materials taken from
             // "../../qucs-filter/material_props.h"
  Relative_Permittivity_Combo->lineEdit()->setText(QString("%1").arg(subs.er));
  SubstrateParameterslayout->addWidget(Relative_Permittivity_Label, 0, 0);
  SubstrateParameterslayout->addWidget(Relative_Permittivity_Combo, 0, 1);

  // Substrate height
  Substrate_Height_Label = new QLabel(tr("Substrate height"));
  Substrate_Height_Spinbox = new QDoubleSpinBox();
  Substrate_Height_Spinbox->setMinimum(0.01);
  val = subs.height, getValueAndIndex(val, index);
  Substrate_Height_Spinbox->setValue(val);
  Substrate_Height_Spinbox->setSingleStep(0.01);
  Substrate_Height_Units = new QComboBox();
  Substrate_Height_Units->addItems(units);
  Substrate_Height_Units->setCurrentIndex(index);
  SubstrateParameterslayout->addWidget(Substrate_Height_Label, 1, 0);
  SubstrateParameterslayout->addWidget(Substrate_Height_Spinbox, 1, 1);
  SubstrateParameterslayout->addWidget(Substrate_Height_Units, 1, 2);

  // Metal thickness
  Thickness_Label = new QLabel(tr("Metal thickness"));
  Thickness_Spinbox = new QDoubleSpinBox();
  Thickness_Spinbox->setMinimum(0.01);
  val = subs.thickness, getValueAndIndex(val, index);
  Thickness_Spinbox->setValue(val);
  Thickness_Spinbox->setSingleStep(0.01);
  Thickness_Units = new QComboBox();
  Thickness_Units->addItems(units);
  Thickness_Units->setCurrentIndex(index);
  SubstrateParameterslayout->addWidget(Thickness_Label, 2, 0);
  SubstrateParameterslayout->addWidget(Thickness_Spinbox, 2, 1);
  SubstrateParameterslayout->addWidget(Thickness_Units, 2, 2);

  // Minimum width
  Minimum_Width_Label = new QLabel(tr("Minimum width"));
  Minimum_Width_Spinbox = new QDoubleSpinBox();
  Minimum_Width_Spinbox->setMinimum(0.01);
  val = subs.minWidth;
  getValueAndIndex(val, index);
  Minimum_Width_Spinbox->setValue(val);
  Minimum_Width_Spinbox->setSingleStep(0.01);
  Minimum_Width_Scale = new QComboBox();
  Minimum_Width_Scale->addItems(units);
  Minimum_Width_Scale->setCurrentIndex(index);
  SubstrateParameterslayout->addWidget(Minimum_Width_Label, 3, 0);
  SubstrateParameterslayout->addWidget(Minimum_Width_Spinbox, 3, 1);
  SubstrateParameterslayout->addWidget(Minimum_Width_Scale, 3, 2);

  // Maximum width
  Maximum_Width_Label = new QLabel(tr("Maximum width"));
  Maximum_Width_Spinbox = new QDoubleSpinBox();
  Maximum_Width_Spinbox->setMinimum(0.01);
  val = subs.maxWidth;
  getValueAndIndex(val, index);
  Maximum_Width_Spinbox->setValue(val);
  Maximum_Width_Spinbox->setSingleStep(0.01);
  Maximum_Width_Scale = new QComboBox();
  Maximum_Width_Scale->addItems(units);
  Maximum_Width_Scale->setCurrentIndex(index);
  SubstrateParameterslayout->addWidget(Maximum_Width_Label, 4, 0);
  SubstrateParameterslayout->addWidget(Maximum_Width_Spinbox, 4, 1);
  SubstrateParameterslayout->addWidget(Maximum_Width_Scale, 4, 2);

  // tanD
  tanD_Label = new QLabel(tr("tanD"));
  tanD_Spinbox = new QDoubleSpinBox();
  tanD_Spinbox->setDecimals(4);
  tanD_Spinbox->setMinimum(0.0001);
  tanD_Spinbox->setValue(subs.tand);
  tanD_Spinbox->setSingleStep(0.0001);
  SubstrateParameterslayout->addWidget(tanD_Label, 5, 0);
  SubstrateParameterslayout->addWidget(tanD_Spinbox, 5, 1);

  // Resistivity
  Resistivity_Label = new QLabel(tr("Resistivity"));
  Resistivity_Edit = new QLineEdit(QString("%1").arg(subs.resistivity));
  SubstrateParameterslayout->addWidget(Resistivity_Label, 6, 0);
  SubstrateParameterslayout->addWidget(Resistivity_Edit, 6, 1);

  // RMA roughness
  Roughness_Label = new QLabel(tr("Roughness"));
  Roughness_Spinbox = new QDoubleSpinBox();
  Roughness_Spinbox->setMinimum(0.01);
  val = subs.roughness;
  getValueAndIndex(val, index);
  Roughness_Spinbox->setValue(val);
  Roughness_Spinbox->setDecimals(2);
  Roughness_Spinbox->setSingleStep(0.01);
  Roughness_Scale = new QComboBox();
  Roughness_Scale->addItems(units);
  Roughness_Scale->setCurrentIndex(index);
  SubstrateParameterslayout->addWidget(Roughness_Label, 7, 0);
  SubstrateParameterslayout->addWidget(Roughness_Spinbox, 7, 1);
  SubstrateParameterslayout->addWidget(Roughness_Scale, 7, 2);

  OK_Button = new QPushButton(tr("OK"));
  Cancel_Button = new QPushButton(tr("Cancel"));
  SubstrateParameterslayout->addWidget(OK_Button, 8, 0);
  SubstrateParameterslayout->addWidget(Cancel_Button, 8, 1);
  connect(OK_Button, SIGNAL(clicked()), SLOT(slot_save_settings()));
  connect(Cancel_Button, SIGNAL(clicked()), SLOT(slot_cancel_settings()));

  setLayout(SubstrateParameterslayout);
}

MatchSubstrateDialog::~MatchSubstrateDialog() {}
void MatchSubstrateDialog::slot_save_settings() {
  substrate_parameters.er =
      Relative_Permittivity_Combo->currentText().split(" ").at(0).toDouble();
  substrate_parameters.height =
      Substrate_Height_Spinbox->value() *
      getScaleFactor(Substrate_Height_Units->currentIndex());
  substrate_parameters.maxWidth =
      Maximum_Width_Spinbox->value() *
      getScaleFactor(Maximum_Width_Scale->currentIndex());
  substrate_parameters.minWidth =
      Minimum_Width_Spinbox->value() *
      getScaleFactor(Minimum_Width_Scale->currentIndex());
  substrate_parameters.resistivity = Resistivity_Edit->text().toDouble();
  substrate_parameters.tand = tanD_Spinbox->value();
  substrate_parameters.roughness =
      Roughness_Spinbox->value() *
      getScaleFactor(Roughness_Scale->currentIndex());
  substrate_parameters.thickness =
      Thickness_Spinbox->value() *
      getScaleFactor(Thickness_Units->currentIndex());
  accept();
}
void MatchSubstrateDialog::slot_cancel_settings() {
  substrate_parameters.er =
      -1; // Indicates that the main window must not update the settings
  accept();
}

struct tSubstrate MatchSubstrateDialog::GetOptions() {
  return substrate_parameters;
}

// This function returns the scale factor given a combo index
double MatchSubstrateDialog::getScaleFactor(int index) {
  switch (index) {
  case 0: // mm
    return 1e-3;
    break;
  case 1: // mil
    return 2.54e-5;
    break;
  case 2: // um
    return 1e-6;
    break;
  }
  return 1e-3; // Never reaches this point, but it fixes "control reaches end of
               // non-void function" warning
}

void MatchSubstrateDialog::getValueAndIndex(double &val, int &index) {
  if (val < 1e-6) // Then use micron units
    val *= 1e6, index = 2;
  else // Use millimeters
    val *= 1e3, index = 0;
}
