/*
 * qucsattenuator.h - Attenuator synthesis tool GUI header
 *
 * Copyright (C) 2018 Qucs Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */


#ifndef QUCSATTENUATOR_H
#define QUCSATTENUATOR_H

#include <QMainWindow>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>

#include "attenuatorfunc.h"

class QComboBox;
class QLineEdit;
class QIntValidator;
class QDoubleValidator;
class QLabel;
class QPushButton;

struct tQucsSettings
{
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;

class QucsAttenuator : public QMainWindow
{
 Q_OBJECT
 public:
  QucsAttenuator();
  ~QucsAttenuator();
  double ConvertPowerUnits(double, QString, QString);

 private slots:
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotTopologyChanged();
  void slotCalculate();
  void slotQuit();
  void slotSetText_Zin(double);
  void slotSetText_Zout(double);
  void slot_ComboInputPowerUnits_Changed(const QString&);
  void slot_ComboR1PowerUnits_Changed(const QString&);
  void slot_ComboR2PowerUnits_Changed(const QString&);
  void slot_ComboR3PowerUnits_Changed(const QString&);
  void slot_ComboR4PowerUnits_Changed(const QString&);

 private:
  QComboBox *ComboTopology;
  QLabel *LabelTopology, *LabelAtten, *LabelImp1, *LabelImp2, *LabelImp2_Ohm;
  QLabel *LabelR1, *LabelR2, *LabelR3, *LabelR4, *pixTopology, *LabelResult;
  QLabel *LabelR3_Ohm, *LabelR4_Ohm;
  QComboBox *ComboR1_PowerUnits, *ComboR2_PowerUnits, *ComboR3_PowerUnits, *ComboR4_PowerUnits, *Combo_InputPowerUnits;
  QLabel *PdissLabel, *Label_Pin;
  QLineEdit *lineEdit_R1, *lineEdit_R2, *lineEdit_R3, *lineEdit_R4, *lineEdit_Results;
  QLineEdit *lineEdit_R1_Pdiss, *lineEdit_R2_Pdiss, *lineEdit_R3_Pdiss, *lineEdit_R4_Pdiss;
  QDoubleSpinBox *QSpinBox_InputPower, *QSpinBox_Attvalue, *QSpinBox_Zin, *QSpinBox_Zout;
  QPushButton *Calculate;
  QDoubleValidator *DoubleVal, *DoubleValPower;
  QCheckBox *SparBoxCheckbox, *minR_Reflection_Att;
  QStringList LastUnits;

  //Quarter-wave attenuators
  QLabel *Label_Freq;
  QComboBox *Combo_FreqUnits;
  QDoubleSpinBox *QSpinBox_Freq;
  QCheckBox *Check_QW_CLC;
};

#endif
