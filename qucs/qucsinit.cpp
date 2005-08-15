/***************************************************************************
                               qucsinit.cpp
                              --------------
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

#include <qaction.h>
#include <qaccel.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qtimer.h>

#include "qucs.h"
#include "qucsview.h"
#include "qucsinit.h"
#include "qucsactions.h"


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

  App->fileNew =
    new QAction(tr("New"),
		QIconSet(QImage(QucsSettings.BitmapDir + "filenew.png")),
		tr("&New"), CTRL+Key_N, App);
  App->fileNew->setStatusTip(tr("Creates a new document"));
  App->fileNew->setWhatsThis(
	        tr("New\n\nCreates a new schematic or data display document"));
  connect(App->fileNew, SIGNAL(activated()), App, SLOT(slotFileNew()));

  App->fileOpen =
    new QAction(tr("Open File"),
		QIconSet(QImage(QucsSettings.BitmapDir + "fileopen.png")),
		tr("&Open..."), CTRL+Key_O, App);
  App->fileOpen->setStatusTip(tr("Opens an existing document"));
  App->fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(App->fileOpen, SIGNAL(activated()), App, SLOT(slotFileOpen()));

  App->fileSave =
    new QAction(tr("Save File"),
		QIconSet(QImage(QucsSettings.BitmapDir + "filesave.png")),
		tr("&Save"), CTRL+Key_S, App);
  App->fileSave->setStatusTip(tr("Saves the current document"));
  App->fileSave->setWhatsThis(tr("Save File\n\nSaves the current document"));
  connect(App->fileSave, SIGNAL(activated()), App, SLOT(slotFileSave()));

  App->fileSaveAs =
    new QAction(tr("Save File As"), tr("Save &as..."), CTRL+Key_Minus, App);
  App->fileSaveAs->setStatusTip(
	tr("Saves the current document under a new filename"));
  App->fileSaveAs->setWhatsThis(
	tr("Save As\n\nSaves the current document under a new filename"));
  connect(App->fileSaveAs, SIGNAL(activated()), App, SLOT(slotFileSaveAs()));

  App->fileSaveAll =
    new QAction(tr("Save All Files"),
		QIconSet(QImage(QucsSettings.BitmapDir + "filesaveall.png")),
		tr("Save &All"), CTRL+Key_Plus, App);
  App->fileSaveAll->setStatusTip(tr("Saves all open documents"));
  App->fileSaveAll->setWhatsThis(
	tr("Save All Files\n\nSaves all open documents"));
  connect(App->fileSaveAll, SIGNAL(activated()), App, SLOT(slotFileSaveAll()));

  App->fileClose =
    new QAction(tr("Close File"),
		QIconSet(QImage(QucsSettings.BitmapDir + "fileclose.png")),
		tr("&Close"), CTRL+Key_W, App);
  App->fileClose->setStatusTip(tr("Closes the current document"));
  App->fileClose->setWhatsThis(
	tr("Close File\n\nCloses the current document"));
  connect(App->fileClose, SIGNAL(activated()), App, SLOT(slotFileClose()));

  App->symEdit =
    new QAction(tr("Edit Circuit Symbol"),
		tr("Edit Circuit Symbol"), Key_F3, App);
  App->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
  App->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  connect(App->symEdit, SIGNAL(activated()), App, SLOT(slotSymbolEdit()));

  App->fileSettings =
    new QAction(tr("Document Settings"),
		tr("Document Settings..."), CTRL+Key_Period, App);
  App->fileSettings->setStatusTip(tr("Document Settings"));
  App->fileSettings->setWhatsThis(
	tr("Settings\n\nSets properties of the file"));
  connect(App->fileSettings, SIGNAL(activated()),
	  App, SLOT(slotFileSettings()));

  App->filePrint =
    new QAction(tr("Print File"),
		QIconSet(QImage(QucsSettings.BitmapDir + "fileprint.png")),
		tr("&Print..."), CTRL+Key_P, App);
  App->filePrint->setStatusTip(tr("Prints the current document"));
  App->filePrint->setWhatsThis(
	tr("Print File\n\nPrints the current document"));
  connect(App->filePrint, SIGNAL(activated()), App, SLOT(slotFilePrint()));

  App->filePrintSel =
    new QAction(tr("Print Selected Elements"),
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

  App->applSettings =
    new QAction(tr("Application Settings"),
		tr("Application Settings..."), CTRL+Key_Comma, App);
  App->applSettings->setStatusTip(tr("Application Settings"));
  App->applSettings->setWhatsThis(
	tr("Qucs Settings\n\nSets properties of the application"));
  connect(App->applSettings, SIGNAL(activated()),
	  App, SLOT(slotApplSettings()));

  Acts->alignTop =
    new QAction(tr("Align top"), tr("Align top"), CTRL+Key_T, App);
  Acts->alignTop->setStatusTip(tr("Align top selected elements"));
  Acts->alignTop->setWhatsThis(
	tr("Align top\n\nAlign selected elements to their upper edge"));
  connect(Acts->alignTop, SIGNAL(activated()), Acts, SLOT(slotAlignTop()));

  Acts->alignBottom =
    new QAction(tr("Align bottom"), tr("Align bottom"), 0, App);
  Acts->alignBottom->setStatusTip(tr("Align bottom selected elements"));
  Acts->alignBottom->setWhatsThis(
	tr("Align bottom\n\nAlign selected elements to their lower edge"));
  connect(Acts->alignBottom, SIGNAL(activated()),
	  Acts, SLOT(slotAlignBottom()));

  Acts->alignLeft = new QAction(tr("Align left"), tr("Align left"), 0, App);
  Acts->alignLeft->setStatusTip(tr("Align left selected elements"));
  Acts->alignLeft->setWhatsThis(
	tr("Align left\n\nAlign selected elements to their left edge"));
  connect(Acts->alignLeft, SIGNAL(activated()), Acts, SLOT(slotAlignLeft()));

  Acts->alignRight = new QAction(tr("Align right"), tr("Align right"), 0, App);
  Acts->alignRight->setStatusTip(tr("Align right selected elements"));
  Acts->alignRight->setWhatsThis(
	tr("Align right\n\nAlign selected elements to their right edge"));
  connect(Acts->alignRight, SIGNAL(activated()), Acts, SLOT(slotAlignRight()));

  Acts->distrHor = new QAction(tr("Distribute horizontally"),
	tr("Distribute horizontally"), 0, App);
  Acts->distrHor->setStatusTip(tr("Distribute equally horizontally"));
  Acts->distrHor->setWhatsThis(
	tr("Distribute horizontally\n\n"
	   "Distribute horizontally selected elements"));
  connect(Acts->distrHor, SIGNAL(activated()), Acts, SLOT(slotDistribHoriz()));

  Acts->distrVert =
    new QAction(tr("Distribute vertically"),
		tr("Distribute vertically"), 0, App);
  Acts->distrVert->setStatusTip(tr("Distribute equally vertically"));
  Acts->distrVert->setWhatsThis(
	tr("Distribute vertically\n\n"
	   "Distribute vertically selected elements"));
  connect(Acts->distrVert, SIGNAL(activated()), Acts, SLOT(slotDistribVert()));

  Acts->onGrid = new QAction(tr("Set on Grid"), tr("Set on Grid"),
				CTRL+Key_U, App);
  Acts->onGrid->setStatusTip(tr("Set on Grid"));
  Acts->onGrid->setWhatsThis(
	tr("Set on Grid\n\nSets selected elements on grid"));
  Acts->onGrid->setToggleAction(true);
  connect(Acts->onGrid, SIGNAL(toggled(bool)), Acts, SLOT(slotOnGrid(bool)));

  Acts->moveText =
    new QAction(tr("Move Component Text"),
		tr("Move Component Text"), CTRL+Key_K, App);
  Acts->moveText->setStatusTip(tr("Move Component Text"));
  Acts->moveText->setWhatsThis(
	tr("Move Component Text\n\nMoves the property text of components"));
  Acts->moveText->setToggleAction(true);
  connect(Acts->moveText, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotMoveText(bool)));

  Acts->changeProps =
    new QAction(tr("Change Property Values..."),
		tr("Change Property Values.."), Key_F7, App);
  Acts->changeProps->setStatusTip(tr("Change Property Values"));
  Acts->changeProps->setWhatsThis(
	tr("Change Property Values\n\nChange Property Value of Components"));
  connect(Acts->changeProps, SIGNAL(activated()),
	  Acts, SLOT(slotChangeProps()));

  App->editCut =
    new QAction(tr("Cut"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editcut.png")),
		tr("Cu&t"), CTRL+Key_X, App);
  App->editCut->setStatusTip(
	tr("Cuts the selected section and puts it to the clipboard"));
  App->editCut->setWhatsThis(
	tr("Cut\n\nCuts the selected section and puts it to the clipboard"));
  connect(App->editCut, SIGNAL(activated()), App, SLOT(slotEditCut()));

  App->editCopy =
    new QAction(tr("Copy"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editcopy.png")),
		tr("&Copy"), CTRL+Key_C, App);
  App->editCopy->setStatusTip(
	tr("Copies the selected section to the clipboard"));
  App->editCopy->setWhatsThis(
	tr("Copy\n\nCopies the selected section to the clipboard"));
  connect(App->editCopy, SIGNAL(activated()), App, SLOT(slotEditCopy()));

  Acts->editPaste =
    new QAction(tr("Paste"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editpaste.png")),
		tr("&Paste"), CTRL+Key_V, App);
  Acts->editPaste->setStatusTip(
	tr("Pastes the clipboard contents to the cursor position"));
  Acts->editPaste->setWhatsThis(
	tr("Paste\n\nPastes the clipboard contents to the cursor position"));
  Acts->editPaste->setToggleAction(true);
  connect(Acts->editPaste, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditPaste(bool)));

  Acts->editDelete =
    new QAction(tr("Delete"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editdelete.png")),
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

  App->undo =
    new QAction(tr("Undo"),
		QIconSet(QImage(QucsSettings.BitmapDir + "undo.png")),
		tr("&Undo"), CTRL+Key_Z, App);
  App->undo->setStatusTip(tr("Undoes the last command"));
  App->undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
  connect(App->undo, SIGNAL(activated()), Acts, SLOT(slotEditUndo()));

  App->redo =
    new QAction(tr("Redo"),
		QIconSet(QImage(QucsSettings.BitmapDir + "redo.png")),
		tr("&Redo"), CTRL+Key_Y, App);
  App->redo->setStatusTip(tr("Redoes the last command"));
  App->redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
  connect(App->redo, SIGNAL(activated()), Acts, SLOT(slotEditRedo()));

  App->projNew = new QAction(tr("New Project"), tr("&New Project..."),
			CTRL+SHIFT+Key_N, App);
  App->projNew->setStatusTip(tr("Creates a new project"));
  App->projNew->setWhatsThis(tr("New Project\n\nCreates a new project"));
  connect(App->projNew, SIGNAL(activated()), App, SLOT(slotProjNewButt()));

  App->projOpen =
    new QAction(tr("Open Project"), tr("&Open Project..."),
		CTRL+SHIFT+Key_O, App);
  App->projOpen->setStatusTip(tr("Opens a project"));
  App->projOpen->setWhatsThis(
	tr("Open Project\n\nOpens an existing project"));
  connect(App->projOpen, SIGNAL(activated()),
	  App, SLOT(slotMenuOpenProject()));

  App->projDel =
    new QAction(tr("Delete Project"), tr("&Delete Project..."),
		CTRL+SHIFT+Key_D, App);
  App->projDel->setStatusTip(tr("Deletes a project"));
  App->projDel->setWhatsThis(
	tr("Delete Project\n\nDeletes an existing project"));
  connect(App->projDel, SIGNAL(activated()), App, SLOT(slotMenuDelProject()));

  App->projClose =
    new QAction(tr("Close Project"), tr("&Close Project"),
		CTRL+SHIFT+Key_W, App);
  App->projClose->setStatusTip(tr("Close current project"));
  App->projClose->setWhatsThis(
	tr("Close Project\n\nCloses the current project"));
  connect(App->projClose, SIGNAL(activated()),
	  App, SLOT(slotMenuCloseProject()));

  App->magAll =
    new QAction(tr("View All"),
                QIconSet(QImage(QucsSettings.BitmapDir + "viewmagfit.png")),
                tr("View All"), Key_0, App);
  App->magAll->setStatusTip(tr("Views the whole page"));
  App->magAll->setWhatsThis(tr("View All\n\nShows the whole page content"));
  connect(App->magAll, SIGNAL(activated()), App, SLOT(slotShowAll()));

  App->magOne =
    new QAction(tr("View 1:1"),
                QIconSet(QImage(QucsSettings.BitmapDir + "viewmag1.png")),
                tr("View 1:1"), Key_1, App);
  App->magOne->setStatusTip(tr("Views without magnification"));
  App->magOne->setWhatsThis(
	tr("View 1:1\n\nShows the page content without magnification"));
  connect(App->magOne, SIGNAL(activated()), App, SLOT(slotShowOne()));

  Acts->magPlus =
    new QAction(tr("Zoom in"),
		QIconSet(QImage(QucsSettings.BitmapDir + "viewmag+.png")),
		tr("Zoom in"), Key_Plus, App);
  Acts->magPlus->setStatusTip(tr("Zooms into the current view"));
  Acts->magPlus->setWhatsThis(tr("Zoom in\n\nZooms the current view"));
  Acts->magPlus->setToggleAction(true);
  connect(Acts->magPlus, SIGNAL(toggled(bool)), Acts, SLOT(slotZoomIn(bool)));

  App->magMinus =
    new QAction(tr("Zoom out"),
		QIconSet(QImage(QucsSettings.BitmapDir + "viewmag-.png")),
		tr("Zoom out"), Key_Minus, App);
  App->magMinus->setStatusTip(tr("Zooms out the current view"));
  App->magMinus->setWhatsThis(tr("Reduce\n\nZooms out the current view"));
  connect(App->magMinus, SIGNAL(activated()), App, SLOT(slotZoomOut()));

  Acts->select =
    new QAction(tr("Select"),
		QIconSet(QImage(QucsSettings.BitmapDir + "pointer.png")),
		tr("Select"), Key_Escape, App);
  Acts->select->setStatusTip(tr("Select mode"));
  Acts->select->setWhatsThis(tr("Select\n\nSelect mode"));
  Acts->select->setToggleAction(true);
  connect(Acts->select, SIGNAL(toggled(bool)), Acts, SLOT(slotSelect(bool)));

  Acts->selectAll =
    new QAction(tr("Select All"), tr("Select All"), CTRL+Key_A, App);
  Acts->selectAll->setStatusTip(tr("Selects all elements"));
  Acts->selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(Acts->selectAll, SIGNAL(activated()), Acts, SLOT(slotSelectAll()));

  Acts->editRotate =
    new QAction(tr("Rotate"),
		QIconSet(QImage(QucsSettings.BitmapDir + "rotate_ccw.png")),
		tr("Rotate"), CTRL+Key_R, App);
  Acts->editRotate->setStatusTip(tr("Rotates the selected component by 90°"));
  Acts->editRotate->setWhatsThis(
    tr("Rotate\n\nRotates the selected component by 90° counter-clockwise"));
  Acts->editRotate->setToggleAction(true);
  connect(Acts->editRotate, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditRotate(bool)));

  Acts->editMirror =
    new QAction(tr("Mirror about X Axis"),
		QIconSet(QImage(QucsSettings.BitmapDir + "mirror.png")),
		tr("Mirror about X Axis"), CTRL+Key_J, App);
  Acts->editMirror->setStatusTip(
			  tr("Mirrors the selected item about X axis"));
  Acts->editMirror->setWhatsThis(
	tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  Acts->editMirror->setToggleAction(true);
  connect(Acts->editMirror, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditMirrorX(bool)));

  Acts->editMirrorY =
    new QAction(tr("Mirror about Y Axis"),
		QIconSet(QImage(QucsSettings.BitmapDir + "mirrory.png")),
		tr("Mirror about Y Axis"), CTRL+Key_M, App);
  Acts->editMirrorY->setStatusTip(
	tr("Mirrors the selected item about Y axis"));
  Acts->editMirrorY->setWhatsThis(
	tr("Mirror about Y Axis\n\nMirrors the selected item about Y Axis"));
  Acts->editMirrorY->setToggleAction(true);
  connect(Acts->editMirrorY, SIGNAL(toggled(bool)),
	Acts, SLOT(slotEditMirrorY(bool)));

  App->intoH =
    new QAction(tr("Go into Subcircuit"),
		QIconSet(QImage(QucsSettings.BitmapDir + "bottom.png")),
		tr("Go into Subcircuit"), CTRL+Key_I, App);
  App->intoH->setStatusTip(tr("Goes inside subcircuit"));
  App->intoH->setWhatsThis(
	tr("Go into Subcircuit\n\nGoes inside the selected subcircuit"));
  connect(App->intoH, SIGNAL(activated()), App, SLOT(slotIntoHierarchy()));
//  App->intoH->setEnabled(false);

  App->popH =
    new QAction(tr("Pop out"),
		QIconSet(QImage(QucsSettings.BitmapDir + "top.png")),
		tr("Pop out"), CTRL+Key_H, App);
  App->popH->setStatusTip(tr("Pop outside subcircuit"));
  App->popH->setWhatsThis(
	tr("Pop out\n\nGoes up one hierarchy level, i.e. leaves subcircuit"));
  connect(App->popH, SIGNAL(activated()), App, SLOT(slotPopHierarchy()));
  App->popH->setEnabled(false);  // only enabled if useful !!!!

  Acts->editActivate =
    new QAction(tr("Deactivate/Activate"),
		QIconSet(QImage(QucsSettings.BitmapDir + "deactiv.png")),
		tr("Deactivate/Activate"), CTRL+Key_D, App);
  Acts->editActivate->setStatusTip(
	tr("Deactivate/Activate the selected item"));
  Acts->editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected item"));
  Acts->editActivate->setToggleAction(true);
  connect(Acts->editActivate, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotEditActivate(bool)));

  Acts->insEquation =
    new QAction(tr("Insert Equation"),
		QIconSet(QImage(QucsSettings.BitmapDir + "equation.png")),
		tr("Insert Equation"), 0, App);
  Acts->insEquation->setStatusTip(tr("Inserts equation"));
  Acts->insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  Acts->insEquation->setToggleAction(true);
  connect(Acts->insEquation, SIGNAL(toggled(bool)),
	Acts, SLOT(slotInsertEquation(bool)));

  Acts->insGround =
    new QAction(tr("Insert Ground"),
		QIconSet(QImage(QucsSettings.BitmapDir + "ground.png")),
		tr("Insert Ground"), CTRL+Key_G, App);
  Acts->insGround->setStatusTip(tr("Inserts ground"));
  Acts->insGround->setWhatsThis(
	tr("Insert Ground\n\nInserts a ground symbol"));
  Acts->insGround->setToggleAction(true);
  connect(Acts->insGround, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotInsertGround(bool)));

  Acts->insPort =
    new QAction(tr("Insert Port"),
		QIconSet(QImage(QucsSettings.BitmapDir + "port.png")),
		tr("Insert Port"), 0, App);
  Acts->insPort->setStatusTip(tr("Inserts port"));
  Acts->insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  Acts->insPort->setToggleAction(true);
  connect(Acts->insPort, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotInsertPort(bool)));

  Acts->insWire =
    new QAction(tr("Insert Wire"),
		QIconSet(QImage(QucsSettings.BitmapDir + "wire.png")),
		tr("Wire"), CTRL+Key_E, App);
  Acts->insWire->setStatusTip(tr("Inserts a wire"));
  Acts->insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  Acts->insWire->setToggleAction(true);
  connect(Acts->insWire, SIGNAL(toggled(bool)), Acts,
			 SLOT(slotSetWire(bool)));

  Acts->insLabel =
    new QAction(tr("Insert Wire/Pin Label"),
		QIconSet(QImage(QucsSettings.BitmapDir + "nodename.png")),
		tr("Wire Label"), CTRL+Key_L, App);
  Acts->insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  Acts->insLabel->setWhatsThis(
	tr("Wire Label\n\nInserts a wire or pin label"));
  Acts->insLabel->setToggleAction(true);
  connect(Acts->insLabel, SIGNAL(toggled(bool)),
	  Acts, SLOT(slotInsertLabel(bool)));

  Acts->callEditor = new QAction(tr("Text editor"), tr("Text Editor"),
				CTRL+Key_1, App);
  Acts->callEditor->setStatusTip(tr("Starts the Qucs text editor"));
  Acts->callEditor->setWhatsThis(
			tr("Text editor\n\nStarts the Qucs text editor"));
  connect(Acts->callEditor, SIGNAL(activated()), Acts, SLOT(slotCallEditor()));

  Acts->callFilter = new QAction(tr("Filter synthesis"), tr("Filter synthesis"),
				CTRL+Key_2, App);
  Acts->callFilter->setStatusTip(tr("Starts QucsFilter"));
  Acts->callFilter->setWhatsThis(
			tr("Filter synthesis\n\nStarts QucsFilter"));
  connect(Acts->callFilter, SIGNAL(activated()), Acts, SLOT(slotCallFilter()));

  Acts->callLine = new QAction(tr("Line calculation"), tr("Line calculation"),
				CTRL+Key_3, App);
  Acts->callLine->setStatusTip(tr("Starts QucsTrans"));
  Acts->callLine->setWhatsThis(
		tr("Line calculation\n\nStarts transmission line calculator"));
  connect(Acts->callLine, SIGNAL(activated()), Acts, SLOT(slotCallLine()));

  Acts->callLib = new QAction(tr("Component Library"), tr("Component Library"),
				CTRL+Key_4, App);
  Acts->callLib->setStatusTip(tr("Starts QucsLib"));
  Acts->callLib->setWhatsThis(
		tr("Component Library\n\nStarts component library program"));
  connect(Acts->callLib, SIGNAL(activated()), Acts, SLOT(slotCallLibrary()));

  Acts->callMatch = new QAction(tr("Matching Circuit"), tr("Matching Circuit"),
				CTRL+Key_5, App);
  Acts->callMatch->setStatusTip(tr("Creates Matching Circuit"));
  Acts->callMatch->setWhatsThis(
		tr("Matching Circuit\n\nDialog for Creating Matching Circuit"));
  connect(Acts->callMatch, SIGNAL(activated()), Acts, SLOT(slotCallMatch()));

  App->simulate =
    new QAction(tr("Simulate"),
		QIconSet(QImage(QucsSettings.BitmapDir + "gear.png")),
		tr("Simulate"), Key_F2, App);
  App->simulate->setStatusTip(tr("Simulates the current schematic"));
  App->simulate->setWhatsThis(
	tr("Simulate\n\nSimulates the current schematic"));
  connect(App->simulate, SIGNAL(activated()), App, SLOT(slotSimulate()));

  App->dpl_sch =
    new QAction(tr("View Data Display/Schematic"),
		QIconSet(QImage(QucsSettings.BitmapDir + "rebuild.png")),
		tr("View Data Display/Schematic"), Key_F4, App);
  App->dpl_sch->setStatusTip(tr("Changes to data display or schematic page"));
  App->dpl_sch->setWhatsThis(
	tr("View Data Display/Schematic\n\n")+
	tr("Changes to data display or schematic page"));
  connect(App->dpl_sch, SIGNAL(activated()), App, SLOT(slotToPage()));

  App->dcbias =
    new QAction(tr("Calculate DC bias"), tr("Calculate DC bias"), Key_F8, App);
  App->dcbias->setStatusTip(tr("Calculates DC bias and shows it"));
  App->dcbias->setWhatsThis(
	tr("Calculate DC bias\n\nCalculates DC bias and shows it"));
  connect(App->dcbias, SIGNAL(activated()), App, SLOT(slotDCbias()));

  Acts->setMarker =
    new QAction(tr("Set Marker"),
		QIconSet(QImage(QucsSettings.BitmapDir + "marker.png")),
		tr("Set Marker on Graph"), CTRL+Key_B, App);
  Acts->setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  Acts->setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  Acts->setMarker->setToggleAction(true);
  connect(Acts->setMarker, SIGNAL(toggled(bool)),
	Acts, SLOT(slotSetMarker(bool)));

  Acts->showMsg =
    new QAction(tr("Show Last Messages"),
		tr("Show Last Messages"), Key_F5, App);
  Acts->showMsg->setStatusTip(tr("Shows last simulation messages"));
  Acts->showMsg->setWhatsThis(
    tr("Show Last Messages\n\nShows the messages of the last simulation"));
  connect(Acts->showMsg, SIGNAL(activated()), Acts, SLOT(slotShowLastMsg()));

  Acts->showNet =
    new QAction(tr("Show Last Netlist"), tr("Show Last Netlist"),
		Key_F6, App);
  Acts->showNet->setStatusTip(tr("Shows last simulation netlist"));
  Acts->showNet->setWhatsThis(
	tr("Show Last Netlist\n\nShows the netlist of the last simulation"));
  connect(Acts->showNet, SIGNAL(activated()),
	  Acts, SLOT(slotShowLastNetlist()));

  viewToolBar =
    new QAction(tr("Toolbar"), tr("Tool&bar"), 0, App, 0, true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)),
	  this, SLOT(slotViewToolBar(bool)));

  viewStatusBar =
    new QAction(tr("Statusbar"), tr("&Statusbar"), 0, App, 0, true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(
	tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)),
	  this, SLOT(slotViewStatusBar(bool)));

  Acts->helpIndex =
    new QAction(tr("Help Index"), tr("Help Index..."), Key_F1, App);
  Acts->helpIndex->setStatusTip(tr("Index of Qucs Help"));
  Acts->helpIndex->setWhatsThis(tr("Help Index\n\nIndex of intern Qucs help"));
  connect(Acts->helpIndex, SIGNAL(activated()), Acts, SLOT(slotHelpIndex()));

  Acts->helpGetStart =
    new QAction(tr("Getting Started"),
		tr("Getting Started..."), 0, App);
  Acts->helpGetStart->setStatusTip(tr("Getting Started with Qucs"));
  Acts->helpGetStart->setWhatsThis(
	tr("Getting Started\n\nShort introduction into Qucs"));
  connect(Acts->helpGetStart, SIGNAL(activated()),
	  Acts, SLOT(slotGettingStarted()));

  helpAboutApp = new QAction(tr("About"), tr("&About Qucs..."), 0, App);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), SLOT(slotHelpAbout()));

  helpAboutQt = new QAction(tr("About Qt"), tr("About Qt..."), 0, App);
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
  Acts->alignTop->addTo(alignMenu);
  Acts->alignBottom->addTo(alignMenu);
  Acts->alignLeft->addTo(alignMenu);
  Acts->alignRight->addTo(alignMenu);
  alignMenu->insertSeparator();
  Acts->distrHor->addTo(alignMenu);
  Acts->distrVert->addTo(alignMenu);

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
  Acts->selectAll->addTo(editMenu);
  Acts->editRotate->addTo(editMenu);
  Acts->editMirror->addTo(editMenu);
  Acts->editMirrorY->addTo(editMenu);
  Acts->editActivate->addTo(editMenu);
  Acts->changeProps->addTo(editMenu);
  editMenu->insertItem(tr("Align"), alignMenu);
  Acts->onGrid->addTo(editMenu);
  Acts->moveText->addTo(editMenu);
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

  toolMenu = new QPopupMenu();  // menuBar entry toolMenu
  Acts->callEditor->addTo(toolMenu);
  Acts->callFilter->addTo(toolMenu);
  Acts->callLine->addTo(toolMenu);
  Acts->callLib->addTo(toolMenu);
  Acts->callMatch->addTo(toolMenu);

  simMenu = new QPopupMenu();  // menuBar entry simMenu
  App->simulate->addTo(simMenu);
  App->dpl_sch->addTo(simMenu);
  App->dcbias->addTo(simMenu);
  Acts->showMsg->addTo(simMenu);
  Acts->showNet->addTo(simMenu);

  viewMenu = new QPopupMenu();  // menuBar entry viewMenu
  App->magAll->addTo(viewMenu);
  App->magOne->addTo(viewMenu);
  Acts->magPlus->addTo(viewMenu);
  App->magMinus->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);

  helpMenu = new QPopupMenu();  // menuBar entry helpMenu
  Acts->helpIndex->addTo(helpMenu);
  Acts->helpGetStart->addTo(helpMenu);
  helpMenu->insertSeparator();
  helpAboutApp->addTo(helpMenu);
  helpAboutQt->addTo(helpMenu);

  App->menuBar()->insertItem(tr("&File"), fileMenu);  // MENUBAR CONFIGURATION
  App->menuBar()->insertItem(tr("&Edit"), editMenu);
  App->menuBar()->insertItem(tr("&Insert"), insMenu);
  App->menuBar()->insertItem(tr("&Project"), projMenu);
  App->menuBar()->insertItem(tr("&Tools"), toolMenu);
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
  Acts->magPlus->addTo(viewToolbar);
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
void QucsInit::initStatusBar()
{
  // To reserve enough space, insert the longest text and rewrite it afterwards.
  WarningLabel =
    new QLabel(tr("Warnings in last simulation! Press F5"), App->statusBar());
  App->statusBar()->addWidget(WarningLabel, 0, true);
  WarningLabel->setText(tr("no warnings"));
  App->statusBar()->message(tr("Ready."), 2000);
}

// #########################################################################
void QucsInit::slotShowWarnings()
{
  static int ResultState = 0;

  if(ResultState == 0) {
    QFont f = WarningLabel->font();
    f.setWeight(QFont::DemiBold);
    WarningLabel->setFont(f);
    WarningLabel->setText(tr("Warnings in last simulation! Press F5"));
  }

  ResultState++;
  if(ResultState & 1)
    WarningLabel->setPaletteForegroundColor(Qt::red);
  else
    WarningLabel->setPaletteForegroundColor(Qt::black);

  if(ResultState < 9)
    QTimer::singleShot(500, this, SLOT(slotShowWarnings()));
  else 
    ResultState = 0;
}

// #########################################################################
void QucsInit::slotResetWarnings()
{
  QFont f = WarningLabel->font();   // reset warning label
  f.setWeight(QFont::Normal);
  WarningLabel->setFont(f);
  WarningLabel->setPaletteForegroundColor(Qt::black);
  WarningLabel->setText(tr("no warnings"));
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
    tr("\nQuite Universal Circuit Simulator\n")+
    tr("Copyright (C) 2003, 2004, 2005 by Michael Margraf\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n"+
    tr("Simulator by Stefan Jahn\n")+
    tr("Special thanks to Jens Flucke and Raimund Jacob\n\n")+
    tr("Translations:\n")+
    tr("German by Stefan Jahn\n")+
    tr("Polish by Dariusz Pienkowski\n")+
    tr("Romanian by Radu Circa\n")+
    tr("French by Vincent Habchi, F5RCS\n")+
    tr("Portuguese by Luciano Franca\n")+
    tr("Spanish by Jose L. Redrejo Rodriguez\n")+
    tr("Japanese by Toyoyuki Ishikawa\n")+
    tr("Italian by Giorgio Luparia\n")+
    tr("Hebrew by Dotan Nahum\n")+
    tr("Swedish by Markus Gothe\n")+
    tr("Hungarian by Jozsef Bus"));
}

// ########################################################################
void QucsInit::slotHelpAboutQt()
{
  QMessageBox::aboutQt(App, tr("About Qt"));
}
