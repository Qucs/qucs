/***************************************************************************
                              importdialog.h
                             ----------------
    begin                : Fri Jun 23 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qdialog.h>
#include <qprocess.h>

class QTextEdit;
class QLineEdit;
class QGridLayout;
class QPushButton;


class ImportDialog : public QDialog  {
   Q_OBJECT
public:
  ImportDialog(QWidget*);
 ~ImportDialog();

private slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotProcessEnded();
  void slotImport();
  void slotAbort();
  void slotBrowse();

private:
  void startSimulator();

public:
  QGridLayout *all;

  QProcess Process;
  QTextEdit *MsgText;
  QLineEdit *ImportEdit, *OutputEdit;
  QPushButton *ImportButt, *CancelButt, *AbortButt;
};

#endif
