/***************************************************************************
                          qucs.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
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

#include "main.h"
#include "qucs.h"
#include "qucsview.h"
#include "wire.h"
#include "components/components.h"
#include "paintings/paintings.h"
#include "diagrams/diagrams.h"
#include "dialogs/messagebox.h"
#include "dialogs/newprojdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/qucssettingsdialog.h"

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
#include <qinputdialog.h>
#include <qclipboard.h>
#include <qdatetime.h>
#include <qfont.h>


#include <limits.h>


#define  COMBO_passive   0
#define  COMBO_Sources   1
#define  COMBO_TLines    2
#define  COMBO_nonlinear 3
#define  COMBO_File      4
#define  COMBO_Sims      5
#define  COMBO_Diagrams  6
#define  COMBO_Paints    7   // must be the last one

QDir QucsWorkDir;
QDir QucsHomeDir;

QucsApp::QucsApp()
{
  setCaption("Qucs " PACKAGE_VERSION);

  QucsFileFilter = tr("Schematic")+" (*.sch);;"+
                   tr("Data Display")+" (*.dpl);;"+
		   tr("Qucs Documents")+" (*.sch *.dpl);;"+
		   tr("Any File")+" (*)";
  QucsWorkDir.setPath(QDir::homeDirPath()+"/.qucs");
  QucsHomeDir.setPath(QDir::homeDirPath()+"/.qucs");

  move  (QucsSettings.x,  QucsSettings.y);
  resize(QucsSettings.dx, QucsSettings.dy);
//  resize(maximumSize());


  initView();
  Init.perform(this);  // initializes toolbar, statusbar, menubar, actions
  Acts.init(this);
  initCursorMenu();

  // default settings of the printer
  Printer.setOrientation(QPrinter::Landscape);
  Printer.setColorMode(QPrinter::Color);


  // switch on the 'select' action
//  Acts.select->blockSignals(true);
  Acts.select->setOn(true);
//  Acts.select->blockSignals(false);
//  Acts.slotSelect(true);

  HierarchyHistory.setAutoDelete(true);

  // creates a document called "untitled"
  view->Docs.append(new QucsDoc(this, ""));
}

QucsApp::~QucsApp()
{
}


// #########################################################################
// ##########                                                     ##########
// ##########       Creates the view area (QTabWidget etc.)       ##########
// ##########                                                     ##########
// #########################################################################
void QucsApp::initView()
{
  QVBox *all = new QVBox(this);   // only to fill the entire view area
  QSplitter *Hsplit = new QSplitter(QSplitter::Horizontal, all);

  TabView  = new QTabWidget(Hsplit);    // tabs on the left side
  QVBox *WorkGroup = new QVBox(Hsplit);
  WorkView = new QTabBar(WorkGroup);    // tab on the right side
  view = new QucsView(WorkGroup);    // work area with documents

  connect(WorkView, SIGNAL(selected(int)), SLOT(slotChangeView(int)));

  Hsplit->setResizeMode(TabView, QSplitter::KeepSize);
  setCentralWidget(all);

  // ----------------------------------------------------------
  // "Project Tab" of the left QTabWidget
  QVBox *ProjGroup = new QVBox(this);
  QHBox *ProjButts = new QHBox(ProjGroup);
  QPushButton *ProjNew   = new QPushButton(tr("New"),ProjButts);
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotProjNewButt()));
  QPushButton *ProjOpen  = new QPushButton(tr("Open"),ProjButts);
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotProjOpenButt()));
  QPushButton *ProjDel   = new QPushButton(tr("Delete"),ProjButts);
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotProjDelButt()));

  Projects = new QListBox(ProjGroup);
  TabView->addTab(ProjGroup, tr("Projects"));
  TabView->setTabToolTip(TabView->page(0),
			 tr("content of the project directory"));

  connect(Projects, SIGNAL(doubleClicked(QListBoxItem*)),
		    SLOT(slotOpenProject(QListBoxItem*)));

  // ----------------------------------------------------------
  // "Content Tab" of the left QTabWidget
  Content = new QListView(this);
  Content->setRootIsDecorated(true); // open/close decoration for root items
  Content->setSorting(-1);    // no sorting
  Content->addColumn(tr("Content of"));
  Content->addColumn(tr("Note"));
  Content->setColumnWidthMode(0,QListView::Manual);
  Content->setColumnWidth(0, 150);

  ConDatasets   = new QListViewItem(Content, tr("Datasets"));
  ConDisplays   = new QListViewItem(Content, tr("Data Displays"));
  ConSchematics = new QListViewItem(Content, tr("Schematics"));
  TabView->addTab(Content,tr("Content"));
  TabView->setTabToolTip(TabView->page(1), tr("content of the open project"));

// QT 3.2
//  connect(Content, SIGNAL(doubleClicked(QListViewItem*, const QPoint &,int)), SLOT(slotOpenContent(QListViewItem*, const QPoint &,int)));
  connect(Content, SIGNAL(doubleClicked(QListViewItem*)),
		   SLOT(slotOpenContent(QListViewItem*)));
  connect(Content, SIGNAL(clicked(QListViewItem*)),
		   SLOT(slotSelectSubcircuit(QListViewItem*)));

  // ----------------------------------------------------------
  // "Component Tab" of the left QTabWidget
  QVBox *CompGroup  = new QVBox(this);
  CompChoose = new QComboBox(CompGroup);
  CompComps  = new QIconView(CompGroup);
  TabView->addTab(CompGroup,tr("Components"));
  TabView->setTabToolTip(TabView->page(2), tr("components and diagrams"));
  fillComboBox(true);

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(clicked(QIconViewItem*)),
		     SLOT(slotSelectComponent(QIconViewItem*)));


  // ---------------------------------------------------------------------
  readProjects();   // reads all projects and inserts them into the ListBox
}

// ----------------------------------------------------------
void QucsApp::fillComboBox(bool setAll)
{
  CompChoose->clear();
  if(setAll) {
    CompChoose->insertItem(tr("lumped components"));
    CompChoose->insertItem(tr("sources"));
    CompChoose->insertItem(tr("transmission lines"));
    CompChoose->insertItem(tr("nonlinear components"));
    CompChoose->insertItem(tr("file data"));
    CompChoose->insertItem(tr("simulations"));
    CompChoose->insertItem(tr("diagrams"));
  }
  CompChoose->insertItem(tr("paintings"));
}

// ----------------------------------------------------------
// Menu that appears if right mouse button is pressed on a file in the
// "Content" ListView.
void QucsApp::initCursorMenu()
{
  ContentMenu = new QPopupMenu(Content);
  ContentMenu->insertItem(tr("Open"),   this, SLOT(slotCMenuOpen()));
  ContentMenu->insertItem(tr("Rename"), this, SLOT(slotCMenuRename()));
  ContentMenu->insertItem(tr("Delete"), this, SLOT(slotCMenuDelete()));

  connect(Content,
	  SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
	  SLOT(slotShowContentMenu(QListViewItem*, const QPoint&, int)));
}

// ----------------------------------------------------------
// Is called if right mouse button is pressed on a file in the "Content" ListView.
// It shows a menu.
void QucsApp::slotShowContentMenu(QListViewItem *item, const QPoint& point, int)
{
  if(item)
    if(item->parent() != 0)   // no component, but item "schematic", ...
      ContentMenu->popup(point);
}

// ----------------------------------------------------------
// for menu that appears if right mouse button is pressed on a file in the "Content" ListView.
void QucsApp::slotCMenuOpen()
{
  QListViewItem *i = Content->selectedItem();
  if(i == 0) return;

  slotOpenContent(i);
}

// ----------------------------------------------------------
// for menu that appears if right mouse button is pressed on a file in the "Content" ListView.
void QucsApp::slotCMenuRename()
{
  QListViewItem *i = Content->selectedItem();
  if(i == 0) return;

  QucsDoc *dc = view->Docs.current();
  // search, if file is open
  for(QucsDoc *d = view->Docs.first(); d!=0; d = view->Docs.next()) {
    if(d->DocName == QucsWorkDir.filePath(i->text(0))) {
      QMessageBox::critical(this, tr("Error"),
				  tr("Cannot rename an open file!"));
      view->Docs.findRef(dc);
      return;
    }
  }
  view->Docs.findRef(dc);

  QString Name   = i->text(0);
  QString Suffix = Name.right(4);    // remember suffix

  bool ok;
  QString s = QInputDialog::getText(tr("Rename file"), tr("Enter new name:"),
		QLineEdit::Normal, Name.left(Name.length()-4), &ok, this);
  if(!ok) return;
  if(s.isEmpty()) return;

  QDir file(QucsWorkDir.path());
  if(!file.rename(Name, s+Suffix)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot rename file: ")+Name);
    return;
  }
  i->setText(0, s+Suffix);
}

// ----------------------------------------------------------
// for menu that appears if right mouse button is pressed on a file in the "Content" ListView.
void QucsApp::slotCMenuDelete()
{
  QListViewItem *item = Content->selectedItem();
  if(item == 0) return;

  QucsDoc *dc = view->Docs.current();
  // search, if file is open
  for(QucsDoc *d = view->Docs.first(); d!=0; d = view->Docs.next()) {
    if(d->DocName == QucsWorkDir.filePath(item->text(0))) {
      QMessageBox::critical(this, tr("Error"),
                   tr("Cannot delete an open file!"));
      view->Docs.findRef(dc);
      return;
    }
  }
  view->Docs.findRef(dc);

  int n = QMessageBox::warning(this, tr("Warning"),
                 tr("This will delete the file permanently! Continue ?"),
                 tr("No"), tr("Yes"));
  if(n != 1) return;

  if(!QFile::remove(QucsWorkDir.filePath(item->text(0)))) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete schematic: ")+item->text(0));
    return;
  }
  if(ConSchematics == item->parent())
    ConSchematics->takeItem(item);
  else if(ConDisplays == item->parent())
    ConDisplays->takeItem(item);
  else
    ConDatasets->takeItem(item);
}

// ----------------------------------------------------------
// Checks situation and reads all existing Qucs projects
void QucsApp::readProjects()
{
  QDir ProjDir(QDir::homeDirPath());
  if(!ProjDir.cd(".qucs")) // work directory exists ?
    if(!ProjDir.mkdir(".qucs")) { // no, then create it
      QMessageBox::warning(this, tr("Warning"),
			tr("Cannot create work directory !"));
      return;
    }
  ProjDir.cd(".qucs");

  // get all directories
  QStringList PrDirs = ProjDir.entryList("*", QDir::Dirs, QDir::Name);
  PrDirs.pop_front(); // delete "." from list
  PrDirs.pop_front(); // delete ".." from list

  Projects->clear();
  QStringList::iterator it;
  // inserts all project directories
  for(it = PrDirs.begin(); it != PrDirs.end(); it++)
     if ((*it).right(4) == "_prj") {   // project directories end with "_prj"
      (*it) = (*it).left((*it).length()-4);// remove "_prj" from project name
      Projects->insertItem(*it);
     }
}

// #########################################################################
bool QucsApp::closeAllFiles()
{
  int  Result = 0;
  bool notForAll = true;
  MessageBox *m = new MessageBox(tr("Closing Qucs document"),
	tr("This document contains unsaved changes!\n"
	   "Do you want to save the changes before closing?"),this);

  // close all files and ask to save changed ones
  for(QucsDoc *ptr = view->Docs.first(); ptr != 0; ) {
    nextDocument(false);
    if(ptr->DocChanged) {
      if(notForAll)  Result = m->exec();
      switch(Result) {
        case 1: slotFileSave();     // save button
                break;
        case 2: Result = 1;         // save all button
                notForAll = false;
                slotFileSave();
                break;
        case 4: Result = 3;         // discard all button
                notForAll = false;
                break;
        case 5: return false;       // cancel button
      }
    }

    view->Docs.remove();
    ptr = view->Docs.current();
  }

  delete m;
  return true;
}

// ########################################################################
// Switches to the next document. Is called when closing a document.
void QucsApp::nextDocument(bool loadDiagrams)
{
  // make new document the current
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));

  QucsDoc *d = view->Docs.current();
  view->resizeContents (int(d->Scale*double(d->ViewX2-d->ViewX1)),
			int(d->Scale*double(d->ViewY2-d->ViewY1)));
  view->setContentsPos(d->PosX, d->PosY);   // set view area

  if(loadDiagrams)
    view->Docs.current()->reloadGraphs();  // load recent simulation data
  view->viewport()->repaint();
  view->drawn = false;

  QString *ps = d->UndoStack.current();
  if(ps != d->UndoStack.getFirst())  undo->setEnabled(true);
  else  undo->setEnabled(false);
  if(ps != d->UndoStack.getLast())  redo->setEnabled(true);
  else  redo->setEnabled(false);

  HierarchyHistory.clear();   // no subcircuit history
  popH->setEnabled(false);
}

// ########################################################################
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(int id)
{
  QucsDoc *d = view->Docs.current();
  d->PosX = view->contentsX();    // save position for old document
  d->PosY = view->contentsY();
  bool oldMode = d->symbolMode;

  d = view->Docs.at(WorkView->indexOf(id));   // new current document
  view->resizeContents(int(d->Scale*double(d->ViewX2-d->ViewX1)),
                       int(d->Scale*double(d->ViewY2-d->ViewY1)));
  view->setContentsPos(d->PosX, d->PosY);
  if(d->symbolMode != oldMode) { // which mode: schematic or symbol editor ?
    d->symbolMode = oldMode;  // because it's changed by following function
    changeSchematicSymbolMode(d);
  }

  view->Docs.current()->reloadGraphs();  // load recent simulation data
  view->viewport()->repaint();
  view->drawn = false;


  QString *ps = d->UndoStack.current();
  if(ps != d->UndoStack.getFirst())  undo->setEnabled(true);
  else  undo->setEnabled(false);
  if(ps != d->UndoStack.getLast())  redo->setEnabled(true);
  else  redo->setEnabled(false);

  if(!HierarchyHistory.isEmpty())
    if(*(HierarchyHistory.getLast()) != "*") {
      HierarchyHistory.clear();   // no subcircuit history anymore
      popH->setEnabled(false);
    }
}

// #######################################################################
// Changes to the document "Name" if possible.
bool QucsApp::gotoPage(const QString& Name)
{
  QucsDoc *d;
  int cNo, No = view->Docs.at();

  // search, if page is already loaded
  for(d = view->Docs.first(); d!=0; d = view->Docs.next())
    if(d->DocName == Name) break;

  if(d != 0) {   // open page found ?
    view->Docs.current()->reloadGraphs();   // load recent simulation data
    // make new document the current
    WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));
    return true;
  }

  d = new QucsDoc(this, Name);
  view->Docs.append(d);   // create new page

  if(!d->load()) {    // load document if possible
    view->Docs.remove();
    view->Docs.at(No);
    view->viewport()->repaint();
    view->drawn = false;
    return false;
  }

  cNo = view->Docs.at();
  view->Docs.at(No);  // back to the last current
  // make new document the current (calls "slotChangeView()" indirectly)
  WorkView->setCurrentTab(WorkView->tabAt(cNo));
  // must be done before the next step, in order to call the change slot !

  // if only an untitled document was open -> close it
  if(view->Docs.count() == 2)
    if(view->Docs.getFirst()->DocName.isEmpty())
      if(!view->Docs.getFirst()->DocChanged)
        view->Docs.removeRef(view->Docs.getFirst());

  view->viewport()->repaint();
  view->drawn = false;
  return true;
}

// #######################################################################
// Changes to the next document in the TabBar.
void QucsApp::slotNextTab()
{
  int No = view->Docs.at() + 1;
  if(view->Docs.current() == view->Docs.getLast())
    No = 0;

  // make new document the current (calls "slotChangeView()" indirectly)
  WorkView->setCurrentTab(WorkView->tabAt(No));
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
void QucsApp::slotFileSettings()
{
  SettingsDialog *d = new SettingsDialog(view->Docs.current(), this);
  d->exec();
  view->viewport()->repaint();
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotApplSettings()
{
  QucsSettingsDialog *d = new QucsSettingsDialog(this);
  d->exec();
  view->viewport()->repaint();
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotFileNew()
{
  statusBar()->message(tr("Creating new schematic..."));

  view->Docs.append(new QucsDoc(this, ""));
  // make new document the current
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  statusBar()->message(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(QucsWorkDir.path(), QucsFileFilter,
				      this, "", tr("Enter a Schematic Name"));
  if(!s.isEmpty())  gotoPage(s); //openDocument(s);
  else statusBar()->message(tr("Opening aborted"), 2000);

  statusBar()->message(tr("Ready."));
}


// ########################################################################
void QucsApp::updatePortNumber(int No)
{
  if(No<0) return;
  QString pathName = view->Docs.current()->DocName;
  QFileInfo Info(pathName);
  QString File, Name = Info.fileName();

  // enter new port number into ListView
  QListViewItem *p;
  for(p = ConSchematics->firstChild(); p!=0; p = p->nextSibling()) {
    if(p->text(0) == Name) {
      if(No == 0) p->setText(1,"");
      else p->setText(1,QString::number(No)+tr("-port"));
      break;
    }
  }
  if(No == 0) return;

  // update all occurencies of subcircuit in all open documents
  QucsDoc *d;
  int DocNo = view->Docs.at();
  for(d = view->Docs.first(); d!=0; d = view->Docs.next())
    for(Component *pc=d->Comps->first(); pc!=0; pc=d->Comps->next())
      if(pc->Model == "Sub") {
	File = pc->Props.getFirst()->Value;
	if((File == pathName) || (File == Name)) {
	  d->Comps->setAutoDelete(false);
	  d->deleteComp(pc);
	  ((Subcircuit*)pc)->recreate();//remakeSymbol(No);
	  d->insertRawComponent(pc);
	  d->Comps->setAutoDelete(true);
	}
      }

  view->Docs.at(DocNo);  // back to the last current document
}


// ######################################################################
bool QucsApp::saveCurrentFile()
{
  if(view->Docs.current()->DocName.isEmpty())
    return saveAs();

  view->Docs.current()->PosX = view->contentsX();
  view->Docs.current()->PosY = view->contentsY();

  updatePortNumber(view->Docs.current()->save());   // SAVE
  return true;
}

// ######################################################################
void QucsApp::slotFileSave()
{
  statusBar()->message(tr("Saving file..."));
  view->blockSignals(true);   // no user interaction during that time

  if(!saveCurrentFile()) {
    view->blockSignals(false);
    statusBar()->message(tr("Saving aborted"), 2000);
    statusBar()->message(tr("Ready."));
    return;
  }

  view->blockSignals(false);
  statusBar()->message(tr("Ready."));
}

// #######################################################################
bool QucsApp::saveAs()
{
  int n = -1;
  bool intoView = true;
  QString s, Filter;
  QFileInfo Info;
  while(true) {
    intoView = true;
    Info.setFile(view->Docs.current()->DocName);
    if(Info.extension() == "dpl")
      s = QFileDialog::getSaveFileName(QucsWorkDir.filePath(Info.fileName()),
				tr("Data Display")+" (*.dpl)", this, "",
				tr("Enter a Document Name"), &Filter);
    else  s = QFileDialog::getSaveFileName(
		QucsWorkDir.filePath(Info.fileName()), QucsFileFilter,
		this, "", tr("Enter a Document Name"), &Filter);
    if(s.isEmpty())  return false;
    Info.setFile(s);                 // try to guess the best extension ...
    if(Info.extension(false).isEmpty())  // ... if no one was specified
      if(Filter.right(7) == "(*.dpl)")  s += ".dpl";
      else s += ".sch";

    Info.setFile(s);
    if(QFile::exists(s)) {
      n = QMessageBox::warning(this, tr("Warning"),
		tr("The file '")+Info.fileName()+tr("' already exists!\n")+
		tr("Saving will overwrite the old one! Continue?"),
		tr("No"), tr("Yes"), tr("Cancel"));
      if(n == 2) return false;    // cancel
      if(n == 0) continue;
      intoView = false;    // file already exists
    }

    QString ext = Info.extension(false);
    if(ext != "sch") if(ext != "dpl") {
      n = QMessageBox::warning(this, tr("Warning"),
		tr("Only the extensions '.sch' and '.dpl'\n")+
		tr("will appear in the content browser! Continue?"),
		tr("No"), tr("Yes"), tr("Cancel"));
      if(n == 2) return false;    // cancel
      if(n == 0) continue;
    }

    // search, if document is open
    QucsDoc *d;
    int No = view->Docs.at();
    for(d = view->Docs.first(); d!=0; d = view->Docs.next())
      if(d->DocName == s) break;
    view->Docs.at(No);   // back to old current document
    if(d) {
      QMessageBox::information(this, tr("Info"),
		tr("Cannot overwrite an open document"));
      return false;
    }

    break;
  }
  view->Docs.current()->setName(s);

  if(intoView) {    // insert new name in Content ListView ?
    Info.setFile(s);
    if(Info.dirPath(true) == QucsWorkDir.absPath())
      if(!ProjName.isEmpty()) {
	s = Info.fileName();  // remove path from file name
	if(Info.extension(false) == "dpl")
	  Content->setSelected(new QListViewItem(ConDisplays, s), true);
	else
	  Content->setSelected(new QListViewItem(ConSchematics, s), true);
      }
  }

  view->Docs.current()->PosX = view->contentsX();
  view->Docs.current()->PosY = view->contentsY();
  updatePortNumber(view->Docs.current()->save());   // SAVE
  return true;
}

// #######################################################################
void QucsApp::slotFileSaveAs()
{
  statusBar()->message(tr("Saving file under new filename..."));
  view->blockSignals(true);   // no user interaction during the time

  if(!saveAs()) {
    view->blockSignals(false);
    statusBar()->message(tr("Saving aborted"), 3000);
    statusBar()->message(tr("Ready."));
    return;
  }

  view->blockSignals(false);
  statusBar()->message(tr("Ready."));
}


// #######################################################################
void QucsApp::slotFileSaveAll()
{
  statusBar()->message(tr("Saving all files..."));

  QucsDoc *tmp = view->Docs.current();  // remember the current
  view->blockSignals(true);   // no user interaction during the time

  for(QucsDoc *ptr = view->Docs.first(); ptr != 0; ptr = view->Docs.next()) {
    if(ptr->DocName.isEmpty())  // make document the current ?
      WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));
    saveCurrentFile();
  }

  view->Docs.findRef(tmp);
  view->blockSignals(false);
  // back to the current document
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));

  statusBar()->message(tr("Ready."));
}

// #######################################################################
void QucsApp::slotFileClose()
{
  statusBar()->message(tr("Closing file..."));

  if(view->Docs.current()->DocChanged) {
    switch(QMessageBox::warning(this,tr("Closing Qucs document"),
      tr("The document contains unsaved changes!\n")+
      tr("Do you want to save the changes before closing?"),
      tr("&Save"), tr("&Discard"), tr("Cancel"), 0, 2)) {
      case 0 : slotFileSave();
               break;
      case 2 : return;
    }
  }

  view->Docs.remove();

  if(view->Docs.isEmpty())  // if no document left, create an untitled
    view->Docs.append(new QucsDoc(this, ""));

  nextDocument(true);

  statusBar()->message(tr("Ready."));
}

// ######################################################################
void QucsApp::slotFilePrint()
{
  statusBar()->message(tr("Printing..."));

  if (Printer.setup(this))  // print dialog
  {
    QPainter painter;
    painter.begin(&Printer);
    view->Docs.current()->paint(&painter);
    painter.end();
  };

  statusBar()->message(tr("Ready."));
}

// ######################################################################
void QucsApp::slotFilePrintSelected()
{
  statusBar()->message(tr("Printing..."));

  if (Printer.setup(this))  // print dialog
  {
    QPainter painter;
    painter.begin(&Printer);
    view->Docs.current()->paintSelected(&painter);
    painter.end();
  };

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
// Exits the application.
void QucsApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));

  int exit = QMessageBox::information(this, tr("Quit..."),
				      tr("Do you really want to quit?"),
				      tr("Yes"), tr("No"));
  if(exit == 0)
    if(closeAllFiles())  qApp->quit();

  statusBar()->message(tr("Ready."));
}

//-----------------------------------------------------------------
// To get all close events.
void QucsApp::closeEvent(QCloseEvent* Event)
{
  Event->ignore();
  if(closeAllFiles())  qApp->quit();
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = view->Docs.current()->copySelected(true);
  if(!s.isEmpty()) {
    cb->setText(s, QClipboard::Clipboard);
    view->viewport()->repaint();
  }

  statusBar()->message(tr("Ready."));
}

// ######################################################################
void QucsApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = view->Docs.current()->copySelected(false);
  if(!s.isEmpty())
    cb->setText(s, QClipboard::Clipboard);

  statusBar()->message(tr("Ready."));
}

// ########################################################################
void QucsApp::slotHelpIndex()
{
  QStringList com;
  com << BINARYDIR "qucshelp" << "index.html";
  QProcess QucsHelp(com);
  if(!QucsHelp.start())
    QMessageBox::critical(this, tr("Error"), tr("Cannot start qucshelp!"));
}

// ########################################################################
void QucsApp::slotGettingStarted()
{
  QStringList com;
  com << BINARYDIR "qucshelp" << "start.html";
  QProcess QucsHelp(com);
  if(!QucsHelp.start())
    QMessageBox::critical(this, tr("Error"), tr("Cannot start qucshelp!"));
}

// ########################################################################
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{
  QucsDoc *Doc = view->Docs.current();
  Component *pc = view->Docs.current()->searchSelSubcircuit();
  if(pc == 0) return;

  QString *ps = new QString("*");
  HierarchyHistory.append(ps);    // sign to not clear HierarchyHistory

  QString s = QucsWorkDir.filePath(pc->Props.getFirst()->Value);
  if(!gotoPage(s)) return;

  *(HierarchyHistory.getLast()) = Doc->DocName; // remember for the way back
  popH->setEnabled(true);
}

// ########################################################################
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{
  if(HierarchyHistory.count() == 0) return;

  QString Doc = *(HierarchyHistory.getLast());
  *(HierarchyHistory.last()) = "*";  // sign to not clear HierarchyHistory

  if(!gotoPage(Doc)) return;

  HierarchyHistory.remove();
  if(HierarchyHistory.count() == 0) popH->setEnabled(false);
}

// ########################################################################
void QucsApp::slotShowAll()
{
  int x1 = view->Docs.current()->UsedX1;
  int y1 = view->Docs.current()->UsedY1;
  int x2 = view->Docs.current()->UsedX2;
  int y2 = view->Docs.current()->UsedY2;

  if(x2==0) if(y2==0) if(x1==0) if(y1==0) return;
  x1 -= 40;  y1 -= 40;
  x2 += 40;  y2 += 40;

  double xScale = double(view->visibleWidth()) / double(x2-x1);
  double yScale = double(view->visibleHeight()) / double(y2-y1);
  if(xScale > yScale) xScale = yScale;
  if(xScale > 10.0) xScale = 10.0;
  if(xScale < 0.01) xScale = 0.01;
  view->Docs.current()->Scale = xScale;

  view->Docs.current()->ViewX1 = x1;
  view->Docs.current()->ViewY1 = y1;
  view->Docs.current()->ViewX2 = x2;
  view->Docs.current()->ViewY2 = y2;
  view->resizeContents(int(xScale*double(x2-x1)), int(xScale*double(y2-y1)));

  view->viewport()->repaint();
  view->drawn = false;
}

// -----------------------------------------------------------
// Sets the scale factor to 1.
void QucsApp::slotShowOne()
{
  QucsDoc *d = view->Docs.current();

  d->Scale = 1.0;

  int x1 = d->UsedX1;
  int y1 = d->UsedY1;
  int x2 = d->UsedX2;
  int y2 = d->UsedY2;

//  view->Docs.current()->sizeOfAll(x1, y1, x2, y2);
  if(x2==0) if(y2==0) if(x1==0) if(y1==0) x2 = y2 = 800;

  d->ViewX1 = x1-40;
  d->ViewY1 = y1-40;
  d->ViewX2 = x2+40;
  d->ViewY2 = y2+40;
  view->resizeContents(x2-x1+80, y2-y1+80);
  view->viewport()->repaint();
  view->drawn = false;
}

void QucsApp::slotZoomIn()
{
  view->Zoom(2.0);
  view->viewport()->repaint();
  view->drawn = false;
}

void QucsApp::slotZoomOut()
{
  view->Zoom(0.5);
  view->viewport()->repaint();
  view->drawn = false;
}

// -----------------------------------------------------------------------
// Is called when the simulate toolbar button is pressed.
void QucsApp::slotSimulate()
{
  if(view->Docs.current()->DocName.isEmpty()) // if document 'untitled' ...
    if(!saveCurrentFile()) return;            // ... save schematic before

  SimMessage *sim = new SimMessage(view->Docs.current()->DataDisplay, this);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed !
  connect(sim, SIGNAL(SimulationEnded(int, SimMessage*)), this,
		SLOT(slotAfterSimulation(int, SimMessage*)));
  connect(sim, SIGNAL(displayDataPage(QString)),
		this, SLOT(slotChangePage(QString)));

//  sim.ProgText->clear();
//  sim.ErrText->clear();
  sim->show();

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time
  sim->ProgText->insert(tr("Starting new simulation on ")+
                        d.toString("ddd dd. MMM yyyy"));
  sim->ProgText->insert(tr(" at ")+t.toString("hh:mm:ss")+"\n\n");

  sim->ProgText->insert(tr("creating netlist ...."));
  QFile NetlistFile(QucsHomeDir.filePath("netlist.txt"));
  if(!view->Docs.current()->File.createNetlist(&NetlistFile)) {
    sim->ErrText->insert(tr("ERROR: Cannot create netlist file!\nAborted."));
    sim->errorSimEnded();
    return;
  }
  sim->ProgText->insert(tr("done.\n"));

  QStringList com;
  com << BINARYDIR "qucsator" << "-i" << QucsHomeDir.filePath("netlist.txt");
  com << "-o" << QucsWorkDir.filePath(view->Docs.current()->DataSet);
  if(!sim->startProcess(com)) {
    sim->ErrText->insert(tr("ERROR: Cannot start simulator!"));
    sim->errorSimEnded();
    return;
  }
}

// ------------------------------------------------------------------------
// Is called after the simulation process terminates.
void QucsApp::slotAfterSimulation(int Status, SimMessage *sim)
{
  bool shouldClosed = false; // simulation window close after simualtion ?
  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time

  if(Status == 0) {  // errors ocurred ?
    sim->ProgText->insert(tr("\nSimulation ended on ")+
                          d.toString("ddd dd. MMM yyyy"));
    sim->ProgText->insert(tr(" at ")+t.toString("hh:mm:ss")+"\n");
    sim->ProgText->insert(tr("Ready.\n"));

    if(view->Docs.current()->SimOpenDpl) {
      shouldClosed = true;
      slotChangePage(sim->DataDisplay);  // switch to data display
      view->viewport()->update();
    }
    else {
      view->Docs.current()->reloadGraphs();  // load recent simulation data
      view->viewport()->update();
    }

    // put all dataset files into "Content"-ListView (update)
/*    QStringList Elements = ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
    for(it = Elements.begin(); it != Elements.end(); ++it)
      new QListViewItem(ConDatasets, (*it).ascii());*/
  }
  else {
    sim->ProgText->insert(tr("\nErrors occured during simulation on ")+
			  d.toString("ddd dd. MMM yyyy"));
    sim->ProgText->insert(tr(" at ")+t.toString("hh:mm:ss")+"\n");
    sim->ProgText->insert(tr("Aborted.\n"));
  }

  QFile file(QucsHomeDir.filePath("log.txt"));  // save simulator messages
  if(file.open(IO_WriteOnly)) {
    int z;
    QTextStream stream(&file);
    stream << tr("Output:\n----------\n\n");
    for(z=0; z<=sim->ProgText->paragraphs(); z++)
      stream << sim->ProgText->text(z) << "\n";
    stream << tr("\n\n\nErrors:\n--------\n\n");
    for(z=0; z<sim->ErrText->paragraphs(); z++)
      stream << sim->ErrText->text(z) << "\n";
    file.close();
  }

  if(shouldClosed) sim->slotClose();  // close and delete simulation window
}

