/***************************************************************************
                               arrowdialog.h
                              ---------------
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

#include <QDialog>

class QLineEdit;
class QGridLayout;
class QPushButton;
class QComboBox;
class QIntValidator;


class ArrowDialog : public QDialog  {
Q_OBJECT
public:
  ArrowDialog(QWidget *parent=0);
 ~ArrowDialog();

  void SetComboBox(Qt::PenStyle);

private slots:
  void slotSetColor();
  void slotSetStyle(int);

public:
  // TODO: I'm not sure whether declaring all this as public is really a good idea :-(
  // data encapsulation => make this stuff private & provide getter & setter methods...
  QLineEdit    *LineWidth{nullptr}, *HeadWidth{nullptr}, *HeadLength{nullptr};
  QPushButton  *ColorButt{nullptr};
  QComboBox    *StyleBox{nullptr}, *ArrowStyleBox{nullptr};
  Qt::PenStyle LineStyle;

  QGridLayout   *all{nullptr};
  QIntValidator *val100{nullptr};
};

#endif
