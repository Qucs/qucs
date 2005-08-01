/***************************************************************************
                              matchdialog.h
                             ---------------
    begin                : Fri Jul 22 2005
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

#ifndef MATCHDIALOG_H
#define MATCHDIALOG_H

#include <qdialog.h>
#include <qptrlist.h>

class Element;
class QLineEdit;
class QComboBox;
class QVBoxLayout;
class QDoubleValidator;


class MatchDialog : public QDialog {
   Q_OBJECT
public:
  MatchDialog(QWidget *parent=0, double Rreal_=0.5, double Rimag_=0.0,
		double Freq_=1e9, double Z0_=50.0);
 ~MatchDialog();

  static void r2z(double&, double&, double);
  static void z2r(double&, double&, double);
  static int  calcMatchingCircuit(double, double, double, double);

private slots:
  void slotButtCreate();
  void slotImpedanceChanged(const QString&);
  void slotReflexionChanged(const QString&);

private:
  QVBoxLayout *all;   // the mother of all widgets
  QDoubleValidator  *DoubleVal;
  QLineEdit   *ReferenceEdit, *FrequencyEdit, *ImpedanceRealEdit,
              *ImpedanceImagEdit, *ReflexionRealEdit, *ReflexionImagEdit;
  QComboBox   *UnitCombo;
};

#endif
