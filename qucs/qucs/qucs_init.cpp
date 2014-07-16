/***************************************************************************
                               qucs_init.cpp
                              ---------------
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
#include <QtGui>



#include "main.h"
#include "qucs.h"
#include "dialogs/vtabbeddockwidget.h"
#include "octave_window.h"


/*
QAction* Q3Action(QString a,QIcon b, QString c,unsigned int d, QObject* e)
{
	QAction* action=new QAction(b,c,e);
	action->setShortcut((Qt::ShortcutContext)d);
	return action;
}

QAction* Q3Action(QString a, QString c, unsigned int d, QObject* e)
{
	QAction* action=new QAction(c,e);
	action->setShortcut((Qt::ShortcutContext)d);
	return action;
}
*/

void 
setDefaultShortcut() 
{
  QMap<QString, QString>* map = &QucsSettings.Shortcut;
  
  map->insert("fileNew",	    QString(QKeySequence(Qt::CTRL+Qt::Key_N)));
  map->insert("textNew",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_V)));
  map->insert("fileOpen",	    QString(QKeySequence(Qt::CTRL+Qt::Key_O)));
  map->insert("fileSave",	    QString(QKeySequence(Qt::CTRL+Qt::Key_S)));
  map->insert("fileSaveAs",	    QString(QKeySequence(Qt::CTRL+Qt::Key_Minus)));
  map->insert("fileSaveAll",	QString(QKeySequence(Qt::CTRL+Qt::Key_Plus)));
  map->insert("fileClose",	    QString(QKeySequence(Qt::CTRL+Qt::Key_W)));
  map->insert("fileExamples",	QString(QKeySequence()));
  map->insert("symEdit",	    QString(QKeySequence(Qt::Key_F9)));
  map->insert("fileSettings",	QString(QKeySequence(Qt::CTRL+Qt::Key_Period)));
  map->insert("filePrint",	    QString(QKeySequence(Qt::CTRL+Qt::Key_P)));
  map->insert("filePrintFit",	QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_P)));
  map->insert("fileQuit",	    QString(QKeySequence(Qt::CTRL+Qt::Key_Q)));
  map->insert("applSettings",	QString(QKeySequence(Qt::CTRL+Qt::Key_Comma)));
  map->insert("refreshSchPath",	QString(QKeySequence()));
  map->insert("alignTop",	    QString(QKeySequence(Qt::CTRL+Qt::Key_T)));
  map->insert("alignBottom",	QString(QKeySequence()));
  map->insert("alignLeft",	    QString(QKeySequence()));
  map->insert("alignRight",	    QString(QKeySequence()));
  map->insert("distrHor",	    QString(QKeySequence()));
  map->insert("distrVert",	    QString(QKeySequence()));
  map->insert("centerHor",	    QString(QKeySequence()));
  map->insert("centerVert",	    QString(QKeySequence()));
  map->insert("onGrid",	        QString(QKeySequence(Qt::CTRL+Qt::Key_U)));
  map->insert("moveText",	    QString(QKeySequence(Qt::CTRL+Qt::Key_K)));
  map->insert("changeProps",	QString(QKeySequence(Qt::Key_F7)));
  map->insert("editCut",	    QString(QKeySequence(Qt::CTRL+Qt::Key_X)));
  map->insert("editCopy",	    QString(QKeySequence(Qt::CTRL+Qt::Key_C)));
  map->insert("editPaste",	    QString(QKeySequence(Qt::CTRL+Qt::Key_V)));
  map->insert("editDelete",	    QString(QKeySequence(Qt::Key_Delete)));
  map->insert("editFind",	    QString(QKeySequence(Qt::CTRL+Qt::Key_F)));
  map->insert("editFindAgain",	QString(QKeySequence(Qt::Key_F3)));
  map->insert("exportAsImage",	QString(QKeySequence()));
  map->insert("undo",	        QString(QKeySequence(Qt::CTRL+Qt::Key_Z)));
  map->insert("redo",	        QString(QKeySequence(Qt::CTRL+Qt::Key_Y)));
  map->insert("projNew",	    QString(QKeySequence(Qt::CTRL+Qt::Key_N)));
  map->insert("projOpen",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O)));
  map->insert("projDel",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D)));
  map->insert("projClose",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_W)));
  map->insert("addToProj",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_A)));
  map->insert("createLib",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_L)));
  map->insert("createPkg",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_Z)));
  map->insert("extractPkg",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_X)));
  map->insert("importData",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_I)));
  map->insert("graph2csv",	    QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_C)));
  map->insert("buildModule",	QString(QKeySequence()));
  map->insert("loadModule",  	QString(QKeySequence()));
  map->insert("magAll",	        QString(QKeySequence(Qt::Key_0)));
  map->insert("magOne",	        QString(QKeySequence(Qt::Key_1)));
  map->insert("magPlus",	    QString(QKeySequence(Qt::Key_Plus)));
  map->insert("magMinus",	    QString(QKeySequence(Qt::Key_Minus)));
  map->insert("select",	        QString(QKeySequence()));
  map->insert("selectAll",	    QString(QKeySequence(Qt::CTRL+Qt::Key_A)));
  map->insert("selectMarker",	QString(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_M)));
  map->insert("editRotate",	    QString(QKeySequence(Qt::CTRL+Qt::Key_R)));
  map->insert("editMirror",	    QString(QKeySequence(Qt::CTRL+Qt::Key_J)));
  map->insert("editMirrorY",	QString(QKeySequence(Qt::CTRL+Qt::Key_M)));
  map->insert("intoH",	        QString(QKeySequence(Qt::CTRL+Qt::Key_I)));
  map->insert("popH",	        QString(QKeySequence(Qt::CTRL+Qt::Key_H)));
  map->insert("editActivate",	QString(QKeySequence(Qt::CTRL+Qt::Key_D)));
  map->insert("insEquation",	QString(QKeySequence(Qt::CTRL+Qt::Key_Less)));
  map->insert("insGround",	    QString(QKeySequence(Qt::CTRL+Qt::Key_G)));
  map->insert("insPort",	    QString(QKeySequence()));
  map->insert("insWire",	    QString(QKeySequence(Qt::CTRL+Qt::Key_E)));
  map->insert("insLabel",	    QString(QKeySequence(Qt::CTRL+Qt::Key_L)));
  map->insert("insEntity",	    QString(QKeySequence(Qt::CTRL+Qt::Key_Space)));
  map->insert("callEditor",     QString(QKeySequence(Qt::CTRL+Qt::Key_1)));
  map->insert("callFilter",     QString(QKeySequence(Qt::CTRL+Qt::Key_2)));
  map->insert("callLine",	    QString(QKeySequence(Qt::CTRL+Qt::Key_3)));
  map->insert("callLib",	    QString(QKeySequence(Qt::CTRL+Qt::Key_4)));
  map->insert("callMatch",	    QString(QKeySequence(Qt::CTRL+Qt::Key_5)));
  map->insert("callAtt",	    QString(QKeySequence(Qt::CTRL+Qt::Key_6)));
  map->insert("simulate",	    QString(QKeySequence(Qt::Key_F2)));
  map->insert("dpl_sch",	    QString(QKeySequence(Qt::Key_F4)));
  map->insert("dcbias",	        QString(QKeySequence(Qt::Key_F8)));
  map->insert("setMarker",	    QString(QKeySequence(Qt::CTRL+Qt::Key_B)));
  map->insert("showMsg",	    QString(QKeySequence(Qt::Key_F5)));
  map->insert("showNet",	    QString(QKeySequence(Qt::Key_F6)));
  map->insert("viewToolBar",	QString(QKeySequence()));
  map->insert("viewStatusBar",	QString(QKeySequence()));
  map->insert("viewBrowseDock", QString(QKeySequence()));
  map->insert("viewOctaveDock", QString(QKeySequence()));
  map->insert("helpIndex",	    QString(QKeySequence(Qt::Key_F1)));
  map->insert("helpGetStart",	QString(QKeySequence()));
  map->insert("helpAboutApp",	QString(QKeySequence()));
  map->insert("helpAboutQt",	QString(QKeySequence()));
}

