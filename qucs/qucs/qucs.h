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
#include "qucsview.h"
#include "simmessage.h"

/**
  * This Class is the base class for your application. It sets up the main
  * window and providing a menubar, toolbar
  * and statusbar. For the main view, an instance of class QucsView is
  * created which creates your view.
  */
class QucsApp : public QMainWindow
{
  Q_OBJECT
  
  public:
    QucsApp();
    ~QucsApp();

    void initActions();    // initializes all QActions of the application
    void initMenuBar();    // creates the menu_bar and inserts the menuitems
    void initToolBar();    // creates the toolbars
    void initStatusBar();  // setup the statusbar
    void initView();       // setup the mainview

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
    void slotEditCut();     // put the marked object into the clipboard and remove it from the document
    void slotEditCopy();    // put the marked object into the clipboard
    void slotEditPaste(bool on);   // paste the clipboard into the document
    void slotEditDelete(bool on);  // delete the selected components
    void slotEditRotate(bool on);  // rotate the selected components
    void slotEditMirrorX(bool on); // mirror the selected components about the X axis
    void slotEditMirrorY(bool on); // mirror the selected components about the Y axis
    
    void slotViewToolBar(bool toggle);    // toggle the toolbar
    void slotViewStatusBar(bool toggle);  // toggle the statusbar

    void slotHelpAbout();    // shows an about dialog
    void slotHelpAboutQt();  // shows the standard about dialog for Qt


    void slotZoomIn();  // Zoom in by 2
    void slotZoomOut(); // Zoom out by 2    
    void slotInsertEquation(bool on);
    void slotInsertGround(bool on);
    void slotInsertPort(bool on);
    void slotSetWire(bool on);

// ##########################################################################################
//  private slots:
    void slotOpenProject(QListBoxItem *item);
    void slotSelectComponent(QIconViewItem *item);
//    void slotOpenContent(QListViewItem *item, const QPoint &, int column);  // Qt3.2
    void slotOpenContent(QListViewItem *item);
    void slotSetCompView(int index);
    void slotProjNewButt();
    void slotProjOpenButt();
    void slotProjDelButt();
    void slotChangeView(int id);
    void slotSimulate();
    void slotAfterSimulation(int Status);
    void slotChangePage();
    void slotSelect(bool on);
    void slotEditActivate(bool on);
    void slotInsertLabel(bool on);

//    void slotActivateCopy(bool on);
    
  private:

    SimMessage sim;   // simulation message window

    QucsView   *view;   // the working area with schematics, data displays etc.
    QPrinter   Printer; // printer global in order to remember the user settings


    QPopupMenu *fileMenu;    // file_menu contains all items of the menubar entry "File"
    QPopupMenu *editMenu;    // edit_menu contains all items of the menubar entry "Edit"
    QPopupMenu *insMenu;     // ins_menu contains all items of the menubar entry "Insert"
    QPopupMenu *projMenu;    // proj_menu contains all items of the menubar entry "Project"
    QPopupMenu *viewMenu;    // view_menu contains all items of the menubar entry "View"
    QPopupMenu *helpMenu;    // view_menu contains all items of the menubar entry "Help"
    
    QToolBar *fileToolbar, *editToolbar, *viewToolbar, *workToolbar;    // the toolbars

    /** actions for the application initialized in initActions() and used to en/disable them
      * according to your needs during the program */
    QAction *fileNew, *fileNewDpl, *fileOpen, *fileSave, *fileSaveAs, *fileSaveAll, *fileClose;
    QAction *fileSettings, *filePrint, *fileQuit;
    QAction *insWire, *insLabel, *insGround, *insPort, *insEquation;
    QAction *projNew, *projOpen, *projDel;
    QAction *editCut, *editCopy, *editPaste, *undo, *redo, *magAll, *magPlus, *magMinus, *select;
    QAction *editRotate, *editMirror, *editMirrorY, *intoH, *popH, *editActivate, *wire, *editDelete;
    QAction *simulate, *dpl_sch;
    QAction *viewToolBar, *viewStatusBar;
    QAction *helpAboutApp, *helpAboutQt;

    QAction *activeAction;    // pointer to the action selected by the user
    
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

    
// ********** Methods ***************************************************
    void OpenProject(const QString& name);
    
};
#endif 

