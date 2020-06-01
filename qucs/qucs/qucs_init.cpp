/***************************************************************************
    copyright            : (C) 2004 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucs.h"
#include "octave_window.h"
#include "misc.h"

#include <QAction>
#include <QShortcut>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
#include <QTimer>
#include <QStatusBar>
#include <QDockWidget>
#include <QMessageBox>
#include <QApplication>

#include <assert.h>

// is this the toolbar?
void QucsApp::initActions()
{
  activeAction = nullptr;   // no active action

  // note: first argument of QAction() for backward compatibility Qt < 3.2

  QIcon fileNewIcon(":/bitmaps/filenew.png");
  assert(!fileNewIcon.isNull());
  fileNew = new QAction(fileNewIcon, tr("Document"), this);
  fileNew->setShortcut(Qt::CTRL+Qt::Key_N);
  fileNew->setStatusTip(tr("Creates a new document"));
  fileNew->setWhatsThis(
	        tr("New\n\nCreates a new schematic or data display document"));
  connect(fileNew, SIGNAL(triggered()), SLOT(slotFileNew()));

  fileNewNoDD = new QAction(QIcon((":/bitmaps/filenew.png")), tr("Stand-alone Document"), this);
  fileNewNoDD->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_H);
  fileNewNoDD->setStatusTip(tr("Creates a new stand-alone document"));
  fileNewNoDD->setWhatsThis(
	        tr("New\n\nCreates a new stand-alone schematic or data display document"));
  connect(fileNewNoDD, SIGNAL(triggered()), SLOT(slotFileNewNoDD()));

  textNew = new QAction(QIcon((":/bitmaps/textnew.png")), tr("&Text Document"), this);
  textNew->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_V);
  textNew->setStatusTip(tr("Creates a new text document"));
  textNew->setWhatsThis(tr("New Text\n\nCreates a new text document"));
  connect(textNew, SIGNAL(triggered()), SLOT(slotTextNew()));

  fileOpen = new QAction(QIcon((":/bitmaps/fileopen.png")),	tr("&Open..."), this);
  fileOpen->setShortcut(Qt::CTRL+Qt::Key_O);
  fileOpen->setStatusTip(tr("Opens an existing document"));
  fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(fileOpen, SIGNAL(triggered()), SLOT(slotFileOpen()));

  fileSave = new QAction(QIcon((":/bitmaps/filesave.png")),	tr("&Save"), this);
  fileSave->setShortcut(Qt::CTRL+Qt::Key_S);
  fileSave->setStatusTip(tr("Saves the current document"));
  fileSave->setWhatsThis(tr("Save File\n\nSaves the current document"));
  connect(fileSave, SIGNAL(triggered()), SLOT(slotFileSave()));

  fileSaveAs = new QAction(tr("Save as..."), this);
  fileSaveAs->setShortcut(Qt::CTRL+Qt::Key_Minus);
  fileSaveAs->setStatusTip(
	tr("Saves the current document under a new filename"));
  fileSaveAs->setWhatsThis(
	tr("Save As\n\nSaves the current document under a new filename"));
  connect(fileSaveAs, SIGNAL(triggered()), SLOT(slotFileSaveAs()));

  fileSaveAll = new QAction(QIcon((":/bitmaps/filesaveall.png")),	tr("Save &All"), this);
  fileSaveAll->setShortcut(Qt::CTRL+Qt::Key_Plus);
  fileSaveAll->setStatusTip(tr("Saves all open documents"));
  fileSaveAll->setWhatsThis(tr("Save All Files\n\nSaves all open documents"));
  connect(fileSaveAll, SIGNAL(triggered()), SLOT(slotFileSaveAll()));

  fileClose = new QAction(QIcon((":/bitmaps/fileclose.png")), tr("&Close"), this);
  fileClose->setShortcut(Qt::CTRL+Qt::Key_W);
  fileClose->setStatusTip(tr("Closes the current document"));
  fileClose->setWhatsThis(tr("Close File\n\nCloses the current document"));
  connect(fileClose, SIGNAL(triggered()), SLOT(slotFileClose()));

  fileCloseOthers = new QAction(QIcon((":/bitmaps/fileclose.png")), tr("Close all but current"), this);
  fileCloseOthers->setStatusTip(tr("Closes all documents except the current one"));
  fileCloseOthers->setWhatsThis(tr("Close all but current\n\nCloses all documents except the current one"));
  connect(fileCloseOthers, SIGNAL(triggered()), SLOT(slotFileCloseOthers()));

  fileCloseAllLeft = new QAction(QIcon((":/bitmaps/fileclose.png")), tr("Close all left"), this);
  fileCloseAllLeft->setStatusTip(tr("Closes all documents to the left of the current one"));
  fileCloseAllLeft->setWhatsThis(tr("Close all left\n\nCloses all documents to the left of the current one"));
  connect(fileCloseAllLeft, SIGNAL(triggered()), SLOT(slotFileCloseAllLeft()));

  fileCloseAllRight = new QAction(QIcon((":/bitmaps/fileclose.png")), tr("Close all right"), this);
  fileCloseAllRight->setStatusTip(tr("Closes all documents to the right of the current one"));
  fileCloseAllRight->setWhatsThis(tr("Close all right\n\nCloses all documents to the right of the current one"));
  connect(fileCloseAllRight, SIGNAL(triggered()), SLOT(slotFileCloseAllRight()));

  fileCloseAll = new QAction(QIcon((":/bitmaps/fileclose.png")), tr("Close &All"), this);
  fileCloseAll->setStatusTip(tr("Closes all documents"));
  fileCloseAll->setWhatsThis(tr("Close All\n\nCloses all documents"));
  connect(fileCloseAll, SIGNAL(triggered()), SLOT(slotFileCloseAll()));

  for (int i = 0; i < MaxRecentFiles; ++i) {
    fileRecentAction[i] = new QAction(this);
    fileRecentAction[i]->setVisible(false);
    connect(fileRecentAction[i], SIGNAL(triggered()), SLOT(slotOpenRecent()));
  }

  fileClearRecent = new QAction(tr("Clear Recent"), this);
  connect(fileClearRecent, SIGNAL(triggered()), SLOT(slotClearRecentFiles()));

  fileExamples = new QAction(tr("&Examples"), this);
  fileExamples->setStatusTip(tr("Opens a file explorer with example documents"));
  fileExamples->setWhatsThis(
	        tr("Examples\n\nOpens a file explorer with example documents"));
  connect(fileExamples, SIGNAL(triggered()), SLOT(slotFileExamples()));


  symEdit = new QAction(tr("&Edit Circuit Symbol"), this);
  symEdit->setShortcut(Qt::Key_F9);
  symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
  symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  connect(symEdit, SIGNAL(triggered()), SLOT(slotSymbolEdit()));

  fileSettings = new QAction(tr("&Document Settings..."), this);
  fileSettings->setShortcut(Qt::CTRL+Qt::Key_Period);
  fileSettings->setStatusTip(tr("Document Settings"));
  fileSettings->setWhatsThis(tr("Settings\n\nSets properties of the file"));
  connect(fileSettings, SIGNAL(triggered()), SLOT(slotFileSettings()));

  filePrint = new QAction(QIcon((":/bitmaps/fileprint.png")), tr("&Print..."), this);
  filePrint->setShortcut(Qt::CTRL+Qt::Key_P);
  filePrint->setStatusTip(tr("Prints the current document"));
  filePrint->setWhatsThis(tr("Print File\n\nPrints the current document"));
  connect(filePrint, SIGNAL(triggered()), SLOT(slotFilePrint()));

  filePrintFit = new QAction(tr("Print Fit to Page..."), this);
  filePrintFit->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_P);
  filePrintFit->setStatusTip(tr("Print Fit to Page"));
  filePrintFit->setWhatsThis(
	tr("Print Fit to Page\n\n"
	   "Print and fit content to the page size"));
  connect(filePrintFit, SIGNAL(triggered()), SLOT(slotFilePrintFit()));

  fileQuit = new QAction(tr("E&xit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(triggered()), SLOT(slotFileQuit()));

  applSettings = new QAction(tr("Application Settings..."), this);
  applSettings->setShortcut(Qt::CTRL+Qt::Key_Comma);
  applSettings->setStatusTip(tr("Application Settings"));
  applSettings->setWhatsThis(
	tr("Qucs Settings\n\nSets properties of the application"));
  connect(applSettings, SIGNAL(triggered()), SLOT(slotApplSettings()));

  refreshSchPath = new QAction(tr("Refresh Search Path..."), this);
  //refreshSchPath->setShortcut(Qt::CTRL+Qt::Key_Comma);
  refreshSchPath->setStatusTip(tr("Refresh Search Path"));
  refreshSchPath->setWhatsThis(
    tr("Refresh Path\n\nRechecks the list of paths for subcircuit files."));
  connect(refreshSchPath, SIGNAL(triggered()), SLOT(slotRefreshSchPath()));

  alignTop = new QAction(tr("Align top"), this);
  alignTop->setShortcut(Qt::CTRL+Qt::Key_T);
  alignTop->setStatusTip(tr("Align top selected elements"));
  alignTop->setWhatsThis(
	tr("Align top\n\nAlign selected elements to their upper edge"));
  connect(alignTop, SIGNAL(triggered()), SLOT(slotAlignTop()));

  alignBottom = new QAction(tr("Align bottom"), this);
  alignBottom->setStatusTip(tr("Align bottom selected elements"));
  alignBottom->setWhatsThis(
	tr("Align bottom\n\nAlign selected elements to their lower edge"));
  connect(alignBottom, SIGNAL(triggered()), SLOT(slotAlignBottom()));

  alignLeft = new QAction(tr("Align left"), this);
  alignLeft->setStatusTip(tr("Align left selected elements"));
  alignLeft->setWhatsThis(
	tr("Align left\n\nAlign selected elements to their left edge"));
  connect(alignLeft, SIGNAL(triggered()), SLOT(slotAlignLeft()));

  alignRight = new QAction(tr("Align right"), this);
  alignRight->setStatusTip(tr("Align right selected elements"));
  alignRight->setWhatsThis(
	tr("Align right\n\nAlign selected elements to their right edge"));
  connect(alignRight, SIGNAL(triggered()), SLOT(slotAlignRight()));

  distrHor = new QAction(tr("Distribute horizontally"), this);
  distrHor->setStatusTip(tr("Distribute equally horizontally"));
  distrHor->setWhatsThis(
	tr("Distribute horizontally\n\n"
	   "Distribute horizontally selected elements"));
  connect(distrHor, SIGNAL(triggered()), SLOT(slotDistribHoriz()));

  distrVert = new QAction(tr("Distribute vertically"), this);
  distrVert->setStatusTip(tr("Distribute equally vertically"));
  distrVert->setWhatsThis(
	tr("Distribute vertically\n\n"
	   "Distribute vertically selected elements"));
  connect(distrVert, SIGNAL(triggered()), SLOT(slotDistribVert()));

  centerHor = new QAction(tr("Center horizontally"), this);
  centerHor->setStatusTip(tr("Center horizontally selected elements"));
  centerHor->setWhatsThis(
	tr("Center horizontally\n\nCenter horizontally selected elements"));
  connect(centerHor, SIGNAL(triggered()), SLOT(slotCenterHorizontal()));

  centerVert = new QAction(tr("Center vertically"), this);
  centerVert->setStatusTip(tr("Center vertically selected elements"));
  centerVert->setWhatsThis(
	tr("Center vertically\n\nCenter vertically selected elements"));
  connect(centerVert, SIGNAL(triggered()), SLOT(slotCenterVertical()));

  onGrid = new QAction(tr("Set on Grid"), this);
  onGrid->setShortcut(Qt::CTRL+Qt::Key_U);
  onGrid->setStatusTip(tr("Sets selected elements on grid"));
  onGrid->setWhatsThis(
	tr("Set on Grid\n\nSets selected elements on grid"));
  onGrid->setCheckable(true);
  connect(onGrid, &QAction::toggled, this, &QucsApp::slotOnGrid);

  moveText = new QAction(tr("Move Component Text"), this);
  moveText->setShortcut(Qt::CTRL+Qt::Key_K);
  moveText->setStatusTip(tr("Moves the property text of components"));
  moveText->setWhatsThis(
	tr("Move Component Text\n\nMoves the property text of components"));
  moveText->setCheckable(true);
  connect(moveText, &QAction::toggled, this, &QucsApp::slotMoveText);

  changeProps = new QAction(tr("Replace..."), this);
  changeProps->setShortcut(Qt::Key_F7);
  changeProps->setStatusTip(tr("Replace component properties or VHDL code"));
  changeProps->setWhatsThis(
	tr("Replace\n\nChange component properties\nor\ntext in VHDL code"));
  connect(changeProps, SIGNAL(triggered()), SLOT(slotChangeProps()));

  editCut = new QAction(QIcon((":/bitmaps/editcut.png")),	tr("Cu&t"), this);
  editCut->setShortcut(Qt::CTRL+Qt::Key_X);
  editCut->setStatusTip(
	tr("Cuts out the selection and puts it into the clipboard"));
  editCut->setWhatsThis(
	tr("Cut\n\nCuts out the selection and puts it into the clipboard"));
  connect(editCut, SIGNAL(triggered()), SLOT(slotEditCut()));

  editCopy = new QAction(QIcon((":/bitmaps/editcopy.png")), tr("&Copy"), this);
  editCopy->setShortcut(Qt::CTRL+Qt::Key_C);
  editCopy->setStatusTip(
	tr("Copies the selection into the clipboard"));
  editCopy->setWhatsThis(
	tr("Copy\n\nCopies the selection into the clipboard"));
  connect(editCopy, SIGNAL(triggered()), SLOT(slotEditCopy()));

  editPaste = new QAction(QIcon((":/bitmaps/editpaste.png")), tr("&Paste"), this);
  editPaste->setShortcut(Qt::CTRL+Qt::Key_V);
  editPaste->setStatusTip(
	tr("Pastes the clipboard contents to the cursor position"));
  editPaste->setWhatsThis(
	tr("Paste\n\nPastes the clipboard contents to the cursor position"));
  editPaste->setCheckable(true);
  connect(editPaste, &QAction::toggled, this, &QucsApp::slotEditPaste);

  editDelete = new QAction(QIcon((":/bitmaps/editdelete.png")), tr("&Delete"), this);
  editDelete->setShortcut(Qt::Key_Delete);
  editDelete->setStatusTip(tr("Deletes the selected components"));
  editDelete->setWhatsThis(tr("Delete\n\nDeletes the selected components"));
  editDelete->setCheckable(true);
  //connect(editDelete, &QAction::toggled, this, &QucsApp::slotEditDelete);
  //connect(editDelete, &QAction::toggled, this, &QucsApp::slotToggle);

  editFind = new QAction(tr("Find..."), this);
  editFind->setShortcut(Qt::CTRL+Qt::Key_F);
  editFind->setStatusTip(tr("Find a piece of text"));
  editFind->setWhatsThis(tr("Find\n\nSearches for a piece of text"));
  connect(editFind, SIGNAL(triggered()), SLOT(slotEditFind()));

#if 0 // not yet?
  // not to be confused with "save".
  // should be just export, ask for file format and provide some options
  exportSchematic = new QAction(tr("Export..."),this);
  connect(exportSchematic,SIGNAL(triggered()),SLOT(slotExportSchematic()));
#endif

  // to ease usage with notebooks, backspace can also be used to delete
  // currently not supported
  //mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Backspace),
  //                       editDelete, SLOT(toggle()) );

  // will be obsolete, once Export works. this is just export with some image
  // file format target.
  exportAsImage = new QAction(tr("Export as image..."),this);
  connect(exportAsImage,SIGNAL(triggered()),SLOT(slotSaveSchematicToGraphicsFile()));
  exportAsImage->setStatusTip(tr("Exports the current document to an image file"));
  exportAsImage->setWhatsThis(tr("Export as image\n\nExports the current document to an image file"));

  // cursor left/right/up/down to move marker on a graph
  // BUG: why can't we move Elements?!
  // BUG: why is tnis not in SchematicDocument::init (or so?)
  cursorLeft = new QShortcut(QKeySequence(Qt::Key_Left), this);
  connect(cursorLeft, SIGNAL(activated()), SLOT(slotCursorLeft()));

  cursorRight = new QShortcut(QKeySequence(Qt::Key_Right), this);
  connect(cursorRight, SIGNAL(activated()), SLOT(slotCursorRight()));

  cursorUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
  connect(cursorUp, SIGNAL(activated()), SLOT(slotCursorUp()));

  cursorDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
  connect(cursorDown, SIGNAL(activated()), SLOT(slotCursorDown()));

  undo = new QAction(QIcon((":/bitmaps/undo.png")), tr("&Undo"), this);
  undo->setShortcut(Qt::CTRL+Qt::Key_Z);
  undo->setStatusTip(tr("Undoes the last command"));
  undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
  connect(undo, SIGNAL(triggered()), SLOT(slotEditUndo()));

  redo = new QAction(QIcon((":/bitmaps/redo.png")), tr("&Redo"), this);
  redo->setShortcut(Qt::CTRL+Qt::Key_Y);
  redo->setStatusTip(tr("Redoes the last command"));
  redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
  connect(redo, SIGNAL(triggered()), SLOT(slotEditRedo()));

  projNew = new QAction(tr("&New Project..."), this);
  projNew->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_N);
  projNew->setStatusTip(tr("Creates a new project"));
  projNew->setWhatsThis(tr("New Project\n\nCreates a new project"));
  connect(projNew, SIGNAL(triggered()), SLOT(slotButtonProjNew()));

  // why this? let's just "Open"
  projOpen = new QAction(tr("&Open Project..."), this);
	projOpen->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_O);
  projOpen->setStatusTip(tr("Opens an existing project"));
  projOpen->setWhatsThis(tr("Open Project\n\nOpens an existing project"));
  connect(projOpen, SIGNAL(triggered()), SLOT(slotMenuProjOpen()));

  projDel = new QAction(tr("&Delete Project..."), this);
  projDel->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_D);
  projDel->setStatusTip(tr("Deletes an existing project"));
  projDel->setWhatsThis(tr("Delete Project\n\nDeletes an existing project"));
  connect(projDel, SIGNAL(triggered()), SLOT(slotMenuProjDel()));

  projClose = new QAction(tr("&Close Project"), this);
  projClose->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_W);
  projClose->setStatusTip(tr("Closes the current project"));
  projClose->setWhatsThis(tr("Close Project\n\nCloses the current project"));
  connect(projClose, SIGNAL(triggered()), SLOT(slotMenuProjClose()));

  addToProj = new QAction(tr("&Add Files to Project..."), this);
  addToProj->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_A);
  addToProj->setStatusTip(tr("Copies files to project directory"));
  addToProj->setWhatsThis(
	tr("Add Files to Project\n\nCopies files to project directory"));
  connect(addToProj, SIGNAL(triggered()), SLOT(slotAddToProject()));

  createLib = new QAction(tr("Create &Library..."), this);
  createLib->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_L);
  createLib->setStatusTip(tr("Create Library from Subcircuits"));
  createLib->setWhatsThis(
	tr("Create Library\n\nCreate Library from Subcircuits"));
  connect(createLib, SIGNAL(triggered()), SLOT(slotCreateLib()));

  createPkg = new QAction(tr("Create &Package..."), this);
  createPkg->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_Z);
  createPkg->setStatusTip(tr("Create compressed Package from Projects"));
  createPkg->setWhatsThis(
    tr("Create Package\n\nCreate compressed Package from complete Projects"));
  connect(createPkg, SIGNAL(triggered()), SLOT(slotCreatePackage()));

  // what's this? a gunzip ui? should be advertised as such.
  extractPkg = new QAction(tr("E&xtract Package..."),  this);
  extractPkg->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_X);
  extractPkg->setStatusTip(tr("Install Content of a Package"));
  extractPkg->setWhatsThis(
	tr("Extract Package\n\nInstall Content of a Package"));
  connect(extractPkg, SIGNAL(triggered()), SLOT(slotExtractPackage()));

  // this is incomplete. what is "data file"?
  // note we have "Export..." above
  importData = new QAction(tr("&Import/Export Data..."), this);
  importData->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_I);
  importData->setStatusTip(tr("Convert data file"));
  importData->setWhatsThis(
	tr("Import/Export Data\n\nConvert data file to various file formats"));
  connect(importData, SIGNAL(triggered()), SLOT(slotImportData()));

  // we need to ask the simulator for data. maybe this should go to the simulator menu
  // (later)
  graph2csv = new QAction(tr("Export to &CSV..."), this);
  graph2csv->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_C);
  graph2csv->setStatusTip(tr("Convert graph data to CSV file"));
  graph2csv->setWhatsThis(
	tr("Export to CSV\n\nConvert graph data to CSV file"));
  connect(graph2csv, SIGNAL(triggered()), SLOT(slotExportGraphAsCsv()));

  buildModule = new QAction(tr("Build Verilog-A module..."), this);
  buildModule->setStatusTip(tr("Run admsXml and C++ compiler"));
  buildModule->setWhatsThis(tr("Build Verilog-A module\nRuns amdsXml and C++ compiler"));
  connect(buildModule, SIGNAL(triggered()), SLOT(slotBuildModule()));

  loadModule = new QAction(tr("Load Verilog-A module..."), this);
  loadModule->setStatusTip(tr("Select Verilog-A symbols to be loaded"));
  loadModule->setWhatsThis(tr("Load Verilog-A module\nLet the user select and load symbols"));
  connect(loadModule, SIGNAL(triggered()), SLOT(slotLoadModule()));

  magAll = new QAction(QIcon((":/bitmaps/viewmagfit.png")), tr("View All"), this);
  magAll->setShortcut(Qt::Key_0);
  magAll->setStatusTip(tr("Show the whole page"));
  magAll->setWhatsThis(tr("View All\n\nShows the whole page content"));
  connect(magAll, SIGNAL(triggered()), SLOT(slotShowAll()));

  magOne = new QAction(QIcon((":/bitmaps/viewmag1.png")), tr("View 1:1"), this);
  magOne->setShortcut(Qt::Key_1);
  magOne->setStatusTip(tr("Views without magnification"));
  magOne->setWhatsThis(tr("View 1:1\n\nShows the page content without magnification"));
  connect(magOne, SIGNAL(triggered()), SLOT(slotShowOne()));

  magPlus = new QAction(QIcon((":/bitmaps/viewmag+.png")),	tr("Zoom in"), this);
  magPlus->setShortcut(Qt::Key_Plus);
  magPlus->setStatusTip(tr("Zooms into the current view"));
  magPlus->setWhatsThis(tr("Zoom in\n\nZooms the current view"));
  magPlus->setCheckable(true);
  connect(magPlus, &QAction::toggled, this, &QucsApp::slotZoomIn);

  magMinus = new QAction(QIcon((":/bitmaps/viewmag-.png")), tr("Zoom out"), this);
  magMinus->setShortcut(Qt::Key_Minus);
  magMinus->setStatusTip(tr("Zooms out the current view"));
  magMinus->setWhatsThis(tr("Zoom out\n\nZooms out the current view"));
  connect(magMinus, SIGNAL(triggered()), SLOT(slotZoomOut()));

  QAction *escape = new QAction(this);
  escape->setShortcut(Qt::Key_Escape);
  connect(escape, SIGNAL(triggered()), SLOT(slotEscape()));
  this->addAction(escape);

  select = new QAction(QIcon((":/bitmaps/pointer.png")), tr("Select"), this);
  select->setStatusTip(tr("Activate select mode"));
  select->setWhatsThis(tr("Select\n\nActivates select mode"));
  select->setCheckable(true);
  connect(select, &QAction::toggled, this, &QucsApp::slotSelect);

  selectAll = new QAction(tr("Select All"), this);
  selectAll->setShortcut(Qt::CTRL+Qt::Key_A);
  selectAll->setStatusTip(tr("Selects all elements"));
  selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(selectAll, SIGNAL(triggered()), SLOT(slotSelectAll()));

  selectMarker = new QAction(tr("Select Markers"), this);
  selectMarker->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_M);
  selectMarker->setStatusTip(tr("Selects all markers"));
  selectMarker->setWhatsThis(
	tr("Select Markers\n\nSelects all diagram markers of the document"));
  connect(selectMarker, SIGNAL(triggered()), SLOT(slotSelectMarker()));

  editRotate = new QAction(QIcon((":/bitmaps/rotate_ccw.png")), tr("Rotate"), this);
  editRotate->setShortcut(Qt::CTRL+Qt::Key_R);
  editRotate->setStatusTip(tr("Rotates the selected component by 90\x00B0"));
  editRotate->setWhatsThis(
    tr("Rotate\n\nRotates the selected component by 90\x00B0 counter-clockwise"));
  editRotate->setCheckable(true);
  connect(editRotate, &QAction::toggled, this, &QucsApp::slotEditRotate);

  editMirror = new QAction(QIcon((":/bitmaps/mirror.png")), tr("Mirror about X Axis"), this);
  editMirror->setShortcut(Qt::CTRL+Qt::Key_J);
  editMirror->setStatusTip(tr("Mirrors the selected item about X Axis"));
  editMirror->setWhatsThis(
	tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  editMirror->setCheckable(true);
  connect(editMirror, &QAction::toggled, this, &QucsApp::slotEditMirrorX);

  editMirrorY = new QAction(QIcon((":/bitmaps/mirrory.png")), tr("Mirror about Y Axis"), this);
  editMirrorY->setShortcut(Qt::CTRL+Qt::Key_M);
  editMirrorY->setStatusTip(tr("Mirrors the selected item about Y Axis"));
  editMirrorY->setWhatsThis(
	tr("Mirror about Y Axis\n\nMirrors the selected item about Y Axis"));
  editMirrorY->setCheckable(true);
  connect(editMirrorY, &QAction::toggled, this, &QucsApp::slotEditMirrorY);

  intoH = new QAction(QIcon((":/bitmaps/bottom.png")), tr("Go into Subcircuit"), this);
  intoH->setShortcut(Qt::CTRL+Qt::Key_I);
  intoH->setStatusTip(tr("Goes inside the selected subcircuit"));
  intoH->setWhatsThis(
	tr("Go into Subcircuit\n\nGoes inside the selected subcircuit"));
  connect(intoH, SIGNAL(triggered()), SLOT(slotIntoHierarchy()));

  popH = new QAction(QIcon((":/bitmaps/top.png")), tr("Pop out"), this);
  popH->setShortcut(Qt::CTRL+Qt::Key_H);
  popH->setStatusTip(tr("Pop outside subcircuit"));
  popH->setWhatsThis(
	tr("Pop out\n\nGoes up one hierarchy level, i.e. leaves subcircuit"));
  connect(popH, SIGNAL(triggered()), SLOT(slotPopHierarchy()));
  popH->setEnabled(false);  // only enabled if useful !!!!

  editActivate = new QAction(QIcon((":/bitmaps/deactiv.png")),	tr("Deactivate/Activate"), this);
  editActivate->setShortcut(Qt::CTRL+Qt::Key_D);
  editActivate->setStatusTip(tr("Deactivate/Activate selected components"));
  editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected components"));
  editActivate->setCheckable(true);
  connect(editActivate, &QAction::toggled, this, &QucsApp::slotEditActivate);

  insEquation = new QAction(QIcon((":/bitmaps/equation.png")),	tr("Insert Equation"), this);
  insEquation->setShortcut(Qt::CTRL+Qt::Key_Less);
  insEquation->setStatusTip(tr("Inserts an equation"));
  insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  insEquation->setCheckable(true);
  connect(insEquation, &QAction::toggled, this, &QucsApp::slotInsertEquation);

  insGround = new QAction(QIcon((":/bitmaps/ground.png")), tr("Insert Ground"), this);
  insGround->setShortcut(Qt::CTRL+Qt::Key_G);
  insGround->setStatusTip(tr("Inserts a ground symbol"));
  insGround->setWhatsThis(tr("Insert Ground\n\nInserts a ground symbol"));
  insGround->setCheckable(true);
  connect(insGround, &QAction::toggled, this, &QucsApp::slotInsertGround);

  insPort = new QAction(QIcon((":/bitmaps/port.png")),	tr("Insert Port"), this);
  insPort->setStatusTip(tr("Inserts a port symbol"));
  insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  insPort->setCheckable(true);
  connect(insPort, &QAction::toggled, this, &QucsApp::slotInsertPort);

  insWire = new QAction(QIcon((":/bitmaps/wire.png")),	tr("Wire"), this);
  insWire->setShortcut(Qt::CTRL+Qt::Key_E);
  insWire->setStatusTip(tr("Inserts a wire"));
  insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  insWire->setCheckable(true);
  connect(insWire, &QAction::toggled, this, &QucsApp::slotSetWire);

  insLabel = new QAction(QIcon((":/bitmaps/nodename.png")), tr("Wire Label"), this);
  insLabel->setShortcut(Qt::CTRL+Qt::Key_L);
  insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  insLabel->setWhatsThis(tr("Wire Label\n\nInserts a wire or pin label"));
  insLabel->setCheckable(true);
  connect(insLabel, &QAction::toggled, this, &QucsApp::slotInsertLabel);

  insEntity = new QAction(tr("VHDL entity"), this);
  insEntity->setShortcut(Qt::CTRL+Qt::Key_Space);
  insEntity->setStatusTip(tr("Inserts skeleton of VHDL entity"));
  insEntity->setWhatsThis(
	tr("VHDL entity\n\nInserts the skeleton of a VHDL entity"));
  connect(insEntity, SIGNAL(triggered()), SLOT(slotInsertEntity()));

  callEditor = new QAction(tr("Text Editor"), this);
  callEditor->setShortcut(Qt::CTRL+Qt::Key_1);
  callEditor->setStatusTip(tr("Starts the Qucs text editor"));
  callEditor->setWhatsThis(tr("Text editor\n\nStarts the Qucs text editor"));
  connect(callEditor, SIGNAL(triggered()), SLOT(slotCallEditor()));

  callFilter = new QAction(tr("Filter synthesis"), this);
  callFilter->setShortcut(Qt::CTRL+Qt::Key_2);
  callFilter->setStatusTip(tr("Starts QucsFilter"));
  callFilter->setWhatsThis(tr("Filter synthesis\n\nStarts QucsFilter"));
  connect(callFilter, SIGNAL(triggered()), SLOT(slotCallFilter()));

  callActiveFilter = new QAction(tr("Active filter synthesis"),this);
  callActiveFilter->setShortcut(Qt::CTRL+Qt::Key_3);
  callActiveFilter->setStatusTip(tr("Starts QucsActiveFilter"));
  callActiveFilter->setWhatsThis(tr("Active filter synthesis\n\nStarts QucsActiveFilter"));
  connect(callActiveFilter, SIGNAL(triggered()), SLOT(slotCallActiveFilter()));

  callLine = new QAction(tr("Line calculation"), this);
  callLine->setShortcut(Qt::CTRL+Qt::Key_4);
  callLine->setStatusTip(tr("Starts QucsTrans"));
  callLine->setWhatsThis(
		tr("Line calculation\n\nStarts transmission line calculator"));
  connect(callLine, SIGNAL(triggered()), SLOT(slotCallLine()));

  callLib = new QAction(tr("Component Library"), this);
  callLib->setShortcut(Qt::CTRL+Qt::Key_5);
  callLib->setStatusTip(tr("Starts QucsLib"));
  callLib->setWhatsThis(
		tr("Component Library\n\nStarts component library program"));
  connect(callLib, SIGNAL(triggered()), SLOT(slotCallLibrary()));

  callMatch = new QAction(tr("Matching Circuit"), this);
  callMatch->setShortcut(Qt::CTRL+Qt::Key_6);
  callMatch->setStatusTip(tr("Creates Matching Circuit"));
  callMatch->setWhatsThis(
	tr("Matching Circuit\n\nDialog for Creating Matching Circuit"));
  connect(callMatch, SIGNAL(triggered()), SLOT(slotCallMatch()));

  callAtt = new QAction(tr("Attenuator synthesis"), this);
  callAtt->setShortcut(Qt::CTRL+Qt::Key_7);
  callAtt->setStatusTip(tr("Starts QucsAttenuator"));
  callAtt->setWhatsThis(
	tr("Attenuator synthesis\n\nStarts attenuator calculation program"));
  connect(callAtt, SIGNAL(triggered()), SLOT(slotCallAtt()));

  callRes = new QAction(tr("Resistor color codes"), this);
  callRes->setShortcut(Qt::CTRL+Qt::Key_8);
  callRes->setStatusTip(tr("Starts Qucs resistor color codes"));
  callRes->setWhatsThis(
  tr("Resistor color codes\n\nStarts standard resistor color code computation program"));
  connect(callRes, SIGNAL(triggered()), SLOT(slotCallRes()));


  callPowerComb = new QAction(tr("Power combining"), this);
  callPowerComb->setShortcut(Qt::CTRL+Qt::Key_9);
  callPowerComb->setStatusTip(tr("Starts Qucs power combining tool"));
  callPowerComb->setWhatsThis(
  tr("Power combining synthesis\n\nStarts power combining synthesis tool"));
  connect(callPowerComb, SIGNAL(triggered()), SLOT(slotCallPowerComb()));


  simulate = new QAction(QIcon((":/bitmaps/gear.png")), tr("Simulate"), this);
  simulate->setShortcut(Qt::Key_F2);
  simulate->setStatusTip(tr("Simulates the current schematic"));
  simulate->setWhatsThis(tr("Simulate\n\nSimulates the current schematic"));
  connect(simulate, SIGNAL(triggered()), SLOT(slotSimulate()));

  dpl_sch = new QAction(QIcon((":/bitmaps/rebuild.png")), tr("View Data Display/Schematic"), this);
  dpl_sch->setShortcut(Qt::Key_F4);
  dpl_sch->setStatusTip(tr("Changes to data display or schematic page"));
  dpl_sch->setWhatsThis(
	tr("View Data Display/Schematic\n\n")+
	tr("Changes to data display or schematic page"));
  connect(dpl_sch, SIGNAL(triggered()), SLOT(slotToPage()));

  dcbias = new QAction(tr("Calculate DC bias"), this);
  dcbias->setShortcut(Qt::Key_F8);
  dcbias->setStatusTip(tr("Calculates DC bias and shows it"));
  dcbias->setWhatsThis(
	tr("Calculate DC bias\n\nCalculates DC bias and shows it"));
  connect(dcbias, SIGNAL(triggered()), SLOT(slotDCbias()));

  setMarker = new QAction(QIcon((":/bitmaps/marker.png")),	tr("Set Marker on Graph"), this);
  setMarker->setShortcut(Qt::CTRL+Qt::Key_B);
  setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  setMarker->setCheckable(true);
  connect(setMarker, &QAction::toggled, this, &QucsApp::slotSetMarker);

  showMsg = new QAction(tr("Show Last Messages"), this);
  showMsg->setShortcut(Qt::Key_F5);
  showMsg->setStatusTip(tr("Shows last simulation messages"));
  showMsg->setWhatsThis(
        tr("Show Last Messages\n\nShows the messages of the last simulation"));
  connect(showMsg, SIGNAL(triggered()), SLOT(slotShowLastMsg()));

  showNet = new QAction(tr("Show Last Netlist"), this);
  showNet->setShortcut(Qt::Key_F6);
  showNet->setStatusTip(tr("Shows last simulation netlist"));
  showNet->setWhatsThis(
	tr("Show Last Netlist\n\nShows the netlist of the last simulation"));
  connect(showNet, &QAction::toggled, this, &QucsApp::slotShowLastNetlist);

  viewToolBar = new QAction(tr("Tool&bar"), this);
  viewToolBar->setCheckable(true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)), SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("&Statusbar"), this);
  viewStatusBar->setCheckable(true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(
	tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), SLOT(slotViewStatusBar(bool)));

  viewBrowseDock = new QAction(tr("&Dock Window"), this);
  viewBrowseDock->setCheckable(true);
  viewBrowseDock->setStatusTip(tr("Enables/disables the browse dock window"));
  viewBrowseDock->setWhatsThis(
	tr("Browse Window\n\nEnables/disables the browse dock window"));
  connect(viewBrowseDock, SIGNAL(toggled(bool)), SLOT(slotViewBrowseDock(bool)));

  viewOctaveDock = new QAction(tr("&Octave Window"), this);
  viewOctaveDock->setCheckable(true);
  viewOctaveDock->setStatusTip(tr("Shows/hides the Octave dock window"));
  viewOctaveDock->setWhatsThis(
      tr("Octave Window\n\nShows/hides the Octave dock window"));
  connect(viewOctaveDock, SIGNAL(toggled(bool)), SLOT(slotViewOctaveDock(bool)));

  helpOnline = new QAction(tr("Qucs-Help website"), this);
  helpOnline->setShortcut(Qt::Key_F1);
  helpOnline->setStatusTip(tr("Open help website in the default browser."));
  helpOnline->setWhatsThis(tr("Qucs-help\n\nOpen help website in the default browser."));
  connect(helpOnline, SIGNAL(triggered()), SLOT(slotHelpOnline()));

  helpAboutApp = new QAction(tr("&About Qucs..."), this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(triggered()), SLOT(slotHelpAbout()));

  helpAboutQt = new QAction(tr("About Qt..."), this);
  helpAboutQt->setStatusTip(tr("About Qt"));
  helpAboutQt->setWhatsThis(tr("About Qt\n\nAbout Qt by Trolltech"));
  connect(helpAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

// ----------------------------------------------------------
void QucsApp::initMenuBar()
{
  fileMenu = new QMenu(tr("&File"));  // menuBar entry fileMenu

  QMenu *newFileMenu = new QMenu(tr("New"), fileMenu);
  newFileMenu->addAction(fileNew);
  newFileMenu->addAction(fileNewNoDD);
  newFileMenu->addAction(textNew);
  fileMenu->addMenu(newFileMenu);

  fileMenu->addAction(fileOpen);

  QMenu *closeFileMenu = new QMenu(tr("Close"), fileMenu);
  closeFileMenu->addAction(fileClose);
  closeFileMenu->addAction(fileCloseOthers);
  closeFileMenu->addAction(fileCloseAllLeft);
  closeFileMenu->addAction(fileCloseAllRight);
  closeFileMenu->addAction(fileCloseAll);
  fileMenu->addMenu(closeFileMenu);

  recentFilesMenu = new QMenu(tr("Open Recent"),fileMenu);
  fileMenu->addMenu(recentFilesMenu);
  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFilesMenu->addAction(fileRecentAction[i]);
  }
  recentFilesMenu->addSeparator();
  recentFilesMenu->addAction(fileClearRecent);

  fileMenu->addSeparator();
  fileMenu->addAction(fileSave);
  fileMenu->addAction(fileSaveAll);
  fileMenu->addAction(fileSaveAs);
  fileMenu->addAction(exportAsImage);
  fileMenu->addAction(filePrint);
  fileMenu->addAction(filePrintFit);
  fileMenu->addSeparator();
  fileMenu->addAction(fileExamples);
  fileMenu->addSeparator();
  fileMenu->addAction(fileSettings);
  fileMenu->addAction(symEdit);
  fileMenu->addSeparator();
  fileMenu->addAction(applSettings);
  fileMenu->addAction(refreshSchPath);
  fileMenu->addSeparator();
  fileMenu->addAction(fileQuit);
  menuBar()->addMenu(fileMenu);

  // forward to QAction instances. effectively simulate button presses.
  editMenu = new QMenu(tr("&Edit"));  // menuBar entry editMenu
  editMenu->addAction(undo);
  editMenu->addAction(redo);
  editMenu->addSeparator();
  editMenu->addAction(editCut);
  editMenu->addAction(editCopy);
  editMenu->addAction(editPaste);
  editMenu->addAction(editDelete);
  editMenu->addSeparator();
  editMenu->addAction(select);
  editMenu->addAction(selectAll);
  editMenu->addAction(selectMarker);
  editMenu->addAction(editFind);
  editMenu->addAction(changeProps);
  editMenu->addAction(editRotate);
  editMenu->addAction(editMirror);
  editMenu->addAction(editMirrorY);
  editMenu->addAction(editActivate);
  editMenu->addSeparator();
  editMenu->addAction(intoH);
  editMenu->addAction(popH);
  menuBar()->addMenu(editMenu);

  alignMenu = new QMenu(tr("P&ositioning"));  // menuBar entry alignMenu
  alignMenu->addAction(moveText);
  alignMenu->addAction(onGrid);
  alignMenu->addSeparator();
  alignMenu->addAction(centerHor);
  alignMenu->addAction(centerVert);
  alignMenu->addSeparator();
  alignMenu->addAction(alignTop);
  alignMenu->addAction(alignBottom);
  alignMenu->addAction(alignLeft);
  alignMenu->addAction(alignRight);
  alignMenu->addSeparator();
  alignMenu->addAction(distrHor);
  alignMenu->addAction(distrVert);
  menuBar()->addMenu(alignMenu);

  insMenu = new QMenu(tr("&Insert"));  // menuBar entry insMenu
  insMenu->addAction(insWire);
  insMenu->addAction(insLabel);
  insMenu->addAction(insEquation);
  insMenu->addAction(insGround);
  insMenu->addAction(insPort);
  insMenu->addAction(setMarker);
  insMenu->addAction(insEntity);
  menuBar()->addMenu(insMenu);

  projMenu = new QMenu(tr("&Project"));  // menuBar entry projMenu
  projMenu->addAction(projNew);
  projMenu->addAction(projOpen);
  projMenu->addAction(addToProj);
  projMenu->addAction(projClose);
  projMenu->addAction(projDel);
  projMenu->addSeparator();
  projMenu->addAction(createLib);
  projMenu->addAction(createPkg);
  projMenu->addAction(extractPkg);
  projMenu->addSeparator();
  projMenu->addAction(importData);
  projMenu->addAction(graph2csv);
  projMenu->addSeparator();
  // TODO only enable if document is VA file
  projMenu->addAction(buildModule);
  projMenu->addAction(loadModule);
  menuBar()->addMenu(projMenu);

  toolMenu = new QMenu(tr("&Tools"));  // menuBar entry toolMenu
  toolMenu->addAction(callEditor);
  toolMenu->addAction(callFilter);
  toolMenu->addAction(callActiveFilter);
  toolMenu->addAction(callLine);
  toolMenu->addAction(callLib);
  toolMenu->addAction(callMatch);
  toolMenu->addAction(callAtt);
  toolMenu->addAction(callRes);
  toolMenu->addAction(callPowerComb);
  menuBar()->addMenu(toolMenu);

  simMenu = new QMenu(tr("&Simulation"));  // menuBar entry simMenu
  simMenu->addAction(simulate);
  simMenu->addAction(dpl_sch);
  simMenu->addAction(dcbias);
  simMenu->addAction(showMsg);
  simMenu->addAction(showNet);
  menuBar()->addMenu(simMenu);

  viewMenu = new QMenu(tr("&View"));  // menuBar entry viewMenu
  viewMenu->addAction(magAll);
  viewMenu->addAction(magOne);
  viewMenu->addAction(magPlus);
  viewMenu->addAction(magMinus);
  viewMenu->addSeparator();
  viewMenu->addAction(viewToolBar);
  viewMenu->addAction(viewStatusBar);
  viewMenu->addAction(viewBrowseDock);
  viewMenu->addAction(viewOctaveDock);
  menuBar()->addMenu(viewMenu);

  helpMenu = new QMenu(tr("&Help"));  // menuBar entry helpMenu
  helpMenu->addAction(helpOnline);

  //Fill submenu's with filenames of PDF documents
  QDir TechnicalDir = QDir(QucsSettings.DocDir.replace('\\','/'));

  if(TechnicalDir.cd("technical"))
  {
    helpTechnical = new QMenu(tr("&Technical Papers"));
    helpMenu->addMenu(helpTechnical);
    TechnicalDir.setFilter(QDir::Files);
    QStringList entries = TechnicalDir.entryList();
    for(int i=0;i<entries.size();i++)
    {
      QAction* helpTechnicalActions = new QAction(entries[i], this);
      helpTechnicalActions->setObjectName ( entries[i] );
      helpTechnicalActions->setStatusTip(tr("Open ")+entries[i]);
      helpTechnicalActions->setWhatsThis(tr("Open ")+entries[i]);
      connect(helpTechnicalActions, SIGNAL(triggered()), SLOT(slotHelpTechnical()));
      helpTechnical->addAction(helpTechnicalActions);
    }

  }

//Fill submenu's with filenames of PDF documents
  QDir ReportDir = QDir(QucsSettings.DocDir.replace('\\','/'));
  if(ReportDir.cd("report"))
  {
    helpReport = new QMenu(tr("Technical &Reports"));
    helpMenu->addMenu(helpReport);
    ReportDir.setFilter(QDir::Files);
    QStringList entries = ReportDir.entryList();
    for(int i=0;i<entries.size();i++)
    {
      QAction* helpReportActions = new QAction(entries[i], this);
      helpReportActions->setObjectName ( entries[i] );
      helpReportActions->setStatusTip(tr("Open ")+entries[i]);
      helpReportActions->setWhatsThis(tr("Open ")+entries[i]);
      connect(helpReportActions, SIGNAL(triggered()), SLOT(slotHelpReport()));
      helpReport->addAction(helpReportActions);
    }
  }

//Fill submenu's with filenames of PDF documents
  QDir TutorialDir = QDir(QucsSettings.DocDir.replace('\\','/'));
  if(TutorialDir.cd("tutorial"))
  {
    helpTutorial = new QMenu(tr("T&utorials"));
    helpMenu->addMenu(helpTutorial);
    TutorialDir.setFilter(QDir::Files);
    QStringList entries = TutorialDir.entryList();
    for(int i=0;i<entries.size();i++)
    {
      QAction* helpTutorialActions = new QAction(entries[i], this);
      helpTutorialActions->setObjectName ( entries[i] );
      helpTutorialActions->setStatusTip(tr("Open ")+entries[i]);
      helpTutorialActions->setWhatsThis(tr("Open ")+entries[i]);
      connect(helpTutorialActions, SIGNAL(triggered()), SLOT(slotHelpTutorial()));
      helpTutorial->addAction(helpTutorialActions);
    }
  }

  helpMenu->addSeparator();
  helpMenu->addAction(helpAboutApp);
  helpMenu->addAction(helpAboutQt);

  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);
}

// ----------------------------------------------------------
void QucsApp::initToolBar()
{
  fileToolbar = new QToolBar(tr("File"));
  fileToolbar->addAction(fileNew);
  fileToolbar->addAction(textNew);
  fileToolbar->addAction(fileOpen);
  fileToolbar->addAction(fileSave);
  fileToolbar->addAction(fileSaveAll);
  fileToolbar->addAction(fileClose);
  fileToolbar->addAction(filePrint);
  addToolBar(fileToolbar);

  editToolbar = new QToolBar(tr("Edit"));
  editToolbar->addAction(editCut);
  editToolbar->addAction(editCopy);
  editToolbar->addAction(editPaste);
  editToolbar->addAction(editDelete);
  editToolbar->addAction(undo);
  editToolbar->addAction(redo);
  addToolBar(editToolbar);

  viewToolbar = new QToolBar(tr("View"));
  viewToolbar->addAction(magAll);
  viewToolbar->addAction(magOne);
  viewToolbar->addAction(magPlus);
  viewToolbar->addAction(magMinus);
  addToolBar(viewToolbar);

  workToolbar = new QToolBar(tr("Work"));
  workToolbar->addAction(select);
  workToolbar->addAction(editActivate);
  workToolbar->addAction(editMirror);
  workToolbar->addAction(editMirrorY);
  workToolbar->addAction(editRotate);
  workToolbar->addAction(intoH);
  workToolbar->addAction(popH);
  workToolbar->addAction(insWire);
  workToolbar->addAction(insLabel);
  workToolbar->addAction(insEquation);
  workToolbar->addAction(insGround);
  workToolbar->addAction(insPort);
  workToolbar->addAction(simulate);
  workToolbar->addAction(dpl_sch);
  workToolbar->addAction(setMarker);
  workToolbar->addSeparator();    // <<<=======================
  addToolBar(workToolbar);
}

// ----------------------------------------------------------
void QucsApp::initStatusBar()
{
  // To reserve enough space, insert the longest text and rewrite it afterwards.
  WarningLabel = new QLabel(tr("no warnings"), statusBar());
  statusBar()->addPermanentWidget(WarningLabel, 0);

  PositionLabel = new QLabel("0 : 0", statusBar());
  PositionLabel->setAlignment(Qt::AlignRight);
  statusBar()->addPermanentWidget(PositionLabel, 0);

  statusBar()->showMessage(tr("Ready."), 2000);
}

// ----------------------------------------------------------
void QucsApp::slotShowWarnings()
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
    misc::setWidgetForegroundColor(WarningLabel, Qt::red);
  else
    misc::setWidgetForegroundColor(WarningLabel, Qt::black);

  if(ResultState < 9)
    QTimer::singleShot(500, this, SLOT(slotShowWarnings()));
  else
    ResultState = 0;
}

// ----------------------------------------------------------
void QucsApp::slotResetWarnings()
{
  QFont f = WarningLabel->font();   // reset warning label
  f.setWeight(QFont::Normal);
  WarningLabel->setFont(f);
  misc::setWidgetForegroundColor(WarningLabel, Qt::black);
  WarningLabel->setText(tr("no warnings"));
}

// ----------------------------------------------------------
void QucsApp::printCursorPosition(int x, int y)
{
  PositionLabel->setText(QString::number(x)+" : "+QString::number(y));
  PositionLabel->setMinimumWidth(PositionLabel->width());
}

// --------------------------------------------------------------
// called by document, update undo state
void QucsApp::slotUpdateUndo(bool isEnabled)
{
  undo->setEnabled(isEnabled);
}

// --------------------------------------------------------------
// called by document, update redo state
void QucsApp::slotUpdateRedo(bool isEnabled)
{
  redo->setEnabled(isEnabled);
}

// ----------------------------------------------------------
// turn Toolbar on or off
void QucsApp::slotViewToolBar(bool toggle)
{
  fileToolbar->setVisible(toggle);
  editToolbar->setVisible(toggle);
  viewToolbar->setVisible(toggle);
  workToolbar->setVisible(toggle);
}

// ----------------------------------------------------------
// turn Statusbar on or off
void QucsApp::slotViewStatusBar(bool toggle)
{
  statusBar()->setVisible(toggle);
}

// ----------------------------------------------------------
// turn Brwose Dock Window on or off
void QucsApp::slotViewBrowseDock(bool toggle)
{
  dock->setVisible(toggle);
}

// ----------------------------------------------------------
void QucsApp::slotToggleDock(bool on)
{
  viewBrowseDock->blockSignals(true);
  viewBrowseDock->setChecked(on);
  viewBrowseDock->blockSignals(false);
}

// ----------------------------------------------------------
// turn Octave Dock Window on or off
void QucsApp::slotViewOctaveDock(bool toggle)
{
  octDock->setVisible(toggle);
  if (toggle) {
    octave->startOctave();
  }
}

// ----------------------------------------------------------
void QucsApp::slotToggleOctave(bool on)
{
  viewOctaveDock->blockSignals(true);
  viewOctaveDock->setChecked(on);
  viewOctaveDock->blockSignals(false);
}

// ----------------------------------------------------------
// here? TODO
#if 0
void QucsApp::connectToolbar(QucsDoc* a)
{
	// toolbar->something(a)?

  disconnect(editDelete);
  a->connectAction(editDelete);
  ...

  //connect(editDelete, &QAction::toggled, this, &QucsApp::slotEditDelete);
  //connect(editDelete, &QAction::toggled, this, &QucsApp::slotToggle);
}
#endif
// ----------------------------------------------------------
// ----------------------------------------------------------