void
QucsApp::setShortcut()
{
  qDebug("now set shortcut");
  QMap<QString, QString>* map = &QucsSettings.Shortcut;
  
  fileNew	    ->setShortcut(QKeySequence(map->value("fileNew")));
  textNew	    ->setShortcut(QKeySequence(map->value("textNew")));
  fileOpen	    ->setShortcut(QKeySequence(map->value("fileOpen")));
  fileSave	    ->setShortcut(QKeySequence(map->value("fileSave")));
  fileSaveAs	->setShortcut(QKeySequence(map->value("fileSaveAs")));
  fileSaveAll	->setShortcut(QKeySequence(map->value("fileSaveAll")));
  fileClose	    ->setShortcut(QKeySequence(map->value("fileClose")));
  fileExamples	->setShortcut(QKeySequence(map->value("fileExamples")));
  symEdit	    ->setShortcut(QKeySequence(map->value("symEdit")));
  fileSettings	->setShortcut(QKeySequence(map->value("fileSettings")));
  filePrint	    ->setShortcut(QKeySequence(map->value("filePrint")));
  filePrintFit	->setShortcut(QKeySequence(map->value("filePrintFit")));
  fileQuit	    ->setShortcut(QKeySequence(map->value("fileQuit")));
  applSettings	->setShortcut(QKeySequence(map->value("applSettings")));
  refreshSchPath->setShortcut(QKeySequence(map->value("refreshSchPath")));
  alignTop	    ->setShortcut(QKeySequence(map->value("alignTop")));
  alignBottom	->setShortcut(QKeySequence(map->value("alignBottom")));
  alignLeft	    ->setShortcut(QKeySequence(map->value("alignLeft")));
  alignRight	->setShortcut(QKeySequence(map->value("alignRight")));
  distrHor	    ->setShortcut(QKeySequence(map->value("distrHor")));
  distrVert	    ->setShortcut(QKeySequence(map->value("distrVert")));
  centerHor	    ->setShortcut(QKeySequence(map->value("centerHor")));
  centerVert	->setShortcut(QKeySequence(map->value("centerVert")));
  onGrid	    ->setShortcut(QKeySequence(map->value("onGrid")));
  moveText	    ->setShortcut(QKeySequence(map->value("moveText")));
  changeProps	->setShortcut(QKeySequence(map->value("changeProps")));
  editCut	    ->setShortcut(QKeySequence(map->value("editCut")));
  editCopy	    ->setShortcut(QKeySequence(map->value("editCopy")));
  editPaste	    ->setShortcut(QKeySequence(map->value("editPaste")));
  editDelete	->setShortcut(QKeySequence(map->value("editDelete")));
  editFind	    ->setShortcut(QKeySequence(map->value("editFind")));
  editFindAgain	->setShortcut(QKeySequence(map->value("editFindAgain")));
  exportAsImage	->setShortcut(QKeySequence(map->value("exportAsImage")));
  undo	        ->setShortcut(QKeySequence(map->value("undo")));
  redo	        ->setShortcut(QKeySequence(map->value("redo")));
  projNew	    ->setShortcut(QKeySequence(map->value("projNew")));
  projOpen	    ->setShortcut(QKeySequence(map->value("projOpen")));
  projDel	    ->setShortcut(QKeySequence(map->value("projDel")));
  projClose	    ->setShortcut(QKeySequence(map->value("projClose")));
  addToProj	    ->setShortcut(QKeySequence(map->value("addToProj")));
  createLib	    ->setShortcut(QKeySequence(map->value("createLib")));
  createPkg	    ->setShortcut(QKeySequence(map->value("createPkg")));
  extractPkg	->setShortcut(QKeySequence(map->value("extractPkg")));
  importData	->setShortcut(QKeySequence(map->value("importData")));
  graph2csv	    ->setShortcut(QKeySequence(map->value("csv")));
  buildModule	->setShortcut(QKeySequence(map->value("buildModule")));
  loadModule  	->setShortcut(QKeySequence(map->value("loadModule")));
  magAll	    ->setShortcut(QKeySequence(map->value("magAll")));
  magOne	    ->setShortcut(QKeySequence(map->value("magOne")));
  magPlus	    ->setShortcut(QKeySequence(map->value("magPlus")));
  magMinus	    ->setShortcut(QKeySequence(map->value("magMinus")));
  select	    ->setShortcut(QKeySequence(map->value("select")));
  selectAll	    ->setShortcut(QKeySequence(map->value("selectAll")));
  selectMarker	->setShortcut(QKeySequence(map->value("selectMarker")));
  editRotate	->setShortcut(QKeySequence(map->value("editRotate")));
  editMirror	->setShortcut(QKeySequence(map->value("editMirror")));
  editMirrorY	->setShortcut(QKeySequence(map->value("editMirrorY")));
  intoH	        ->setShortcut(QKeySequence(map->value("intoH")));
  popH	        ->setShortcut(QKeySequence(map->value("popH")));
  editActivate	->setShortcut(QKeySequence(map->value("editActivate")));
  insEquation	->setShortcut(QKeySequence(map->value("insEquation")));
  insGround	    ->setShortcut(QKeySequence(map->value("insGround")));
  insPort	    ->setShortcut(QKeySequence(map->value("insPort")));
  insWire	    ->setShortcut(QKeySequence(map->value("insWire")));
  insLabel	    ->setShortcut(QKeySequence(map->value("insLabel")));
  insEntity	    ->setShortcut(QKeySequence(map->value("insEntity")));
  callEditor    ->setShortcut(QKeySequence(map->value("callEditor")));
  callFilter    ->setShortcut(QKeySequence(map->value("callFilter")));
  callLine	    ->setShortcut(QKeySequence(map->value("callLine")));
  callLib	    ->setShortcut(QKeySequence(map->value("callLib")));
  callMatch	    ->setShortcut(QKeySequence(map->value("callMatch")));
  callAtt	    ->setShortcut(QKeySequence(map->value("callAtt")));
  simulate	    ->setShortcut(QKeySequence(map->value("simulate")));
  dpl_sch	    ->setShortcut(QKeySequence(map->value("dpl_sch")));
  dcbias	    ->setShortcut(QKeySequence(map->value("dcbias")));
  setMarker	    ->setShortcut(QKeySequence(map->value("setMarker")));
  showMsg	    ->setShortcut(QKeySequence(map->value("showMsg")));
  showNet	    ->setShortcut(QKeySequence(map->value("showNet")));
  viewToolBar	->setShortcut(QKeySequence(map->value("viewToolBar")));
  viewStatusBar	->setShortcut(QKeySequence(map->value("viewStatusBar")));
  viewBrowseDock->setShortcut(QKeySequence(map->value("viewBrowseDock")));
  viewOctaveDock->setShortcut(QKeySequence(map->value("viewOctaveDock")));
  helpIndex	    ->setShortcut(QKeySequence(map->value("helpIndex")));
  helpGetStart	->setShortcut(QKeySequence(map->value("helpGetStart")));
  helpAboutApp	->setShortcut(QKeySequence(map->value("helpAboutApp")));
  helpAboutQt	->setShortcut(QKeySequence(map->value("helpAboutQt")));
}

