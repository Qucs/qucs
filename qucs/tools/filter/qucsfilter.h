/***************************************************************************
                                qucsfilter.h
                             -------------------
    begin                : Wed Mar 02 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCSFILTER_H
#define QUCSFILTER_H

#include <QMainWindow>
#include <QGroupBox>
#include <QLabel>
#include <QtSvg>

class QGridLayout;
class QComboBox;
class QLineEdit;
class QLabel;
class QIntValidator;
class QDoubleValidator;


struct tQucsSettings {
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;

enum FilterImplementation {
  LC_LADDER_PI,
  LC_LADDER_TEE,
  END_COUPLED,
  PARALLEL_COUPLED_TL,
  STEPPED_IMPEDANCE,
  QW_RESONATORS,
  QW_RING,
  CAP_COUPLED_SHUNT_RESONATORS,
  EQN_DEFINED
};

class QucsFilter : public QMainWindow
{
  Q_OBJECT
public:
  QucsFilter();
 ~QucsFilter();

private slots:
  void slotQuit();
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotCalculate();
  void slotTypeChanged(int);
  void slotClassChanged(int);
  void slotShowResult();
  void slotRealizationChanged(int);
  void slotTakeEr(const QString&);
  void FlushImage();
  void on_MicrostripcheckBox_clicked();

private:
  void setError(const QString&);
  QString * calculateFilter(struct tFilter *);

  int ResultState;

  QGridLayout *all{nullptr}, *gbox1{nullptr}, *gbox2{nullptr}, *gboxImage{nullptr};
  QGroupBox *box1{nullptr}, *box2{nullptr}, *boxImage{nullptr};
  QComboBox *ComboRealize{nullptr}, *ComboType{nullptr}, *ComboClass{nullptr}, *ComboCorner{nullptr}, *ComboStop{nullptr}, *ComboBandStop{nullptr}, *ComboEr;
  QLineEdit *EditOrder{nullptr}, *EditCorner{nullptr}, *EditStop{nullptr}, *EditRipple{nullptr}, *EditImpedance{nullptr}, *EditThickness{nullptr}, *EditHeight{nullptr}, *EditMinWidth{nullptr}, *EditMaxWidth{nullptr};
  QLineEdit *EditAtten{nullptr}, *EditBandStop{nullptr};
  QLabel *LabelRipple{nullptr}, *LabelRipple_dB{nullptr}, *LabelStart{nullptr}, *LabelStop{nullptr}, *LabelResult{nullptr};
  QLabel *LabelAtten{nullptr}, *LabelAtten_dB{nullptr}, *LabelBandStop{nullptr}, *LabelOrder{nullptr}, *LabelImpedance{nullptr}, *LabelOhm{nullptr};
  QIntValidator *IntVal{nullptr};
  QDoubleValidator *DoubleVal{nullptr};
  QSvgWidget *imgLayout{nullptr};
  QCheckBox * MicrostripcheckBox{nullptr};
};

#endif
