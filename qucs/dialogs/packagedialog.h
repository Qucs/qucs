/***************************************************************************
                             packagedialog.h
                            -----------------
    begin                : Sun Jun 25 2006
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

#ifndef PACKAGEDIALOG_H
#define PACKAGEDIALOG_H

#include <qfile.h>
#include <qdialog.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qcheckbox.h>

class QucsApp;
class QString;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QVBoxLayout;
class QDataStream;
class QPushButton;
class QListViewItem;
class QVButtonGroup;


class PackageDialog : public QDialog {
   Q_OBJECT
public:
  PackageDialog(QWidget*, bool);
 ~PackageDialog();

  void extractPackage();

private slots:
  void slotCreate();
  void slotBrowse();

private:
  int insertFile(const QString&, QFile&, QDataStream&);
  int insertDirectory(const QString&, QDataStream&);
  int insertLibraries(QDataStream&);

  int extractFile(QFile&, Q_UINT32, QDir&);
  int extractDirectory(QFile&, Q_UINT32, QDir&);
  int extractLibrary(QFile&, Q_UINT32);

  QVBoxLayout *all;   // the mother of all widgets
  QLineEdit *NameEdit;
  QTextEdit *MsgText;
  QCheckBox *LibraryCheck;
  QVButtonGroup *Group;
  QPtrList<QCheckBox> BoxList;

  QPushButton *ButtClose;
};

#endif