// ----------------------------------------------------------
// initializes all QActions of the application
void QucsApp::initActions()
{
  activeAction = 0;   // no active action

  // note: first argument of QAction() for backward compatibility Qt < 3.2

  fileNew = new QAction(QIcon((":/bitmaps/filenew.png")), tr("&New"), this);
  fileNew->setStatusTip(tr("Creates a new document"));
  fileNew->setWhatsThis(
	        tr("New\n\nCreates a new schematic or data display document"));
  connect(fileNew, SIGNAL(activated()), SLOT(slotFileNew()));

  textNew = new QAction(QIcon((":/bitmaps/textnew.png")), tr("New &Text"), this);
  textNew->setStatusTip(tr("Creates a new text document"));
  textNew->setWhatsThis(tr("New Text\n\nCreates a new text document"));
  connect(textNew, SIGNAL(activated()), SLOT(slotTextNew()));

  fileOpen = new QAction(QIcon((":/bitmaps/fileopen.png")),	tr("&Open..."), this);
  fileOpen->setStatusTip(tr("Opens an existing document"));
  fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(fileOpen, SIGNAL(activated()), SLOT(slotFileOpen()));

  fileSave = new QAction(QIcon((":/bitmaps/filesave.png")),	tr("&Save"), this);
  fileSave->setStatusTip(tr("Saves the current document"));
  fileSave->setWhatsThis(tr("Save File\n\nSaves the current document"));
  connect(fileSave, SIGNAL(activated()), SLOT(slotFileSave()));

  fileSaveAs = new QAction(tr("Save as..."), this);
  fileSaveAs->setStatusTip(
	tr("Saves the current document under a new filename"));
  fileSaveAs->setWhatsThis(
	tr("Save As\n\nSaves the current document under a new filename"));
  connect(fileSaveAs, SIGNAL(activated()), SLOT(slotFileSaveAs()));

  fileSaveAll = new QAction(QIcon((":/bitmaps/filesaveall.png")),	tr("Save &All"), this);
  fileSaveAll->setStatusTip(tr("Saves all open documents"));
  fileSaveAll->setWhatsThis(tr("Save All Files\n\nSaves all open documents"));
  connect(fileSaveAll, SIGNAL(activated()), SLOT(slotFileSaveAll()));

  fileClose = new QAction(QIcon((":/bitmaps/fileclose.png")), tr("&Close"), this);
  fileClose->setStatusTip(tr("Closes the current document"));
  fileClose->setWhatsThis(tr("Close File\n\nCloses the current document"));
  connect(fileClose, SIGNAL(activated()), SLOT(slotFileClose()));

  fileExamples = new QAction(tr("&Examples"), this);
  fileExamples->setStatusTip(tr("Opens a file explorer with example documents"));
  fileExamples->setWhatsThis(
	        tr("Examples\n\nOpens a file explorer with example documents"));
  connect(fileExamples, SIGNAL(activated()), SLOT(slotFileExamples()));


  symEdit = new QAction(tr("&Edit Circuit Symbol"), this);
  symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
  symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  connect(symEdit, SIGNAL(activated()), SLOT(slotSymbolEdit()));

  fileSettings = new QAction(tr("&Document Settings..."), this);
  fileSettings->setStatusTip(tr("Document Settings"));
  fileSettings->setWhatsThis(tr("Settings\n\nSets properties of the file"));
  connect(fileSettings, SIGNAL(activated()), SLOT(slotFileSettings()));

  filePrint = new QAction(QIcon((":/bitmaps/fileprint.png")), tr("&Print..."), this);
  filePrint->setStatusTip(tr("Prints the current document"));
  filePrint->setWhatsThis(tr("Print File\n\nPrints the current document"));
  connect(filePrint, SIGNAL(activated()), SLOT(slotFilePrint()));

  filePrintFit = new QAction(tr("Print Fit to Page..."), this);
  filePrintFit->setStatusTip(tr("Print Fit to Page"));
  filePrintFit->setWhatsThis(
	tr("Print Fit to Page\n\n"
	   "Print and fit content to the page size"));
  connect(filePrintFit, SIGNAL(activated()), SLOT(slotFilePrintFit()));

  fileQuit = new QAction(tr("E&xit"), this);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(activated()), SLOT(slotFileQuit()));

  applSettings = new QAction(tr("Application Settings..."), this);
  applSettings->setStatusTip(tr("Application Settings"));
  applSettings->setWhatsThis(
	tr("Qucs Settings\n\nSets properties of the application"));
  connect(applSettings, SIGNAL(activated()), SLOT(slotApplSettings()));

  refreshSchPath = new QAction(tr("Refresh Search Path..."), this);
  //refreshSchPath->setShortcut(Qt::CTRL+Qt::Key_Comma);
  refreshSchPath->setStatusTip(tr("Refresh Search Path"));
  refreshSchPath->setWhatsThis(
    tr("Refresh Path\n\nRechecks the list of paths for subcircuit files."));
  connect(refreshSchPath, SIGNAL(activated()), SLOT(slotRefreshSchPath()));

  alignTop = new QAction(tr("Align top"), this);
  alignTop->setStatusTip(tr("Align top selected elements"));
  alignTop->setWhatsThis(
	tr("Align top\n\nAlign selected elements to their upper edge"));
  connect(alignTop, SIGNAL(activated()), SLOT(slotAlignTop()));

  alignBottom = new QAction(tr("Align bottom"), this);
  alignBottom->setStatusTip(tr("Align bottom selected elements"));
  alignBottom->setWhatsThis(
	tr("Align bottom\n\nAlign selected elements to their lower edge"));
  connect(alignBottom, SIGNAL(activated()), SLOT(slotAlignBottom()));

  alignLeft = new QAction(tr("Align left"), this);
  alignLeft->setStatusTip(tr("Align left selected elements"));
  alignLeft->setWhatsThis(
	tr("Align left\n\nAlign selected elements to their left edge"));
  connect(alignLeft, SIGNAL(activated()), SLOT(slotAlignLeft()));

  alignRight = new QAction(tr("Align right"), this);
  alignRight->setStatusTip(tr("Align right selected elements"));
  alignRight->setWhatsThis(
	tr("Align right\n\nAlign selected elements to their right edge"));
  connect(alignRight, SIGNAL(activated()), SLOT(slotAlignRight()));

  distrHor = new QAction(tr("Distribute horizontally"), this);
  distrHor->setStatusTip(tr("Distribute equally horizontally"));
  distrHor->setWhatsThis(
	tr("Distribute horizontally\n\n"
	   "Distribute horizontally selected elements"));
  connect(distrHor, SIGNAL(activated()), SLOT(slotDistribHoriz()));

  distrVert = new QAction(tr("Distribute vertically"), this);
  distrVert->setStatusTip(tr("Distribute equally vertically"));
  distrVert->setWhatsThis(
	tr("Distribute vertically\n\n"
	   "Distribute vertically selected elements"));
  connect(distrVert, SIGNAL(activated()), SLOT(slotDistribVert()));

  centerHor = new QAction(tr("Center horizontally"), this);
  centerHor->setStatusTip(tr("Center horizontally selected elements"));
  centerHor->setWhatsThis(
	tr("Center horizontally\n\nCenter horizontally selected elements"));
  connect(centerHor, SIGNAL(activated()), SLOT(slotCenterHorizontal()));

  centerVert = new QAction(tr("Center vertically"), this);
  centerVert->setStatusTip(tr("Center vertically selected elements"));
  centerVert->setWhatsThis(
	tr("Center vertically\n\nCenter vertically selected elements"));
  connect(centerVert, SIGNAL(activated()), SLOT(slotCenterVertical()));

  onGrid = new QAction(tr("Set on Grid"), this);
  onGrid->setStatusTip(tr("Sets selected elements on grid"));
  onGrid->setWhatsThis(
	tr("Set on Grid\n\nSets selected elements on grid"));
  onGrid->setToggleAction(true);
  connect(onGrid, SIGNAL(toggled(bool)), SLOT(slotOnGrid(bool)));

  moveText = new QAction(tr("Move Component Text"), this);
  moveText->setStatusTip(tr("Moves the property text of components"));
  moveText->setWhatsThis(
	tr("Move Component Text\n\nMoves the property text of components"));
  moveText->setToggleAction(true);
  connect(moveText, SIGNAL(toggled(bool)), SLOT(slotMoveText(bool)));

  changeProps = new QAction(tr("Replace..."), this);
  changeProps->setStatusTip(tr("Replace component properties or VHDL code"));
  changeProps->setWhatsThis(
	tr("Replace\n\nChange component properties\nor\ntext in VHDL code"));
  connect(changeProps, SIGNAL(activated()), SLOT(slotChangeProps()));

  editCut = new QAction(QIcon((":/bitmaps/editcut.png")),	tr("Cu&t"), this);
  editCut->setStatusTip(
	tr("Cuts out the selection and puts it into the clipboard"));
  editCut->setWhatsThis(
	tr("Cut\n\nCuts out the selection and puts it into the clipboard"));
  connect(editCut, SIGNAL(activated()), SLOT(slotEditCut()));

  editCopy = new QAction(QIcon((":/bitmaps/editcopy.png")), tr("&Copy"), this);
  editCopy->setStatusTip(
	tr("Copies the selection into the clipboard"));
  editCopy->setWhatsThis(
	tr("Copy\n\nCopies the selection into the clipboard"));
  connect(editCopy, SIGNAL(activated()), SLOT(slotEditCopy()));

  editPaste = new QAction(QIcon((":/bitmaps/editpaste.png")), tr("&Paste"), this);
  editPaste->setStatusTip(
	tr("Pastes the clipboard contents to the cursor position"));
  editPaste->setWhatsThis(
	tr("Paste\n\nPastes the clipboard contents to the cursor position"));
  editPaste->setToggleAction(true);
  connect(editPaste, SIGNAL(toggled(bool)), SLOT(slotEditPaste(bool)));

  editDelete = new QAction(QIcon((":/bitmaps/editdelete.png")), tr("&Delete"), this);
  editDelete->setStatusTip(tr("Deletes the selected components"));
  editDelete->setWhatsThis(tr("Delete\n\nDeletes the selected components"));
  editDelete->setToggleAction(true);
  connect(editDelete, SIGNAL(toggled(bool)), SLOT(slotEditDelete(bool)));

  editFind = new QAction(tr("Find..."), this);
  editFind->setStatusTip(tr("Find a piece of text"));
  editFind->setWhatsThis(tr("Find\n\nSearches for a piece of text"));
  connect(editFind, SIGNAL(activated()), SLOT(slotEditFind()));

  editFindAgain = new QAction(tr("Find Again"), this);
  editFindAgain->setStatusTip(tr("Find same text again"));
  editFindAgain->setWhatsThis(
                 tr("Find\n\nSearches for the same piece of text again"));
  connect(editFindAgain, SIGNAL(activated()), SLOT(slotEditFindAgain()));

  // to ease usage with notebooks, backspace can also be used to delete
  // currently not supported
  //mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Backspace),
  //                       editDelete, SLOT(toggle()) );

  exportAsImage = new QAction(tr("Export as image"),this);
  connect(exportAsImage,SIGNAL(activated()),SLOT(slotSaveSchematicToGraphicsFile()));

  // cursor left/right/up/down to move marker on a graph
  cursorLeft = new QShortcut(QKeySequence(Qt::Key_Left), this);
  connect(cursorLeft, SIGNAL(activated()), SLOT(slotCursorLeft()));

  cursorRight = new QShortcut(QKeySequence(Qt::Key_Right), this);
  connect(cursorRight, SIGNAL(activated()), SLOT(slotCursorRight()));

  cursorUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
  connect(cursorUp, SIGNAL(activated()), SLOT(slotCursorUp()));

  cursorDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
  connect(cursorDown, SIGNAL(activated()), SLOT(slotCursorDown()));


