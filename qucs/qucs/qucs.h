/***************************************************************************
                          qucs.h  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
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
#include <qdir.h>


// application specific includes
#include "qucsinit.h"
#include "qucsactions.h"
#include "qucsview.h"
#include "dialogs/simmessage.h"

extern QDir QucsWorkDir;
extern QDir QucsHomeDir;

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

    void initView();       // setup the mainview
    void initCursorMenu();

    bool closeAllFiles();
    static int testFile(const QString&);


protected:
    void closeEvent(QCloseEvent*);

public slots:
    void slotFileNew();     // generate a new schematic in the view TabBar
    void slotFileOpen();    // open a document
    void slotFileSave();    // save a document
    void slotFileSaveAs();  // save a document under a different filename
    void slotFileSaveAll(); // save all open documents
    void slotFileClose();   // close the actual file
    void slotSymbolEdit();   // edit the symbol for the schematic
    void slotFileSettings();// open dialog to change file settings
    void slotFilePrint();   // print the current file
    void slotFilePrintSelected();  // Print selected elements
    void slotFileQuit();    // exits the application
    void slotEditCut();     // put marked object into clipboard and delete it
    void slotEditCopy();    // put the marked object into the clipboard
    void slotEditUndo();    // makes the last operation undone
    void slotEditRedo();    // makes the last undo undone
    void slotApplSettings();// open dialog to change application settings
    void slotAlignTop();    // align selected elements with respect to top
    void slotAlignBottom(); // align selected elements with respect to bottom
    void slotAlignLeft();   // align selected elements with respect to left
    void slotAlignRight();  // align selected elements with respect to right
    void slotDistribHoriz();// distribute horizontally selected elements
    void slotDistribVert(); // distribute vertically selected elements

    void slotHelpIndex();       // shows a HTML docu: Help Index
    void slotGettingStarted();  // shows a HTML docu: Getting started

    void slotIntoHierarchy();
    void slotPopHierarchy();

    void slotShowAll();
    void slotShowOne();
    void slotZoomIn();  // Zoom in by 2
    void slotZoomOut(); // Zoom out by 2

    // for menu that appears by right click in content ListView
    void slotShowContentMenu(QListViewItem*, const QPoint&, int);
    void slotCMenuOpen();
    void slotCMenuRename();
    void slotCMenuDelete();

// ########################################################################
//  private slots:
    void slotMenuOpenProject();
    void slotOpenProject(QListBoxItem*);
    void slotMenuCloseProject();
    void slotSelectComponent(QIconViewItem*);
//    void slotOpenContent(QListViewItem *item, const QPoint &, int column);  // Qt3.2
    void slotSelectSubcircuit(QListViewItem*);
    void slotOpenContent(QListViewItem*);
    void slotSetCompView(int);
    void slotProjNewButt();
    void slotProjOpenButt();
    void slotProjDelButt();
    void slotMenuDelProject();
    void slotChangeView(int);
    void slotSimulate();
    void slotShowLastMsg();
    void slotShowLastNetlist();
    void slotAfterSimulation(int, SimMessage*);
    void slotChangePage(QString);
    void slotToPage();
    void slotSelectAll();
    void slotNextTab();

public:
    QucsView  *view; // the working area with schematics, data displays etc.
    QTabBar   *WorkView;

    // menu appearing by right mouse button click on content listview
    QPopupMenu *ContentMenu;

    QAction *fileNew, *fileNewDpl, *fileOpen, *fileSave, *fileSaveAs;
    QAction *fileSaveAll, *fileClose, *fileSettings, *filePrint, *fileQuit;
    QAction *projNew, *projOpen, *projDel, *projClose, *applSettings;
    QAction *editCut, *editCopy, *undo, *redo, *magAll, *magOne;
    QAction *magPlus, *magMinus, *distrHor, *distrVert, *filePrintSel;
    QAction *intoH, *popH, *alignTop, *alignBottom, *alignLeft, *alignRight;
    QAction *simulate, *dpl_sch, *selectAll, *showMsg, *showNet;
    QAction *helpIndex, *helpGetStart, *symEdit;

    QAction *activeAction;    // pointer to the action selected by the user
    QucsActions  Acts;    // contains most of the toggle actions

private:
    QPrinter    Printer; // printer is global (to remember the user settings)
    QucsInit    Init;    // initializes toolbars, menubar, actions ...

// ********* Widgets on the main area **********************************
    QTabWidget    *TabView;

    QListBox      *Projects;
    QListView     *Content;
    QListViewItem *ConSchematics, *ConDisplays, *ConDatasets;

    QComboBox     *CompChoose;
    QIconView     *CompComps;

// ********** Properties ************************************************
    QString       ProjName;   // name of the project, that is open
    QPtrList<QString> HierarchyHistory; // keeps track of "go into subcircuit"

    QString       QucsFileFilter;


// ********** Methods ***************************************************
    bool saveCurrentFile();
    bool saveAs();
    void readProjects();
    void OpenProject(const QString&, const QString&);
    bool DeleteProject(const QString&, const QString&);
    void updatePortNumber(int);
    bool gotoPage(const QString&);
    void nextDocument(bool);
    void fillComboBox(bool);
    void switchEditMode(bool);
    void changeSchematicSymbolMode(QucsDoc*);
};
#endif 

