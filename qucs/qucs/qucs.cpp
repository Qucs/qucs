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
# include <config.h>
#endif

#include "qucs.h"
#include "qucsview.h"
#include "wire.h"
#include "paintings/paintings.h"
#include "diagrams/diagrams.h"
#include "dialogs/messagebox.h"
#include "dialogs/helpdialog.h"
#include "dialogs/newprojdialog.h"
#include "dialogs/fileshowdialog.h"
#include "dialogs/settingsdialog.h"

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


#include <limits.h>


#define  COMBO_passive   0
#define  COMBO_Sources   1
#define  COMBO_TLines    2
#define  COMBO_nonlinear 3
#define  COMBO_File      4
#define  COMBO_Sims      5
#define  COMBO_Paints    6
#define  COMBO_Diagrams  7


QucsApp::QucsApp()
{
  setCaption("Qucs " PACKAGE_VERSION);

  QucsFileFilter = tr("Schematic (*.sch);; Data Display (*.dpl);;")+
                   tr("Qucs Documents (*.sch *dpl);; Any File (*)");

  if(!loadSettings())
    resize(600,400);


  initView();
  Init.perform(this);  // initializes toolbar, statusbar, menubar, actions
  initCursorMenu();


  // default settings of the printer
  Printer.setOrientation(QPrinter::Landscape);
  Printer.setColorMode(QPrinter::Color);


  // switch on the 'select' action
  select->blockSignals(true);
  select->setOn(true);
  select->blockSignals(false);
  slotSelect(true);

  HierarchyHistory.setAutoDelete(true);
}

QucsApp::~QucsApp()
{
}

// ###########################################################################
// Loads the settings file and restores the settings.
bool QucsApp::loadSettings()
{
  QFile file(QDir::homeDirPath()+"/.qucs/qucsrc");
  if(!file.open(IO_ReadOnly)) return false; // settings file doesn't exist

  QString Line, Setting;
  QTextStream stream(&file);

  int  x, y;
  bool ok;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "Position")  { x = Line.section(",",0,0).toInt(&ok);
                                 y = Line.section(",",1,1).toInt(&ok);
                                 move(x,y); }
    else if(Setting == "Size") { x = Line.section(",",0,0).toInt(&ok);
                                 y = Line.section(",",1,1).toInt(&ok);
                                 resize(x,y); }
  }
  file.close();
//  resize(600,400);
//  resize(maximumSize());
  return true;
}

// ###########################################################################
// Saves the settings in the settings file.
void QucsApp::saveSettings()
{
  QFile file(QDir::homeDirPath()+"/.qucs/qucsrc");
  if(!file.open(IO_WriteOnly)) {    // settings file cannot be created
    QMessageBox::warning(this, tr("Warning"), tr("Cannot save settings !"));
    return;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, Qucs " PACKAGE_VERSION "\n";
  stream << "Position=" << x() << "," << y() << "\n";
  stream << "Size=" << width() << "," << height() << "\n";
  file.close();
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
  view = new QucsView(WorkGroup);       // work area with documents

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
  Content->setRootIsDecorated(true);  // root items should have open/close decoration to their left
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

  CompChoose->insertItem(tr("lumped components"));
  CompChoose->insertItem(tr("sources"));
  CompChoose->insertItem(tr("transmission lines"));
  CompChoose->insertItem(tr("nonlinear components"));
  CompChoose->insertItem(tr("file data"));
  CompChoose->insertItem(tr("simulations"));
  CompChoose->insertItem(tr("paintings"));
  CompChoose->insertItem(tr("diagrams"));

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(clicked(QIconViewItem*)),
		     SLOT(slotSelectComponent(QIconViewItem*)));


  // ---------------------------------------------------------------------
  readProjects();   // reads all projects and inserts them into the ListBox

  view->Docs.append(new QucsDoc(WorkView, ""));  // creates a document called "untitled"

  // ...............................................................
//  connect(view, SIGNAL(CompsSelected(bool)), this, SLOT(slotActivateCopy(bool)));
}