// ------------------------------------------------------------------------
// Is called to show the output messages of the last simulation.
void QucsApp::slotShowLastMsg()
{
  QString com = QucsSettings.Editor+" "+QucsHomeDir.filePath("log.txt");
  QProcess QucsEditor(QStringList::split(" ", com));
  if(!QucsEditor.start())
    QMessageBox::critical(this, tr("Error"), tr("Cannot start text editor!"));
}

// ------------------------------------------------------------------------
// Is called to show the netlist of the last simulation.
void QucsApp::slotShowLastNetlist()
{
  QString com = QucsSettings.Editor+" "+QucsHomeDir.filePath("netlist.txt");
  QProcess QucsEditor(QStringList::split(" ", com));
  if(!QucsEditor.start())
    QMessageBox::critical(this, tr("Error"), tr("Cannot start text editor!"));
}

// ------------------------------------------------------------------------
// Changes to the corresponding data display page or vice versa.
void QucsApp::slotChangePage(QString Name)
{
  if(Name.isEmpty())  return;

  QucsDoc   *d;
  QFileInfo Info;
  QucsDoc   *Doc = view->Docs.current();

  // search, if page is already loaded
  for(d = view->Docs.first(); d!=0; d = view->Docs.next()) {
    Info.setFile(d->DocName);
    if(Info.fileName() == Name) break;
  }

  if(d == 0) {   // no open page found ?
    Info.setFile(Name);
    if(Info.isRelative())
      Name = QucsWorkDir.filePath(Name);
    QFile file(Name);
    if(!file.open(IO_ReadOnly))       // load page
      if(!file.open(IO_ReadWrite)) {  // if it doesn't exist, create
        view->Docs.findRef(Doc);
        QMessageBox::critical(this, tr("Error"),
			tr("Cannot create ")+Info.dirPath(true)+"/"+Name);
        return;
      }
      else new QListViewItem(ConDisplays, Info.fileName()); // add new name
    file.close();

    d = new QucsDoc(this, Name);
    view->Docs.append(d);   // create new page

    if(!d->load()) {
      view->Docs.remove();
      view->Docs.findRef(Doc);
      view->viewport()->repaint();
      view->drawn = false;
      return;
    }
  }

  int cNo = view->Docs.at();
  view->Docs.findRef(Doc);  // back to the last current
  WorkView->setCurrentTab(WorkView->tabAt(cNo));  // make new doc the current
  // must be done before the next step, in order to call the change slot !

  TabView->setCurrentPage(2);   // switch to "Component"-Tab
  if(Name.right(4) == ".dpl") {
    CompChoose->setCurrentItem(COMBO_Diagrams);   // switch to diagrams
    slotSetCompView(COMBO_Diagrams);
  }
}

