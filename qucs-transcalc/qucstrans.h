/***************************************************************************
                          qucstrans.h  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2005 by Stefan Jahn
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
class QVBoxLayout;
class QRadioButton;

class transline;

// Current limit defintions.
#define MAX_TRANS_BOXES   4
#define MAX_TRANS_TYPES   4
#define MAX_TRANS_RESULTS 7

// Types of transmission line properties.
#define TRANS_SUBSTRATE  0
#define TRANS_COMPONENT  1
#define TRANS_PHYSICAL   2
#define TRANS_ELECTRICAL 3

// Application settings.
struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;          // font
  QString BitmapDir;   // pixmap directory
  QString LangDir;     // translation directory
  int length_unit;     // default length unit
  int freq_unit;       // default frequency unit
  int res_unit;        // default resistance unit
  int ang_unit;        // default angle unit
};

extern tQucsSettings QucsSettings;

// Transmission line value.
struct TransValue {
  char * name;           // label
  double value;          // value
  QString * tip;         // tool tip description
  char * units[8];       // unit choise
  QLabel * label;        // Qt label widget
  QLineEdit * lineedit;  // Qt value widget
  QComboBox * combobox;  // Qt unit widget
  QRadioButton * radio;  // Qt fixed widget
};

// Array of transmission line values.
struct TransArray {
  struct TransValue item[12];
};

// Extraneous calculation results.
struct TransResult {
  QString * name; // textual description
  QLabel * label; // Qt name widget
  QLabel * value; // Qt value widget
};

// Transmission line types.
enum TransMode {
  ModeMicrostrip,
  ModeCoupledMicrostrip,
  ModeRectangular,
  ModeCoaxial,
  ModeNone
};

// A transmission line type structure.
struct TransType {
  int type;           // type of transmission line (see TransMode)
  char * description; // description
  transline * line;   // transmission line instance
  struct TransArray array[MAX_TRANS_BOXES];
  int results;        // number of extraneous results
  struct TransResult result[MAX_TRANS_RESULTS];
  int radio[4];
};

/**
  *@author Stefan Jahn
  */

class QucsTranscalc : public QDialog  {
   Q_OBJECT
public:
  QucsTranscalc();
 ~QucsTranscalc();

  void   setProperty (QString, double);
  double getProperty (QString);
  char * getUnit (QString);
  void   setResult (int, char *);
  bool   isSelected (QString);

private slots:
  void slotAbout();
  void slotQuit();
  void slotSelectType(int);
  void slotSynthesize();
  void slotAnalyze();

private:
  void updateSelection ();
  void createPropItem (QVBox *, TransValue *, int);
  void createResultItem (QVBox *, TransResult *);
  void updateResultItem (TransResult *);
  void createResultItems (QVBox *);
  void updateResultItems ();
  QVBox * createScrollable (QWidget *);
  void createPropItems (QVBox *, int);
  int getTypeIndex ();
  void updatePropItem (TransValue *);
  void setMode (int);
  struct TransValue * findProperty (QString);
  void setupTranslations ();

 private:
  void closeEvent (QCloseEvent*);

private:
  QVGroupBox * screen;
  QLabel * pix;
  QComboBox * tranType;
  QVBox * result;
  int mode;
};
  
#endif /* QUCSTRANS_H */