// ----------------------------------------------------------
// Menu that appears if right mouse button is pressed on a file in the "Content" ListView.
void QucsApp::initCursorMenu()
{
  ContentMenu = new QPopupMenu(Content);
  ContentMenu->insertItem(tr("Open"),   this, SLOT(slotCMenuOpen()));
  ContentMenu->insertItem(tr("Rename"), this, SLOT(slotCMenuRename()));
  ContentMenu->insertItem(tr("Delete"), this, SLOT(slotCMenuDelete()));

  connect(Content,
	  SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
	  this,
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
  for(QucsDoc *d = view->Docs.first(); d!=0; d = view->Docs.next()) {  // search, if file is open
    if(d->DocName == QDir::current().path()+"/"+i->text(0)) {
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

  QDir file("");
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
    if(d->DocName == QDir::current().path()+"/"+item->text(0)) {
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

  if(!QFile::remove(item->text(0)))
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete schematic: ")+item->text(0));

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
  QDir ProjDir;
  if(!ProjDir.cd(QDir::homeDirPath()+"/.qucs"))   // is work directory already existing ?
    if(!ProjDir.mkdir(QDir::homeDirPath()+"/.qucs")) {  // no, then create it
      QMessageBox::warning(this, tr("Warning"),
			tr("Cannot create work directory !"));
      return;
    }
  ProjDir.setPath(QDir::homeDirPath()+"/.qucs");
  QDir::setCurrent(QDir::homeDirPath()+"/.qucs");

  // get all directories
  QStringList PrDirs = ProjDir.entryList("*", QDir::Dirs, QDir::Name);
  PrDirs.pop_front(); // delete "." from list
  PrDirs.pop_front(); // delete ".." from list

  Projects->clear();
  QStringList::iterator it;
  // inserts all project directories
  for(it = PrDirs.begin(); it != PrDirs.end(); it++)
     if ((*it).right(4) == "_prj") {   // project directories end with "_prj"
      (*it) = (*it).left((*it).length()-4);      // remove "_prj" from project name
      Projects->insertItem(*it);
     }
}

// #########################################################################
bool QucsApp::closeAllFiles()
{
  int  Result = 0;
  bool notForAll = true;
  MessageBox *m = new MessageBox(tr("Closing Qucs document"),
     tr("This document contains unsaved changes!\nDo you want to save the changes before closing?"),this);

  // close all files and ask to save changed ones
  for(QucsDoc *ptr = view->Docs.first(); ptr != 0; ) {
    WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));  // make next document the current
    view->viewport()->repaint();
    view->drawn = false;
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
  
  d = new QucsDoc(WorkView, Name);
  view->Docs.append(d);   // create new page

  if(!d->load()) {    // load document if possible
    view->Docs.remove();
    view->Docs.at(No);
    return false;
  }

  cNo = view->Docs.at();
  view->Docs.at(No);  // back to the last current
  // make new document the current
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

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void QucsApp::slotFileSettings()
{
  SettingsDialog *d = new SettingsDialog(view->Docs.current(), this);
  d->exec();
  delete d;
  view->viewport()->repaint();
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotFileNew()
{
  statusBar()->message(tr("Creating new schematic..."));

  view->Docs.append(new QucsDoc(WorkView, ""));
  // make new document the current
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  statusBar()->message(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(".", QucsFileFilter, this,
                                           "", tr("Enter a Schematic Name"));
  if(!s.isEmpty())  gotoPage(s); //openDocument(s);
  else statusBar()->message(tr("Opening aborted"), 2000);

  statusBar()->message(tr("Ready."));
}


// ########################################################################
void QucsApp::updatePortNumber(int No)
{
  if(No<0) return;
  QFileInfo Info(view->Docs.current()->DocName);
  QString Name = Info.fileName();

  for(QListViewItem *p = ConSchematics->firstChild(); p!=0; p = p->nextSibling()) {
    if(p->text(0) == Name) {
      if(No == 0) p->setText(1,"");
      else p->setText(1,QString::number(No)+tr("-port"));
      break;
    }
}
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
  QString s;
  QFileInfo Info;
  while(true) {
    intoView = true;
    Info.setFile(view->Docs.current()->DocName);
    if(Info.extension() == "dpl")
      s = QFileDialog::getSaveFileName("./"+Info.fileName(),
				tr("Data Display (*.dpl)"), this, "",
				tr("Enter a Document Name"));
    else  s = QFileDialog::getSaveFileName("./"+Info.fileName(),
				QucsFileFilter, this, "",
				tr("Enter a Document Name"));
    if(s.isEmpty())  return false;
    Info.setFile(s);                 // try to guess the best extension ...
    if(Info.extension(false).isEmpty()) {    // ... if no one was specified
      if(!view->Docs.current()->DocName.isEmpty()) {
        Info.setFile(view->Docs.current()->DocName);
        s += '.' + Info.extension(false);
      }
      else s += ".sch";
    }

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
    if(Info.dirPath(true) == QDir::currentDirPath())
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
    statusBar()->message(tr("Saving aborted"), 2000);
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
  // make new document the current
  WorkView->setCurrentTab(WorkView->tabAt(view->Docs.at()));

  if(view->Docs.isEmpty())  // if no document left, create an untitled
    view->Docs.append(new QucsDoc(WorkView, ""));

  QucsDoc *d = view->Docs.current();
  view->resizeContents(int(d->Scale*double(d->ViewX2-d->ViewX1)),
                       int(d->Scale*double(d->ViewY2-d->ViewY1)));
  view->setContentsPos(d->PosX, d->PosY);   // set view area
  view->viewport()->repaint();
  view->drawn = false;

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

// --------------------------------------------------------------------
// Exits the application.
void QucsApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));

  int exit = QMessageBox::information(this, tr("Quit..."),
			  tr("Do you really want to quit?"),
			  QMessageBox::Ok, QMessageBox::Cancel);
  if(exit == 1)
    if(closeAllFiles()) {
      saveSettings();
      qApp->quit();
    }


  statusBar()->message(tr("Ready."));
}

//-----------------------------------------------------------------
// To get all close events.
void QucsApp::closeEvent(QCloseEvent* Event)
{
  Event->ignore();
  if(closeAllFiles()) {
    saveSettings();
    qApp->quit();
  }
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

// ######################################################################
void QucsApp::slotEditPaste(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }

  if(!view->pasteElements()) {
    editPaste->blockSignals(true); // do not call toggle slot
    editPaste->setOn(false);       // set toolbar button off
    editPaste->blockSignals(false);
    return;   // if clipboard empty
  }

  statusBar()->message(tr("Inserting clipboard contents..."));

  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = editPaste;

  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMovePaste;
  view->MousePressAction = &QucsView::MouseDoNothing;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;

  statusBar()->message(tr("Ready."));
}

// ########################################################################
void QucsApp::slotHelpIndex()
{
  HelpDialog *d = new HelpDialog("index.html");
  d->show();
}

// ########################################################################
void QucsApp::slotGettingStarted()
{
  HelpDialog *d = new HelpDialog("start.html");
  d->show();
}

// ########################################################################
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{
  QucsDoc *Doc = view->Docs.current();
  Component *pc = view->Docs.current()->searchSelSubcircuit();
  if(pc == 0) return;
  QString s = pc->Props.getFirst()->Value;
  if(s.at(0) != '/') s = QDir::currentDirPath()+"/"+s;
  if(!gotoPage(s)) return;

  QString *ps = new QString(Doc->DocName);
  HierarchyHistory.append(ps);    // remember for the way back
  popH->setEnabled(true);
}

// ########################################################################
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{
  if(HierarchyHistory.count() == 0) return;

  if(!gotoPage( *(HierarchyHistory.last()) )) return;

  HierarchyHistory.remove();
  if(HierarchyHistory.count() == 0) popH->setEnabled(false);
}

// ########################################################################
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(int id)
{
  QucsDoc *d = view->Docs.current();
  d->PosX = view->contentsX();    // save position for old document
  d->PosY = view->contentsY();

  d = view->Docs.at(WorkView->indexOf(id));   // new current document
  view->resizeContents(int(d->Scale*double(d->ViewX2-d->ViewX1)),
                       int(d->Scale*double(d->ViewY2-d->ViewY1)));
  view->setContentsPos(d->PosX, d->PosY);

  view->Docs.current()->reloadGraphs();  // load recent simulation data
  view->viewport()->repaint();
  view->drawn = false;

  HierarchyHistory.clear();   // no subcircuit history
  popH->setEnabled(false);
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
  view->Docs.current()->Scale = 1.0;

  int x1 = view->Docs.current()->UsedX1;
  int y1 = view->Docs.current()->UsedY1;
  int x2 = view->Docs.current()->UsedX2;
  int y2 = view->Docs.current()->UsedY2;

//  view->Docs.current()->sizeOfAll(x1, y1, x2, y2);
  if(x2==0) if(y2==0) if(x1==0) if(y1==0) x2 = y2 = 800;

  view->Docs.current()->ViewX1 = x1-40;
  view->Docs.current()->ViewY1 = y1-40;
  view->Docs.current()->ViewX2 = x2+40;
  view->Docs.current()->ViewY2 = y2+40;
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

  SimMessage *sim = new SimMessage(this);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed !
  connect(sim, SIGNAL(SimulationEnded(int, SimMessage*)), this,
               SLOT(slotAfterSimulation(int, SimMessage*)));
  connect(sim, SIGNAL(displayDataPage()), this, SLOT(slotChangePage()));

//  sim.ProgText->clear();
//  sim.ErrText->clear();
  sim->show();

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time
  sim->ProgText->insert(tr("Starting new simulation on ")+
                        d.toString("ddd dd. MMM yyyy"));
  sim->ProgText->insert(tr(" at ")+t.toString("hh:mm:ss")+"\n\n");

  sim->ProgText->insert(tr("creating netlist ...."));
  QFile NetlistFile("netlist.txt");
  if(!view->Docs.current()->createNetlist(&NetlistFile)) {
    sim->ErrText->insert(tr("ERROR: Cannot create netlist file!\nAborted."));
    sim->errorSimEnded();
    return;
  }
  sim->ProgText->insert(tr("done.\n"));

  QStringList com;
  com << BINARYDIR "qucsator" << "-i" << "netlist.txt";
  com << "-o" << view->Docs.current()->DataSet;
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
  bool shouldClosed = false;     // should simulation window close after simualtion
  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time

  if(Status == 0) {  // errors ocurred ?
    sim->ProgText->insert(tr("\nSimulation ended on ")+
                          d.toString("ddd dd. MMM yyyy"));
    sim->ProgText->insert(tr(" at ")+t.toString("hh:mm:ss")+"\n");
    sim->ProgText->insert(tr("Ready.\n"));

    if(view->Docs.current()->SimOpenDpl) {
      shouldClosed = true;
      slotChangePage();   // switch to the corresponding data display page
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


  QFile file("log.txt");    // save simulator messages
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

  if(shouldClosed) sim->slotClose();    // close and delete simulation window
}

// ------------------------------------------------------------------------
// Is called to show the output messages of the last simulation.
void QucsApp::slotShowLastMsg()
{
  FileShowDialog *d = new FileShowDialog("log.txt", this);
  d->show();
}

// ------------------------------------------------------------------------
// Is called to show the netlist of the last simulation.
void QucsApp::slotShowLastNetlist()
{
  FileShowDialog *d = new FileShowDialog("netlist.txt", this);
  d->show();
}

// ------------------------------------------------------------------------
// Changes to the corresponding data display page or vice versa.
void QucsApp::slotChangePage()
{
  QucsDoc *Doc = view->Docs.current();
  QString Name = Doc->DataDisplay;
  if(Name.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No page set !"));
    return;
  }
  
  QFileInfo Info;
  QucsDoc *d;

  // search, if page is already loaded
  for(d = view->Docs.first(); d!=0; d = view->Docs.next()) {
    Info.setFile(d->DocName);
    if(Info.fileName() == Name) break;
  }

  if(d == 0) {   // no open page found ?
    Info.setFile(Doc->DocName);
    QFile file(Info.dirPath(true)+"/"+Name);
    if(!file.open(IO_ReadOnly))       // load page
      if(!file.open(IO_ReadWrite)) {  // if it doesn't exist, create
        view->Docs.findRef(Doc);
        QMessageBox::critical(this, tr("Error"),
			tr("Cannot create ")+Info.dirPath(true)+"/"+Name);
        return;
      }
      else new QListViewItem(ConDisplays, Name);  // insert new name
    file.close();

    d = new QucsDoc(WorkView, Info.dirPath(true)+"/"+Name);
    view->Docs.append(d);   // create new page

    if(!d->load()) {
      view->Docs.remove();
      view->Docs.findRef(Doc);
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

// #######################################################################
// Is called when a double click is made in the content ListView.
//void QucsApp::slotOpenContent(QListViewItem *item, const QPoint &, int column)   // QT 3.2
void QucsApp::slotOpenContent(QListViewItem *item)
{
  if(item == 0) return;   // no item was double clicked
  if(item->parent() == 0) return;  // no component, but item "schematic", ...

  gotoPage(QDir::currentDirPath()+"/"+item->text(0));

  if(item->text(1).isEmpty()) return;
  // switch on the 'select' action
  select->blockSignals(true);
  select->setOn(true);
  select->blockSignals(false);
  slotSelect(true);
}

// ########################################################################
// Is called when the open project menu is called.
void QucsApp::slotMenuOpenProject()
{
  QFileDialog *d = new QFileDialog(QDir::homeDirPath());
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
  OpenProject(QDir::homeDirPath()+"/.qucs/"+item->text()+"_prj",
	      item->text());
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
  if(stream.atEnd()) {
    file.close();
    return -2;
  }
  Line = stream.readLine();
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
  view->Docs.append(new QucsDoc(WorkView, ""));   // create 'untitled' file
  view->viewport()->repaint();
  view->drawn = false;


  if(!QDir::setCurrent(Path)) {  // change to project directory
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: ")+Path);
    return;
  }
  QDir ProjDir(".");

  Content->setColumnText(0,tr("Content of '")+Name+tr("'"));  // column text
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
    n = testFile((*it).ascii());
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

  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: ")+Name);  // show name in title of main window
}

// #######################################################################
// Is called, when "Create New Project"-Button is pressed.
void QucsApp::slotProjNewButt()
{
  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir *projDir = new QDir();
  projDir->setPath(QDir::homeDirPath()+"/.qucs");
  if(projDir->mkdir(d->ProjName->text()+"_prj")) {
    Projects->insertItem(d->ProjName->text(),0);  // at first position
    if(d->OpenProj->isChecked()) slotOpenProject(Projects->firstItem());
  }
  else QMessageBox::information(this, tr("Info"),
                    tr("Cannot create project directory !"));
}

// #######################################################################
// Whenever the Component Library ComboBox is changed, this slot fills the Component IconView
// with the appropriat components.
void QucsApp::slotSetCompView(int index)
{
  CompComps->clear();   // clear the IconView
  switch(index) {
    case COMBO_passive:
      new QIconViewItem(CompComps, tr("Resistor"), QImage(BITMAPDIR "resistor.xpm"));
      new QIconViewItem(CompComps, tr("Resistor US"), QImage(BITMAPDIR "resistor_us.xpm"));
      new QIconViewItem(CompComps, tr("Capacitor"), QImage(BITMAPDIR "capacitor.xpm"));
      new QIconViewItem(CompComps, tr("Inductor"), QImage(BITMAPDIR "inductor.xpm"));
      new QIconViewItem(CompComps, tr("Ground"), QImage(BITMAPDIR "ground.xpm"));
      new QIconViewItem(CompComps, tr("Subcircuit Port"), QImage(BITMAPDIR "port.xpm"));
      new QIconViewItem(CompComps, tr("Transformer"), QImage(BITMAPDIR "transformer.xpm"));
      new QIconViewItem(CompComps, tr("symmetric Transformer"), QImage(BITMAPDIR "symtrans.xpm"));
      new QIconViewItem(CompComps, tr("dc Block"), QImage(BITMAPDIR "dcblock.xpm"));
      new QIconViewItem(CompComps, tr("dc Feed"), QImage(BITMAPDIR "dcfeed.xpm"));
      new QIconViewItem(CompComps, tr("Bias T"), QImage(BITMAPDIR "biast.xpm"));
      new QIconViewItem(CompComps, tr("Attenuator"), QImage(BITMAPDIR "attenuator.xpm"));
      new QIconViewItem(CompComps, tr("Isolator"), QImage(BITMAPDIR "isolator.xpm"));
      new QIconViewItem(CompComps, tr("Circulator"), QImage(BITMAPDIR "circulator.xpm"));
      new QIconViewItem(CompComps, tr("Gyrator"), QImage(BITMAPDIR "gyrator.xpm"));
      new QIconViewItem(CompComps, tr("Phase Shifter"), QImage(BITMAPDIR "pshifter.xpm"));
      new QIconViewItem(CompComps, tr("Current Probe"), QImage(BITMAPDIR "iprobe.xpm"));
      break;
    case COMBO_Sources:
      new QIconViewItem(CompComps, tr("dc Voltage Source"), QImage(BITMAPDIR "dc_voltage.xpm"));
      new QIconViewItem(CompComps, tr("dc Current Source"), QImage(BITMAPDIR "dc_current.xpm"));
      new QIconViewItem(CompComps, tr("ac Voltage Source"), QImage(BITMAPDIR "ac_voltage.xpm"));
      new QIconViewItem(CompComps, tr("Power Source"), QImage(BITMAPDIR "source.xpm"));
      new QIconViewItem(CompComps, tr("Noise Voltage Source"), QImage(BITMAPDIR "noise_volt.xpm"));
      new QIconViewItem(CompComps, tr("Noise Current Source"), QImage(BITMAPDIR "noise_current.xpm"));
      new QIconViewItem(CompComps, tr("Voltage Controlled Current Source"), QImage(BITMAPDIR "vccs.xpm"));
      new QIconViewItem(CompComps, tr("Current Controlled Current Source"), QImage(BITMAPDIR "cccs.xpm"));
      new QIconViewItem(CompComps, tr("Voltage Controlled Voltage Source"), QImage(BITMAPDIR "vcvs.xpm"));
      new QIconViewItem(CompComps, tr("Current Controlled Voltage Source"), QImage(BITMAPDIR "ccvs.xpm"));
      break;
    case COMBO_TLines:
      new QIconViewItem(CompComps, tr("Transmission Line"), QImage(BITMAPDIR "tline.xpm"));
      new QIconViewItem(CompComps, tr("Substrate"), QImage(BITMAPDIR "substrate.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Line"), QImage(BITMAPDIR "msline.xpm"));
      new QIconViewItem(CompComps, tr("Coupled Microstrip Line"), QImage(BITMAPDIR "mscoupled.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Step"), QImage(BITMAPDIR "msstep.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Corner"), QImage(BITMAPDIR "mscorner.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Tee"), QImage(BITMAPDIR "mstee.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Cross"), QImage(BITMAPDIR "mscross.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Mitered Bend"), QImage(BITMAPDIR "msmbend.xpm"));
      new QIconViewItem(CompComps, tr("Microstrip Open"), QImage(BITMAPDIR "msopen.xpm"));
      new QIconViewItem(CompComps, tr("Coplanar Line"), QImage(BITMAPDIR "coplanar.xpm"));
      break;
    case COMBO_nonlinear:
      new QIconViewItem(CompComps, tr("Diode"), QImage(BITMAPDIR "diode.xpm"));
      break;
    case COMBO_File:
      new QIconViewItem(CompComps, tr("1-port S parameter file"), QImage(BITMAPDIR "spfile1.xpm"));
      new QIconViewItem(CompComps, tr("2-port S parameter file"), QImage(BITMAPDIR "spfile2.xpm"));
      new QIconViewItem(CompComps, tr("3-port S parameter file"), QImage(BITMAPDIR "spfile3.xpm"));
      new QIconViewItem(CompComps, tr("4-port S parameter file"), QImage(BITMAPDIR "spfile4.xpm"));
      new QIconViewItem(CompComps, tr("5-port S parameter file"), QImage(BITMAPDIR "spfile5.xpm"));
      new QIconViewItem(CompComps, tr("6-port S parameter file"), QImage(BITMAPDIR "spfile6.xpm"));
      break;
    case COMBO_Sims:
      new QIconViewItem(CompComps, tr("dc simulation"), QImage(BITMAPDIR "dc.xpm"));
      new QIconViewItem(CompComps, tr("Transient simulation"), QImage(BITMAPDIR "tran.xpm"));
      new QIconViewItem(CompComps, tr("ac simulation"), QImage(BITMAPDIR "ac.xpm"));
      new QIconViewItem(CompComps, tr("S-parameter simulation"), QImage(BITMAPDIR "sparameter.xpm"));
      new QIconViewItem(CompComps, tr("Harmonic balance"), QImage(BITMAPDIR "hb.xpm"));
      new QIconViewItem(CompComps, tr("Parameter sweep"), QImage(BITMAPDIR "sweep.xpm"));
      break;
    case COMBO_Paints:
      new QIconViewItem(CompComps, tr("Line"), QImage(BITMAPDIR "line.xpm"));
      new QIconViewItem(CompComps, tr("Arrow"), QImage(BITMAPDIR "arrow.xpm"));
      new QIconViewItem(CompComps, tr("Text"), QImage(BITMAPDIR "text.xpm"));
      new QIconViewItem(CompComps, tr("Ellipse"), QImage(BITMAPDIR "ellipse.xpm"));
      new QIconViewItem(CompComps, tr("Rectangle"), QImage(BITMAPDIR "rectangle.xpm"));
      break;
    case COMBO_Diagrams:
      new QIconViewItem(CompComps, tr("Cartesian"), QImage(BITMAPDIR "rect.xpm"));
      new QIconViewItem(CompComps, tr("Polar"), QImage(BITMAPDIR "polar.xpm"));
      new QIconViewItem(CompComps, tr("Tabular"), QImage(BITMAPDIR "tabular.xpm"));
      new QIconViewItem(CompComps, tr("Smith Chart"), QImage(BITMAPDIR "smith.xpm"));
      break;
  }
}

// ----------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QIconViewItem *item)
{
  // delete previously selected elements
  if(view->selComp != 0)  delete view->selComp;
  if(view->selDiag != 0)  delete view->selDiag;
  if(view->selPaint != 0)  delete view->selDiag;
  view->selComp = 0;   // no component selected
  view->selDiag = 0;   // no diagram selected
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

  switch(CompChoose->currentItem()) {
    case COMBO_passive:
          switch(CompComps->index(item)) {
              case 0: view->selComp = new Resistor();    break;
              case 1: view->selComp = new ResistorUS();  break;
              case 2: view->selComp = new Capacitor();   break;
              case 3: view->selComp = new Inductor();    break;
              case 4: view->selComp = new Ground();      break;
              case 5: view->selComp = new SubCirPort();  break;
              case 6: view->selComp = new Transformer(); break;
              case 7: view->selComp = new symTrafo();    break;
              case 8: view->selComp = new dcBlock();     break;
              case 9: view->selComp = new dcFeed();      break;
              case 10: view->selComp = new BiasT();      break;
              case 11: view->selComp = new Attenuator(); break;
              case 12: view->selComp = new Isolator();   break;
              case 13: view->selComp = new Circulator(); break;
              case 14: view->selComp = new Gyrator();    break;
              case 15: view->selComp = new Phaseshifter(); break;
              case 16: view->selComp = new iProbe();     break;
          }
          break;
    case COMBO_Sources:
          switch(CompComps->index(item)) {
              case 0: view->selComp = new Volt_dc();   break;
              case 1: view->selComp = new Ampere_dc(); break;
              case 2: view->selComp = new Volt_ac();   break;
              case 3: view->selComp = new Source_ac();    break;
              case 4: view->selComp = new Volt_noise();   break;
              case 5: view->selComp = new Ampere_noise(); break;
              case 6: view->selComp = new VCCS();  break;
              case 7: view->selComp = new CCCS();  break;
              case 8: view->selComp = new VCVS();  break;
              case 9: view->selComp = new CCVS();  break;
          }
          break;
    case COMBO_TLines:
          switch(CompComps->index(item)) {
              case  0: view->selComp = new TLine();     break;
              case  1: view->selComp = new Substrate(); break;
              case  2: view->selComp = new MSline();    break;
              case  3: view->selComp = new MScoupled(); break;
              case  4: view->selComp = new MSstep();    break;
              case  5: view->selComp = new MScorner();  break;
              case  6: view->selComp = new MStee();     break;
              case  7: view->selComp = new MScross();   break;
              case  8: view->selComp = new MSmbend();   break;
              case  9: view->selComp = new MSopen();    break;
              case 10: view->selComp = new Coplanar();  break;
          }
          break;
    case COMBO_nonlinear:
          switch(CompComps->index(item)) {
              case 0: view->selComp = new Diode();  break;
          }
          break;
    case COMBO_File:
          switch(CompComps->index(item)) {
              case 0: view->selComp = new SParamFile(1);  break;
              case 1: view->selComp = new SParamFile(2);  break;
              case 2: view->selComp = new SParamFile(3);  break;
              case 3: view->selComp = new SParamFile(4);  break;
              case 4: view->selComp = new SParamFile(5);  break;
              case 5: view->selComp = new SParamFile(6);  break;
          }
          break;
    case COMBO_Sims:
          switch(CompComps->index(item)) {
              case 0: view->selComp = new DC_Sim();  break;
              case 1: view->selComp = new TR_Sim();  break;
              case 2: view->selComp = new AC_Sim();  break;
              case 3: view->selComp = new SP_Sim();  break;
              case 4: view->selComp = new HB_Sim();  break;
              case 5: view->selComp = new Param_Sweep();  break;
          }
          break;
    case COMBO_Paints:
          switch(CompComps->index(item)) {
              case 0: view->selPaint = new GraphicLine(); break;
              case 1: view->selPaint = new Arrow();       break;
              case 2: view->selPaint = new GraphicText(); break;
              case 3: view->selPaint = new Ellipse();     break;
              case 4: view->selPaint = new Rectangle();   break;
          }
          if(view->drawn) view->viewport()->repaint();
          view->drawn = false;
          view->MouseMoveAction = &QucsView::MMovePainting;
          view->MousePressAction = &QucsView::MPressPainting;
          view->MouseReleaseAction = &QucsView::MouseDoNothing;
          view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
          return;
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
  int n = testFile(item->text(0));
  if(n<=0) return;    // return, if not a subcircuit

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

  view->selComp = new Subcircuit(n);
  view->selComp->Props.first()->Value = item->text(0);

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
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
  view->MouseMoveAction = &QucsView::MMoveLabel;
  view->MousePressAction = &QucsView::MPressLabel;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
void QucsApp::slotSetMarker(bool on)
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
  activeAction = setMarker;

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveMarker;
  view->MousePressAction = &QucsView::MPressMarker;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// -----------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsApp::slotSelect(bool on)
{
  if(!on) {
    // do not disturb diagram resize
    if(view->MouseMoveAction == &QucsView::MMoveSelect) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(true);        // set back select on
      activeAction->blockSignals(false);
      return;
    }

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
  view->MouseReleaseAction = &QucsView::MReleaseSelect;
  view->MouseDoubleClickAction = &QucsView::MDoubleClickSelect;
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

// -----------------------------------------------------------------------
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

  if(view->Docs.current()->activateComponents()) {
    editActivate->blockSignals(true);
    editActivate->setOn(false);  // release toolbar button
    editActivate->blockSignals(false);
  }
  else {
    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = editActivate;

    // if no component is selected, activate the one that will be clicked on
    view->MouseMoveAction = &QucsView::MMoveActivate;
    view->MousePressAction = &QucsView::MPressActivate;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }

  view->viewport()->repaint();
  view->drawn = false;
}

// -----------------------------------------------------------------------
// Is called if "Wire"-Button is pressed.
void QucsApp::slotSetWire(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    if(view->drawn) view->viewport()->repaint();
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

// ------------------------------------------------------------------------
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

  if(view->Docs.current()->deleteElements()) {
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

    // if no component is selected, delete the one that will be clicked on
    view->MouseMoveAction = &QucsView::MMoveDelete;
    view->MousePressAction = &QucsView::MPressDelete;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// ########################################################################
// Is called when the mouse is clicked upon the equation toolbar button.
void QucsApp::slotInsertEquation(bool on)
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
  activeAction = insEquation;

  if(view->selComp != 0)
    delete view->selComp;  // delete previously selected component

  view->selComp = new Equation();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// #######################################################################
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

// #######################################################################
// Is called when the mouse is clicked upon the port toolbar button.
void QucsApp::slotInsertPort(bool on)
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
  activeAction = insPort;

  if(view->selComp != 0)
    delete view->selComp;  // delete previously selected component

  view->selComp = new SubCirPort();

  if(view->drawn) view->viewport()->repaint();
  view->drawn = false;
  view->MouseMoveAction = &QucsView::MMoveComponent;
  view->MousePressAction = &QucsView::MPressComponent;
  view->MouseReleaseAction = &QucsView::MouseDoNothing;
  view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
}

// ########################################################################
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

  if(view->Docs.current()->rotateElements()) {
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

    view->MouseMoveAction = &QucsView::MMoveRotate;
    view->MousePressAction = &QucsView::MPressRotate;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// ######################################################################
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

    view->MouseMoveAction = &QucsView::MMoveMirrorX;
    view->MousePressAction = &QucsView::MPressMirrorX;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
  view->viewport()->repaint();
  view->drawn = false;
}

// ######################################################################
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorY(bool on)
{
  if(!on) {
    view->MouseMoveAction = &QucsView::MouseDoNothing;
    view->MousePressAction = &QucsView::MouseDoNothing;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
    activeAction = 0;   // no action active
    return;
  }

  if(view->Docs.current()->mirrorYComponents()) {
    editMirrorY->blockSignals(true);
    editMirrorY->setOn(false);  // release toolbar button
    editMirrorY->blockSignals(false);
  }
  else {
    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = editMirrorY;

    view->MouseMoveAction = &QucsView::MMoveMirrorY;
    view->MousePressAction = &QucsView::MPressMirrorY;
    view->MouseReleaseAction = &QucsView::MouseDoNothing;
    view->MouseDoubleClickAction = &QucsView::MouseDoNothing;
  }
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
     tr("This will destroy all the project\nfiles permanently !\nGo on ?"),
     tr("&Yes"), tr("&No"), 0,1,1))  return false;

  QDir *projDir = new QDir();
  projDir->setPath(Path);
  QStringList ProjFiles = projDir->entryList("*", QDir::Files);  // all files

  // removes every file, remove("*") does not work
  QStringList::iterator it;
  for(it = ProjFiles.begin(); it != ProjFiles.end(); it++) {
     if(!projDir->remove(*it)) {
       QMessageBox::information(this, tr("Info"),
				tr("Cannot remove project file: ")+(*it));
       return false;
     }
  }

  projDir->cdUp();  // leave project directory for deleting
  if(!projDir->rmdir(Name+"_prj")) {
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
  QFileDialog *d = new QFileDialog(QDir::homeDirPath());
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

  if(!DeleteProject(QDir::homeDirPath()+"/.qucs/"+item->text()+"_prj",
	item->text()))  return;
  Projects->removeItem(Projects->currentItem());  // remove from project list
}
