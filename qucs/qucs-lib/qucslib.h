/***************************************************************************
                               qucslib.h
                              -----------
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

#ifndef QUCSLIB_H
#define QUCSLIB_H

#include <QMainWindow>
#include <QDir>
#include <QFont>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QComboBox>

#include "symbolwidget.h"
#include "libcomp.h"




extern tQucsSettings QucsSettings;
extern QDir UserLibDir;

class QucsLib : public QMainWindow  {
   Q_OBJECT
public:
  QucsLib();
  ~QucsLib();

  QListWidget *CompList;
  QStringList   LibraryComps;
  QComboBox    *Library;
  QString DefaultSymbol;

private slots:
  void slotAbout();
  void slotQuit();
  void slotHelp();
  void slotCopyToClipBoard();
  void slotShowModel();
  void slotSelectLibrary(int);
  void slotSearchComponent(const QString &);
  void slotSearchClear();
  void slotShowComponent(QListWidgetItem*);
  void slotManageLib();

private:
  void closeEvent(QCloseEvent*);
  void putLibrariesIntoCombobox();
//  bool getSection(QString, QString &, QString &);

  QMenu *fileMenu, *helpMenu;

  int UserLibCount;
  int libCurIdx;
  SymbolWidget *symWidget;
  QucsLibComponent *SelectedComponent;
  QTextEdit    *CompDescr;
  QVBoxLayout  *all;
  QLineEdit *CompSearch;
};

#endif /* QUCSLIB_H */