// ------------------------------------------------------------------------
// Changes to the data display of current page.
void QucsApp::slotToPage()
{
  QString Name = view->Docs.current()->DataDisplay;
  if(Name.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No page set !"));
    return;
  }

  slotChangePage(Name);
}

// #######################################################################
// Is called when a double click is made in the content ListView.
//void QucsApp::slotOpenContent(QListViewItem *item, const QPoint &, int column)   // QT 3.2
void QucsApp::slotOpenContent(QListViewItem *item)
{
  if(item == 0) return;   // no item was double clicked
  if(item->parent() == 0) return;  // no component, but item "schematic", ...

  
  QucsWorkDir.setPath(QucsHomeDir.path());
  if(!QucsWorkDir.cd(ProjName+"_prj/")) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: ")+
			  QucsWorkDir.path());
    return;
  }

  gotoPage(QucsWorkDir.filePath(item->text(0)));

  if(item->text(1).isEmpty()) return;
  // switch on the 'select' action
  Acts.select->blockSignals(true);
  Acts.select->setOn(true);
  Acts.select->blockSignals(false);
  Acts.slotSelect(true);
}

// ########################################################################
// Is called when the close project menu is called.
void QucsApp::slotMenuCloseProject()
{
  if(!closeAllFiles()) return;   // close files and ask for saving them
  view->Docs.append(new QucsDoc(this, ""));   // create 'untitled' file
  view->viewport()->repaint();
  view->drawn = false;

  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: "));
  QucsWorkDir.setPath(QDir::homeDirPath()+"/.qucs");

  Content->setColumnText(0,tr("Content of")); // column text

  Content->clear();   // empty content view
  ConDatasets   = new QListViewItem(Content, tr("Datasets"));
  ConDisplays   = new QListViewItem(Content, tr("Data Displays"));
  ConSchematics = new QListViewItem(Content, tr("Schematics"));

  TabView->setCurrentPage(0);   // switch to "Projects"-Tab
  ProjName = "";
}

