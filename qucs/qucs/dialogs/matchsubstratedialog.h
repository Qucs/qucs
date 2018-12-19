/***************************************************************************
                                matchsubstratedialog.h
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

#ifndef MATCHSUBSTRATEDIALOG_H
#define MATCHSUBSTRATEDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "../../qucs-filter/material_props.h"

struct tSubstrate {
  double er = 4.7;
  double height = 1.5e-3;
  double thickness = 36e-6;
  double tand = 0.0125;
  double resistivity = 2.43902e-08;
  double roughness;
  double minWidth = 0.4e-3, maxWidth = 5.0;
};

class MatchSubstrateDialog : public QDialog {
  Q_OBJECT
public:
  MatchSubstrateDialog(struct tSubstrate, QWidget *parent = 0);
  ~MatchSubstrateDialog();
  struct tSubstrate GetOptions();

private:
  struct tSubstrate substrate_parameters;
  QLabel *Relative_Permittivity_Label, *Substrate_Height_Label,
      *Thickness_Label, *Minimum_Width_Label, *Maximum_Width_Label, *tanD_Label,
      *Resistivity_Label, *Roughness_Label;
  QDoubleSpinBox *Thickness_Spinbox, *Substrate_Height_Spinbox,
      *Minimum_Width_Spinbox, *Maximum_Width_Spinbox, *tanD_Spinbox,
      *Roughness_Spinbox;
  QComboBox *Relative_Permittivity_Combo, *Thickness_Units,
      *Substrate_Height_Units, *Minimum_Width_Scale, *Maximum_Width_Scale,
      *Roughness_Scale;
  QPushButton *OK_Button, *Cancel_Button;
  QLineEdit *Resistivity_Edit;
  double getScaleFactor(int);
  void getValueAndIndex(double &, int &);

public slots:
  void slot_save_settings();
  void slot_cancel_settings();
};
#endif
