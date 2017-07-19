/***************************************************************************
                               markerdialog.h
                              ----------------
    begin                : Wed April 21 2004
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef MARKERDIALOG_H
#define MARKERDIALOG_H
#include "marker.h"
#include <QDialog>
#include <QDebug>
#include <QLabel>

class QLineEdit;
class QComboBox;
class QCheckBox;

class MarkerDialog : public QDialog  {
Q_OBJECT
public:
  MarkerDialog(Marker *pm_, QWidget *parent=0);
 ~MarkerDialog();

private slots:
  void slotAcceptValues();
  void slotSetColor();
  void slotDeltaMode();
public:
  Marker *pMarker;

  QComboBox  *NumberBox, *RefMarkerComboBox;
  QLineEdit  *Precision, *MarkerLineWidth;
  QLineEdit  *SourceImpedance, *MarkerID;
  QCheckBox  *TransBox, *DeltaModeCheckBox;
  QPushButton *ColorButton;
  QColor MarkerColor;
  QLabel * ColorLabel;
  QWidget *ZYSelectBox;
  QCheckBox *ZCheckBox, *YCheckBox;

private:
  QMap<QString, struct MarkerData> ActiveMarkers;
};

#endif