// ########################################################################
// Is called when the open project menu is called.
void QucsApp::slotMenuOpenProject()
{
  QFileDialog *d = new QFileDialog(QucsHomeDir.path());
  d->setCaption(tr("Choose Project Directory for Opening"));
  d->setShowHiddenFiles(true);
  d->setMode(QFileDialog::DirectoryOnly);
  if(d->exec() != QDialog::Accepted) return;

  QString s = d->selectedFile();
  if(s.isEmpty()) return;

  s = s.left(s.length()-1);   // cut off trailing '/'
  int i = s.findRev('/');
  if(i > 0) s = s.mid(i+1);   // cut out the last subdirectory
  s.remove("_prj");
  OpenProject(d->selectedFile(), s);
}

// #######################################################################
// Is called when the open project button is pressed.
void QucsApp::slotOpenProject(QListBoxItem *item)
{
  OpenProject(QucsHomeDir.filePath(item->text()+"_prj"), item->text());
}

// ########################################################################
// Checks whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int QucsApp::testFile(const QString& DocName)
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    return -1;
  }

  QString Line;
  // *****************************************************************
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();
  file.close();
  QTextStream stream(&FileString, IO_ReadOnly);


  // read header **************************
  do {
    if(stream.atEnd()) {
      file.close();
      return -2;
    }
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();
  } while(Line.isEmpty());
  
  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    return -3;
  }

  QString s = PACKAGE_VERSION;
  s.remove('.');
  Line = Line.mid(16, Line.length()-17);
  Line.remove('.');
  if(Line > s) {  // wrong version number ? (only backward compatible)
    file.close();
    return -4;
  }

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Components>") break;
  }

  int z=0;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Components>") {
      file.close();
      return z;       // return number of ports
    }

    Line = Line.stripWhiteSpace();
    s    = Line.section(' ',0,0);    // component type
    if(s == "<Port") z++;
  }
  return -5;  // component field not closed
}

