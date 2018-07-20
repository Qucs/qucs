/***************************************************************************
                              qf_dialog.h
                             -------------
    begin                : Fri May 1 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QLineEdit;
class QComboBox;
class QLabel;
class QCheckBox;
class QRadioButton;
class QVButtonGroup;
class QPushButton;
class QWidget;

class FilterDialog : public QDialog
{
  Q_OBJECT
public:
  FilterDialog(QWidget *);
 ~FilterDialog();

private slots:
 void slotHelpAbout();
 void slotHelpAboutQt();
 void slotQuit();

private:

public:
 QPushButton * okButton, * cancelButton; 
 QLineEdit * EnterCutoff, * EnterZin, * EnterZout, * EnterStopband,
  * EnterAngle, * EnterBandwidth, * EnterAttenuation, * EnterRipple;
 QVBoxLayout * all;
 QComboBox * CutoffCombo, * StopbandCombo, * FilterName, * TformName,
   * OrderCombo, * SubOrderCombo, * BandwidthCombo;
 QLabel * CutoffLabel, * ZinLabel, * OhmLabel, * ZoutLabel, * OhmLabel_2,
   * StopbandLabel, * AngleLabel, * AngleUnitLabel, * BandwidthLabel,
   * AttenuationLabel, * dBLabel, * RippleLabel, * RippleUnitLabel;
 QCheckBox * OrderBox, * DualBox, * CauerPoleBox, * OptimizeCauerBox,
   * EqualInductorBox, * UseCrossBox;
 QRadioButton * Cmax, * Cmin, * NoC, * Lmin, * NoL, * Lmax;
 QVButtonGroup * Lboxes, * Cboxes;
};

#endif