/*
  mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Left),
			this, SLOT(slotCursorLeft()));
  mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Right),
			this, SLOT(slotCursorRight()));
  mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Up),
			this, SLOT(slotCursorUp()));
  mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Down),
			this, SLOT(slotCursorDown()));
  mainAccel->connectItem(mainAccel->insertItem(Qt::Key_Tab),
			this, SLOT(slotNextTab()));
*/

  undo = new QAction(QIcon((":/bitmaps/undo.png")), tr("&Undo"), this);
  undo->setStatusTip(tr("Undoes the last command"));
  undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
  connect(undo, SIGNAL(activated()), SLOT(slotEditUndo()));

  redo = new QAction(QIcon((":/bitmaps/redo.png")), tr("&Redo"), this);
  redo->setStatusTip(tr("Redoes the last command"));
  redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
  connect(redo, SIGNAL(activated()), SLOT(slotEditRedo()));

  projNew = new QAction(tr("&New Project..."), this);
  projNew->setStatusTip(tr("Creates a new project"));
  projNew->setWhatsThis(tr("New Project\n\nCreates a new project"));
  connect(projNew, SIGNAL(activated()), SLOT(slotProjNewButt()));

  projOpen = new QAction(tr("&Open Project..."), this);
  projOpen->setStatusTip(tr("Opens an existing project"));
  projOpen->setWhatsThis(tr("Open Project\n\nOpens an existing project"));
  connect(projOpen, SIGNAL(activated()), SLOT(slotMenuOpenProject()));

  projDel = new QAction(tr("&Delete Project..."), this);
  projDel->setStatusTip(tr("Deletes an existing project"));
  projDel->setWhatsThis(tr("Delete Project\n\nDeletes an existing project"));
  connect(projDel, SIGNAL(activated()), SLOT(slotMenuDelProject()));

  projClose = new QAction(tr("&Close Project"), this);
  projClose->setStatusTip(tr("Closes the current project"));
  projClose->setWhatsThis(tr("Close Project\n\nCloses the current project"));
  connect(projClose, SIGNAL(activated()), SLOT(slotMenuCloseProject()));

  addToProj = new QAction(tr("&Add Files to Project..."), this);
  addToProj->setStatusTip(tr("Copies files to project directory"));
  addToProj->setWhatsThis(
	tr("Add Files to Project\n\nCopies files to project directory"));
  connect(addToProj, SIGNAL(activated()), SLOT(slotAddToProject()));

  createLib = new QAction(tr("Create &Library..."), this);
  createLib->setStatusTip(tr("Create Library from Subcircuits"));
  createLib->setWhatsThis(
	tr("Create Library\n\nCreate Library from Subcircuits"));
  connect(createLib, SIGNAL(activated()), SLOT(slotCreateLib()));

  createPkg = new QAction(tr("Create &Package..."), this);
  createPkg->setStatusTip(tr("Create compressed Package from Projects"));
  createPkg->setWhatsThis(
    tr("Create Package\n\nCreate compressed Package from complete Projects"));
  connect(createPkg, SIGNAL(activated()), SLOT(slotCreatePackage()));

  extractPkg = new QAction(tr("E&xtract Package..."),  this);
  extractPkg->setStatusTip(tr("Install Content of a Package"));
  extractPkg->setWhatsThis(
	tr("Extract Package\n\nInstall Content of a Package"));
  connect(extractPkg, SIGNAL(activated()), SLOT(slotExtractPackage()));

  importData = new QAction(tr("&Import/Export Data..."), this);
  importData->setStatusTip(tr("Convert data file"));
  importData->setWhatsThis(
	tr("Import/Export Data\n\nConvert data file to various file formats"));
  connect(importData, SIGNAL(activated()), SLOT(slotImportData()));

  graph2csv = new QAction(tr("Export to &CSV..."), this);
  graph2csv->setStatusTip(tr("Convert graph data to CSV file"));
  graph2csv->setWhatsThis(
	tr("Export to CSV\n\nConvert graph data to CSV file"));
  connect(graph2csv, SIGNAL(activated()), SLOT(slotExportGraphAsCsv()));

  buildModule = new QAction(tr("Build Verilog-A module..."), this);
  buildModule->setStatusTip(tr("Run admsXml and C++ compiler"));
  buildModule->setWhatsThis(tr("Build Verilog-A module\nRuns amdsXml and C++ compiler"));
  connect(buildModule, SIGNAL(activated()), SLOT(slotBuildModule()));

  loadModule = new QAction(tr("Load Verilog-A module..."), this);
  loadModule->setStatusTip(tr("Select Verilog-A symbols to be loaded"));
  loadModule->setWhatsThis(tr("Load Verilog-A module\nLet the user select and load symbols"));
  connect(loadModule, SIGNAL(activated()), SLOT(slotLoadModule()));

  magAll = new QAction(QIcon((":/bitmaps/viewmagfit.png")), tr("View All"), this);
  magAll->setStatusTip(tr("Show the whole page"));
  magAll->setWhatsThis(tr("View All\n\nShows the whole page content"));
  connect(magAll, SIGNAL(activated()), SLOT(slotShowAll()));

  magOne = new QAction(QIcon((":/bitmaps/viewmag1.png")), tr("View 1:1"), this);
  magOne->setStatusTip(tr("Views without magnification"));
  magOne->setWhatsThis(tr("View 1:1\n\nShows the page content without magnification"));
  connect(magOne, SIGNAL(activated()), SLOT(slotShowOne()));

  magPlus = new QAction(QIcon((":/bitmaps/viewmag+.png")),	tr("Zoom in"), this);
  magPlus->setStatusTip(tr("Zooms into the current view"));
  magPlus->setWhatsThis(tr("Zoom in\n\nZooms the current view"));
  magPlus->setToggleAction(true);
  connect(magPlus, SIGNAL(toggled(bool)), SLOT(slotZoomIn(bool)));

  magMinus = new QAction(QIcon((":/bitmaps/viewmag-.png")), tr("Zoom out"), this);
  magMinus->setStatusTip(tr("Zooms out the current view"));
  magMinus->setWhatsThis(tr("Zoom out\n\nZooms out the current view"));
  connect(magMinus, SIGNAL(activated()), SLOT(slotZoomOut()));

  QAction *escape = new QAction(this);
  escape->setShortcut(Qt::Key_Escape);
  connect(escape, SIGNAL(activated()), SLOT(slotEscape()));
  this->addAction(escape);

  select = new QAction(QIcon((":/bitmaps/pointer.png")), tr("Select"), this);
  select->setStatusTip(tr("Activate select mode"));
  select->setWhatsThis(tr("Select\n\nActivates select mode"));
  select->setToggleAction(true);
  connect(select, SIGNAL(toggled(bool)), SLOT(slotSelect(bool)));

  selectAll = new QAction(tr("Select All"), this);
  selectAll->setStatusTip(tr("Selects all elements"));
  selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(selectAll, SIGNAL(activated()), SLOT(slotSelectAll()));

  selectMarker = new QAction(tr("Select Markers"), this);
  selectMarker->setStatusTip(tr("Selects all markers"));
  selectMarker->setWhatsThis(
	tr("Select Markers\n\nSelects all diagram markers of the document"));
  connect(selectMarker, SIGNAL(activated()), SLOT(slotSelectMarker()));

  editRotate = new QAction(QIcon((":/bitmaps/rotate_ccw.png")), tr("Rotate"), this);
  editRotate->setStatusTip(tr("Rotates the selected component by 90\x00B0"));
  editRotate->setWhatsThis(
    tr("Rotate\n\nRotates the selected component by 90\x00B0 counter-clockwise"));
  editRotate->setToggleAction(true);
  connect(editRotate, SIGNAL(toggled(bool)), SLOT(slotEditRotate(bool)));

  editMirror = new QAction(QIcon((":/bitmaps/mirror.png")), tr("Mirror about X Axis"), this);
  editMirror->setStatusTip(tr("Mirrors the selected item about X Axis"));
  editMirror->setWhatsThis(
	tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  editMirror->setToggleAction(true);
  connect(editMirror, SIGNAL(toggled(bool)), SLOT(slotEditMirrorX(bool)));

  editMirrorY = new QAction(QIcon((":/bitmaps/mirrory.png")), tr("Mirror about Y Axis"), this);
  editMirrorY->setStatusTip(tr("Mirrors the selected item about Y Axis"));
  editMirrorY->setWhatsThis(
	tr("Mirror about Y Axis\n\nMirrors the selected item about Y Axis"));
  editMirrorY->setToggleAction(true);
  connect(editMirrorY, SIGNAL(toggled(bool)), SLOT(slotEditMirrorY(bool)));

  intoH = new QAction(QIcon((":/bitmaps/bottom.png")), tr("Go into Subcircuit"), this);
  intoH->setStatusTip(tr("Goes inside the selected subcircuit"));
  intoH->setWhatsThis(
	tr("Go into Subcircuit\n\nGoes inside the selected subcircuit"));
  connect(intoH, SIGNAL(activated()), SLOT(slotIntoHierarchy()));

  popH = new QAction(QIcon((":/bitmaps/top.png")), tr("Pop out"), this);
  popH->setStatusTip(tr("Pop outside subcircuit"));
  popH->setWhatsThis(
	tr("Pop out\n\nGoes up one hierarchy level, i.e. leaves subcircuit"));
  connect(popH, SIGNAL(activated()), SLOT(slotPopHierarchy()));
  popH->setEnabled(false);  // only enabled if useful !!!!

  editActivate = new QAction(QIcon((":/bitmaps/deactiv.png")),	tr("Deactivate/Activate"), this);
  editActivate->setStatusTip(tr("Deactivate/Activate selected components"));
  editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected components"));
  editActivate->setToggleAction(true);
  connect(editActivate, SIGNAL(toggled(bool)), SLOT(slotEditActivate(bool)));

  insEquation = new QAction(QIcon((":/bitmaps/equation.png")),	tr("Insert Equation"), this);
  insEquation->setStatusTip(tr("Inserts an equation"));
  insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  insEquation->setToggleAction(true);
  connect(insEquation, SIGNAL(toggled(bool)), SLOT(slotInsertEquation(bool)));

  insGround = new QAction(QIcon((":/bitmaps/ground.png")), tr("Insert Ground"), this);
  insGround->setStatusTip(tr("Inserts a ground symbol"));
  insGround->setWhatsThis(tr("Insert Ground\n\nInserts a ground symbol"));
  insGround->setToggleAction(true);
  connect(insGround, SIGNAL(toggled(bool)), SLOT(slotInsertGround(bool)));

  insPort = new QAction(QIcon((":/bitmaps/port.png")),	tr("Insert Port"), this);
  insPort->setStatusTip(tr("Inserts a port symbol"));
  insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  insPort->setToggleAction(true);
  connect(insPort, SIGNAL(toggled(bool)), SLOT(slotInsertPort(bool)));

  insWire = new QAction(QIcon((":/bitmaps/wire.png")),	tr("Wire"), this);
  insWire->setStatusTip(tr("Inserts a wire"));
  insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  insWire->setToggleAction(true);
  connect(insWire, SIGNAL(toggled(bool)), SLOT(slotSetWire(bool)));

  insLabel = new QAction(QIcon((":/bitmaps/nodename.png")), tr("Wire Label"), this);
  insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  insLabel->setWhatsThis(tr("Wire Label\n\nInserts a wire or pin label"));
  insLabel->setToggleAction(true);
  connect(insLabel, SIGNAL(toggled(bool)), SLOT(slotInsertLabel(bool)));

  insEntity = new QAction(tr("VHDL entity"), this);
  insEntity->setStatusTip(tr("Inserts skeleton of VHDL entity"));
  insEntity->setWhatsThis(
	tr("VHDL entity\n\nInserts the skeleton of a VHDL entity"));
  connect(insEntity, SIGNAL(activated()), SLOT(slotInsertEntity()));

  callEditor = new QAction(tr("Text Editor"), this);
  callEditor->setStatusTip(tr("Starts the Qucs text editor"));
  callEditor->setWhatsThis(tr("Text editor\n\nStarts the Qucs text editor"));
  connect(callEditor, SIGNAL(activated()), SLOT(slotCallEditor()));

  callFilter = new QAction(tr("Filter synthesis"), this);
  callFilter->setStatusTip(tr("Starts QucsFilter"));
  callFilter->setWhatsThis(tr("Filter synthesis\n\nStarts QucsFilter"));
  connect(callFilter, SIGNAL(activated()), SLOT(slotCallFilter()));

  callLine = new QAction(tr("Line calculation"), this);
  callLine->setStatusTip(tr("Starts QucsTrans"));
  callLine->setWhatsThis(
		tr("Line calculation\n\nStarts transmission line calculator"));
  connect(callLine, SIGNAL(activated()), SLOT(slotCallLine()));

  callLib = new QAction(tr("Component Library"), this);
  callLib->setStatusTip(tr("Starts QucsLib"));
  callLib->setWhatsThis(
		tr("Component Library\n\nStarts component library program"));
  connect(callLib, SIGNAL(activated()), SLOT(slotCallLibrary()));

  callMatch = new QAction(tr("Matching Circuit"), this);
  callMatch->setStatusTip(tr("Creates Matching Circuit"));
  callMatch->setWhatsThis(
	tr("Matching Circuit\n\nDialog for Creating Matching Circuit"));
  connect(callMatch, SIGNAL(activated()), SLOT(slotCallMatch()));

  callAtt = new QAction(tr("Attenuator synthesis"), this);
  callAtt->setStatusTip(tr("Starts QucsAttenuator"));
  callAtt->setWhatsThis(
	tr("Attenuator synthesis\n\nStarts attenuator calculation program"));
  connect(callAtt, SIGNAL(activated()), SLOT(slotCallAtt()));

  simulate = new QAction(QIcon((":/bitmaps/gear.png")), tr("Simulate"), this);
  simulate->setStatusTip(tr("Simulates the current schematic"));
  simulate->setWhatsThis(tr("Simulate\n\nSimulates the current schematic"));
  connect(simulate, SIGNAL(activated()), SLOT(slotSimulate()));

  dpl_sch = new QAction(QIcon((":/bitmaps/rebuild.png")), tr("View Data Display/Schematic"), this);
  dpl_sch->setStatusTip(tr("Changes to data display or schematic page"));
  dpl_sch->setWhatsThis(
	tr("View Data Display/Schematic\n\n")+
	tr("Changes to data display or schematic page"));
  connect(dpl_sch, SIGNAL(activated()), SLOT(slotToPage()));

  dcbias = new QAction(tr("Calculate DC bias"), this);
  dcbias->setStatusTip(tr("Calculates DC bias and shows it"));
  dcbias->setWhatsThis(
	tr("Calculate DC bias\n\nCalculates DC bias and shows it"));
  connect(dcbias, SIGNAL(activated()), SLOT(slotDCbias()));

  setMarker = new QAction(QIcon((":/bitmaps/marker.png")),	tr("Set Marker on Graph"), this);
  setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  setMarker->setToggleAction(true);
  connect(setMarker, SIGNAL(toggled(bool)), SLOT(slotSetMarker(bool)));

  showMsg = new QAction(tr("Show Last Messages"), this);
  showMsg->setStatusTip(tr("Shows last simulation messages"));
  showMsg->setWhatsThis(
        tr("Show Last Messages\n\nShows the messages of the last simulation"));
  connect(showMsg, SIGNAL(activated()), SLOT(slotShowLastMsg()));

  showNet = new QAction(tr("Show Last Netlist"), this);
  showNet->setStatusTip(tr("Shows last simulation netlist"));
  showNet->setWhatsThis(
	tr("Show Last Netlist\n\nShows the netlist of the last simulation"));
  connect(showNet, SIGNAL(activated()), SLOT(slotShowLastNetlist()));

  viewToolBar = new QAction(tr("Tool&bar"), this);
  viewToolBar->setToggleAction(true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)), SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("&Statusbar"), this);
  viewStatusBar->setToggleAction(true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(
	tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), SLOT(slotViewStatusBar(bool)));

  viewBrowseDock = new QAction(tr("&Dock Window"), this);
  viewBrowseDock->setToggleAction(true);
  viewBrowseDock->setStatusTip(tr("Enables/disables the browse dock window"));
  viewBrowseDock->setWhatsThis(
	tr("Browse Window\n\nEnables/disables the browse dock window"));
  connect(viewBrowseDock, SIGNAL(toggled(bool)), SLOT(slotViewBrowseDock(bool)));

  viewOctaveDock = new QAction(tr("&Octave Window"), this);
  viewOctaveDock->setToggleAction(true);
  viewOctaveDock->setStatusTip(tr("Shows/hides the Octave dock window"));
  viewOctaveDock->setWhatsThis(
      tr("Octave Window\n\nShows/hides the Octave dock window"));
  connect(viewOctaveDock, SIGNAL(toggled(bool)), SLOT(slotViewOctaveDock(bool)));

  helpIndex = new QAction(tr("Help Index..."), this);
  helpIndex->setStatusTip(tr("Index of Qucs Help"));
  helpIndex->setWhatsThis(tr("Help Index\n\nIndex of intern Qucs help"));
  connect(helpIndex, SIGNAL(activated()), SLOT(slotHelpIndex()));

  helpGetStart = new QAction(tr("Getting Started..."), this);
  helpGetStart->setStatusTip(tr("Getting Started with Qucs"));
  helpGetStart->setWhatsThis(
	tr("Getting Started\n\nShort introduction into Qucs"));
  connect(helpGetStart, SIGNAL(activated()), SLOT(slotGettingStarted()));

  helpAboutApp = new QAction(tr("&About Qucs..."), this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), SLOT(slotHelpAbout()));

  helpAboutQt = new QAction(tr("About Qt..."), this);
  helpAboutQt->setStatusTip(tr("About Qt"));
  helpAboutQt->setWhatsThis(tr("About Qt\n\nAbout Qt by Trolltech"));
  connect(helpAboutQt, SIGNAL(activated()), SLOT(slotHelpAboutQt()));

  setShortcut();
}