// #######################################################################
// Opens an existing project.
void QucsApp::OpenProject(const QString& Path, const QString& Name)
{
  if(!closeAllFiles()) return;   // close files and ask for saving them
  view->Docs.append(new QucsDoc(this, ""));   // create 'untitled' file
  view->viewport()->repaint();
  view->drawn = false;

  QDir ProjDir(QDir::cleanDirPath(Path));
  if(!ProjDir.exists() || !ProjDir.isReadable()) { // check project directory

    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: ")+Path);
    return;
  }
  QucsWorkDir.setPath(ProjDir.path());

  Content->setColumnText(0,tr("Content of '")+Name+tr("'")); // column text
//  Content->setColumnWidth(0, Content->width()-5);

  Content->clear();   // empty content view
  ConDatasets   = new QListViewItem(Content, tr("Datasets"));
  ConDisplays   = new QListViewItem(Content, tr("Data Displays"));
  ConSchematics = new QListViewItem(Content, tr("Schematics"));

  int n;
  // put all schematic files into "Content"-ListView
  QStringList Elements = ProjDir.entryList("*.sch", QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Elements.begin(); it != Elements.end(); ++it) {
    n = testFile(ProjDir.filePath((*it).ascii()));
    if(n >= 0) {
      if(n > 0)
        new QListViewItem(ConSchematics, (*it).ascii(),
			  QString::number(n)+tr("-port"));
      else new QListViewItem(ConSchematics, (*it).ascii());
    }
  }

  // put all data display files into "Content"-ListView
  Elements = ProjDir.entryList("*.dpl", QDir::Files, QDir::Name);
  for(it = Elements.begin(); it != Elements.end(); ++it)
    new QListViewItem(ConDisplays, (*it).ascii());

  // put all dataset files into "Content"-ListView
  Elements = ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
  for(it = Elements.begin(); it != Elements.end(); ++it)
    new QListViewItem(ConDatasets, (*it).ascii());

  TabView->setCurrentPage(1);   // switch to "Content"-Tab
  Content->firstChild()->setOpen(true);  // show schematics
  ProjName = Name;   // remember the name of project

  // show name in title of main window
  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: ")+Name);
}

