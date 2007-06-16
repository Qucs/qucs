/***************************************************************************
                             librarydialog.h
                            -----------------
    begin                : Sun Jun 04 2006
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

#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <qregexp.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qdialog.h>
#include <qfile.h>
#include <qdir.h>
#include <qcheckbox.h>

class QLabel;
class QucsApp;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QVBoxLayout;
class QListViewItem;
class QVButtonGroup;
class QRegExpValidator;


class LibraryDialog : public QDialog {
   Q_OBJECT
public:
  LibraryDialog(QucsApp*, QListViewItem*);
 ~LibraryDialog();

private slots:
  void slotCreate();
  void slotNext();
  void slotSelectNone();
  void slotSelectAll();

private:
  void intoStream(QTextStream&, QString&, const char*);
  int intoFile(QString&, QString&,  QStringList&);

private:
  QVBoxLayout *all;   // the mother of all widgets
  QLabel *theLabel;
  QLineEdit *NameEdit;
  QTextEdit *ErrText;
  QVButtonGroup *Group;
  QPushButton *ButtCreate, *ButtCancel, *ButtSelectAll, *ButtSelectNone;
  QPtrList<QCheckBox> BoxList;
  QStringList Descriptions;

  QucsApp *App;
  QFile LibFile;
  QDir LibDir;
  QRegExp Expr;
  QRegExpValidator *Validator;
};

#endif