// ----------------------------------------------------------
void QucsApp::initMenuBar()
{
  fileMenu = new QMenu(tr("&File"));  // menuBar entry fileMenu
  fileMenu->addAction(fileNew);
  fileMenu->addAction(textNew);
  fileMenu->addAction(fileOpen);
  fileMenu->addAction(fileClose);

  recentfilesMenu = new QMenu(tr("Open Recent"),fileMenu);
  fileMenu->addMenu(recentfilesMenu);
  slotUpdateRecentFiles();

  fileMenu->insertSeparator();
  fileMenu->addAction(fileSave);
  fileMenu->addAction(fileSaveAll);
  fileMenu->addAction(fileSaveAs);
  fileMenu->addAction(exportAsImage);
  fileMenu->addAction(filePrint);
  fileMenu->addAction(filePrintFit);
  fileMenu->insertSeparator();
  fileMenu->addAction(fileExamples);
  fileMenu->insertSeparator();
  fileMenu->addAction(fileSettings);
  fileMenu->addAction(symEdit);
  fileMenu->insertSeparator();
  fileMenu->addAction(applSettings);
  fileMenu->addAction(refreshSchPath);
  fileMenu->insertSeparator();
  fileMenu->addAction(fileQuit);

  editMenu = new QMenu(tr("&Edit"));  // menuBar entry editMenu
  editMenu->addAction(undo);
  editMenu->addAction(redo);
  editMenu->insertSeparator();
  editMenu->addAction(editCut);
  editMenu->addAction(editCopy);
  editMenu->addAction(editPaste);
  editMenu->addAction(editDelete);
  editMenu->insertSeparator();
  editMenu->addAction(select);
  editMenu->addAction(selectAll);
  editMenu->addAction(selectMarker);
  editMenu->addAction(editFind);
  editMenu->addAction(editFindAgain);
  editMenu->addAction(changeProps);
  editMenu->addAction(editRotate);
  editMenu->addAction(editMirror);
  editMenu->addAction(editMirrorY);
  editMenu->addAction(editActivate);
  editMenu->insertSeparator();
  editMenu->addAction(intoH);
  editMenu->addAction(popH);



  alignMenu = new QMenu(tr("P&ositioning"));  // menuBar entry alignMenu
  alignMenu->addAction(moveText);
  alignMenu->addAction(onGrid);
  alignMenu->insertSeparator();
  alignMenu->addAction(centerHor);
  alignMenu->addAction(centerVert);
  alignMenu->insertSeparator();
  alignMenu->addAction(alignTop);
  alignMenu->addAction(alignBottom);
  alignMenu->addAction(alignLeft);
  alignMenu->addAction(alignRight);
  alignMenu->insertSeparator();
  alignMenu->addAction(distrHor);
  alignMenu->addAction(distrVert);



  insMenu = new QMenu(tr("&Insert"));  // menuBar entry insMenu
  insMenu->addAction(insWire);
  insMenu->addAction(insLabel);
  insMenu->addAction(insEquation);
  insMenu->addAction(insGround);
  insMenu->addAction(insPort);
  insMenu->addAction(setMarker);
  insMenu->addAction(insEntity);


  projMenu = new QMenu(tr("&Project"));  // menuBar entry projMenu
  projMenu->addAction(projNew);
  projMenu->addAction(projOpen);
  projMenu->addAction(addToProj);
  projMenu->addAction(projClose);
  projMenu->addAction(projDel);
  projMenu->insertSeparator();
  projMenu->addAction(createLib);
  projMenu->addAction(createPkg);
  projMenu->addAction(extractPkg);
  projMenu->insertSeparator();
  projMenu->addAction(importData);
  projMenu->addAction(graph2csv);
  projMenu->insertSeparator();
  // TODO only enable if document is VA file
  projMenu->addAction(buildModule);
  projMenu->addAction(loadModule);

  toolMenu = new QMenu(tr("&Tools"));  // menuBar entry toolMenu
  toolMenu->addAction(callEditor);
  toolMenu->addAction(callFilter);
  toolMenu->addAction(callLine);
  toolMenu->addAction(callLib);
  toolMenu->addAction(callMatch);
  toolMenu->addAction(callAtt);



  simMenu = new QMenu(tr("&Simulation"));  // menuBar entry simMenu
  simMenu->addAction(simulate);
  simMenu->addAction(dpl_sch);
  simMenu->addAction(dcbias);
  simMenu->addAction(showMsg);
  simMenu->addAction(showNet);


  viewMenu = new QMenu(tr("&View"));  // menuBar entry viewMenu
  viewMenu->addAction(magAll);
  viewMenu->addAction(magOne);
  viewMenu->addAction(magPlus);
  viewMenu->addAction(magMinus);
  viewMenu->insertSeparator();
  viewMenu->setCheckable(true);
  viewMenu->addAction(viewToolBar);
  viewMenu->addAction(viewStatusBar);
  viewMenu->addAction(viewBrowseDock);
  viewMenu->addAction(viewOctaveDock);


  helpMenu = new QMenu(tr("&Help"));  // menuBar entry helpMenu
  helpMenu->addAction(helpIndex);
  helpMenu->addAction(helpGetStart);
  helpMenu->insertSeparator();



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
      helpTechnicalActions->setWhatsThis(tr(entries[i]+"\n\nOpen "+entries[i]));
      connect(helpTechnicalActions, SIGNAL(activated()), SLOT(slotHelpTechnical()));
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
      helpReportActions->setWhatsThis(tr(entries[i]+"\n\nOpen "+entries[i]));
      connect(helpReportActions, SIGNAL(activated()), SLOT(slotHelpReport()));
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
      helpTutorialActions->setWhatsThis(tr(entries[i]+"\n\nOpen "+entries[i]));
      connect(helpTutorialActions, SIGNAL(activated()), SLOT(slotHelpTutorial()));
      helpTutorial->addAction(helpTutorialActions);
    }
  }


  helpMenu->insertSeparator();
  helpMenu->addAction(helpAboutApp);
  helpMenu->addAction(helpAboutQt);


  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(editMenu);
  menuBar()->addMenu(alignMenu);
  menuBar()->addMenu(insMenu);
  menuBar()->addMenu(projMenu);
  menuBar()->addMenu(toolMenu);
  menuBar()->addMenu(simMenu);
  menuBar()->addMenu(viewMenu);
  menuBar()->insertSeparator();
  menuBar()->addMenu(helpMenu);

}

