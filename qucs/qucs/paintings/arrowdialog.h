/***************************************************************************
                          arrowdialog.h  -  description
                             -------------------
    begin                : Fri Nov 28 2003
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

#ifndef ARROWDIALOG_H
#define ARROWDIALOG_H

#include <qdialog.h>

class QLineEdit;
class QVBoxLayout;
class QPushButton;
class QComboBox;
class QIntValidator;

/**
  *@author Michael Margraf
  */

class ArrowDialog : public QDialog  {
Q_OBJECT
public:
  ArrowDialog(QWidget *parent=0, const char *name=0);
  ~ArrowDialog();

  void SetComboBox(Qt::PenStyle _Style);

private slots:
  void slotSetColor();
  void slotSetStyle(int index);

public:
  QLineEdit    *LineWidth, *HeadWidth, *HeadLength;
  QPushButton  *ColorButt;
  QComboBox    *StyleBox;
  Qt::PenStyle LineStyle;

  QVBoxLayout *vert;
  QIntValidator *val100;
};

#endif
