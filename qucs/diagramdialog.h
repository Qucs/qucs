/***************************************************************************
                          diagramdialog.h  -  description
                             -------------------
    begin                : Sun Oct 5 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIAGRAMDIALOG_H
#define DIAGRAMDIALOG_H

#include "diagram.h"

#include <qdialog.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qpoint.h>


/**
  *@author Michael Margraf
  */

class DiagramDialog : public QDialog  {
Q_OBJECT
public: 
	DiagramDialog(Diagram *d, QDir *_ProjDir, QWidget *parent=0, const char *name=0);
	~DiagramDialog();

  bool loadVarData(const QString& var);
  
public slots:
  void slotReadVars(int index);
  void slotTakeVar(QListViewItem *Item);
  void slotNewGraph();
  void slotDeleteGraph();
  void slotApplyGraphInput();
  void slotOK();
  void slotApply();
  void slotFuncHelp();

public:
  Diagram *Diag;
  QDir    *ProjDir;
  
  QComboBox *ChooseData;
  QListView *ChooseVars;
  QListBox  *SelectedVars;

  QLineEdit *GraphInput, *xLabel, *yLabel;
};

#endif