// ----------------------------------------------------------
void QucsApp::initToolBar()
{
  fileToolbar = new QToolBar(tr("File"));
  this->addToolBar(fileToolbar);
  fileToolbar->addAction(fileNew);
  fileToolbar->addAction(textNew);
  fileToolbar->addAction(fileOpen);
  fileToolbar->addAction(fileSave);
  fileToolbar->addAction(fileSaveAll);
  fileToolbar->addAction(fileClose);
  fileToolbar->addAction(filePrint);



  editToolbar = new QToolBar(tr("Edit"));
  this->addToolBar(editToolbar);
  editToolbar->addAction(editCut);
  editToolbar->addAction(editCopy);
  editToolbar->addAction(editPaste);
  editToolbar->addAction(editDelete);
  editToolbar->addAction(undo);
  editToolbar->addAction(redo);



  viewToolbar = new QToolBar(tr("View"));
  this->addToolBar(viewToolbar);
  viewToolbar->addAction(magAll);
  viewToolbar->addAction(magOne);
  viewToolbar->addAction(magPlus);
  viewToolbar->addAction(magMinus);


  workToolbar = new QToolBar(tr("Work"));
  this->addToolBar(workToolbar);
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

}

// ----------------------------------------------------------
void QucsApp::initStatusBar()
{
  // To reserve enough space, insert the longest text and rewrite it afterwards.
  WarningLabel = new QLabel(tr("no warnings"), statusBar());
  statusBar()->addWidget(WarningLabel, 0, true);

  PositionLabel = new QLabel("0 : 0", statusBar());
  PositionLabel->setAlignment(Qt::AlignRight);
  statusBar()->addWidget(PositionLabel, 0, true);

  statusBar()->message(tr("Ready."), 2000);
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
    WarningLabel->setPaletteForegroundColor(Qt::red);
  else
    WarningLabel->setPaletteForegroundColor(Qt::black);

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
  WarningLabel->setPaletteForegroundColor(Qt::black);
  WarningLabel->setText(tr("no warnings"));
}

