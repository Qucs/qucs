/***************************************************************************
                          filldialog.h  -  description
                             -------------------
    begin                : Thu May 20 2004
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

#ifndef FILLDIALOG_H
#define FILLDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>


/**
  *@author Michael Margraf
  */

class FillDialog : public QDialog  {
Q_OBJECT
public:
  FillDialog(const QString& _Caption, bool show=true, QWidget *parent=0);
  ~FillDialog();

  void SetComboBox(Qt::PenStyle _Style);
  void SetFillComboBox(Qt::BrushStyle _Style);

public slots:
  void slotCheckFilled(bool on);

private slots:
  void slotSetColor();
  void slotSetFillColor();
  void slotSetStyle(int index);
  void slotSetFillStyle(int index);

public:
  QLabel        *FillLabel1, *FillLabel2;
  QCheckBox     *CheckFilled;
  QLineEdit     *LineWidth;
  QPushButton   *ColorButt, *FillColorButt;
  QComboBox     *StyleBox, *FillStyleBox;
  Qt::PenStyle   LineStyle;
  Qt::BrushStyle FillStyle;
};

#endif
