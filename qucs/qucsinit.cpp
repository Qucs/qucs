/***************************************************************************
                          QucsInit.cpp  -  description
                             -------------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
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
# include <config.h>
#endif

#include "qucsinit.h"

#include "qucs.h"

#include <qaction.h>
#include <qaccel.h>
#include <qmenubar.h>


QucsInit::QucsInit()
{
}

QucsInit::~QucsInit()
{
}

// ########################################################################
// makes the whole job
void QucsInit::perform(QucsApp *p_)
{
  App = p_;

  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();

  viewToolBar->setOn(true);
  viewStatusBar->setOn(true);
}

// ########################################################################
// initializes all QActions of the application
void QucsInit::initActions()
{
  // to connect more than one key to an action
  mainAccel = new QAccel(App);


  App->activeAction = 0;   // no active action

  App->fileNew = new QAction(tr("New"),
                        QIconSet(QImage(BITMAPDIR "filenew.png")),
                        tr("&New"), CTRL+Key_N, App);
  App->fileNew->setStatusTip(tr("Creates a new document"));
  App->fileNew->setWhatsThis(
	tr("New\n\nCreates a new schematic or data display document"));
  connect(App->fileNew, SIGNAL(activated()), App, SLOT(slotFileNew()));

  App->fileOpen = new QAction(tr("Open File"),
                         QIconSet(QImage(BITMAPDIR "fileopen.png")),
                         tr("&Open..."), CTRL+Key_O, App);
  App->fileOpen->setStatusTip(tr("Opens an existing document"));
  App->fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(App->fileOpen, SIGNAL(activated()), App, SLOT(slotFileOpen()));

  App->fileSave = new QAction(tr("Save File"),
                         QIconSet(QImage(BITMAPDIR "filesave.png")),
                         tr("&Save"), CTRL+Key_S, App);
  App->fileSave->setStatusTip(tr("Saves the current document"));
  App->fileSave->setWhatsThis(tr("Save File\n\nSaves the current document"));
  connect(App->fileSave, SIGNAL(activated()), App, SLOT(slotFileSave()));

  App->fileSaveAs = new QAction(tr("Save File As"), tr("Save &as..."),
				0, App);
  App->fileSaveAs->setStatusTip(
	tr("Saves the current document under a new filename"));
  App->fileSaveAs->setWhatsThis(
	tr("Save As\n\nSaves the current document under a new filename"));
  connect(App->fileSaveAs, SIGNAL(activated()), App, SLOT(slotFileSaveAs()));

  App->fileSaveAll = new QAction(tr("Save All Files"),
                            QIconSet(QImage(BITMAPDIR "filesaveall.png")),
                            tr("Save &All"), 0, App);
  App->fileSaveAll->setStatusTip(tr("Saves all open documents"));
  App->fileSaveAll->setWhatsThis(
	tr("Save All Files\n\nSaves all open documents"));
  connect(App->fileSaveAll, SIGNAL(activated()),
	  App, SLOT(slotFileSaveAll()));

  App->fileClose = new QAction(tr("Close File"),
                          QIconSet(QImage(BITMAPDIR "fileclose.png")),
                          tr("&Close"), CTRL+Key_W, App);
  App->fileClose->setStatusTip(tr("Closes the current document"));
  App->fileClose->setWhatsThis(
	tr("Close File\n\nCloses the current document"));
  connect(App->fileClose, SIGNAL(activated()), App, SLOT(slotFileClose()));

  App->fileSettings = new QAction(tr("Settings"), tr("Settings..."), 0, App);
  App->fileSettings->setStatusTip(tr("Settings"));
  App->fileSettings->setWhatsThis(
	tr("Settings\n\nSets properties of the file"));
  connect(App->fileSettings, SIGNAL(activated()),
	  App, SLOT(slotFileSettings()));

  App->filePrint = new QAction(tr("Print File"),
                          QIconSet(QImage(BITMAPDIR "fileprint.png")),
                          tr("&Print..."), CTRL+Key_P, App);
  App->filePrint->setStatusTip(tr("Prints the current document"));
  App->filePrint->setWhatsThis(
	tr("Print File\n\nPrints the current document"));
  connect(App->filePrint, SIGNAL(activated()), App, SLOT(slotFilePrint()));

  App->fileQuit = new QAction(tr("Exit"), tr("E&xit"),
                         QAccel::stringToKey(tr("Ctrl+Q")), App);
  App->fileQuit->setStatusTip(tr("Quits the application"));
  App->fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(App->fileQuit, SIGNAL(activated()), App, SLOT(slotFileQuit()));

  App->editCut = new QAction(tr("Cut"),
                        QIconSet(QImage(BITMAPDIR "editcut.png")),
                        tr("Cu&t"), CTRL+Key_X, App);
  App->editCut->setStatusTip(
	tr("Cuts the selected section and puts it to the clipboard"));
  App->editCut->setWhatsThis(
	tr("Cut\n\nCuts the selected section and puts it to the clipboard"));
  connect(App->editCut, SIGNAL(activated()), App, SLOT(slotEditCut()));

  App->editCopy = new QAction(tr("Copy"),
                         QIconSet(QImage(BITMAPDIR "editcopy.png")),
                         tr("&Copy"), CTRL+Key_C, App);
  App->editCopy->setStatusTip(
	tr("Copies the selected section to the clipboard"));
  App->editCopy->setWhatsThis(
	tr("Copy\n\nCopies the selected section to the clipboard"));
  connect(App->editCopy, SIGNAL(activated()), App, SLOT(slotEditCopy()));

  App->editPaste = new QAction(tr("Paste"),
                          QIconSet(QImage(BITMAPDIR "editpaste.png")),
                          tr("&Paste"), CTRL+Key_V, App);
  App->editPaste->setStatusTip(
	tr("Pastes the clipboard contents to the cursor position"));
  App->editPaste->setWhatsThis(
	tr("Paste\n\nPastes the clipboard contents to the cursor position"));
  App->editPaste->setToggleAction(true);
  connect(App->editPaste, SIGNAL(toggled(bool)),
	  App, SLOT(slotEditPaste(bool)));

  App->editDelete = new QAction(tr("Delete"),
                           QIconSet(QImage(BITMAPDIR "editdelete.png")),
                           tr("&Delete"), Key_Delete, App);
  App->editDelete->setStatusTip(tr("Deletes the selected components"));
  App->editDelete->setWhatsThis(
	tr("Delete\n\nDeletes the selected components"));
  App->editDelete->setToggleAction(true);
  connect(App->editDelete, SIGNAL(toggled(bool)),
	  App, SLOT(slotEditDelete(bool)));
  // to ease usage with notebooks, backspace can also be used to delete
  mainAccel->connectItem(mainAccel->insertItem(Key_Backspace),
                         App->editDelete, SLOT(toggle()) );

  // cursor left/right to move marker on a graph
  mainAccel->connectItem(mainAccel->insertItem(Key_Left),
                         App->view, SLOT(slotMarkerLeft()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Right),
                         App->view, SLOT(slotMarkerRight()));

  App->undo = new QAction(tr("Undo"), QIconSet(QImage(BITMAPDIR "undo.png")),
                     tr("&Undo"), CTRL+Key_Z, App);
  App->undo->setStatusTip(tr("Undoes the last command"));
  App->undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
//  connect(App->undo, SIGNAL(activated()), App, SLOT(slot()));
  App->undo->setEnabled(false);

  App->redo = new QAction(tr("Redo"), QIconSet(QImage(BITMAPDIR "redo.png")),
                     tr("&Redo"), CTRL+Key_Y, App);
  App->redo->setStatusTip(tr("Redoes the last command"));
  App->redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
//  connect(App->redo, SIGNAL(activated()), App, SLOT(slot()));
  App->redo->setEnabled(false);

  App->projNew = new QAction(tr("New Project"), tr("&New Project..."),
			     0, App);
  App->projNew->setStatusTip(tr("Creates a new project"));
  App->projNew->setWhatsThis(tr("New Project\n\nCreates a new project"));
  connect(App->projNew, SIGNAL(activated()), App, SLOT(slotProjNewButt()));

  App->projOpen = new QAction(tr("Open Project"), tr("&Open Project..."),
                         0, App);
  App->projOpen->setStatusTip(tr("Opens a project"));
  App->projOpen->setWhatsThis(
	tr("Open Project\n\nOpens an existing project"));
  connect(App->projOpen, SIGNAL(activated()),
	  App, SLOT(slotMenuOpenProject()));

  App->projDel = new QAction(tr("Delete Project"), tr("&Delete Project..."),
                        0, App);
  App->projDel->setStatusTip(tr("Deletes a project"));
  App->projDel->setWhatsThis(
	tr("Delete Project\n\nDeletes an existing project"));
  connect(App->projDel, SIGNAL(activated()), App, SLOT(slotMenuDelProject()));

  App->magAll = new QAction(tr("View All"),
                       QIconSet(QImage(BITMAPDIR "viewmagfit.png")),
                       tr("View All"), 0, App);
  App->magAll->setStatusTip(tr("Views the whole page"));
  App->magAll->setWhatsThis(tr("View All\n\nShows the whole page content"));
  connect(App->magAll, SIGNAL(activated()), App, SLOT(slotShowAll()));

  App->magOne = new QAction(tr("View 1:1"),
                       QIconSet(QImage(BITMAPDIR "viewmag1.png")),
                       tr("View 1:1"), Key_1, App);
  App->magOne->setStatusTip(tr("Views without magnification"));
  App->magOne->setWhatsThis(
	tr("View 1:1\n\nShows the page content without magnification"));
  connect(App->magOne, SIGNAL(activated()), App, SLOT(slotShowOne()));

  App->magPlus = new QAction(tr("Zoom in"),
                        QIconSet(QImage(BITMAPDIR "viewmag+.png")),
                        tr("Zoom in"), Key_Plus, App);
  App->magPlus->setStatusTip(tr("Zooms into the current view"));
  App->magPlus->setWhatsThis(tr("Zoom in\n\nZooms the current view"));
  connect(App->magPlus, SIGNAL(activated()), App, SLOT(slotZoomIn()));

  App->magMinus = new QAction(tr("Zoom out"),
                         QIconSet(QImage(BITMAPDIR "viewmag-.png")),
                         tr("Zoom out"), Key_Minus, App);
  App->magMinus->setStatusTip(tr("Zooms out the current view"));
  App->magMinus->setWhatsThis(tr("Reduce\n\nZooms out the current view"));
  connect(App->magMinus, SIGNAL(activated()), App, SLOT(slotZoomOut()));

  App->select = new QAction(tr("Select"),
                       QIconSet(QImage(BITMAPDIR "pointer.png")),
                       tr("Select"), Key_Escape, App);
  App->select->setStatusTip(tr("Select mode"));
  App->select->setWhatsThis(tr("Select\n\nSelect mode"));
  App->select->setToggleAction(true);
  connect(App->select, SIGNAL(toggled(bool)), App, SLOT(slotSelect(bool)));

  App->selectAll = new QAction(tr("Select All"), tr("Select All"),
                          CTRL+Key_A, App);
  App->selectAll->setStatusTip(tr("Selects all elements"));
  App->selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(App->selectAll, SIGNAL(activated()), App, SLOT(slotSelectAll()));

  App->editRotate = new QAction(tr("Rotate"),
                           QIconSet(QImage(BITMAPDIR "rotate_ccw.png")),
                           tr("Rotate"), CTRL+Key_R, App);
  App->editRotate->setStatusTip(tr("Rotates the selected component by 90°"));
  App->editRotate->setWhatsThis(
    tr("Rotate\n\nRotates the selected component by 90° counter-clockwise"));
  App->editRotate->setToggleAction(true);
  connect(App->editRotate, SIGNAL(toggled(bool)),
	  App, SLOT(slotEditRotate(bool)));

  App->editMirror = new QAction(tr("Mirror about X Axis"),
                           QIconSet(QImage(BITMAPDIR "mirror.png")),
                           tr("Mirror about X Axis"), 0, App);
  App->editMirror->setStatusTip(tr("Mirrors the selected item about X axis"));
  App->editMirror->setWhatsThis(
	tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  App->editMirror->setToggleAction(true);
  connect(App->editMirror, SIGNAL(toggled(bool)),
	  App, SLOT(slotEditMirrorX(bool)));

  App->editMirrorY = new QAction(tr("Mirror about Y Axis"),
                            QIconSet(QImage(BITMAPDIR "mirrory.png")),
                            tr("Mirror about Y Axis"), CTRL+Key_M, App);
  App->editMirrorY->setStatusTip(
	tr("Mirrors the selected item about Y axis"));
  App->editMirrorY->setWhatsThis(
	tr("Mirror about Y Axis\n\nMirrors the selected item about Y Axis"));
  App->editMirrorY->setToggleAction(true);
  connect(App->editMirrorY, SIGNAL(toggled(bool)),
	App, SLOT(slotEditMirrorY(bool)));

  App->intoH = new QAction(tr("Go into Subcircuit"),
                      QIconSet(QImage(BITMAPDIR "bottom.png")),
                      tr("Go into Subcircuit"), CTRL+Key_I, App);
  App->intoH->setStatusTip(tr("Goes inside subcircuit"));
  App->intoH->setWhatsThis(
	tr("Go into Subcircuit\n\nGoes inside the selected subcircuit"));
  connect(App->intoH, SIGNAL(activated()), App, SLOT(slotIntoHierarchy()));
//  App->intoH->setEnabled(false);

  App->popH = new QAction(tr("Pop out"),
		     QIconSet(QImage(BITMAPDIR "top.png")),
                     tr("Pop out"), CTRL+Key_H, App);
  App->popH->setStatusTip(tr("Pop outside subcircuit"));
  App->popH->setWhatsThis(
	tr("Pop out\n\nGoes up one hierarchy level, i.e. leaves subcircuit"));
  connect(App->popH, SIGNAL(activated()), App, SLOT(slotPopHierarchy()));
  App->popH->setEnabled(false);  // only enabled if useful !!!!

  App->editActivate = new QAction(tr("Deactivate/Activate"),
                             QIconSet(QImage(BITMAPDIR "deactiv.png")),
                             tr("Deactivate/Activate"), CTRL+Key_D, App);
  App->editActivate->setStatusTip(
	tr("Deactivate/Activate the selected item"));
  App->editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected item"));
  App->editActivate->setToggleAction(true);
  connect(App->editActivate, SIGNAL(toggled(bool)),
	  App, SLOT(slotEditActivate(bool)));

  App->insEquation = new QAction(tr("Insert Equation"),
                            QIconSet(QImage(BITMAPDIR "equation.png")),
                            tr("Insert Equation"), 0, App);
  App->insEquation->setStatusTip(tr("Inserts equation"));
  App->insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  App->insEquation->setToggleAction(true);
  connect(App->insEquation, SIGNAL(toggled(bool)),
	App, SLOT(slotInsertEquation(bool)));

  App->insGround = new QAction(tr("Insert Ground"),
                          QIconSet(QImage(BITMAPDIR "ground.png")),
                          tr("Insert Ground"), CTRL+Key_G, App);
  App->insGround->setStatusTip(tr("Inserts ground"));
  App->insGround->setWhatsThis(
	tr("Insert Ground\n\nInserts a ground symbol"));
  App->insGround->setToggleAction(true);
  connect(App->insGround, SIGNAL(toggled(bool)),
	  App, SLOT(slotInsertGround(bool)));

  App->insPort = new QAction(tr("Insert Port"),
                        QIconSet(QImage(BITMAPDIR "port.png")),
                        tr("Insert Port"), 0, App);
  App->insPort->setStatusTip(tr("Inserts port"));
  App->insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  App->insPort->setToggleAction(true);
  connect(App->insPort, SIGNAL(toggled(bool)),
	  App, SLOT(slotInsertPort(bool)));

  App->insWire = new QAction(tr("Insert Wire"),
                        QIconSet(QImage(BITMAPDIR "wire.png")),
                        tr("Wire"), CTRL+Key_E, App);
  App->insWire->setStatusTip(tr("Inserts a wire"));
  App->insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  App->insWire->setToggleAction(true);
  connect(App->insWire, SIGNAL(toggled(bool)), App, SLOT(slotSetWire(bool)));

  App->insLabel = new QAction(tr("Insert Wire/Pin Label"),
                         QIconSet(QImage(BITMAPDIR "nodename.png")),
                         tr("Wire Label"), CTRL+Key_L, App);
  App->insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  App->insLabel->setWhatsThis(
	tr("Wire Label\n\nInserts a wire or pin label"));
  App->insLabel->setToggleAction(true);
  connect(App->insLabel, SIGNAL(toggled(bool)),
	  App, SLOT(slotInsertLabel(bool)));

  App->simulate = new QAction(tr("Simulate"),
                         QIconSet(QImage(BITMAPDIR "gear.png")),
                         tr("Simulate"), Key_F2, App);
  App->simulate->setStatusTip(tr("Simulates the current schematic"));
  App->simulate->setWhatsThis(
	tr("Simulate\n\nSimulates the current schematic"));
  connect(App->simulate, SIGNAL(activated()), App, SLOT(slotSimulate()));

  App->dpl_sch = new QAction(tr("View Data Display/Schematic"),
                        QIconSet(QImage(BITMAPDIR "rebuild.png")),
                        tr("View Data Display/Schematic"), Key_F4, App);
  App->dpl_sch->setStatusTip(tr("Changes to data display or schematic page"));
  App->dpl_sch->setWhatsThis(
	tr("View Data Display/Schematic\n\n")+
	tr("Changes to data display or schematic page"));
  connect(App->dpl_sch, SIGNAL(activated()), App, SLOT(slotChangePage()));

  App->setMarker = new QAction(tr("Set Marker"),
                          QIconSet(QImage(BITMAPDIR "marker.png")),
                          tr("Set Marker on Graph"), CTRL+Key_B, App);
  App->setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  App->setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  App->setMarker->setToggleAction(true);
  connect(App->setMarker, SIGNAL(toggled(bool)),
	App, SLOT(slotSetMarker(bool)));

  App->showMsg = new QAction(tr("Show Last Messages"),
                        tr("Show Last Messages"), 0, App);
  App->showMsg->setStatusTip(tr("Shows last simulation messages"));
  App->showMsg->setWhatsThis(
    tr("Show Last Messages\n\nShows the messages of the last simulation"));
  connect(App->showMsg, SIGNAL(activated()), App, SLOT(slotShowLastMsg()));

  App->showNet = new QAction(tr("Show Last Netlist"), tr("Show Last Netlist"),
                        0, App);
  App->showNet->setStatusTip(tr("Shows last simulation netlist"));
  App->showNet->setWhatsThis(
	tr("Show Last Netlist\n\nShows the netlist of the last simulation"));
  connect(App->showNet, SIGNAL(activated()),
	  App, SLOT(slotShowLastNetlist()));

  viewToolBar = new QAction(tr("Toolbar"), tr("Tool&bar"),
                            0, App, 0, true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)),
	  this, SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("Statusbar"), tr("&Statusbar"),
                              0, App, 0, true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(
	tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)),
	  this, SLOT(slotViewStatusBar(bool)));

  App->helpIndex = new QAction(tr("Help Index"), tr("Help Index..."),
                          Key_F1, App);
  App->helpIndex->setStatusTip(tr("Index of Qucs Help"));
  App->helpIndex->setWhatsThis(tr("Help Index\n\nIndex of intern Qucs help"));
  connect(App->helpIndex, SIGNAL(activated()), App, SLOT(slotHelpIndex()));

  App->helpGetStart = new QAction(tr("Getting Started"),
                             tr("Getting Started..."), 0, App);
  App->helpGetStart->setStatusTip(tr("Getting Started with Qucs"));
  App->helpGetStart->setWhatsThis(
	tr("Getting Started\n\nShort introduction into Qucs"));
  connect(App->helpGetStart, SIGNAL(activated()),
	  App, SLOT(slotGettingStarted()));

  helpAboutApp = new QAction(tr("About"), tr("&About Qucs..."), 0, App);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), SLOT(slotHelpAbout()));

  helpAboutQt = new QAction(tr("About Qt"), tr("&About Qt..."), 0, App);
  helpAboutQt->setStatusTip(tr("About Qt"));
  helpAboutQt->setWhatsThis(tr("About Qt\n\nAbout Qt by Trolltech"));
  connect(helpAboutQt, SIGNAL(activated()), SLOT(slotHelpAboutQt()));
}

// #########################################################################
void QucsInit::initMenuBar()
{
  fileMenu=new QPopupMenu();  // menuBar entry fileMenu
  App->fileNew->addTo(fileMenu);
  App->fileOpen->addTo(fileMenu);
  App->fileClose->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->fileSave->addTo(fileMenu);
  App->fileSaveAll->addTo(fileMenu);
  App->fileSaveAs->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->fileSettings->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->filePrint->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->fileQuit->addTo(fileMenu);

  editMenu=new QPopupMenu();  // menuBar entry editMenu
  App->undo->addTo(editMenu);
  App->redo->addTo(editMenu);
  editMenu->insertSeparator();
  App->editCut->addTo(editMenu);
  App->editCopy->addTo(editMenu);
  App->editPaste->addTo(editMenu);
  App->editDelete->addTo(editMenu);
  editMenu->insertSeparator();
  App->select->addTo(editMenu);
  App->selectAll->addTo(editMenu);
  App->editRotate->addTo(editMenu);
  App->editMirror->addTo(editMenu);
  App->editMirrorY->addTo(editMenu);
  App->editActivate->addTo(editMenu);
  editMenu->insertSeparator();
  App->intoH->addTo(editMenu);
  App->popH->addTo(editMenu);

  insMenu=new QPopupMenu();  // menuBar entry insMenu
  App->insWire->addTo(insMenu);
  App->insLabel->addTo(insMenu);
  App->insEquation->addTo(insMenu);
  App->insGround->addTo(insMenu);
  App->insPort->addTo(insMenu);
  App->setMarker->addTo(insMenu);

  projMenu=new QPopupMenu();  // menuBar entry projMenu
  App->projNew->addTo(projMenu);
  App->projOpen->addTo(projMenu);
  App->projDel->addTo(projMenu);

  simMenu=new QPopupMenu();  // menuBar entry simMenu
  App->simulate->addTo(simMenu);
  App->dpl_sch->addTo(simMenu);
  App->showMsg->addTo(simMenu);
  App->showNet->addTo(simMenu);

  viewMenu=new QPopupMenu();  // menuBar entry viewMenu
  App->magAll->addTo(viewMenu);
  App->magOne->addTo(viewMenu);
  App->magPlus->addTo(viewMenu);
  App->magMinus->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);

  helpMenu=new QPopupMenu();  // menuBar entry helpMenu
  App->helpIndex->addTo(helpMenu);
  App->helpGetStart->addTo(helpMenu);
  helpMenu->insertSeparator();
  helpAboutApp->addTo(helpMenu);
  helpAboutQt->addTo(helpMenu);

  App->menuBar()->insertItem(tr("&File"), fileMenu);  // MENUBAR CONFIGURATION
  App->menuBar()->insertItem(tr("&Edit"), editMenu);
  App->menuBar()->insertItem(tr("&Insert"), insMenu);
  App->menuBar()->insertItem(tr("&Project"), projMenu);
  App->menuBar()->insertItem(tr("&Simulation"), simMenu);
  App->menuBar()->insertItem(tr("&View"), viewMenu);
  App->menuBar()->insertSeparator();
  App->menuBar()->insertItem(tr("&Help"), helpMenu);

}

// #########################################################################
void QucsInit::initToolBar()
{
  fileToolbar = new QToolBar(App);
  App->fileNew->addTo(fileToolbar);
  App->fileOpen->addTo(fileToolbar);
  App->fileSave->addTo(fileToolbar);
  App->fileSaveAll->addTo(fileToolbar);
  App->fileClose->addTo(fileToolbar);
  App->filePrint->addTo(fileToolbar);

  editToolbar = new QToolBar(App);
  App->editCut->addTo(editToolbar);
  App->editCopy->addTo(editToolbar);
  App->editPaste->addTo(editToolbar);
  App->editDelete->addTo(editToolbar);
  App->undo->addTo(editToolbar);
  App->redo->addTo(editToolbar);

  viewToolbar = new QToolBar(App);
  App->magAll->addTo(viewToolbar);
  App->magOne->addTo(viewToolbar);
  App->magPlus->addTo(viewToolbar);
  App->magMinus->addTo(viewToolbar);

  workToolbar = new QToolBar(App);
  App->select->addTo(workToolbar);
  App->editMirror->addTo(workToolbar);
  App->editMirrorY->addTo(workToolbar);
  App->editRotate->addTo(workToolbar);
  App->intoH->addTo(workToolbar);
  App->popH->addTo(workToolbar);
  App->editActivate->addTo(workToolbar);
  App->insWire->addTo(workToolbar);
  App->insLabel->addTo(workToolbar);
  App->insEquation->addTo(workToolbar);
  App->insGround->addTo(workToolbar);
  App->insPort->addTo(workToolbar);
  App->simulate->addTo(workToolbar);
  App->dpl_sch->addTo(workToolbar);
  App->setMarker->addTo(workToolbar);
  workToolbar->addSeparator();    // <<<=======================
  QWhatsThis::whatsThisButton(workToolbar);

}

// #########################################################################
// Statusbar
void QucsInit::initStatusBar()
{
  App->statusBar()->message(tr("Ready."), 2000);
}

// ######################################################################
// turn Toolbar on or off
void QucsInit::slotViewToolBar(bool toggle)
{
  App->statusBar()->message(tr("Toggle toolbar..."));

  if (toggle== false) {
    fileToolbar->hide();
    editToolbar->hide();
    viewToolbar->hide();
    workToolbar->hide();
  }
  else {
    fileToolbar->show();
    editToolbar->show();
    viewToolbar->show();
    workToolbar->show();
  }

  App->statusBar()->message(tr("Ready."));
}

// ########################################################################
// turn Statusbar on or off
void QucsInit::slotViewStatusBar(bool toggle)
{
  App->statusBar()->message(tr("Toggle statusbar..."));

  if (toggle == false) App->statusBar()->hide();
  else App->statusBar()->show();

  App->statusBar()->message(tr("Ready."));
}

// ########################################################################
void QucsInit::slotHelpAbout()
{
  QMessageBox::about(App, tr("About..."),
    tr("Qucs Version ")+PACKAGE_VERSION+tr("\nQt universal circuit simulator\n")+
    tr("Copyright (C) 2003,2004 by Michael Margraf\n")+
    tr("Simulator by Stefan Jahn\n")+
    tr("Special thanks to Jens Flucke\n\n")+
    tr("Translations:\n")+
    tr("German by Stefan Jahn\n")+
    tr("Polish by Dariusz Pienkowski\n")+
    tr("French by Eric Marzolf"));
}

// ########################################################################
void QucsInit::slotHelpAboutQt()
{
  QMessageBox::aboutQt(App, tr("About Qt"));
}