// ----------------------------------------------------------
void QucsApp::printCursorPosition(int x, int y)
{
  PositionLabel->setText(QString::number(x)+" : "+QString::number(y));
  PositionLabel->setMinimumWidth(PositionLabel->width());
}

// ----------------------------------------------------------
// turn Toolbar on or off
void QucsApp::slotViewToolBar(bool toggle)
{
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
}

// ----------------------------------------------------------
// turn Statusbar on or off
void QucsApp::slotViewStatusBar(bool toggle)
{
  if (toggle == false)
    statusBar()->hide();
  else
    statusBar()->show();
}

// ----------------------------------------------------------
// turn Brwose Dock Window on or off
void QucsApp::slotViewBrowseDock(bool toggle)
{
  if (toggle == false)
    dock->hide();
  else
    dock->show();
}

// ----------------------------------------------------------
void QucsApp::slotToggleDock(bool on)
{
  viewBrowseDock->blockSignals(true);
  viewBrowseDock->setOn(on);
  viewBrowseDock->blockSignals(false);
}

// ----------------------------------------------------------
// turn Octave Dock Window on or off
void QucsApp::slotViewOctaveDock(bool toggle)
{
  if(toggle) {
    octDock->show();
    octave->startOctave();
  }
  else
    octDock->hide();
}

