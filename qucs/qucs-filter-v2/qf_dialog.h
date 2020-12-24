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
class QGroupBox;
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
 QPushButton * okButton{nullptr}, * cancelButton{nullptr};
 QLineEdit * EnterCutoff{nullptr}, * EnterZin{nullptr}, * EnterZout{nullptr}, * EnterStopband{nullptr},
  * EnterAngle{nullptr}, * EnterBandwidth{nullptr}, * EnterAttenuation{nullptr}, * EnterRipple{nullptr};
 QVBoxLayout * all{nullptr};
 QComboBox * CutoffCombo{nullptr}, * StopbandCombo{nullptr}, * FilterName{nullptr}, * TformName{nullptr},
   * OrderCombo{nullptr}, * SubOrderCombo{nullptr}, * BandwidthCombo{nullptr};
 QLabel * CutoffLabel{nullptr}, * ZinLabel{nullptr}, * OhmLabel{nullptr}, * ZoutLabel{nullptr}, * OhmLabel_2{nullptr},
   * StopbandLabel{nullptr}, * AngleLabel{nullptr}, * AngleUnitLabel{nullptr}, * BandwidthLabel{nullptr},
   * AttenuationLabel{nullptr}, * dBLabel{nullptr}, * RippleLabel{nullptr}, * RippleUnitLabel{nullptr};
 QCheckBox * OrderBox{nullptr}, * DualBox{nullptr}, * CauerPoleBox{nullptr}, * OptimizeCauerBox{nullptr},
   * EqualInductorBox{nullptr}, * UseCrossBox{nullptr};
 QRadioButton * Cmax{nullptr}, * Cmin{nullptr}, * NoC{nullptr}, * Lmin{nullptr}, * NoL{nullptr}, * Lmax{nullptr};
 QGroupBox * Lboxes{nullptr}, * Cboxes{nullptr};
};

#endif