// #######################################################################
// Is called, when "Create New Project"-Button is pressed.
void QucsApp::slotProjNewButt()
{
  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir projDir(QucsHomeDir.path());
  if(projDir.mkdir(d->ProjName->text()+"_prj")) {
    Projects->insertItem(d->ProjName->text(),0);  // at first position
    if(d->OpenProj->isChecked()) slotOpenProject(Projects->firstItem());
  }
  else QMessageBox::information(this, tr("Info"),
                    tr("Cannot create project directory !"));
}

// ######################################################################
// The following arrays contains the components that appear in the
// component listview.
typedef Component*  (*pInfoFunc) (QString&, char* &, bool);
pInfoFunc Simulations[7] =
  {&DC_Sim::info, &TR_Sim::info, &AC_Sim::info, &SP_Sim::info,
   &HB_Sim::info, &Param_Sweep::info, 0};

pInfoFunc lumpedComponents[18] =
  {&Resistor::info, &Resistor::info_us, &Capacitor::info, &Inductor::info,
   &Ground::info, &SubCirPort::info, &Transformer::info, &symTrafo::info,
   &dcBlock::info, &dcFeed::info, &BiasT::info, &Attenuator::info,
   &Isolator::info, &Circulator::info, &Gyrator::info, &Phaseshifter::info,
   &iProbe::info, 0};

pInfoFunc Sources[14] =
  {&Volt_dc::info, &Ampere_dc::info, &Volt_ac::info, &Ampere_ac::info,
   &Source_ac::info, &Volt_noise::info, &Ampere_noise::info, &VCCS::info,
   &CCCS::info, &VCVS::info, &CCVS::info, &vPulse::info, &iPulse::info, 0};

pInfoFunc TransmissionLines[13] =
  {&TLine::info, &Substrate::info, &MSline::info, &MScoupled::info,
   &MScorner::info, &MSmbend::info, &MSstep::info, &MStee::info,
   &MScross::info, &MSopen::info, &MSgap::info, &Coplanar::info, 0};

pInfoFunc nonlinearComps[] =
  {&Diode::info, &BJT::info, &BJT::info_pnp, &BJTsub::info,
   &BJTsub::info_pnp, &JFET::info, &JFET::info_p,
   &MOSFET::info, &MOSFET::info_p, &MOSFET::info_depl,
   &MOSFET_sub::info, &MOSFET_sub::info_p, &MOSFET_sub::info_depl, 0};

