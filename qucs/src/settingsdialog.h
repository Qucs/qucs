/***************************************************************************
                              settingsdialog.h
                             ------------------
    begin                : Mon Oct 20 2003
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

class SchematicDoc;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QComboBox;
class QVBoxLayout;
class QRegExpValidator;
class QTreeView;
class QFileSystemModel;
class QModelIndex;

class SettingsDialog : public QDialog  {
   Q_OBJECT
public:
  SettingsDialog(SchematicDoc*);
 ~SettingsDialog();

private slots:
  void slotDataSetBrowse();
  void slotDataDisplayBrowse();
  void slotScriptBrowse();
  void slotOK();
  void slotApply();

public:
  SchematicDoc *Doc{nullptr};

  QComboBox *Combo_Frame{nullptr};
  QTextEdit *Input_Frame0{nullptr};
  QLineEdit *Input_Frame1{nullptr}, *Input_Frame2{nullptr}, *Input_Frame3{nullptr};
  QLineEdit *Input_DataSet{nullptr}, *Input_DataDisplay{nullptr}, *Input_Script{nullptr};
  QLineEdit *Input_GridX{nullptr}, *Input_GridY{nullptr};
  QCheckBox *Check_OpenDpl{nullptr}, *Check_GridOn{nullptr}, *Check_RunScript{nullptr};

  QVBoxLayout *all{nullptr};
  QRegExpValidator *valExpr{nullptr};
};

class AuxFilesDialog : public QDialog {
   Q_OBJECT
public:
  AuxFilesDialog(QWidget *, const QString &);

  QString fileName;

private slots:
  void slotSelect();
  void slotDoubleClick(const QModelIndex &);

private:
  QFileSystemModel *model{nullptr};
  QTreeView *tree{nullptr};
};

#endif
