/***************************************************************************
                          qucsinit.cpp  -  description
                             -------------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucsinit.h"
#include "qucs.h"
#include "qucsactions.h"

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
  App  = p_;
  Acts = &(App->Acts);

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

  App->symEdit = new QAction(tr("Edit Circuit Symbol"),
				tr("Edit Circuit Symbol"), CTRL+Key_U, App);
  App->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
  App->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  connect(App->symEdit, SIGNAL(activated()), App, SLOT(slotSymbolEdit()));

  App->fileSettings = new QAction(tr("Document Settings"),
				  tr("Document Settings..."), 0, App);
  App->fileSettings->setStatusTip(tr("Document Settings"));
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

  App->filePrintSel = new QAction(tr("Print Selected Elements"),
			tr("Print Selection..."), 0, App);
  App->filePrintSel->setStatusTip(tr("Prints Selected Elements"));
  App->filePrintSel->setWhatsThis(
	tr("Print Selected Elements\n\n"
	   "Prints selected elements of the current document"));
  connect(App->filePrintSel, SIGNAL(activated()),
	  App, SLOT(slotFilePrintSelected()));

  App->fileQuit = new QAction(tr("Exit"), tr("E&xit"), CTRL+Key_Q, App);
  App->fileQuit->setStatusTip(tr("Quits the application"));
  App->fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(App->fileQuit, SIGNAL(activated()), App, SLOT(slotFileQuit()));

  App->applSettings = new QAction(tr("Application Settings"),
				  tr("Application Settings..."), 0, App);
  App->applSettings->setStatusTip(tr("Application Settings"));
  App->applSettings->setWhatsThis(
	tr("Qucs Settings\n\nSets properties of the application"));
  connect(App->applSettings, SIGNAL(activated()),
	  App, SLOT(slotApplSettings()));

  App->alignTop = new QAction(tr("Align top"), tr("Align top"),
				CTRL+Key_T, App);
  App->alignTop->setStatusTip(tr("Align top selected elements"));
  App->alignTop->setWhatsThis(
	tr("Align top\n\nAlign selected elements to their upper edge"));
  connect(App->alignTop, SIGNAL(activated()), App, SLOT(slotAlignTop()));

  App->alignBottom = new QAction(tr("Align bottom"), tr("Align bottom"),
					0, App);
  App->alignBottom->setStatusTip(tr("Align bottom selected elements"));
  App->alignBottom->setWhatsThis(
	tr("Align bottom\n\nAlign selected elements to their lower edge"));
  connect(App->alignBottom, SIGNAL(activated()),
	  App, SLOT(slotAlignBottom()));

  App->alignLeft = new QAction(tr("Align left"), tr("Align left"), 0, App);
  App->alignLeft->setStatusTip(tr("Align left selected elements"));
  App->alignLeft->setWhatsThis(
	tr("Align left\n\nAlign selected elements to their left edge"));
  connect(App->alignLeft, SIGNAL(activated()), App, SLOT(slotAlignLeft()));

  App->alignRight = new QAction(tr("Align right"), tr("Align right"), 0, App);
  App->alignRight->setStatusTip(tr("Align right selected elements"));
  App->alignRight->setWhatsThis(
	tr("Align right\n\nAlign selected elements to their right edge"));
  connect(App->alignRight, SIGNAL(activated()), App, SLOT(slotAlignRight()));

  App->distrHor = new QAction(tr("Distribute horizontally"),
	tr("Distribute horizontally"), 0, App);
  App->distrHor->setStatusTip(tr("Distribute equally horizontally"));
  App->distrHor->setWhatsThis(
	tr("Distribute horizontally\n\n"
	   "Distribute horizontally selected elements"));
  connect(App->distrHor, SIGNAL(activated()), App, SLOT(slotDistribHoriz()));

  App->distrVert = new QAction(tr("Distribute vertically"),
	tr("Distribute vertically"), 0, App);
  App->distrVert->setStatusTip(tr("Distribute equally vertically"));
  App->distrVert->setWhatsThis(
	tr("Distribute vertically\n\n"
	   "Distribute vertically selected elements"));
  connect(App->distrVert, SIGNAL(activated()), App, SLOT(slotDistribVert()));

  Acts->onGrid = new QAction(tr("Set on Grid"), tr("Set on Grid"), 0, App);
  Acts->onGrid->setStatusTip(tr("Set on Grid"));
  Acts->onGrid->setWhatsThis(
	tr("Set on Grid\n\nSets selected elements on grid"));
  Acts->onGrid->setToggleAction(true);
  connect(Acts->onGrid, SIGNAL(toggled(bool)), Acts, SLOT(slotOnGrid(bool)));

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

  Acts->editPaste = new QAction(tr("Paste"),
			QIconSet(QImage(BITMAPDIR "editpaste.png")),
			tr("&Paste"), CTRL+Key_V, App);
  Acts->editPaste->setStatusTip(
	tr("Pastes the clipboard contents to the cursor position"));
  Acts->editPaste->setWhatsThis(
	tr("Paste\n\nPastes the clipboard contents to the cursor position"));
  Acts->editPaste->setToggleAction(true);
  connect(Acts->editPaste, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditPaste(bool)));

  Acts->editDelete = new QAction(tr("Delete"),
                           QIconSet(QImage(BITMAPDIR "editdelete.png")),
                           tr("&Delete"), Key_Delete, App);
  Acts->editDelete->setStatusTip(tr("Deletes the selected components"));
  Acts->editDelete->setWhatsThis(
	tr("Delete\n\nDeletes the selected components"));
  Acts->editDelete->setToggleAction(true);
  connect(Acts->editDelete, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditDelete(bool)));
  // to ease usage with notebooks, backspace can also be used to delete
  mainAccel->connectItem(mainAccel->insertItem(Key_Backspace),
                         Acts->editDelete, SLOT(toggle()) );

  // cursor left/right to move marker on a graph
  mainAccel->connectItem(mainAccel->insertItem(Key_Left),
                         App->view, SLOT(slotCursorLeft()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Right),
                         App->view, SLOT(slotCursorRight()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Up),
                         App->view, SLOT(slotCursorUp()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Down),
                         App->view, SLOT(slotCursorDown()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Tab),
                         App, SLOT(slotNextTab()));

  App->undo = new QAction(tr("Undo"), QIconSet(QImage(BITMAPDIR "undo.png")),
			  tr("&Undo"), CTRL+Key_Z, App);
  App->undo->setStatusTip(tr("Undoes the last command"));
  App->undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
  connect(App->undo, SIGNAL(activated()), App, SLOT(slotEditUndo()));

  App->redo = new QAction(tr("Redo"), QIconSet(QImage(BITMAPDIR "redo.png")),
			  tr("&Redo"), CTRL+Key_Y, App);
  App->redo->setStatusTip(tr("Redoes the last command"));
  App->redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
  connect(App->redo, SIGNAL(activated()), App, SLOT(slotEditRedo()));

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

  App->projClose = new QAction(tr("Close Project"), tr("&Close Project"),
				0, App);
  App->projClose->setStatusTip(tr("Close current project"));
  App->projClose->setWhatsThis(
	tr("Close Project\n\nCloses the current project"));
  connect(App->projClose, SIGNAL(activated()),
	  App, SLOT(slotMenuCloseProject()));

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

  Acts->select = new QAction(tr("Select"),
                       QIconSet(QImage(BITMAPDIR "pointer.png")),
                       tr("Select"), Key_Escape, App);
  Acts->select->setStatusTip(tr("Select mode"));
  Acts->select->setWhatsThis(tr("Select\n\nSelect mode"));
  Acts->select->setToggleAction(true);
  connect(Acts->select, SIGNAL(toggled(bool)), Acts, SLOT(slotSelect(bool)));

  App->selectAll = new QAction(tr("Select All"), tr("Select All"),
                          CTRL+Key_A, App);
  App->selectAll->setStatusTip(tr("Selects all elements"));
  App->selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(App->selectAll, SIGNAL(activated()), App, SLOT(slotSelectAll()));

  Acts->editRotate = new QAction(tr("Rotate"),
                           QIconSet(QImage(BITMAPDIR "rotate_ccw.png")),
                           tr("Rotate"), CTRL+Key_R, App);
  Acts->editRotate->setStatusTip(tr("Rotates the selected component by 90°"));
  Acts->editRotate->setWhatsThis(
    tr("Rotate\n\nRotates the selected component by 90° counter-clockwise"));
  Acts->editRotate->setToggleAction(true);
  connect(Acts->editRotate, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditRotate(bool)));

  Acts->editMirror = new QAction(tr("Mirror about X Axis"),
			  QIconSet(QImage(BITMAPDIR "mirror.png")),
			  tr("Mirror about X Axis"), CTRL+Key_J, App);
  Acts->editMirror->setStatusTip(
			  tr("Mirrors the selected item about X axis"));
  Acts->editMirror->setWhatsThis(
	tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  Acts->editMirror->setToggleAction(true);
  connect(Acts->editMirror, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditMirrorX(bool)));

  Acts->editMirrorY = new QAction(tr("Mirror about Y Axis"),
			    QIconSet(QImage(BITMAPDIR "mirrory.png")),
			    tr("Mirror about Y Axis"), CTRL+Key_M, App);
  Acts->editMirrorY->setStatusTip(
	tr("Mirrors the selected item about Y axis"));
  Acts->editMirrorY->setWhatsThis(
	tr("Mirror about Y Axis\n\nMirrors the selected item about Y Axis"));
  Acts->editMirrorY->setToggleAction(true);
  connect(Acts->editMirrorY, SIGNAL(toggled(bool)),
	Acts, SLOT(slotEditMirrorY(bool)));

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

  Acts->editActivate = new QAction(tr("Deactivate/Activate"),
                             QIconSet(QImage(BITMAPDIR "deactiv.png")),
                             tr("Deactivate/Activate"), CTRL+Key_D, App);
  Acts->editActivate->setStatusTip(
	tr("Deactivate/Activate the selected item"));
  Acts->editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected item"));
  Acts->editActivate->setToggleAction(true);
  connect(Acts->editActivate, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditActivate(bool)));

  Acts->insEquation = new QAction(tr("Insert Equation"),
                            QIconSet(QImage(BITMAPDIR "equation.png")),
                            tr("Insert Equation"), 0, App);
  Acts->insEquation->setStatusTip(tr("Inserts equation"));
  Acts->insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  Acts->insEquation->setToggleAction(true);
  connect(Acts->insEquation, SIGNAL(toggled(bool)),
	Acts, SLOT(slotInsertEquation(bool)));

  Acts->insGround = new QAction(tr("Insert Ground"),
                          QIconSet(QImage(BITMAPDIR "ground.png")),
                          tr("Insert Ground"), CTRL+Key_G, App);
  Acts->insGround->setStatusTip(tr("Inserts ground"));
  Acts->insGround->setWhatsThis(
	tr("Insert Ground\n\nInserts a ground symbol"));
  Acts->insGround->setToggleAction(true);
  connect(Acts->insGround, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotInsertGround(bool)));

  Acts->insPort = new QAction(tr("Insert Port"),
                        QIconSet(QImage(BITMAPDIR "port.png")),
                        tr("Insert Port"), 0, App);
  Acts->insPort->setStatusTip(tr("Inserts port"));
  Acts->insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  Acts->insPort->setToggleAction(true);
  connect(Acts->insPort, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotInsertPort(bool)));

  Acts->insWire = new QAction(tr("Insert Wire"),
                        QIconSet(QImage(BITMAPDIR "wire.png")),
                        tr("Wire"), CTRL+Key_E, App);
  Acts->insWire->setStatusTip(tr("Inserts a wire"));
  Acts->insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  Acts->insWire->setToggleAction(true);
  connect(Acts->insWire, SIGNAL(toggled(bool)), Acts,
			 SLOT(slotSetWire(bool)));

  Acts->insLabel = new QAction(tr("Insert Wire/Pin Label"),
                         QIconSet(QImage(BITMAPDIR "nodename.png")),
                         tr("Wire Label"), CTRL+Key_L, App);
  Acts->insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  Acts->insLabel->setWhatsThis(
	tr("Wire Label\n\nInserts a wire or pin label"));
  Acts->insLabel->setToggleAction(true);
  connect(Acts->insLabel, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotInsertLabel(bool)));

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
  connect(App->dpl_sch, SIGNAL(activated()), App, SLOT(slotToPage()));

  Acts->setMarker = new QAction(tr("Set Marker"),
                          QIconSet(QImage(BITMAPDIR "marker.png")),
                          tr("Set Marker on Graph"), CTRL+Key_B, App);
  Acts->setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  Acts->setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  Acts->setMarker->setToggleAction(true);
  connect(Acts->setMarker, SIGNAL(toggled(bool)),
	Acts, SLOT(slotSetMarker(bool)));

  App->showMsg = new QAction(tr("Show Last Messages"),
                        tr("Show Last Messages"), Key_F5, App);
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
  fileMenu = new QPopupMenu();  // menuBar entry fileMenu
  App->fileNew->addTo(fileMenu);
  App->fileOpen->addTo(fileMenu);
  App->fileClose->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->fileSave->addTo(fileMenu);
  App->fileSaveAll->addTo(fileMenu);
  App->fileSaveAs->addTo(fileMenu);
  App->filePrint->addTo(fileMenu);
  App->filePrintSel->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->fileSettings->addTo(fileMenu);
  App->symEdit->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->applSettings->addTo(fileMenu);
  fileMenu->insertSeparator();
  App->fileQuit->addTo(fileMenu);

  alignMenu = new QPopupMenu();  // submenu for "editMenu"
  App->alignTop->addTo(alignMenu);
  App->alignBottom->addTo(alignMenu);
  App->alignLeft->addTo(alignMenu);
  App->alignRight->addTo(alignMenu);
  alignMenu->insertSeparator();
  App->distrHor->addTo(alignMenu);
  App->distrVert->addTo(alignMenu);

  editMenu = new QPopupMenu();  // menuBar entry editMenu
  App->undo->addTo(editMenu);
  App->redo->addTo(editMenu);
  editMenu->insertSeparator();
  App->editCut->addTo(editMenu);
  App->editCopy->addTo(editMenu);
  Acts->editPaste->addTo(editMenu);
  Acts->editDelete->addTo(editMenu);
  editMenu->insertSeparator();
  Acts->select->addTo(editMenu);
  App->selectAll->addTo(editMenu);
  Acts->editRotate->addTo(editMenu);
  Acts->editMirror->addTo(editMenu);
  Acts->editMirrorY->addTo(editMenu);
  Acts->editActivate->addTo(editMenu);
  editMenu->insertItem(tr("Align"), alignMenu);
  Acts->onGrid->addTo(editMenu);
  editMenu->insertSeparator();
  App->intoH->addTo(editMenu);
  App->popH->addTo(editMenu);

  insMenu = new QPopupMenu();  // menuBar entry insMenu
  Acts->insWire->addTo(insMenu);
  Acts->insLabel->addTo(insMenu);
  Acts->insEquation->addTo(insMenu);
  Acts->insGround->addTo(insMenu);
  Acts->insPort->addTo(insMenu);
  Acts->setMarker->addTo(insMenu);

  projMenu = new QPopupMenu();  // menuBar entry projMenu
  App->projNew->addTo(projMenu);
  App->projOpen->addTo(projMenu);
  App->projClose->addTo(projMenu);
  App->projDel->addTo(projMenu);

  simMenu = new QPopupMenu();  // menuBar entry simMenu
  App->simulate->addTo(simMenu);
  App->dpl_sch->addTo(simMenu);
  App->showMsg->addTo(simMenu);
  App->showNet->addTo(simMenu);

  viewMenu = new QPopupMenu();  // menuBar entry viewMenu
  App->magAll->addTo(viewMenu);
  App->magOne->addTo(viewMenu);
  App->magPlus->addTo(viewMenu);
  App->magMinus->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);

  helpMenu = new QPopupMenu();  // menuBar entry helpMenu
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
  Acts->editPaste->addTo(editToolbar);
  Acts->editDelete->addTo(editToolbar);
  App->undo->addTo(editToolbar);
  App->redo->addTo(editToolbar);

  viewToolbar = new QToolBar(App);
  App->magAll->addTo(viewToolbar);
  App->magOne->addTo(viewToolbar);
  App->magPlus->addTo(viewToolbar);
  App->magMinus->addTo(viewToolbar);

  workToolbar = new QToolBar(App);
  Acts->select->addTo(workToolbar);
  Acts->editMirror->addTo(workToolbar);
  Acts->editMirrorY->addTo(workToolbar);
  Acts->editRotate->addTo(workToolbar);
  App->intoH->addTo(workToolbar);
  App->popH->addTo(workToolbar);
  Acts->editActivate->addTo(workToolbar);
  Acts->insWire->addTo(workToolbar);
  Acts->insLabel->addTo(workToolbar);
  Acts->insEquation->addTo(workToolbar);
  Acts->insGround->addTo(workToolbar);
  Acts->insPort->addTo(workToolbar);
  App->simulate->addTo(workToolbar);
  App->dpl_sch->addTo(workToolbar);
  Acts->setMarker->addTo(workToolbar);
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
    tr("Qucs Version ")+PACKAGE_VERSION+
    tr("\nQt universal circuit simulator\n")+
    tr("Copyright (C) 2003,2004 by Michael Margraf\n")+
    tr("Simulator by Stefan Jahn\n")+
    tr("Special thanks to Jens Flucke\n\n")+
    tr("Translations:\n")+
    tr("German by Stefan Jahn\n")+
    tr("Polish by Dariusz Pienkowski\n")+
    tr("Romanian by Radu Circa\n")+
    tr("French by Eric Marzolf\n")+
    tr("Portuguese by Luciano Franca\n")+
    tr("Spanish by Jose L. Redrejo Rodriguez\n")+
    tr("Italian by Giorgio Luparia"));
}

// ########################################################################
void QucsInit::slotHelpAboutQt()
{
  QMessageBox::aboutQt(App, tr("About Qt"));
}