// ----------------------------------------------------------
void QucsApp::slotToggleOctave(bool on)
{
  viewOctaveDock->blockSignals(true);
  viewOctaveDock->setOn(on);
  viewOctaveDock->blockSignals(false);
}

// ----------------------------------------------------------
void QucsApp::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    tr("Qucs Version")+" "+PACKAGE_VERSION+"\n"+
    tr("Quite Universal Circuit Simulator")+"\n"+
    tr("Copyright (C)")+" 2003-2009 "+
    tr("by Michael Margraf")+"\n"+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n"+
    tr("Simulator by Stefan Jahn")+"\n"+
    tr("VHDL simulator 'FreeHDL' by Edwin Naroska and Marius Vollmer")+"\n"+
    tr("Special thanks to Jens Flucke and Raimund Jacob")+"\n"+
    tr("Many thanks to Mike Brinson for correcting the VHDL output")+"\n"+
    tr("GUI improvements by Gopala Krishna A")+"\n"+
    tr("Verilog-AMS interface by Helene Parruitte")+"\n\n"+
    tr("Translations:")+"\n"+
    tr("German by Stefan Jahn")+"\n"+
    tr("Polish by Dariusz Pienkowski")+"\n"+
    tr("Romanian by Radu Circa")+"\n"+
    tr("French by Vincent Habchi, F5RCS")+"\n"+
    tr("Portuguese by Luciano Franca and Helio de Sousa")+"\n"+
    tr("Spanish by Jose L. Redrejo Rodriguez")+"\n"+
    tr("Japanese by Toyoyuki Ishikawa")+"\n"+
    tr("Italian by Giorgio Luparia and Claudio Girardi")+"\n"+
    tr("Hebrew by Dotan Nahum")+"\n"+
    tr("Swedish by Peter Landgren")+"\n"+
    tr("Turkish by Onur and Ozgur Cobanoglu")+"\n"+
    tr("Hungarian by Jozsef Bus")+"\n"+
    tr("Russian by Igor Gorbounov")+"\n"+
    tr("Czech by Marek Straka")+"\n"+
    tr("Catalan by Antoni Subirats")+"\n"+
    tr("Arabic by Chabane Noureddine")+"\n"+
    tr("Kazakh by Erbol Keshubaev"));
}

// ----------------------------------------------------------
void QucsApp::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

