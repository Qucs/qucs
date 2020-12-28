//-*- C++ -*-
/****************************************************************************
**     Qucs Attenuator Synthesis
**     qucsattenuator.h
**
**
**
**
**
**
**
*****************************************************************************/

#ifndef QUCSATTENUATOR_H
#define QUCSATTENUATOR_H

#include <QMainWindow>
#include <QLabel>

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

 private slots:
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotTopologyChanged();
  void slotCalculate();
  void slotQuit();
  void slotSetText_Zin(const QString &);
  void slotSetText_Zout(const QString &);

 private:
  QComboBox *ComboTopology{nullptr};
  QLabel *LabelTopology{nullptr}, *LabelAtten{nullptr}, *LabelImp1{nullptr}, *LabelImp2{nullptr};
  QLabel *LabelR1{nullptr}, *LabelR2{nullptr}, *LabelR3{nullptr}, *pixTopology{nullptr}, *LabelResult{nullptr};
  QLabel *LabelR3_Ohm{nullptr};
  QLineEdit *lineEdit_Attvalue{nullptr}, *lineEdit_Zin{nullptr}, *lineEdit_Zout{nullptr};
  QLineEdit *lineEdit_R1{nullptr}, *lineEdit_R2{nullptr}, *lineEdit_R3{nullptr}, *lineEdit_Results{nullptr};
  QPushButton *Calculate{nullptr};
  QDoubleValidator *DoubleVal{nullptr};

};

#endif
