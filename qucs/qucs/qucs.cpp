/***************************************************************************
                          qucs.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qucs.h"
#include "qucsview.h"
#include "newprojdialog.h"
#include "wire.h"
#include "rectdiagram.h"
#include "polardiagram.h"
#include "smithdiagram.h"

#include <qaccel.h>
#include <qimage.h>
#include <qsplitter.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qpainter.h>
#include <qfiledialog.h>



QucsApp::QucsApp()
{
  setCaption(tr("Qucs " VERSION));
  resize(600,400);

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();

  initView();

  viewToolBar->setOn(true);
  viewStatusBar->setOn(true);
}

QucsApp::~QucsApp()
{
}

// #########################################################################################
// initializes all QActions of the application
void QucsApp::initActions()
{
  activeAction = 0;

  fileNew = new QAction(tr("New"), QIconSet(QImage("bitmaps/filenew.png")), tr("&New"), QAccel::stringToKey(tr("Ctrl+N")), this);
  fileNew->setStatusTip(tr("Creates a new document"));
  fileNew->setWhatsThis(tr("New\n\nCreates a new schematic or data display document"));
  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

  fileOpen = new QAction(tr("Open File"), QIconSet(QImage("bitmaps/fileopen.png")), tr("&Open..."), 0, this);
  fileOpen->setStatusTip(tr("Opens an existing document"));
  fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));

  fileSave = new QAction(tr("Save File"), QIconSet(QImage("bitmaps/filesave.png")), tr("&Save"), QAccel::stringToKey(tr("Ctrl+S")), this);
  fileSave->setStatusTip(tr("Saves the actual document"));
  fileSave->setWhatsThis(tr("Save File\n\nSaves the actual document"));
  connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));

  fileSaveAs = new QAction(tr("Save File As"), tr("Save &as..."), 0, this);
  fileSaveAs->setStatusTip(tr("Saves the actual document under a new filename"));
  fileSaveAs->setWhatsThis(tr("Save As\n\nSaves the actual document under a new filename"));
  connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSave()));

  fileSaveAll = new QAction(tr("Save All Files"), QIconSet(QImage("bitmaps/filesaveall.png")), tr("Save &All"), QAccel::stringToKey(tr("Ctrl+A")), this);
  fileSaveAll->setStatusTip(tr("Saves all open documents"));
  fileSaveAll->setWhatsThis(tr("Save All Files\n\nSaves all open documents"));
  connect(fileSaveAll, SIGNAL(activated()), this, SLOT(slotFileSaveAll()));

  fileClose = new QAction(tr("Close File"), QIconSet(QImage("bitmaps/fileclose.png")), tr("&Close"), QAccel::stringToKey(tr("Ctrl+W")), this);
  fileClose->setStatusTip(tr("Closes the actual document"));
  fileClose->setWhatsThis(tr("Close File\n\nCloses the actual document"));
  connect(fileClose, SIGNAL(activated()), this, SLOT(slotFileClose()));

  filePrint = new QAction(tr("Print File"), QIconSet(QImage("bitmaps/fileprint.png")), tr("&Print"), QAccel::stringToKey(tr("Ctrl+P")), this);
  filePrint->setStatusTip(tr("Prints out the actual document"));
  filePrint->setWhatsThis(tr("Print File\n\nPrints out the actual document"));
  connect(filePrint, SIGNAL(activated()), this, SLOT(slotFilePrint()));

  fileQuit = new QAction(tr("Exit"), tr("E&xit"), QAccel::stringToKey(tr("Ctrl+Q")), this);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(activated()), this, SLOT(slotFileQuit()));

  editCut = new QAction(tr("Cut"), QIconSet(QImage("bitmaps/editcut.png")), tr("Cu&t"), QAccel::stringToKey(tr("Ctrl+X")), this);
  editCut->setStatusTip(tr("Cuts the selected section and puts it to the clipboard"));
  editCut->setWhatsThis(tr("Cut\n\nCuts the selected section and puts it to the clipboard"));
  connect(editCut, SIGNAL(activated()), this, SLOT(slotEditCut()));

  editCopy = new QAction(tr("Copy"), QIconSet(QImage("bitmaps/editcopy.png")), tr("&Copy"), QAccel::stringToKey(tr("Ctrl+C")), this);
  editCopy->setStatusTip(tr("Copies the selected section to the clipboard"));
  editCopy->setWhatsThis(tr("Copy\n\nCopies the selected section to the clipboard"));
  connect(editCopy, SIGNAL(activated()), this, SLOT(slotEditCopy()));

  editPaste = new QAction(tr("Paste"), QIconSet(QImage("bitmaps/editpaste.png")), tr("&Paste"), QAccel::stringToKey(tr("Ctrl+V")), this);
  editPaste->setStatusTip(tr("Pastes the clipboard contents to actual position"));
  editPaste->setWhatsThis(tr("Paste\n\nPastes the clipboard contents to actual position"));
  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  editDelete = new QAction(tr("Delete"), QIconSet(QImage("bitmaps/editdelete.png")), tr("&Delete"), QAccel::stringToKey(tr("Del")), this);
  editDelete->setStatusTip(tr("Deletes the selected components"));
  editDelete->setWhatsThis(tr("Delete\n\nDeletes the selected components"));
  editDelete->setToggleAction(true);
  connect(editDelete, SIGNAL(toggled(bool)), this, SLOT(slotEditDelete(bool)));

  undo = new QAction(tr("Undo"), QIconSet(QImage("bitmaps/undo.png")), tr("&Undo"), QAccel::stringToKey(tr("Ctrl+Z")), this);
  undo->setStatusTip(tr("Undoes the last command"));
  undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  redo = new QAction(tr("Redo"), QIconSet(QImage("bitmaps/redo.png")), tr("&Redo"), QAccel::stringToKey(tr("Ctrl+Y")), this);
  redo->setStatusTip(tr("Redoes the last command"));
  redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  projNew = new QAction(tr("New Project"), tr("&New Project"), 0, this);
  projNew->setStatusTip(tr("Creates a new project"));
  projNew->setWhatsThis(tr("New Project\n\nCreates a new project"));
//  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

  projOpen = new QAction(tr("Open Project"), tr("&Open Project"), 0, this);
  projOpen->setStatusTip(tr("Opens a project"));
  projOpen->setWhatsThis(tr("Open Project\n\nOpens an existing project"));
//  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

  projDel = new QAction(tr("Delete Project"), tr("&Delete Project"), 0, this);
  projDel->setStatusTip(tr("Deletes a project"));
  projDel->setWhatsThis(tr("Delete Project\n\nDeletes an existing project"));
//  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

  magAll = new QAction(tr("View All"), QIconSet(QImage("bitmaps/viewmagfit.png")), tr("View All"), QAccel::stringToKey(tr("Ctrl+A")), this);
  magAll->setStatusTip(tr("Views the whole page"));
  magAll->setWhatsThis(tr("View All\n\nShows the whole page content"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  magPlus = new QAction(tr("Zoom in"), QIconSet(QImage("bitmaps/viewmag+.png")), tr("Zoom in"), QAccel::stringToKey(tr("Ctrl+P")), this);
  magPlus->setStatusTip(tr("Zooms into the actual view"));
  magPlus->setWhatsThis(tr("Zoom in\n\nZooms the actual view"));
  connect(magPlus, SIGNAL(activated()), this, SLOT(slotZoomIn()));

  magMinus = new QAction(tr("Zoom out"), QIconSet(QImage("bitmaps/viewmag-.png")), tr("Zoom out"), QAccel::stringToKey(tr("Ctrl+M")), this);
  magMinus->setStatusTip(tr("Zooms out the actual view"));
  magMinus->setWhatsThis(tr("Reduce\n\nZooms out the actual view"));
  connect(magMinus, SIGNAL(activated()), this, SLOT(slotZoomOut()));

  select = new QAction(tr("Select"), QIconSet(QImage("bitmaps/pointer.png")), tr("Select"), 0, this);
  select->setStatusTip(tr("Select mode"));
  select->setWhatsThis(tr("Select\n\nSelect mode"));
  select->setToggleAction(true);
  connect(select, SIGNAL(toggled(bool)), this, SLOT(slotSelect(bool)));

  editRotate = new QAction(tr("Rotate"), QIconSet(QImage("bitmaps/rotate_ccw.png")), tr("Rotate"), 0, this);
  editRotate->setStatusTip(tr("Rotates the selected component by 90°"));
  editRotate->setWhatsThis(tr("Rotate\n\nRotates the selected component by 90° counter-clockwise"));
  editRotate->setToggleAction(true);
  connect(editRotate, SIGNAL(toggled(bool)), this, SLOT(slotEditRotate(bool)));

  editMirror = new QAction(tr("Mirror about X Axis"), QIconSet(QImage("bitmaps/mirror.png")), tr("Mirror about X Axis"), 0, this);
  editMirror->setStatusTip(tr("Mirrors the selected item about X axis"));
  editMirror->setWhatsThis(tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  editMirror->setToggleAction(true);
  connect(editMirror, SIGNAL(toggled(bool)), this, SLOT(slotEditMirrorX(bool)));

  intoH = new QAction(tr("Push into Hierarchy"), QIconSet(QImage("bitmaps/bottom.png")), tr("Push into Hierarchy"), 0, this);
  intoH->setStatusTip(tr("Goes inside subcircuit"));
  intoH->setWhatsThis(tr("Push into Hierarchy\n\nGoes inside the selected subcircuit"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  popH = new QAction(tr("Pop out"), QIconSet(QImage("bitmaps/top.png")), tr("Pop out"), 0, this);
  popH->setStatusTip(tr("Pop outside subcircuit"));
  popH->setWhatsThis(tr("Pop out\n\nGoes up one hierarchy level, i.e. leaves subcircuit"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  editActivate = new QAction(tr("Deactivate/Activate"), QIconSet(QImage("bitmaps/deactiv.png")), tr("Deactivate/Activate"), 0, this);
  editActivate->setStatusTip(tr("Deactivate/Activate the selected item"));
  editActivate->setWhatsThis(tr("Deactivate/Activate\n\nDeactivate/Activate the selected item"));
  editActivate->setToggleAction(true);
  connect(editActivate, SIGNAL(toggled(bool)), this, SLOT(slotEditActivate(bool)));

  insEquation = new QAction(tr("Insert Equation"), QIconSet(QImage("bitmaps/equation.png")), tr("Insert Equation"), 0, this);
  insEquation->setStatusTip(tr("Inserts equation"));
  insEquation->setWhatsThis(tr("Insert Equation\n\nInserts a user defined equation"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  insGround = new QAction(tr("Insert Ground"), QIconSet(QImage("bitmaps/ground.png")), tr("Insert Ground"), 0, this);
  insGround->setStatusTip(tr("Inserts ground"));
  insGround->setWhatsThis(tr("Insert Ground\n\nInserts a ground symbol"));
  insGround->setToggleAction(true);
  connect(insGround, SIGNAL(toggled(bool)), this, SLOT(slotInsertGround(bool)));

  insPort = new QAction(tr("Insert Port"), QIconSet(QImage("bitmaps/port.png")), tr("Insert Port"), 0, this);
  insPort->setStatusTip(tr("Inserts port"));
  insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  insPort->setToggleAction(true);
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  insWire = new QAction(tr("Insert Wire"), QIconSet(QImage("bitmaps/wire.png")), tr("Wire"), 0, this);
  insWire->setStatusTip(tr("Inserts a wire"));
  insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  insWire->setToggleAction(true);
  connect(insWire, SIGNAL(toggled(bool)), this, SLOT(slotSetWire(bool)));

  insLabel = new QAction(tr("Insert Wire/Pin Label"), QIconSet(QImage("bitmaps/nodename.png")), tr("Wire Label"), 0, this);
  insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  insLabel->setWhatsThis(tr("Wire Label\n\nInserts a wire or pin label"));
  insLabel->setToggleAction(true);
  connect(insLabel, SIGNAL(toggled(bool)), this, SLOT(slotInsertLabel(bool)));

  simulate = new QAction(tr("Simulate"), QIconSet(QImage("bitmaps/gear.png")), tr("Simulate"), 0, this);
  simulate->setStatusTip(tr("Simulates the actual schematic"));
  simulate->setWhatsThis(tr("Simulate\n\nSimulates the actual schematic"));
  connect(simulate, SIGNAL(activated()), this, SLOT(slotSimulate()));

  dpl_sch = new QAction(tr("View Data Display/Schematic"), QIconSet(QImage("bitmaps/rebuild.png")), tr("View Data Display/Schematic"), 0, this);
  dpl_sch->setStatusTip(tr("Changes to data display or schematic page"));
  dpl_sch->setWhatsThis(tr("View Data Display/Schematic\n\nChanges to data display or schematic page"));
//  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  viewToolBar = new QAction(tr("Toolbar"), tr("Tool&bar"), 0, this, 0, true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("Statusbar"), tr("&Statusbar"), 0, this, 0, true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));

  helpAboutApp = new QAction(tr("About"), tr("&About..."), 0, this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

}

void QucsApp::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR

  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu
  fileMenu=new QPopupMenu();
  fileNew->addTo(fileMenu);
  fileOpen->addTo(fileMenu);
  fileClose->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileSave->addTo(fileMenu);
  fileSaveAll->addTo(fileMenu);
  fileSaveAs->addTo(fileMenu);
  fileMenu->insertSeparator();
  filePrint->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileQuit->addTo(fileMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu=new QPopupMenu();
  undo->addTo(editMenu);
  redo->addTo(editMenu);
  editMenu->insertSeparator();
  editCut->addTo(editMenu);
  editCopy->addTo(editMenu);
  editPaste->addTo(editMenu);
  editDelete->addTo(editMenu);
  editMenu->insertSeparator();
  select->addTo(editMenu);
  editRotate->addTo(editMenu);
  editMirror->addTo(editMenu);
  editActivate->addTo(editMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry insMenu
  insMenu=new QPopupMenu();
  insWire->addTo(insMenu);
  insLabel->addTo(insMenu);
  insGround->addTo(insMenu);
  insPort->addTo(insMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry projMenu
  projMenu=new QPopupMenu();
  projNew->addTo(projMenu);
  projOpen->addTo(projMenu);
  projDel->addTo(projMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu=new QPopupMenu();
  magAll->addTo(viewMenu);
  magPlus->addTo(viewMenu);
  magMinus->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);
  ///////////////////////////////////////////////////////////////////
  // EDIT YOUR APPLICATION SPECIFIC MENUENTRIES HERE

  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
  helpMenu=new QPopupMenu();
  helpAboutApp->addTo(helpMenu);

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  menuBar()->insertItem(tr("&File"), fileMenu);
  menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("&Insert"), insMenu);
  menuBar()->insertItem(tr("&Project"), projMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(tr("&Help"), helpMenu);

}

void QucsApp::initToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  fileToolbar = new QToolBar(this, "file operations");
  fileNew->addTo(fileToolbar);
  fileOpen->addTo(fileToolbar);
  fileSave->addTo(fileToolbar);
  fileSaveAll->addTo(fileToolbar);
  fileClose->addTo(fileToolbar);
  filePrint->addTo(fileToolbar);
//  fileToolbar->addSeparator();

  editToolbar = new QToolBar(this, "edit operations");
  editCut->addTo(editToolbar);
  editCopy->addTo(editToolbar);
  editPaste->addTo(editToolbar);
  editDelete->addTo(editToolbar);
  undo->addTo(editToolbar);
  redo->addTo(editToolbar);
//  fileToolbar->addSeparator();

  viewToolbar = new QToolBar(this, "view operations");
  magAll->addTo(viewToolbar);
  magPlus->addTo(viewToolbar);
  magMinus->addTo(viewToolbar);
//  fileToolbar->addSeparator();

  workToolbar = new QToolBar(this, "work operations");
  select->addTo(workToolbar);
  editMirror->addTo(workToolbar);
  editRotate->addTo(workToolbar);
  intoH->addTo(workToolbar);
  popH->addTo(workToolbar);
  editActivate->addTo(workToolbar);
  insWire->addTo(workToolbar);
  insLabel->addTo(workToolbar);
  insEquation->addTo(workToolbar);
  insGround->addTo(workToolbar);
  insPort->addTo(workToolbar);
  simulate->addTo(workToolbar);
  dpl_sch->addTo(workToolbar);
  workToolbar->addSeparator();
  QWhatsThis::whatsThisButton(workToolbar);

}

void QucsApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //STATUSBAR
  statusBar()->message(tr("Ready."), 2000);
}


// #########################################################################
// ##########                                                     ##########
// ##########       Creates the view area (QTabWidget etc.)       ##########
// ##########                                                     ##########
// #########################################################################
void QucsApp::initView()
{
  QVBox *all = new QVBox(this);   // only to fill the entire view area completely
  QSplitter *Hsplit = new QSplitter(QSplitter::Horizontal, all);

  TabView  = new QTabWidget(Hsplit);    // tabs on the left side
  QVBox *WorkGroup = new QVBox(Hsplit);
  WorkView = new QTabBar(WorkGroup);    // tab on the right side
  view = new QucsView(WorkGroup);       // work area with schematics, data displays etc.

  connect(WorkView, SIGNAL(selected(int)), SLOT(slotChangeView(int)));

  Hsplit->setResizeMode(TabView, QSplitter::KeepSize);
  setCentralWidget(all);

  // ----------------------------------------------------------
  // "Project Tab" of the left QTabWidget
  QVBox *ProjGroup = new QVBox(this);
  QHBox *ProjButts = new QHBox(ProjGroup);
  QPushButton *ProjNew   = new QPushButton("New",ProjButts);
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotProjNewButt()));
  QPushButton *ProjOpen  = new QPushButton("Open",ProjButts);
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotProjOpenButt()));
  QPushButton *ProjDel   = new QPushButton("Delete",ProjButts);
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotProjDelButt()));

  Projects = new QListBox(ProjGroup);
  TabView->addTab(ProjGroup, "Projects");
  TabView->setTabToolTip(TabView->page(0), "content of the project directory");

  connect(Projects, SIGNAL(doubleClicked(QListBoxItem*)), SLOT(slotOpenProject(QListBoxItem*)));

  // ----------------------------------------------------------
  // "Content Tab" of the left QTabWidget
  Content = new QListView(this);
  Content->setRootIsDecorated(true);  // root items should have open/close decoration to their left
  Content->setSorting(-1);    // no sorting
  Content->addColumn("Content of");
  Content->setColumnWidthMode(0,QListView::Manual);
  Content->setColumnWidth(0, 180);

  ConDatasets   = new QListViewItem(Content, "Datasets");
  ConDisplays   = new QListViewItem(Content, "Data Displays");
  ConSchematics = new QListViewItem(Content, "Schematics");
  TabView->addTab(Content,"Content");
  TabView->setTabToolTip(TabView->page(1), "content of the open project");

// QT 3.2
//  connect(Content, SIGNAL(doubleClicked(QListViewItem*, const QPoint &,int)), SLOT(slotOpenContent(QListViewItem*, const QPoint &,int)));
  connect(Content, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotOpenContent(QListViewItem*)));

  // ----------------------------------------------------------
  // "Component Tab" of the left QTabWidget
  QVBox *CompGroup  = new QVBox(this);
  CompChoose = new QComboBox(CompGroup);
  CompComps  = new QIconView(CompGroup);
  TabView->addTab(CompGroup,"Components");
  TabView->setTabToolTip(TabView->page(2), "components and diagrams");

  CompChoose->insertItem("lumped components");
  CompChoose->insertItem("sources");
  CompChoose->insertItem("transmission lines");
  CompChoose->insertItem("nonlinear components");
  CompChoose->insertItem("simulations");
  CompChoose->insertItem("paintings");
  CompChoose->insertItem("diagrams");

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(clicked(QIconViewItem*)), SLOT(slotSelectComponent(QIconViewItem*)));


  // ----------------------------------------------------------
  // Checks situation and reads all existing Qucs projects
  if(!view->ProjDir.cd(QDir::homeDirPath()+"/.qucs"))   // is work directory already existing ?
    if(!view->ProjDir.mkdir(QDir::homeDirPath()+"/.qucs"))  // no, then create it
      QMessageBox::warning(this, "Warning", "Cannot create work directory !");
  view->ProjDir.setPath(QDir::homeDirPath()+"/.qucs");
      
  QStringList PrDirs = view->ProjDir.entryList("*", QDir::Dirs, QDir::Name);  // get all directories
  PrDirs.pop_front(); // delete "." from list
  PrDirs.pop_front(); // delete ".." from list

  QStringList::iterator it, itt;
  for(it = PrDirs.begin(); it != PrDirs.end(); ) {  // removes all non-project directories
     itt = it++;
     if ((*itt).right(4) != "_prj") PrDirs.remove(itt);   // project directories end with "_prj"
     else (*itt) = (*itt).left((*itt).length()-4);      // remove "_prj" from project name
  }
  Projects->insertStringList(PrDirs);

  // ---------------------------------------------------------------------
  // creates a document called "untitled"
  view->Docs.append(new QucsDoc(WorkView, ""));
}


// #########################################################################
bool QucsApp::queryExit()
{
  int exit=QMessageBox::information(this, tr("Quit..."),
                                    tr("Do your really want to quit?"),
                                    QMessageBox::Ok, QMessageBox::Cancel);

  if (exit==1)
  {

  }
  else
  {

  };

  return (exit==1);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void QucsApp::slotFileNew()
{
  statusBar()->message(tr("Creating new schematic..."));

  view->Docs.append(new QucsDoc(WorkView, ""));
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));  // make new document the current

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  statusBar()->message(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(view->ProjDir.absPath(), "Schematic (*.sch)", this,
                                           "open file dialog", "Enter a Schematic Name");
  if(!s.isEmpty()) {
    QucsDoc *d = new QucsDoc(WorkView, s);
    view->Docs.append(d);
    if(!d->load()) {
      view->Docs.remove();
      return;
    }

    WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));  // make new document the current
  }
  else statusBar()->message(tr("Opening aborted"), 2000);

  statusBar()->message(tr("Ready."));
}


// ###################################################################################
void QucsApp::slotFileSave()
{
  statusBar()->message(tr("Saving file..."));
  view->blockSignals(true);   // no user interaction during the time

  if(view->Docs.current()->DocName.isEmpty()) {
    QString s = QFileDialog::getSaveFileName(view->ProjDir.absPath(), "Schematic (*.sch)", this,
                                             "save file dialog", "Enter a Schematic Name");
    if(s.isEmpty()) {
      statusBar()->message(tr("Saving aborted"), 2000);
      statusBar()->message(tr("Ready."));
      return;
    }
    view->Docs.current()->setName(s);
    
    QFileInfo Info(s);
    s = Info.fileName();  // remove path from file name
    new QListViewItem(ConSchematics, s);    // insert new name in Content ListView
  }

  view->Docs.current()->save();
  view->blockSignals(false);
  statusBar()->message(tr("Ready."));
}

// ###################################################################################
void QucsApp::slotFileSaveAs()
{
  statusBar()->message(tr("Saving file under new filename..."));
  view->blockSignals(true);   // no user interaction during the time
  
  QString s = QFileDialog::getSaveFileName(view->ProjDir.absPath(), "Schematic (*.sch)", this,
                                            "save file dialog", "Enter a Schematic Name");
  if(!s.isEmpty()) {
    view->Docs.current()->setName(s);

    QFileInfo Info(s);
    s = Info.fileName();  // remove path from file name
    new QListViewItem(ConSchematics, s);    // insert new name in Content ListView

    view->Docs.current()->save();
  }
  else statusBar()->message(tr("Saving aborted"), 2000);

  view->blockSignals(false);
  statusBar()->message(tr("Ready."));
}


// ###################################################################################
void QucsApp::slotFileSaveAll() {
  statusBar()->message(tr("Saving all files..."));

  QucsDoc *tmp = view->Docs.current();  // remember the current
  view->blockSignals(true);   // no user interaction during the time
  
  for(QucsDoc *ptr = view->Docs.first(); ptr != 0; ptr = view->Docs.next()) {
    if(ptr->DocName.isEmpty()) {
      QString s = QFileDialog::getSaveFileName(view->ProjDir.absPath(), "Schematic (*.sch)", this,
                                             "save file dialog", "Enter a Schematic Name");
      if(s.isEmpty()) continue;
      ptr->setName(s);

      QFileInfo Info(s);
      s = Info.fileName();  // remove path from file name
      new QListViewItem(ConSchematics, s);    // insert new name in Content ListView
    }

    ptr->save();
  }
  
  view->Docs.findRef(tmp);
  view->blockSignals(false);

  statusBar()->message(tr("Ready."));
}

// ###################################################################################
void QucsApp::slotFileClose()
{
  statusBar()->message(tr("Closing file..."));

  if(view->Docs.current()->DocChanged) {
    switch(QMessageBox::warning(this,"Closing Qucs document",
      "The document contains unsaved changes!\nDo you want to save the changes before closing?",
      "&Save", "&Discard", "Cancel", 0, 2)) {
      case 0 : slotFileSave();
               break;
      case 2 : return;
    }
  }

//  WorkView->removeTab(WorkView->tab(WorkView->currentTab()));
  view->Docs.remove();
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));  // make new document the current

  if(view->Docs.isEmpty())  // if no document left, create an untitled
    view->Docs.append(new QucsDoc(WorkView, ""));

  view->viewport()->repaint();
  view->drawn = false;

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotFilePrint()
{
  statusBar()->message(tr("Printing..."));
  QPrinter printer;
  if (printer.setup(this))
  {
    QPainter painter;
    painter.begin(&printer);

    ///////////////////////////////////////////////////////////////////
    // TODO: Define printing by using the QPainter methods here

    painter.end();
  };

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));
  ///////////////////////////////////////////////////////////////////
  // exits the Application
  if(true) //doc->isModified())
  {
    if(queryExit())
    {
      qApp->quit();
    }
    else
    {

    };
  }
  else
  {
    qApp->quit();
  };

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotEditPaste()
{
  statusBar()->message(tr("Inserting clipboard contents..."));

  statusBar()->message(tr("Ready."));
}


void QucsApp::slotViewToolBar(bool toggle)
{
  statusBar()->message(tr("Toggle toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if (toggle== false)
  {
    fileToolbar->hide();
  }
  else
  {
    fileToolbar->show();
  };

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotViewStatusBar(bool toggle)
{
  statusBar()->message(tr("Toggle statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if (toggle == false)
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  statusBar()->message(tr("Ready."));
}

void QucsApp::slotHelpAbout()
{
  QMessageBox::about(this,tr("About..."),
                      tr("Qucs\nVersion " VERSION "\n(c) 2003 by Michael Margraf") );
}

// ###################################################################################
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(int id)
{
  view->Docs.first();
  for(int n = WorkView->indexOf(id); n>0; n--) view->Docs.next();
  view->viewport()->repaint();
  view->drawn = false;
}

// ###################################################################################
void QucsApp::slotZoomIn()
{
  view->Zoom(2);
  view->viewport()->repaint();
  view->drawn = false;
}

void QucsApp::slotZoomOut()
{
  view->Zoom(0.5);
  view->viewport()->repaint();
  view->drawn = false;
}

// -------------------------------------------------------------------------------
// Is called when the simulate toolbar button is pressed.
void QucsApp::slotSimulate()
{
  sim.ProgText->clear();
  sim.ErrText->clear();
  sim.show();

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time
  sim.ProgText->insert("Starting new simulation on "+d.toString("ddd dd. MMM yyyy"));
  sim.ProgText->insert(" at "+t.toString("hh:mm:ss")+"\n\n");
  sim.ProgText->insert("creating netlist ....");

  QFile NetlistFile(view->ProjDir.path()+"/netlist.net");
  if(!view->Docs.current()->createNetlist(&NetlistFile)) {
    sim.ErrText->insert("ERROR: Cannot create netlist file!\nAborted.");
    return;
  }

  sim.ProgText->insert(" done.\n");
}

// #########################################################################################
// Is called when a double click is made in the content ListView.
//void QucsApp::slotOpenContent(QListViewItem *item, const QPoint &, int column)   // QT 3.2
void QucsApp::slotOpenContent(QListViewItem *item)
{
  if(item == 0) return;   // no item was double clicked
  if(item->parent() == 0) return;   // no component, but item "schematic", ...


  QucsDoc *d = new QucsDoc(WorkView, view->ProjDir.absPath()+"/"+item->text(0));
  view->Docs.append(d);

  if(!d->load()) view->Docs.remove();
  else WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));  // make new document the current
}

// #########################################################################################
// Is called when the open project button is pressed.
void QucsApp::slotOpenProject(QListBoxItem *item)
{
  OpenProject(item->text());
}

// #########################################################################################
// Opens an existing project.
void QucsApp::OpenProject(const QString& name)
{
//  QDir *projDir = new QDir();
  view->ProjDir.setPath(QDir::homeDirPath()+"/.qucs/"+name+"_prj");  // define project directory

  Content->setColumnText(0,"Content of '"+name+"'");  // column text of Content ListView
//  Content->setColumnWidth(0, Content->width()-5);

  Content->clear();   // empty content view
  ConDatasets   = new QListViewItem(Content, "Datasets");
  ConDisplays   = new QListViewItem(Content, "Data Displays");
  ConSchematics = new QListViewItem(Content, "Schematics");

  // put all schematic files into "Content"-ListView
  QStringList Elements = view->ProjDir.entryList("*.sch", QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Elements.begin(); it != Elements.end(); ++it)
     new QListViewItem(ConSchematics, (*it).ascii());

  // put all data display files into "Content"-ListView
  Elements = view->ProjDir.entryList("*.dpl", QDir::Files, QDir::Name);
  for(it = Elements.begin(); it != Elements.end(); ++it)
     new QListViewItem(ConDisplays, (*it).ascii());

  // put all dataset files into "Content"-ListView
  Elements = view->ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
  for(it = Elements.begin(); it != Elements.end(); ++it)
     new QListViewItem(ConDatasets, (*it).ascii());

  TabView->setCurrentPage(1);   // switch to "Content"-Tab
  Content->firstChild()->setOpen(true);  // show schematics
  ProjName = name;   // remember the name of project

  setCaption(tr("Qucs " VERSION " - Project: ")+name);  // show name in title of main window
}

// #########################################################################################
// Is called, when "Create New Project"-Button is pressed.
void QucsApp::slotProjNewButt()
{
  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir *projDir = new QDir();
  projDir->setPath(QDir::homeDirPath()+"/.qucs");
  if(projDir->mkdir(d->ProjName->text()+"_prj")) {
    Projects->insertItem(d->ProjName->text(),0);  // new project at first position
    if(d->OpenProj->isChecked()) slotOpenProject(Projects->firstItem());
  }
  else QMessageBox::information(this, "Info", "Cannot create project directory !");
}

// #########################################################################################
// Whenever the Component Library ComboBox is changed, this slot fills the Component IconView
// with the appropriat components.
void QucsApp::slotSetCompView(int index)
{
  CompComps->clear();   // clear the IconView
  switch(index) {
    case 0 : new QIconViewItem(CompComps, "Resistor", QImage("bitmaps/resistor.xpm"));
             new QIconViewItem(CompComps, "Resistor US", QImage("bitmaps/resistor_us.xpm"));
             new QIconViewItem(CompComps, "Capacitor", QImage("bitmaps/capacitor.xpm"));
             new QIconViewItem(CompComps, "Inductor", QImage("bitmaps/inductor.xpm"));
             new QIconViewItem(CompComps, "Transformer", QImage("bitmaps/transformer.xpm"));
             new QIconViewItem(CompComps, "symmetric Transformer", QImage("bitmaps/symtrans.xpm"));
             new QIconViewItem(CompComps, "Ground", QImage("bitmaps/ground.xpm"));
             new QIconViewItem(CompComps, "dc Block", QImage("bitmaps/dcblock.xpm"));
             new QIconViewItem(CompComps, "dc Feed", QImage("bitmaps/dcfeed.xpm"));
             new QIconViewItem(CompComps, "Bias T", QImage("bitmaps/biast.xpm"));
             new QIconViewItem(CompComps, "Attenuator", QImage("bitmaps/attenuator.xpm"));
             new QIconViewItem(CompComps, "Isolator", QImage("bitmaps/isolator.xpm"));
             new QIconViewItem(CompComps, "Circulator", QImage("bitmaps/circulator.xpm"));
             break;
    case 1 : new QIconViewItem(CompComps, "dc Voltage Source", QImage("bitmaps/dc_voltage.xpm"));
             new QIconViewItem(CompComps, "dc Current Source", QImage("bitmaps/dc_current.xpm"));
             new QIconViewItem(CompComps, "ac Voltage Source", QImage("bitmaps/ac_voltage.xpm"));
             new QIconViewItem(CompComps, "Power Source", QImage("bitmaps/source.xpm"));
             new QIconViewItem(CompComps, "Voltage Controlled Current Source", QImage("bitmaps/vccs.xpm"));
             new QIconViewItem(CompComps, "Current Controlled Current Source", QImage("bitmaps/cccs.xpm"));
             new QIconViewItem(CompComps, "Voltage Controlled Voltage Source", QImage("bitmaps/vcvs.xpm"));
             new QIconViewItem(CompComps, "Current Controlled Voltage Source", QImage("bitmaps/ccvs.xpm"));
             break;
    case 2 : new QIconViewItem(CompComps, "Transmission Line", QImage("bitmaps/tline.xpm"));
             new QIconViewItem(CompComps, "Substrate", QImage("bitmaps/substrate.xpm"));
             new QIconViewItem(CompComps, "Microstrip Line", QImage("bitmaps/msline.xpm"));
             new QIconViewItem(CompComps, "Microstrip Step", QImage("bitmaps/msstep.xpm"));
             new QIconViewItem(CompComps, "Microstrip Corner", QImage("bitmaps/mscorner.xpm"));
             new QIconViewItem(CompComps, "Microstrip Tee", QImage("bitmaps/mstee.xpm"));
             new QIconViewItem(CompComps, "Microstrip Cross", QImage("bitmaps/mscross.xpm"));
             break;
    case 3 : new QIconViewItem(CompComps, "Diode", QImage("bitmaps/diode.xpm"));
             break;
    case 4 : new QIconViewItem(CompComps, "dc simulation", QImage("bitmaps/dc.xpm"));
             new QIconViewItem(CompComps, "Transienten simulation", QImage("bitmaps/tran.xpm"));
             new QIconViewItem(CompComps, "ac simulation", QImage("bitmaps/ac.xpm"));
             new QIconViewItem(CompComps, "S-parameter simulation", QImage("bitmaps/sparameter.xpm"));
             new QIconViewItem(CompComps, "Harmonic balance", QImage("bitmaps/hb.xpm"));
             new QIconViewItem(CompComps, "Parameter sweep", QImage("bitmaps/sweep.xpm"));
             break;
    case 5 : new QIconViewItem(CompComps, "Line", QImage("bitmaps/line.xpm"));
             new QIconViewItem(CompComps, "Arrow", QImage("bitmaps/arrow.xpm"));
             new QIconViewItem(CompComps, "Text", QImage("bitmaps/text.xpm"));
             new QIconViewItem(CompComps, "Circle", QImage("bitmaps/circle.xpm"));
             new QIconViewItem(CompComps, "Rectangle", QImage("bitmaps/rectangle.xpm"));
             break;
    case 6 : new QIconViewItem(CompComps, "Cartesian", QImage("bitmaps/rect.xpm"));
             new QIconViewItem(CompComps, "Polar", QImage("bitmaps/polar.xpm"));
             new QIconViewItem(CompComps, "Tabular", QImage("bitmaps/tabular.xpm"));
             new QIconViewItem(CompComps, "Smith Chart", QImage("bitmaps/smith.xpm"));
             break;
  }
}

// -------------------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QIconViewItem *item)
{
  if(view->selComp != 0)  delete view->selComp;  // delete previously selected component
  if(view->selDiag != 0)  delete view->selDiag;  // delete previously selected diagram

  if(item == 0) {   // mouse button pressed not over an item ?
    CompComps->setSelected(CompComps->currentItem(), false);  // deselect component in ViewList
    view->selComp = 0;   // no component selected
    view->selDiag = 0;   // no diagram selected
    return;
  }

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  switch(CompChoose->currentItem()) {
    case 0: switch(CompComps->index(item)) {
              case 0: view->selComp = new Resistor();
                      break;
              case 1: view->selComp = new ResistorUS();
                      break;
              case 2: view->selComp = new Capacitor();
                      break;
              case 3: view->selComp = new Inductor();
                      break;
              case 4: view->selComp = new Transformer();
                      break;
              case 5: view->selComp = new symTrafo();
                      break;
              case 6: view->selComp = new Ground();
                      break;
              case 7: view->selComp = new dcBlock();
                      break;
              case 8: view->selComp = new dcFeed();
                      break;
              case 9: view->selComp = new BiasT();
                      break;
              case 10: view->selComp = new Attenuator();
                       break;
              case 11: view->selComp = new Isolator();
                       break;
              case 12: view->selComp = new Circulator();
                       break;
            }
            break;
    case 1: switch(CompComps->index(item)) {
              case 0: view->selComp = new Volt_dc();
                      break;
              case 1: view->selComp = new Ampere_dc();
                      break;
              case 2: view->selComp = new Volt_ac();
                      break;
              case 3: view->selComp = new Source_ac();
                      break;
              case 4: view->selComp = new VCCS();
                      break;
              case 5: view->selComp = new CCCS();
                      break;
              case 6: view->selComp = new VCVS();
                      break;
              case 7: view->selComp = new CCVS();
                      break;
            }
            break;
    case 2: switch(CompComps->index(item)) {
              case 0: view->selComp = new TLine();
                      break;
              case 1: view->selComp = new Substrate();
                      break;
              case 2: view->selComp = new MSline();
                      break;
              case 3: view->selComp = new MSstep();
                      break;
              case 4: view->selComp = new MScorner();
                      break;
              case 5: view->selComp = new MStee();
                      break;
              case 6: view->selComp = new MScross();
                      break;
            }
            break;
    case 3: switch(CompComps->index(item)) {
              case 0: view->selComp = new Diode();
                      break;
            }
            break;
    case 4: switch(CompComps->index(item)) {
              case 0: view->selComp = new DC_Sim();
                      break;
              case 1: view->selComp = new TR_Sim();
                      break;
              case 2: view->selComp = new AC_Sim();
                      break;
              case 3: view->selComp = new SP_Sim();
                      break;
              case 4: view->selComp = new HB_Sim();
                      break;
              case 5: view->selComp = new Param_Sweep();
                      break;
            }
            break;
    case 5: if(view->drawn) view->viewport()->repaint();
            view->drawn = false;
            view->MouseMoveAction = &QucsView::MouseDoNothing;
            view->MousePressAction = &QucsView::MouseDoNothing;
            view->MouseReleaseAction = &QucsView::MouseDoNothing;
            view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
            return;
    case 6: switch(CompComps->index(item)) {
              case 0: view->selDiag = new RectDiagram();
                      break;
              case 1: view->selDiag = new PolarDiagram();
                      break;
              case 3: view->selDiag = new SmithDiagram();
                      break;
            }

            if(view->drawn) view->viewport()->repaint();
            view->drawn = false;
            view->MouseMoveAction = &QucsView::MMoveDiagram;
            view->MousePressAction = &QucsView::MPressDiagram;
            view->MouseReleaseAction = &QucsView::MouseDoNothing;
            view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
            return;
  }

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -------------------------------------------------------------------------------
void QucsApp::slotInsertLabel(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insLabel;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MouseDoNothing;
  view->MousePressAction = &QucsView::MPressLabel;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -------------------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsApp::slotSelect(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = select;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MouseDoNothing;
  view->MousePressAction = &QucsView::MPressSelect;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
}

// -------------------------------------------------------------------------------
// Is called when the activate/deactivate toolbar button is pressed.
void QucsApp::slotEditActivate(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = editActivate;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MouseDoNothing;
  view->MousePressAction = &QucsView::MPressActivate;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -------------------------------------------------------------------------------
// Is called if "Wire"-Button is pressed.
void QucsApp::slotSetWire(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insWire;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveWire1;
  view->MousePressAction = &QucsView::MPressWire1;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -------------------------------------------------------------------------------
// Is called if "Delete"-Button is pressed.
void QucsApp::slotEditDelete(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }
  
  if(view->Docs.current()->deleteComponents()) {
    editDelete->blockSignals(true);
    editDelete->setOn(false);  // release toolbar button
    editDelete->blockSignals(false);
  }
  else {
    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = editDelete;

    view->MouseMoveAction = &QucsView::MouseDoNothing;  // if no component is selected, delete the one
    view->MousePressAction = &QucsView::MPressDelete;   // that will be clicked
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// #########################################################################################
// Is called when the mouse is clicked upon the ground toolbar button.
void QucsApp::slotInsertGround(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insGround;

  if(view->selComp != 0)
    delete view->selComp;  // delete previously selected component

  view->selComp = new Ground();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// ###################################################################################
// Is called when the rotate toolbar button is pressed.
void QucsApp::slotEditRotate(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->rotateComponents()) {
    editRotate->blockSignals(true);
    editRotate->setOn(false);  // release toolbar button
    editRotate->blockSignals(false);
  }
  else {
    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = editRotate;

    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// #########################################################################################
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorX(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->mirrorXComponents()) {
    editMirror->blockSignals(true);
    editMirror->setOn(false);  // release toolbar button
    editMirror->blockSignals(false);
  }
  else {
    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = editMirror;

    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// #########################################################################################
// Is called, when "Open Project"-Button is pressed.
void QucsApp::slotProjOpenButt()
{
  QListBoxItem *item = Projects->selectedItem();
  if(item) slotOpenProject(item);
  else QMessageBox::information(this, "Info", "No project is selected !");
}

// #########################################################################################
// Is called, when "Delete Project"-Button is pressed.
void QucsApp::slotProjDelButt()
{
  QListBoxItem *item = Projects->selectedItem();
  if(!item) {
    QMessageBox::information(this, "Info", "No project is selected !");
    return;
  }
  if(item->text() == ProjName) {
    QMessageBox::information(this, "Info", "Cannot delete an open project !");
    return;
  }

  // first ask, if really delete project ?
  if(QMessageBox::warning(this, "Warning",
     "This will destroy all the project\nfiles permanently !\nGo on ?","&Yes","&No",0,1,1)) return;

  QDir *projDir = new QDir();
  projDir->setPath(QDir::homeDirPath()+"/.qucs");
  projDir->cd(item->text()+"_prj");
  QStringList ProjFiles = projDir->entryList("*", QDir::Files);  // get all files

  // removes every file, remove("*") does not work
  QStringList::iterator it;
  for(it = ProjFiles.begin(); it != ProjFiles.end(); it++) {
     if(!projDir->remove(*it)) {
       QMessageBox::information(this, "Info", "Cannot remove project file: "+(*it));
       return;
     }
  }

  projDir->cdUp();  // leave project directory for deleting
  if(!projDir->rmdir(item->text()+"_prj")) {
    QMessageBox::information(this, "Info", "Cannot remove project directory !");
    return;
  }
  Projects->removeItem(Projects->currentItem());  // remove from project list
}
