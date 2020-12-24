/***************************************************************************
                           qucssettingsdialog.h
                          ----------------------
    begin                : Sun May 23 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    copyright            : (C) 2016 by Qucs Team (see AUTHORS file)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCSSETTINGSDIALOG_H
#define QUCSSETTINGSDIALOG_H

#include "qucs.h"

#include <QDialog>
#include <QFont>
#include <QRegExp>
#include <QVBoxLayout>

class QLineEdit;
class QCheckBox;
class QVBoxLayout;
class QPushButton;
class QComboBox;
class QIntValidator;
class QRegExpValidator;
class QStandardItemModel;
class QTableWidget;

class QucsSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    QucsSettingsDialog(QucsApp *parent=0);
    ~QucsSettingsDialog();

private slots:
    void slotOK();
    void slotApply();
    void slotFontDialog();
    void slotBGColorDialog();
    void slotDefaultValues();
    void slotAddFileType();
    void slotRemoveFileType();
    void slotColorComment();
    void slotColorString();
    void slotColorInteger();
    void slotColorReal();
    void slotColorCharacter();
    void slotColorDataType();
    void slotColorAttribute();
    void slotColorDirective();
    void slotColorTask();
    void slotTableClicked(int,int);
    void slotPathTableClicked(int,int);

    void slotHomeDirBrowse();
    void slotAdmsXmlDirBrowse();
    void slotAscoDirBrowse();
    void slotEditorBrowse();
    void slotOctaveDirBrowse();

    void slotAddPath();
    void slotAddPathWithSubFolders();
    void slotRemovePath();
    void slotPathSelectionChanged();

public:
    QucsApp *App{nullptr};

    QFont Font;
    QCheckBox *checkWiring{nullptr}, *checkLoadFromFutureVersions{nullptr},
              *checkAntiAliasing{nullptr}, *checkTextAntiAliasing{nullptr},
              *checkShowSchematicDescription{nullptr};
    QComboBox *LanguageCombo{nullptr};
    QPushButton *FontButton{nullptr}, *BGColorButton{nullptr};
    QLineEdit *LargeFontSizeEdit{nullptr}, *undoNumEdit{nullptr}, *editorEdit{nullptr}, *Input_Suffix{nullptr},
              *Input_Program{nullptr}, *homeEdit{nullptr}, *admsXmlEdit{nullptr}, *ascoEdit{nullptr}, *octaveEdit{nullptr};
    QTableWidget *fileTypesTableWidget{nullptr}, *pathsTableWidget{nullptr};
    QStandardItemModel *model{nullptr};
    QPushButton *ColorComment{nullptr}, *ColorString{nullptr}, *ColorInteger{nullptr},
                *ColorReal{nullptr}, *ColorCharacter{nullptr}, *ColorDataType{nullptr}, *ColorAttribute{nullptr},
                *ColorDirective{nullptr}, *ColorTask{nullptr};
    QPushButton *RemovePathButt{nullptr};

    QVBoxLayout *all{nullptr};
    QIntValidator *val50{nullptr};
    QIntValidator *val200{nullptr};
    QRegExp Expr;
    QRegExpValidator *Validator{nullptr};

private:
    QStringList currentPaths;
    

private:
    void makePathTable();

};

#endif