// #######################################################################
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriat components.
void QucsApp::slotSetCompView(int index)
{
  char *File;
  QString Name;
  pInfoFunc *Infos = 0;

  CompComps->clear();   // clear the IconView
  if((index+1) >= CompChoose->count()) {
    new QIconViewItem(CompComps, tr("Line"),
		QImage(BITMAPDIR "line.xpm"));

    if(CompChoose->count() == 1) {
      new QIconViewItem(CompComps, tr("Elliptic Arc"),
		QImage(BITMAPDIR "ellipsearc.xpm"));
      return;
    }

    new QIconViewItem(CompComps, tr("Arrow"),
		QImage(BITMAPDIR "arrow.xpm"));
    new QIconViewItem(CompComps, tr("Text"),
		QImage(BITMAPDIR "text.xpm"));
    new QIconViewItem(CompComps, tr("Ellipse"),
		QImage(BITMAPDIR "ellipse.xpm"));
    new QIconViewItem(CompComps, tr("Rectangle"),
		QImage(BITMAPDIR "rectangle.xpm"));
    new QIconViewItem(CompComps, tr("filled Ellipse"),
		QImage(BITMAPDIR "filledellipse.xpm"));
    new QIconViewItem(CompComps, tr("filled Rectangle"),
		QImage(BITMAPDIR "filledrect.xpm"));
    new QIconViewItem(CompComps, tr("Elliptic Arc"),
		QImage(BITMAPDIR "ellipsearc.xpm"));
    return;
  }

  switch(index) {
    case COMBO_passive:   Infos = &lumpedComponents[0];  break;
    case COMBO_Sources:   Infos = &Sources[0];           break;
    case COMBO_TLines:    Infos = &TransmissionLines[0]; break;
    case COMBO_nonlinear: Infos = &nonlinearComps[0];    break;
    case COMBO_File:
      new QIconViewItem(CompComps, tr("1-port S parameter file"),
		QImage(BITMAPDIR "spfile1.xpm"));
      new QIconViewItem(CompComps, tr("2-port S parameter file"),
		QImage(BITMAPDIR "spfile2.xpm"));
      new QIconViewItem(CompComps, tr("3-port S parameter file"),
		QImage(BITMAPDIR "spfile3.xpm"));
      new QIconViewItem(CompComps, tr("4-port S parameter file"),
		QImage(BITMAPDIR "spfile4.xpm"));
      new QIconViewItem(CompComps, tr("5-port S parameter file"),
		QImage(BITMAPDIR "spfile5.xpm"));
      new QIconViewItem(CompComps, tr("6-port S parameter file"),
		QImage(BITMAPDIR "spfile6.xpm"));
      return;
    case COMBO_Sims:     Infos = &Simulations[0];  break;
    case COMBO_Diagrams:
      new QIconViewItem(CompComps, tr("Cartesian"),
		QImage(BITMAPDIR "rect.xpm"));
      new QIconViewItem(CompComps, tr("Polar"),
		QImage(BITMAPDIR "polar.xpm"));
      new QIconViewItem(CompComps, tr("Tabular"),
		QImage(BITMAPDIR "tabular.xpm"));
      new QIconViewItem(CompComps, tr("Smith Chart"),
		QImage(BITMAPDIR "smith.xpm"));
      return;
  }

  while(*Infos != 0) {
    (**Infos) (Name, File, false);
    new QIconViewItem(CompComps, Name,
		QImage(BITMAPDIR+QString(File)+".xpm"));
    Infos++;
  }
}

