/***************************************************************************
                          qucs.h  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
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

#ifndef QUCS_H
#define QUCS_H

// include files for QT
#include <qapplication.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qwhatsthis.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qtabbar.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qiconview.h>
#include <qcombobox.h>
#include <qtabwidget.h>


// application specific includes
#include "qucsinit.h"
#include "qucsview.h"
#include "dialogs/simmessage.h"


/**
  * This Class is the base class for the application. It sets up the main
  * window and providing a menubar, toolbar and statusbar. For the main view,
  * an instance of class QucsView is created which creates the view.
  */
class QucsApp : public QMainWindow
{
  Q_OBJECT
public:
    QucsApp();
    ~QucsApp();

    bool loadSettings();   // loads the settings file and restores the settings
    void saveSettings();   // saves the settings in the settings file

    void initView();       // setup the mainview
    void initCursorMenu();

    bool closeAllFiles();
    int  testFile(const QString& DocName);


protected:
    void closeEvent(QCloseEvent* Event);

public slots:
    void slotFileNew();     // generate a new schematic in the view TabBar
    void slotFileOpen();    // open a document
    void slotFileSave();    // save a document
    void slotFileSaveAs();  // save a document under a different filename
    void slotFileSaveAll(); // save all open documents
    void slotFileClose();   // close the actual file
    void slotFileSettings();// open dialog to change file settings
    void slotFilePrint();   // print the actual file
    void slotFileQuit();    // exits the application
    void slotEditCut();     // put marked object into the clipboard and remove it from the document
    void slotEditCopy();    // put the marked object into the clipboard
    void slotEditPaste(bool on);   // paste the clipboard into the document
    void slotEditDelete(bool on);  // delete the selected components
    void slotEditRotate(bool on);  // rotate the selected components
    void slotEditMirrorX(bool on); // mirror the selected components about the X axis
    void slotEditMirrorY(bool on); // mirror the selected components about the Y axis

    void slotHelpIndex();       // shows a HTML docu: Help Index
    void slotGettingStarted();  // shows a HTML docu: Getting started

    void slotIntoHierarchy();
    void slotPopHierarchy();

    void slotShowAll();
    void slotShowOne();
    void slotZoomIn();  // Zoom in by 2
    void slotZoomOut(); // Zoom out by 2
    void slotInsertEquation(bool on);
    void slotInsertGround(bool on);
    void slotInsertPort(bool on);
    void slotSetWire(bool on);

    // for menu that appears by right click in content ListView
    void slotShowContentMenu(QListViewItem *item, const QPoint& point, int);
    void slotCMenuOpen();
    void slotCMenuRename();
    void slotCMenuDelete();

// ########################################################################
//  private slots:
    void slotMenuOpenProject();
    void slotOpenProject(QListBoxItem *item);
    void slotSelectComponent(QIconViewItem *item);
//    void slotOpenContent(QListViewItem *item, const QPoint &, int column);  // Qt3.2
    void slotSelectSubcircuit(QListViewItem *item);
    void slotOpenContent(QListViewItem *item);
    void slotSetCompView(int index);
    void slotProjNewButt();
    void slotProjOpenButt();
    void slotProjDelButt();
    void slotMenuDelProject();
    void slotChangeView(int id);
    void slotSimulate();
    void slotShowLastMsg();
    void slotShowLastNetlist();
    void slotAfterSimulation(int Status, SimMessage *sim);
    void slotChangePage();
    void slotSelect(bool on);
    void slotSelectAll();
    void slotEditActivate(bool on);
    void slotInsertLabel(bool on);
    void slotSetMarker(bool on);

public:
    QucsView   *view;   // the working area with schematics, data displays etc.

    QAction *fileNew, *fileNewDpl, *fileOpen, *fileSave, *fileSaveAs;
    QAction *fileSaveAll, *fileClose, *fileSettings, *filePrint, *fileQuit;
    QAction *insWire, *insLabel, *insGround, *insPort, *insEquation;
    QAction *projNew, *projOpen, *projDel;
    QAction *editCut, *editCopy, *editPaste, *undo, *redo, *magAll, *magOne;
    QAction *magPlus, *magMinus, *select, *editRotate, *editMirror;
    QAction *editMirrorY, *intoH, *popH, *editActivate, *wire, *editDelete;
    QAction *simulate, *dpl_sch, *selectAll, *showMsg, *showNet, *setMarker;
    QAction *helpIndex, *helpGetStart;

    QAction *activeAction;    // pointer to the action selected by the user

private:
    QPrinter   Printer; // printer is global (to remember the user settings)
    QucsInit   Init;    // initializes toolbars, menubar, actions ...

    // menu appearing by right mouse button click on content listview
    QPopupMenu *ContentMenu;

// ********* Widgets on the main area **********************************
    QTabWidget    *TabView;
    QTabBar       *WorkView;

    QListBox      *Projects;
    QListView     *Content;
    QListViewItem *ConSchematics, *ConDisplays, *ConDatasets;

    QComboBox     *CompChoose;
    QIconView     *CompComps;

// ********** Properties ************************************************
    QString       ProjName;   // name of the project, that is open
    QPtrList<QString> HierarchyHistory; // keeps track of the "go into subcircuit"

    QString       QucsFileFilter;


// ********** Methods ***************************************************
    bool saveCurrentFile();
    bool saveAs();
    void readProjects();
    void OpenProject(const QString& Path, const QString& Name);
    bool DeleteProject(const QString& Path, const QString& Name);
    void updatePortNumber(int No);
    bool gotoPage(const QString& Name);
    
};
#endif 

