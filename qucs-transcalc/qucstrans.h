/***************************************************************************
                          qucstrans.h  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2004 by Stefan Jahn
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

#ifndef QUCSTRANS_H
#define QUCSTRANS_H

#include <qdialog.h>
#include <qfont.h>
#include <qstring.h>

class QComboBox;
class QLineEdit;
class QLabel;
class QVGroupBox;
class QVBox;

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  QString BitmapDir;
  QString LangDir;
};

extern tQucsSettings QucsSettings;

struct TransValue {
  char * name;   // label
  double value;  // value
  char * units[6]; // unit combobox
  QLabel * label;
  QLineEdit * lineedit;
  QComboBox * combobox;
};

struct TransArray {
  struct TransValue item[12];
};

struct TransType {
  int type;
  char * description;
  struct TransArray array[4];
};

enum TransMode {
  ModeMicrostrip,
  ModeCoupledMicrostrip,
  ModeRectangular,
  ModeCoaxial,
  ModeNone
};

/**
  *@author Stefan Jahn
  */

class QucsTranscalc : public QDialog  {
   Q_OBJECT
public:
  QucsTranscalc();
 ~QucsTranscalc();

  void setProperty(QString, double);
  double getProperty(QString);

private slots:
  void slotAbout();
  void slotQuit();
  void slotSelectType(int);
  void slotSynthesize();
  void slotAnalyze();

private:
  void closeEvent(QCloseEvent*);
  void addPropItem(QWidget *, TransValue *);
  QVBox * createScrollable(QWidget *);
  void createPropItems(QWidget *, int);
  int getTypeIndex();
  void changePropItem(TransValue *);
  void setMode(int);
  struct TransValue * findProperty(QString);

  QVGroupBox * screen;
  QLabel * pix;
  QComboBox * tranType;
  int mode;
};
  
#endif