// ----------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QIconViewItem *item)
{
  // delete previously selected elements
  if(view->selComp != 0)  delete view->selComp;
  if(view->selDiag != 0)  delete view->selDiag;
  if(view->selPaint != 0) delete view->selDiag;
  view->selComp  = 0;   // no component selected
  view->selDiag  = 0;   // no diagram selected
  view->selPaint = 0;   // no painting selected

  if(item == 0) {   // mouse button pressed not over an item ?
//    CompComps->setSelected(CompComps->currentItem(), false);  // deselect component in ViewList
    CompComps->clearSelection();  // deselect component in ViewList
    if(view->drawn) view->viewport()->repaint();
    view->drawn = false;
    return;
  }

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;


  if((CompChoose->currentItem()+1) >= CompChoose->count()) {
    if(CompChoose->count() == 1)
      switch(CompComps->index(item)) { // in "edit symbol" mode
	case 0: view->selPaint = new GraphicLine();   break;
	case 1: view->selPaint = new EllipseArc();    break;
      }
    else
      switch(CompComps->index(item)) { // in normal ("edit schematic") mode
	case 0: view->selPaint = new GraphicLine();   break;
	case 1: view->selPaint = new Arrow();         break;
	case 2: view->selPaint = new GraphicText();   break;
	case 3: view->selPaint = new Ellipse();       break;
	case 4: view->selPaint = new Rectangle();     break;
	case 5: view->selPaint = new Ellipse(true);   break;
	case 6: view->selPaint = new Rectangle(true); break;
	case 7: view->selPaint = new EllipseArc();    break;
      }
    if(view->drawn) view->viewport()->repaint();
    view->drawn = false;
    view->MouseMoveAction = &QucsView::MMovePainting;
    view->MousePressAction = &QucsView::MPressPainting;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    return;
  }

  pInfoFunc Infos = 0;
  switch(CompChoose->currentItem()) {
    case COMBO_passive:
	 Infos = lumpedComponents[CompComps->index(item)];
	 break;
    case COMBO_Sources:
	 Infos = Sources[CompComps->index(item)];
	 break;
    case COMBO_TLines:
	 Infos = TransmissionLines[CompComps->index(item)];
	 break;
    case COMBO_nonlinear:
	 Infos = nonlinearComps[CompComps->index(item)];
	 break;
    case COMBO_File:
	 view->selComp = new SParamFile(CompComps->index(item)+1);
	 break;
    case COMBO_Sims:
	 Infos = Simulations[CompComps->index(item)];
	 break;
    case COMBO_Diagrams:
          switch(CompComps->index(item)) {
              case 0: view->selDiag = new RectDiagram();  break;
              case 1: view->selDiag = new PolarDiagram(); break;
              case 2: view->selDiag = new TabDiagram();   break;
              case 3: view->selDiag = new SmithDiagram(); break;
          }

          if(view->drawn) view->viewport()->repaint();
          view->drawn = false;
          view->MouseMoveAction = &QucsView::MMoveDiagram;
          view->MousePressAction = &QucsView::MPressDiagram;
          view->MouseReleaseAction = &QucsView::MouseDoNothing;
          view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
          return;
  }

  char *Dummy2;
  QString Dummy1;
  if(Infos) view->selComp = (*Infos) (Dummy1, Dummy2, true);

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectSubcircuit(QListViewItem *item)
{
  if(item == 0) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    if(view->drawn) view->viewport()->repaint();
    view->drawn = false;
    return;
  }

  if(item->parent() == 0) return;
  if(item->parent()->text(0) != tr("Schematics"))
    return;   // return, if not clicked on schematic
  if(item->text(1).isEmpty()) return;   // return, if not a subcircuit

  // delete previously selected elements
  if(view->selComp != 0)  delete view->selComp;
  if(view->selDiag != 0)  delete view->selDiag;
  view->selComp = 0;   // no component selected
  view->selDiag = 0;   // no diagram selected

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  view->selComp = new Subcircuit();
  view->selComp->Props.first()->Value = item->text(0);
  view->selComp->recreate();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
// Is called when the select all action is activated.
void QucsApp::slotSelectAll()
{
  view->Docs.current()->selectElements(INT_MIN, INT_MIN,
				INT_MAX, INT_MAX, true);
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Open Project"-Button is pressed.
void QucsApp::slotProjOpenButt()
{
  QListBoxItem *item = Projects->selectedItem();
  if(item) slotOpenProject(item);
  else QMessageBox::information(this, tr("Info"),
				tr("No project is selected !"));
}

// #######################################################################
bool QucsApp::DeleteProject(const QString& Path, const QString& Name)
{
  if(Name == ProjName) {
    QMessageBox::information(this, tr("Info"),
			tr("Cannot delete an open project !"));
    return false;
  }

  // first ask, if really delete project ?
  if(QMessageBox::warning(this, tr("Warning"),
     tr("This will destroy all the project files permanently ! Continue ?"),
     tr("&Yes"), tr("&No"), 0,1,1))  return false;

  QDir projDir = QDir(Path);
  QStringList ProjFiles = projDir.entryList("*", QDir::Files);  // all files

  // removes every file, remove("*") does not work
  QStringList::iterator it;
  for(it = ProjFiles.begin(); it != ProjFiles.end(); it++) {
     if(!projDir.remove(*it)) {
       QMessageBox::information(this, tr("Info"),
				tr("Cannot remove project file: ")+(*it));
       return false;
     }
  }

  projDir.cdUp();  // leave project directory for deleting
  if(!projDir.rmdir(Name+"_prj")) {
    QMessageBox::information(this, tr("Info"),
			     tr("Cannot remove project directory !"));
    return false;
  }

  return true;
}

// ########################################################################
// Is called, when "Delete Project"-menu is pressed.
void QucsApp::slotMenuDelProject()
{
  QFileDialog *d = new QFileDialog(QucsHomeDir.path());
  d->setCaption(tr("Choose Project Directory for Deleting"));
  d->setShowHiddenFiles(true);
  d->setMode(QFileDialog::DirectoryOnly);
  if(d->exec() != QDialog::Accepted) return;

  QString s = d->selectedFile();
  if(s.isEmpty()) return;

  s = s.left(s.length()-1);   // cut off trailing '/'
  int i = s.findRev('/');
  if(i > 0) s = s.mid(i+1);   // cut out the last subdirectory
  s.remove("_prj");
  DeleteProject(d->selectedFile(), s);
  readProjects();   // re-reads all projects and inserts them into the ListBox
}

// #######################################################################
// Is called, when "Delete Project"-Button is pressed.
void QucsApp::slotProjDelButt()
{
  QListBoxItem *item = Projects->selectedItem();
  if(!item) {
    QMessageBox::information(this, tr("Info"),
			     tr("No project is selected !"));
    return;
  }

  if(!DeleteProject(QucsHomeDir.filePath(item->text()+"_prj"),
	item->text()))  return;
  Projects->removeItem(Projects->currentItem());  // remove from project list
}

// #######################################################################
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditUndo()
{
  view->Docs.current()->undo();
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditRedo()
{
  view->Docs.current()->redo();
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Align top" action is activated.
void QucsApp::slotAlignTop()
{
  if(!view->Docs.current()->aligning(0))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Align bottom" action is activated.
void QucsApp::slotAlignBottom()
{
  if(!view->Docs.current()->aligning(1))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Align left" action is activated.
void QucsApp::slotAlignLeft()
{
  if(!view->Docs.current()->aligning(2))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Align right" action is activated.
void QucsApp::slotAlignRight()
{
  if(!view->Docs.current()->aligning(3))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Distribute horizontally" action is activated.
void QucsApp::slotDistribHoriz()
{
  view->Docs.current()->distribHoriz();
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
// Is called, when "Distribute vertically" action is activated.
void QucsApp::slotDistribVert()
{
  view->Docs.current()->distribVert();
  view->viewport()->repaint();
  view->drawn = false;
}

// #######################################################################
void QucsApp::switchEditMode(bool SchematicMode)
{
  fillComboBox(SchematicMode);

  Acts.editActivate->setEnabled(SchematicMode);
  Acts.insEquation->setEnabled(SchematicMode);
  Acts.insGround->setEnabled(SchematicMode);
  Acts.insPort->setEnabled(SchematicMode);
  Acts.insWire->setEnabled(SchematicMode);
  Acts.insLabel->setEnabled(SchematicMode);
  Acts.setMarker->setEnabled(SchematicMode);
  simulate->setEnabled(SchematicMode);
}

// #######################################################################
void QucsApp::changeSchematicSymbolMode(QucsDoc *d)
{
  if(d->symbolMode) {
    d->symbolMode = false;
    symEdit->setMenuText("Edit Circuit Symbol");
    switchEditMode(true);

    d->Comps  = &(d->DocComps);
    d->Wires  = &(d->DocWires);
    d->Nodes  = &(d->DocNodes);
    d->Diags  = &(d->DocDiags);
    d->Paints = &(d->DocPaints);

    QString *ps = d->UndoStack.current();
    if(ps != d->UndoStack.getFirst())  undo->setEnabled(true);
    else  undo->setEnabled(false);
    if(ps != d->UndoStack.getLast())  redo->setEnabled(true);
    else  redo->setEnabled(false);
  }
  else {
    // go into select modus to avoid placing a forbidden element
    Acts.select->setOn(true);

    d->symbolMode = true;
    symEdit->setMenuText("Edit Schematic");
    switchEditMode(false);

    d->Comps  = &(SymbolComps);
    d->Wires  = &(SymbolWires);
    d->Nodes  = &(SymbolNodes);
    d->Diags  = &(SymbolDiags);
    d->Paints = &(d->SymbolPaints);

    // If the number of ports is not equal, remove or add some.
    int countPort = d->adjustPortNumbers();

    // If a symbol does not yet exist, create one.
    if(d->SymbolPaints.isEmpty()) {
      int h = 30*((countPort-1)/2) + 10;
      d->SymbolPaints.append(
	new GraphicLine(-20, -h, 40,  0, QPen(QPen::darkBlue,2)));
      d->SymbolPaints.append(
	new GraphicLine( 20, -h,  0,2*h, QPen(QPen::darkBlue,2)));
      d->SymbolPaints.append(
	new GraphicLine(-20,  h, 40,  0, QPen(QPen::darkBlue,2)));
      d->SymbolPaints.append(
	new GraphicLine(-20, -h,  0,2*h, QPen(QPen::darkBlue,2)));
//  Texts.append(new Text( -7,  0,"sub"));

      int i=0, y = 10-h;
      while(i<countPort) {
	i++;
	d->SymbolPaints.append(
	  new GraphicLine(-30, y, 10, 0, QPen(QPen::darkBlue,2)));
	d->SymbolPaints.at(i-1)->setCenter(-30,  y);

	if(i == countPort)  break;
	i++;
	d->SymbolPaints.append(
	  new GraphicLine( 20, y, 10, 0, QPen(QPen::darkBlue,2)));
	d->SymbolPaints.at(i-1)->setCenter(30,  y);
	y += 60;
      }
    }

    QString *ps = d->UndoSymbol.current();
    if(ps != d->UndoSymbol.getFirst())  undo->setEnabled(true);
    else  undo->setEnabled(false);
    if(ps != d->UndoSymbol.getLast())  redo->setEnabled(true);
    else  redo->setEnabled(false);
  }
  slotSetCompView(0);
}

// #######################################################################
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: Schematic and (subcircuit)
// symbol.
void QucsApp::slotSymbolEdit()
{
  QucsDoc *d = view->Docs.current();
  d->switchPaintMode();   // twist the view coordinates
  changeSchematicSymbolMode(d);

  // This can only be true when switching to the symbol the first time.
  if(d->UndoSymbol.isEmpty()) {
    d->setChanged(false, true); // "not changed" state, but put on undo stack
    d->UndoSymbol.current()->at(1) = 'i';  // state of being unchanged
  }

  view->resizeContents(int(d->Scale*double(d->ViewX2-d->ViewX1)),
                       int(d->Scale*double(d->ViewY2-d->ViewY1)));
  view->setContentsPos(d->PosX, d->PosY);
  view->viewport()->repaint();
  view->drawn = false;
}
