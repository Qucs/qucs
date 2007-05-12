/***************************************************************************
                                displaydialog.h
                               -----------------
    begin                : Sat May 28 2005
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

#ifndef DISPLAYDIALOG_H
#define DISPLAYDIALOG_H

#include <qdialog.h>

class QTextEdit;
class QVBoxLayout;


class DisplayDialog : public QDialog  {
   Q_OBJECT
public:
  DisplayDialog(QWidget *parent = 0);
 ~DisplayDialog();

  QTextEdit *Text, *VHDLText, *VerilogText;

private slots:
  void slotClose();

private:
  QVBoxLayout *vLayout;
};

#endif
