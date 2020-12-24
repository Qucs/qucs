/*
 * librarydialog.h - declaration of dialog to create library
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QRegExp>

#include <QList>
#include <QStringList>
#include <QTextStream>
#include <QDialog>
#include <QFile>
#include <QDir>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>

class QLabel;
class QLineEdit;
class QTextEdit;
class QPlainTextEdit;
class QPushButton;
class QVBoxLayout;
class QTreeWidgetItem;
class QGroupBox;
class QRegExpValidator;
class QStackedWidget;
class QStringList;


class LibraryDialog : public QDialog {
   Q_OBJECT
public:
  LibraryDialog(QWidget *);
 ~LibraryDialog();

  void fillSchematicList(QStringList);

private slots:
  void slotCreateNext();
  void slotSave();
  void slotSelectNone();
  void slotSelectAll();
  void slotCheckDescrChanged(int);
  void slotPrevDescr();
  void slotNextDescr();
  void slotUpdateDescription();

private:
  void intoStream(QTextStream&, QString&, const char*);
  int intoFile(QString&, QString&,  QStringList&);

private:
  int curDescr;
  QVBoxLayout *all{nullptr};   // the mother of all widgets
  QVBoxLayout *checkBoxLayout{nullptr};
  QStackedWidget *stackedWidgets{nullptr};
  QLabel *theLabel{nullptr};
  QLabel *checkedCktName{nullptr};
  QLabel *libSaveName{nullptr};
  QLineEdit *NameEdit{nullptr};
  QPlainTextEdit *ErrText{nullptr};
  QTextEdit *textDescr{nullptr};
  QGroupBox *Group{nullptr};
  QPushButton *ButtCreateNext{nullptr}, *ButtCancel{nullptr}, *ButtSelectAll{nullptr}, *ButtSelectNone{nullptr};
  QPushButton *prevButt{nullptr}, *nextButt{nullptr};
  QPushButton *createButt{nullptr};
  QList<QCheckBox *> BoxList;
  QStringList SelectedNames;
  QStringList Descriptions;
  QCheckBox *checkDescr{nullptr};

  QFile LibFile;
  QDir LibDir;
  QRegExp Expr;
  QRegExpValidator *Validator{nullptr};
};

#endif
