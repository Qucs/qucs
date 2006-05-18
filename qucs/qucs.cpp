/***************************************************************************
                                 qucs.cpp
                                ----------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003, 2004, 2005, 2006 by Michael Margraf
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

#include <limits.h>

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
#include <qapplication.h>
#include <qclipboard.h>
#include <qfont.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qaction.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qiconview.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qprinter.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qprocess.h>
#include <qlineedit.h>

#include "main.h"
#include "qucs.h"
#include "qucsdoc.h"
#include "textdoc.h"
#include "schematic.h"
#include "mouseactions.h"
#include "wire.h"
#include "components/components.h"
#include "paintings/paintings.h"
#include "diagrams/diagrams.h"
#include "dialogs/messagebox.h"
#include "dialogs/newprojdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/digisettingsdialog.h"
#include "dialogs/qucssettingsdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/sweepdialog.h"
#include "dialogs/labeldialog.h"
#include "dialogs/matchdialog.h"
#include "dialogs/simmessage.h"

extern const char *empty_xpm[];

QDir QucsWorkDir;  // current project path
QDir QucsHomeDir;  // Qucs user directory where all projects are located


// IconView without dragging icon bitmap
class myIconView : public QIconView
{
public:
  myIconView(QWidget* parent_) : QIconView(parent_, 0, 0) {};
 ~myIconView() {};

protected:
  QDragObject *dragObject() {
    QIconViewItem *Item = currentItem();
    if(!Item) return 0;

    // no picture during dragging, but bounding rectangles in QListView
    QIconDrag *DragPic = new QIconDrag( viewport() );
    DragPic->setPixmap( QPixmap(empty_xpm), QPoint(0, 0) );
    DragPic->append( QIconDragItem(),
        QRect( Item->pixmapRect().width() / -2,
               Item->pixmapRect().height() / -2,
               Item->pixmapRect().width(), Item->pixmapRect().height() ),
        QRect( Item->textRect().width() / -2,
               Item->pixmapRect().height() / 2 + 5,
               Item->textRect().width(), Item->textRect().height() ) );
    return DragPic;
  };
};



QucsApp::QucsApp()
{
  setCaption("Qucs " PACKAGE_VERSION);

  QucsFileFilter =
    tr("Schematic")+" (*.sch);;"+
    tr("Data Display")+" (*.dpl);;"+
    tr("Qucs Documents")+" (*.sch *.dpl);;"+
    tr("VHDL Sources")+" (*.vhdl *.vhd);;"+
    tr("Any File")+" (*)";
  QucsWorkDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  QucsHomeDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));

  move  (QucsSettings.x,  QucsSettings.y);
  resize(QucsSettings.dx, QucsSettings.dy);

  initView();
  initActions();
  initMenuBar();
  initToolBar();
  viewToolBar->setOn(true);
  initStatusBar();
  viewStatusBar->setOn(true);
  initCursorMenu();
  HierarchyHistory.setAutoDelete(true);

  // default settings of the printer
  Printer = new QPrinter(QPrinter::PrinterResolution);
//  Printer->setOrientation(QPrinter::Landscape);
#if defined (QT_VERSION) && QT_VERSION > 0x030200
  Printer->setOptionEnabled(QPrinter::PrintPageRange, false);
#endif
  Printer->setColorMode(QPrinter::Color);
  Printer->setFullPage(true);

  SearchDia = new SearchDialog(this);

  // creates a document called "untitled"
  new Schematic(this, "");

  select->setOn(true);  // switch on the 'select' action
  switchSchematicDoc(true);  // "untitled" document is schematic

  // load documents given as command line arguments
  for(int z=1; z<qApp->argc(); z++)
    if(*(qApp->argv()[z]) != '-')
      gotoPage(qApp->argv()[z]);
}

QucsApp::~QucsApp()
{
  delete Printer;
}


// #######################################################################
// ##########                                                   ##########
// ##########     Creates the working area (QTabWidget etc.)    ##########
// ##########                                                   ##########
// #######################################################################
void QucsApp::initContentListView()
{
  Content->clear();   // remove all documents
  ConOthers     = new QListViewItem(Content, tr("Others"));
  ConDatasets   = new QListViewItem(Content, tr("Datasets"));
  ConDisplays   = new QListViewItem(Content, tr("Data Displays"));
  ConSources    = new QListViewItem(Content, tr("VHDL"));
  ConSchematics = new QListViewItem(Content, tr("Schematics"));
}

void QucsApp::initView()
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));

  QVBox *all = new QVBox(this);   // only to fill the entire view area
  QSplitter *Hsplit = new QSplitter(QSplitter::Horizontal, all);
  TabView     = new QTabWidget(Hsplit);  // tabs on the left side
  DocumentTab = new QTabWidget(Hsplit);  // tab on the right side
  connect(DocumentTab,
          SIGNAL(currentChanged(QWidget*)), SLOT(slotChangeView(QWidget*)));

  Hsplit->setResizeMode(TabView, QSplitter::KeepSize);
  setCentralWidget(all);

  view = new MouseActions();

  editText = new QLineEdit(this);  // for editing component properties
  editText->setFrame(false);
  editText->setHidden(true);
  editText->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(editText, SIGNAL(returnPressed()), SLOT(slotApplyCompText()));
  connect(editText, SIGNAL(textChanged(const QString&)),
          SLOT(slotResizePropEdit(const QString&)));
  connect(editText, SIGNAL(lostFocus()), SLOT(slotHideEdit()));

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
			 tr("content of project directory"));

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

  initContentListView();
  TabView->addTab(Content,tr("Content"));
  TabView->setTabToolTip(TabView->page(1), tr("content of current project"));

  connect(Content, SIGNAL(doubleClicked(QListViewItem*)),
		   SLOT(slotOpenContent(QListViewItem*)));
  connect(Content, SIGNAL(clicked(QListViewItem*)),
		   SLOT(slotSelectSubcircuit(QListViewItem*)));

  // ----------------------------------------------------------
  // "Component Tab" of the left QTabWidget
  QVBox *CompGroup  = new QVBox(this);
  CompChoose = new QComboBox(CompGroup);
  CompComps  = new myIconView(CompGroup);
  TabView->addTab(CompGroup,tr("Components"));
  TabView->setTabToolTip(TabView->page(2), tr("components and diagrams"));
  fillComboBox(true);

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(clicked(QIconViewItem*)),
		     SLOT(slotSelectComponent(QIconViewItem*)));

  // ............................................
  readProjects(); // reads all projects and inserts them into the ListBox
}

// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose number is "No".
// If No < 0 then a pointer to the current document is returned.
QucsDoc* QucsApp::getDoc(int No)
{
  QWidget *w;
  if(No < 0)
    w = DocumentTab->currentPage();
  else
    w = DocumentTab->page(No);

  if(w) {
    if(w->inherits("QTextEdit"))
      return (QucsDoc*) ((TextDoc*)w);
    return (QucsDoc*) ((Schematic*)w);
  }

  return 0;
}

// ####################################################################
// #####  The following arrays contains the elements that appear  #####
// #####  in the component listview.                              #####
// ####################################################################
typedef Element*  (*pInfoFunc) (QString&, char* &, bool);
pInfoFunc lumpedComponents[] =
  {&Resistor::info, &Resistor::info_us, &Capacitor::info, &Inductor::info,
   &Ground::info, &SubCirPort::info, &Transformer::info, &symTrafo::info,
   &dcBlock::info, &dcFeed::info, &BiasT::info, &Attenuator::info,
   &Amplifier::info, &Isolator::info, &Circulator::info,
   &Gyrator::info, &Phaseshifter::info, &Coupler::info, &iProbe::info,
   &vProbe::info, &Mutual::info, &Mutual2::info, &Switch::info,
   &Relais::info, 0};

pInfoFunc Sources[] =
  {&Volt_dc::info, &Ampere_dc::info, &Volt_ac::info, &Ampere_ac::info,
   &Source_ac::info, &Volt_noise::info, &Ampere_noise::info, &VCCS::info,
   &CCCS::info, &VCVS::info, &CCVS::info, &vPulse::info, &iPulse::info,
   &vRect::info, &iRect::info, &Noise_ii::info, &Noise_vv::info,
   &Noise_iv::info, &AM_Modulator::info, &PM_Modulator::info, 0};

pInfoFunc TransmissionLines[] =
  {&TLine::info, &CoaxialLine::info, &Substrate::info, &MSline::info,
   &MScoupled::info, &MScorner::info, &MSmbend::info, &MSstep::info,
   &MStee::info, &MScross::info, &MSopen::info, &MSgap::info, &MSvia::info,
   &Coplanar::info, &CPWopen::info, &CPWshort::info, &CPWgap::info,
   &CPWstep::info, 0};

pInfoFunc nonlinearComps[] =
  {&Diode::info, &BJT::info, &BJT::info_pnp, &BJTsub::info,
   &BJTsub::info_pnp, &JFET::info, &JFET::info_p,
   &MOSFET::info, &MOSFET::info_p, &MOSFET::info_depl,
   &MOSFET_sub::info, &MOSFET_sub::info_p, &MOSFET_sub::info_depl,
   &OpAmp::info, 0};

pInfoFunc digitalComps[] =
  {&Digi_Source::info, &Logical_Inv::info, &Logical_OR::info,
   &Logical_NOR::info, &Logical_AND::info, &Logical_NAND::info,
   &Logical_XOR::info, &Logical_XNOR::info, &RS_FlipFlop::info,
   &D_FlipFlop::info, &JK_FlipFlop::info, &VHDL_File::info,
   &Digi_Sim::info, 0};

pInfoFunc Simulations[] =
  {&DC_Sim::info, &TR_Sim::info, &AC_Sim::info, &SP_Sim::info,
   &HB_Sim::info, &Param_Sweep::info, &Digi_Sim::info, 0};

pInfoFunc FileComponents[] =
  {&SpiceFile::info, &SParamFile::info1, &SParamFile::info2,
   &SParamFile::info, &Subcircuit::info, 0};

pInfoFunc Diagrams[] =
  {&RectDiagram::info, &PolarDiagram::info, &TabDiagram::info,
   &SmithDiagram::info, &SmithDiagram::info_y, &PSDiagram::info,
   &PSDiagram::info_sp, &Rect3DDiagram::info, &CurveDiagram::info,
   &TimingDiagram::info, &TruthDiagram::info, 0};

pInfoFunc Paintings[] =
  {&GraphicLine::info, &Arrow::info, &GraphicText::info,
   &Ellipse::info, &Rectangle::info, &Ellipse::info_filled,
   &Rectangle::info_filled, &EllipseArc::info, 0};

// Order of the component groups in the ComboBox
pInfoFunc *ComponentGroups[] =
  {lumpedComponents, Sources, TransmissionLines, nonlinearComps,
   digitalComps, FileComponents, Simulations, Diagrams, 0};

// ---------------------------------------------------------------
// Put the component groups into the ComboBox. It is possible to
// only put the paintings in it, because of "symbol painting mode".
void QucsApp::fillComboBox(bool setAll)
{
  CompChoose->clear();
  if(setAll) {
    CompChoose->insertItem(tr("lumped components"));
    CompChoose->insertItem(tr("sources"));
    CompChoose->insertItem(tr("transmission lines"));
    CompChoose->insertItem(tr("nonlinear components"));
    CompChoose->insertItem(tr("digital components"));
    CompChoose->insertItem(tr("file components"));
    CompChoose->insertItem(tr("simulations"));
    CompChoose->insertItem(tr("diagrams"));
  }
  CompChoose->insertItem(tr("paintings"));
}

// ----------------------------------------------------------
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriat components.
void QucsApp::slotSetCompView(int index)
{
  editText->setHidden(true); // disable text edit of component property

  char *File;
  QString Name;
  pInfoFunc *Infos = 0;

  CompComps->clear();   // clear the IconView
  if((index+1) >= CompChoose->count())  // because of symbol edit mode
    Infos = &Paintings[0];
  else
    Infos = ComponentGroups[index];

  while(*Infos != 0) {
    (**Infos) (Name, File, false);
    new QIconViewItem(CompComps, Name,
		QImage(QucsSettings.BitmapDir+QString(File)+".png"));
    Infos++;
  }
}

// ------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QIconViewItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem  = 0;   // no component/diagram/painting selected

  if(item == 0) {   // mouse button pressed not over an item ?
    CompComps->clearSelection();  // deselect component in ViewList
    return;
  }

  if(view->drawn)
    ((QScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;


  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  pInfoFunc Infos = 0;
  int i = CompComps->index(item);
  if((CompChoose->currentItem()+1) >= CompChoose->count())
    Infos = Paintings[i];   // the only one in "symbol-painting" mode
  else
    Infos = *(ComponentGroups[CompChoose->currentItem()] + i);

  char *Dummy2;
  QString Dummy1;
  if(Infos)
    view->selElem = (*Infos) (Dummy1, Dummy2, true);
}

// ####################################################################
// #####  Functions for the menu that appears when left-clicking  #####
// #####  on a file in the "Content" ListView.                    #####
// ####################################################################

void QucsApp::initCursorMenu()
{
  ContentMenu = new QPopupMenu(Content);
  ContentMenu->insertItem(tr("Open"), this, SLOT(slotCMenuOpen()));
  ContentMenu->insertItem(tr("Rename"), this, SLOT(slotCMenuRename()));
  ContentMenu->insertItem(tr("Delete"), this, SLOT(slotCMenuDelete()));
  ContentMenu->insertItem(tr("Delete Group"), this, SLOT(slotCMenuDelGroup()));

  connect(Content,
	  SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
	  SLOT(slotShowContentMenu(QListViewItem*, const QPoint&, int)));
}

// ----------------------------------------------------------
// Shows the menu.
void QucsApp::slotShowContentMenu(QListViewItem *item, const QPoint& point, int)
{
  if(item)
    if(item->parent() != 0) {   // no component, but item "schematic", ...
      if(item->parent()->nextSibling()) // "Others" section in listview ?
        ContentMenu->setItemEnabled(ContentMenu->idAt(3), true);
      else
        ContentMenu->setItemEnabled(ContentMenu->idAt(3), false);
      ContentMenu->popup(point);
    }
}

// ----------------------------------------------------------
void QucsApp::slotCMenuOpen()
{
  QListViewItem *Item = Content->selectedItem();
  if(Item == 0) return;

  slotOpenContent(Item);
}

// ----------------------------------------------------------
void QucsApp::slotCMenuRename()
{
  QListViewItem *Item = Content->selectedItem();
  if(!Item) return;

  int No=0;
  QucsDoc *d;  // search, if file is open
  while((d=getDoc(No++)) != 0)
    if(d->DocName == QucsWorkDir.filePath(Item->text(0))) {
      QMessageBox::critical(this, tr("Error"),
				  tr("Cannot rename an open file!"));
      return;
    }

  QString Name   = Item->text(0);
  QString Suffix = Name.section('.',2);   // remember suffix
  if(!Suffix.isEmpty())
    Suffix = '.' + Suffix;

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
  Item->setText(0, s+Suffix);
}

// ----------------------------------------------------------
void QucsApp::slotCMenuDelete()
{
  QListViewItem *item = Content->selectedItem();
  if(item == 0) return;
  QString FileName = QucsWorkDir.filePath(item->text(0));

  int No=0;
  QucsDoc *d;  // search, if file is open
  while((d=getDoc(No++)) != 0)
    if(d->DocName == FileName) {
      QMessageBox::critical(this, tr("Error"),
                   tr("Cannot delete an open file!"));
      return;
   }

  No = QMessageBox::warning(this, tr("Warning"),
             tr("This will delete the file permanently! Continue ?"),
             tr("No"), tr("Yes"));
  if(No != 1) return;

  if(!QFile::remove(FileName)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete schematic: ")+item->text(0));
    return;
  }

  item->parent()->takeItem(item);
  delete item;
}

// ----------------------------------------------------------
// Deletes all files with that name (and suffix sch, dpl, dat, vhdl).
void QucsApp::slotCMenuDelGroup()
{
  QListViewItem *item = Content->selectedItem();
  if(item == 0) return;
  QString s = item->text(0);
  s = s.section('.',0,0);  // cut off suffix from file name

  QString NameSCH = QucsWorkDir.filePath(s+".sch");
  QString NameDPL = QucsWorkDir.filePath(s+".dpl");
  QString NameDAT = QucsWorkDir.filePath(s+".dat");
  QString NameDIG = QucsWorkDir.filePath(s+".vhdl");

  int No=0;
  QucsDoc *d;  // search, if files are open
  while((d=getDoc(No++)) != 0) {
    if(d->DocName == NameSCH) {
      QMessageBox::critical(this, tr("Error"),
                   tr("Cannot delete the open file: ")+NameSCH);
      return;
    }
    if(d->DocName == NameDPL) {
      QMessageBox::critical(this, tr("Error"),
                   tr("Cannot delete the open file: ")+NameDPL);
      return;
    }
    if(d->DocName == NameDIG) {
      QMessageBox::critical(this, tr("Error"),
                   tr("Cannot delete the open file: ")+NameDIG);
      return;
    }
  }


  bool SCH_exists = QFile::exists(NameSCH);
  bool DPL_exists = QFile::exists(NameDPL);
  bool DAT_exists = QFile::exists(NameDAT);
  bool DIG_exists = QFile::exists(NameDIG);

  QString Str;
  if(SCH_exists)  Str += s+".sch\n";
  if(DPL_exists)  Str += s+".dpl\n";
  if(DAT_exists)  Str += s+".dat\n";
  if(DIG_exists)  Str += s+".vhdl\n";

  No = QMessageBox::warning(this, tr("Warning"),
	tr("This will delete the files\n")+Str+tr("permanently! Continue ?"),
	tr("No"), tr("Yes"));
  if(No != 1) return;

  // remove files .................
  if(SCH_exists)
    if(!QFile::remove(NameSCH)) {
      QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete schematic: ")+s+".sch");
      return;
    }
  if(DPL_exists)
    if(!QFile::remove(NameDPL)) {
      QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete data display: ")+s+".dpl");
      return;
    }
  if(DAT_exists)
    if(!QFile::remove(NameDAT)) {
      QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete data file: ")+s+".dat");
      return;
    }
  if(DIG_exists)
    if(!QFile::remove(NameDIG)) {
      QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete VHDL source: ")+s+".vhdl");
      return;
    }

  // remove items from listview ........
  if(SCH_exists) {
    item = Content->findItem(s+".sch", 0);
    if(item) {
      item->parent()->takeItem(item);
      delete item;
    }
  }
  if(DPL_exists) {
    item = Content->findItem(s+".dpl", 0);
    if(item) {
      item->parent()->takeItem(item);
      delete item;
    }
  }
  if(DAT_exists) {
    item = Content->findItem(s+".dat", 0);
    if(item) {
      item->parent()->takeItem(item);
      delete item;
    }
  }
  if(DIG_exists) {
    item = Content->findItem(s+".vhdl", 0);
    if(item) {
      item->parent()->takeItem(item);
      delete item;
    }
  }
}


// ################################################################
// #####    Functions that handle the project operations.     #####
// ################################################################

// Checks for qucs directory and reads all existing Qucs projects.
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
     if ((*it).right(4) == "_prj") {  // project directories end with "_prj"
       (*it) = (*it).left((*it).length()-4); // remove "_prj" from name
       Projects->insertItem(*it);
     }
}

// ----------------------------------------------------------
// Is called, when "Create New Project" button is pressed.
void QucsApp::slotProjNewButt()
{
  editText->setHidden(true); // disable text edit of component property

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

// ----------------------------------------------------------
// Checks whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int QucsApp::testFile(const QString& DocName)
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    return -1;
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();
  file.close();
  QTextStream stream(&FileString, IO_ReadOnly);


  // read header ........................
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

  // read content ....................
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

// ----------------------------------------------------------
// Reads all files in the project directory and sort them into the
// content ListView
void QucsApp::readProjectFiles()
{
  initContentListView();

  int n;
  // put all files into "Content"-ListView
  QStringList Elements = QucsWorkDir.entryList("*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  QString Str;
  for(it = Elements.begin(); it != Elements.end(); ++it) {
    Str = (*it).section('.',1);
    if(Str == "sch") {
      n = testFile(QucsWorkDir.filePath((*it).ascii()));
      if(n >= 0) {
        if(n > 0)
          new QListViewItem(ConSchematics, (*it).ascii(),
				QString::number(n)+tr("-port"));
        else new QListViewItem(ConSchematics, (*it).ascii());
      }
    }
    else if(Str == "dpl")
      new QListViewItem(ConDisplays, (*it).ascii());
    else if(Str == "dat")
      new QListViewItem(ConDatasets, (*it).ascii());
    else if(Str == "vhdl")
      new QListViewItem(ConSources, (*it).ascii());
    else
      new QListViewItem(ConOthers, (*it).ascii());
  }

  Content->firstChild()->setOpen(true);  // show schematics
}

// ----------------------------------------------------------
// Opens an existing project.
void QucsApp::OpenProject(const QString& Path, const QString& Name)
{
  editText->setHidden(true); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  new Schematic(this, "");
  view->drawn = false;

  slotResetWarnings();

  QDir ProjDir(QDir::cleanDirPath(Path));
  if(!ProjDir.exists() || !ProjDir.isReadable()) { // check project directory
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: ")+Path);
    return;
  }
  QucsWorkDir.setPath(ProjDir.path());

  Content->setColumnText(0,tr("Content of '")+Name+tr("'")); // column text
  readProjectFiles();

  TabView->setCurrentPage(1);   // switch to "Content"-Tab
  ProjName = Name;   // remember the name of project

  // show name in title of main window
  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: ")+Name);
}

// ----------------------------------------------------------
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

// ----------------------------------------------------------
// Is called, when "Open Project" button is pressed.
void QucsApp::slotProjOpenButt()
{
  editText->setHidden(true); // disable text edit of component property

  QListBoxItem *item = Projects->selectedItem();
  if(item) slotOpenProject(item);
  else QMessageBox::information(this, tr("Info"),
				tr("No project is selected !"));
}

// ----------------------------------------------------------
// Is called when project is double-clicked to open it.
void QucsApp::slotOpenProject(QListBoxItem *item)
{
  OpenProject(QucsHomeDir.filePath(item->text()+"_prj"), item->text());
}

// ----------------------------------------------------------
// Is called when the close project menu is called.
void QucsApp::slotMenuCloseProject()
{
  editText->setHidden(true); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  new Schematic(this, "");
  view->drawn = false;

  slotResetWarnings();
  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: "));
  QucsWorkDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));

  Content->setColumnText(0,tr("Content of")); // column text
  initContentListView();

  TabView->setCurrentPage(0);   // switch to "Projects"-Tab
  ProjName = "";
}

// ----------------------------------------------------------
bool QucsApp::DeleteProject(const QString& Path, const QString& Name)
{
  editText->setHidden(true); // disable text edit of component property

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

// ----------------------------------------------------------
// Is called, when "Delete Project" menu is activated.
void QucsApp::slotMenuDelProject()
{
  QFileDialog *d = new QFileDialog(QucsHomeDir.path());
  d->setCaption(tr("Choose Project Directory for Deleting"));
  d->setShowHiddenFiles(true);
  d->setMode(QFileDialog::DirectoryOnly);
  if(d->exec() != QDialog::Accepted) return;

  QString s = d->selectedFile();
  if(s.isEmpty()) return;

  s = s.left(s.length()-1);  // cut off trailing '/'
  int i = s.findRev('/');
  if(i > 0) s = s.mid(i+1);  // cut out the last subdirectory
  s = s.left(s.length()-4);  // remove "_prj" from name
  DeleteProject(d->selectedFile(), s);
  readProjects();   // re-reads all projects and inserts them into the ListBox
}

// ----------------------------------------------------------
// Is called, when "Delete Project" button is pressed.
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


// ################################################################
// #####  Functions that handle the file operations for the   #####
// #####  documents.                                          #####
// ################################################################

void QucsApp::slotFileNew()
{
  statusBar()->message(tr("Creating new schematic..."));
  editText->setHidden(true); // disable text edit of component property

  new Schematic(this, "");

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotTextNew()
{
  statusBar()->message(tr("Creating new text editor..."));
  editText->setHidden(true); // disable text edit of component property

  new TextDoc(this, "");

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
// Changes to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
bool QucsApp::gotoPage(const QString& Name)
{
  int No = DocumentTab->currentPageIndex();

  int i=0;
  QucsDoc *d;  // search, if page is already loaded
  while((d=getDoc(i++)) != 0)
    if(QDir::convertSeparators (d->DocName) == QDir::convertSeparators (Name))
      break;

  if(d) {   // open page found ?
    d->becomeCurrent(true);
    DocumentTab->setCurrentPage(i-1);  // make new document the current
    return true;
  }

  QFileInfo Info(Name);
  if((Info.extension(false) == "sch") || (Info.extension(false) == "dpl"))
    d = new Schematic(this, Name);
  else
    d = new TextDoc(this, Name);

  if(!d->load()) {    // load document if possible
    delete d;
    DocumentTab->setCurrentPage(No);
    view->drawn = false;
    return false;
  }
  slotChangeView(DocumentTab->currentPage());

  // if only an untitled document was open -> close it
  if(DocumentTab->count() == 2)
    if(getDoc(0)->DocName.isEmpty())
      if(!getDoc(0)->DocChanged)
        delete DocumentTab->page(0);

  view->drawn = false;
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  static QString lastDir;  // to remember last directory and file
  editText->setHidden(true); // disable text edit of component property

  statusBar()->message(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(
	lastDir.isEmpty() ? QString(".") : lastDir,
	QucsFileFilter, this, 0, tr("Enter a Schematic Name"));

  if(s.isEmpty())
    statusBar()->message(tr("Opening aborted"), 2000);
  else {
    gotoPage(s);
    lastDir = s;   // remember last directory and file
    statusBar()->message(tr("Ready."));
  }
}

// --------------------------------------------------------------
bool QucsApp::saveFile(QucsDoc *Doc)
{
  if(!Doc)
    Doc = getDoc();

  if(Doc->DocName.isEmpty())
    return saveAs();

  int Result = Doc->save();
  if(Result < 0)  return false;

  updatePortNumber(Doc, Result);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSave()
{
  statusBar()->message(tr("Saving file..."));
  DocumentTab->blockSignals(true);   // no user interaction during that time
  editText->setHidden(true); // disable text edit of component property

  if(!saveFile()) {
    DocumentTab->blockSignals(false);
    statusBar()->message(tr("Saving aborted"), 2000);
    statusBar()->message(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
bool QucsApp::saveAs()
{
  static QString lastDir;  // to remember last directory and file
  QWidget *w = DocumentTab->currentPage();
  QucsDoc *Doc = getDoc();

  int n = -1;
  bool intoView = true;
  QString s, Filter;
  QFileInfo Info;
  while(true) {
    intoView = true;
    s = Doc->DocName;
    Info.setFile(s);
    if(s.isEmpty()) {   // which is default directory ?
      if(ProjName.isEmpty()) {
        if(lastDir.isEmpty())  s = QDir::currentDirPath();
        else  s = lastDir;
      }
      else s = QucsWorkDir.path();
    }

    if(w->inherits("QTextEdit"))
      Filter = tr("VHDL Sources")+" (*.vhdl *.vhd);;" + tr("Any File")+" (*)";
    else
      Filter = QucsFileFilter;
    s = QFileDialog::getSaveFileName(s, Filter,
                     this, "", tr("Enter a Document Name"));
    if(s.isEmpty())  return false;
    Info.setFile(s);                 // try to guess the best extension ...
    if(Info.extension(false).isEmpty()) {  // ... if no one was specified
      if(w->inherits("QTextEdit"))
        s += ".vhdl";
      else
        s += ".sch";
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

    // search, if document is open
    int No=0;
    QucsDoc *d;
    while((d=getDoc(No++)) != 0)
      if(d->DocName == s) break;
    if(d) {
      QMessageBox::information(this, tr("Info"),
		tr("Cannot overwrite an open document"));
      return false;
    }

    break;
  }
  Doc->setName(s);
  lastDir = Info.dirPath(true);  // remember last directory and file

  if(intoView) {    // insert new name in Content ListView ?
    if(Info.dirPath(true) == QucsWorkDir.absPath())
      if(!ProjName.isEmpty()) {
        s = Info.fileName();  // remove path from file name
        if(Info.extension(false) == "sch")
          Content->setSelected(new QListViewItem(ConSchematics, s), true);
        else if(Info.extension(false) == "dpl")
          Content->setSelected(new QListViewItem(ConDisplays, s), true);
        else if(Info.extension(false) == "dat")
          Content->setSelected(new QListViewItem(ConDatasets, s), true);
        else if(Info.extension(false) == "vhdl")
          Content->setSelected(new QListViewItem(ConSources, s), true);
        else
          Content->setSelected(new QListViewItem(ConOthers, s), true);
      }
  }

  n = Doc->save();   // SAVE
  if(n < 0)  return false;

  updatePortNumber(Doc, n);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSaveAs()
{
  statusBar()->message(tr("Saving file under new filename..."));
  DocumentTab->blockSignals(true);   // no user interaction during the time
  editText->setHidden(true); // disable text edit of component property

  if(!saveAs()) {
    DocumentTab->blockSignals(false);
    statusBar()->message(tr("Saving aborted"), 3000);
    statusBar()->message(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->message(tr("Ready."));
}


// --------------------------------------------------------------
void QucsApp::slotFileSaveAll()
{
  statusBar()->message(tr("Saving all files..."));
  editText->setHidden(true); // disable text edit of component property
  DocumentTab->blockSignals(true);   // no user interaction during the time

  int No=0;
  QucsDoc *Doc;  // search, if page is already loaded
  while((Doc=getDoc(No++)) != 0) {
    if(Doc->DocName.isEmpty())  // make document the current ?
      DocumentTab->setCurrentPage(No-1);
    saveFile(Doc);
  }

  DocumentTab->blockSignals(false);
  // Call update() to update subcircuit symbols in current document.
  ((QScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;
  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotFileClose()
{
  statusBar()->message(tr("Closing file..."));
  editText->setHidden(true); // disable text edit of component property

  QucsDoc *Doc = getDoc();
  if(Doc->DocChanged) {
    switch(QMessageBox::warning(this,tr("Closing Qucs document"),
      tr("The document contains unsaved changes!\n")+
      tr("Do you want to save the changes before closing?"),
      tr("&Save"), tr("&Discard"), tr("Cancel"), 0, 2)) {
      case 0 : slotFileSave();
               break;
      case 2 : return;
    }
  }

  delete Doc;

  if(DocumentTab->count() < 1)  // if no document left, create an untitled
    new Schematic(this, "");

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
bool QucsApp::closeAllFiles()
{
  int  Result = 0;
  bool notForAll = true;
  CloseMessageBox *m = new CloseMessageBox(tr("Closing Qucs document"),
	tr("This document contains unsaved changes!\n"
	   "Do you want to save the changes before closing?"),this);

  // close all files and ask to save changed ones
  QucsDoc *Doc;
  DocumentTab->setCurrentPage(0);
  while ((Doc=getDoc()) != 0) {
    if (Doc->DocChanged) {
      if(notForAll)  Result = m->exec();
      switch(Result) {
	case 1: if(!saveFile(Doc))  return false;  // save button
		break;
	case 2: Result = 1;         // save all button
		notForAll = false;
		if(!saveFile(Doc))  return false;
		break;
	case 4: Result = 3;         // discard all button
		notForAll = false;
		break;
	case 5: return false;       // cancel button
      }
    }

    delete Doc;
  }

  delete m;
  switchEditMode(true);   // set schematic edit mode
  return true;
}

// --------------------------------------------------------------
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(QWidget *w)
{
  editText->setHidden(true); // disable text edit of component property

  QucsDoc *Doc;
  if(w->inherits("QTextEdit")) {
    TextDoc *d = (TextDoc*)w;
    Doc = (QucsDoc*)d;

    if(mainAccel->isEnabled())
      switchSchematicDoc(false);
  }
  else {
    Schematic *d = (Schematic*)w;
    Doc = (QucsDoc*)d;

    if(mainAccel->isEnabled()) {
      // which mode: schematic or symbol editor ?
      if((CompChoose->count() > 1) == d->symbolMode)
        changeSchematicSymbolMode(d);
    }
    else {
      switchSchematicDoc(true);
      changeSchematicSymbolMode(d);
    }
  }

  Doc->becomeCurrent(true);
  view->drawn = false;

  if(!HierarchyHistory.isEmpty())
    if(*(HierarchyHistory.getLast()) != "*") {
      HierarchyHistory.clear();   // no subcircuit history anymore
      popH->setEnabled(false);
    }
}

// --------------------------------------------------------------
// Changes to the next document in the TabBar.
void QucsApp::slotNextTab()
{
  int No = DocumentTab->currentPageIndex() + 1;
  if(No >= DocumentTab->count())
    No = 0;

  // make new document the current (calls "slotChangeView(int)" indirectly)
  DocumentTab->setCurrentPage(No);
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotFileSettings()
{
  editText->setHidden(true); // disable text edit of component property

  QWidget *w = DocumentTab->currentPage();
  if(w->inherits("QTextEdit")) {
    DigiSettingsDialog *d = new DigiSettingsDialog((TextDoc*)w);
    d->exec();
  }
  else {
    SettingsDialog *d = new SettingsDialog((Schematic*)w);
    d->exec();
  }
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotApplSettings()
{
  editText->setHidden(true); // disable text edit of component property

  QucsSettingsDialog *d = new QucsSettingsDialog(this);
  d->exec();
  view->drawn = false;
}


// --------------------------------------------------------------
void QucsApp::updatePortNumber(QucsDoc *currDoc, int No)
{
  if(No<0) return;

  QString pathName = currDoc->DocName;
  QFileInfo Info(pathName);
  QString Model, File, Name = Info.fileName();

  if(Info.extension() == "sch") {
    Model = "Sub";

    // enter new port number into ListView
    QListViewItem *p;
    for(p = ConSchematics->firstChild(); p!=0; p = p->nextSibling()) {
      if(p->text(0) == Name) {
        if(No == 0) p->setText(1,"");
        else p->setText(1,QString::number(No)+tr("-port"));
        break;
      }
    }
  }
  else
    Model = "VHDL";


  // update all occurencies of subcircuit in all open documents
  No = 0;
  QWidget *w;
  Component *pc_tmp;
  while((w=DocumentTab->page(No++)) != 0) {
    if(w->inherits("QTextEdit"))  continue;

    // start from the last to omit re-appended components
    Schematic *Doc = (Schematic*)w;
    for(Component *pc=Doc->Components->last(); pc!=0; ) {
      if(pc->Model == Model) {
        File = pc->Props.getFirst()->Value;
        if((File == pathName) || (File == Name)) {
          pc_tmp = Doc->Components->prev();
          Doc->recreateComponent(pc);  // delete and re-append component
          if(!pc_tmp)  break;
          Doc->Components->findRef(pc_tmp);
          pc = Doc->Components->current();
          continue;
        }
      }
      pc = Doc->Components->prev();
    }
  }
}


// --------------------------------------------------------------
void QucsApp::slotFilePrint()
{
  statusBar()->message(tr("Printing..."));
  editText->setHidden(true); // disable text edit of component property

  if(Printer->setup(this))   // printer dialog
    getDoc()->print(Printer, true);

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotFilePrintSelected()
{
  statusBar()->message(tr("Printing selected..."));
  editText->setHidden(true); // disable text edit of component property

  if(Printer->setup(this))   // printer dialog
    getDoc()->print(Printer, false);

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
// Exits the application.
void QucsApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));
  editText->setHidden(true); // disable text edit of component property

  int exit = QMessageBox::information(this,
      tr("Quit..."), tr("Do you really want to quit?"),
      tr("Yes"), tr("No"));

  if(exit == 0)
    if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
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
    emit signalKillEmAll();   // kill all subprocesses
    qApp->quit();
  }
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->cut();
    return;
  }

  editText->setHidden(true); // disable text edit of component property
  QClipboard *cb = QApplication::clipboard();  // get system clipboard

  QString s = Doc->copySelected(true);
  if(!s.isEmpty()) {
    cb->setText(s, QClipboard::Clipboard);
    Doc->viewport()->update();
  }

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
void QucsApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->copy();
    return;
  }
  
  editText->setHidden(true); // disable text edit of component property
  QClipboard *cb = QApplication::clipboard();  // get system clipboard

  QString s = Doc->copySelected(false);
  if(!s.isEmpty())
    cb->setText(s, QClipboard::Clipboard);

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  Component *pc = Doc->searchSelSubcircuit();
  if(pc == 0) return;

  QString *ps = new QString("*");
  HierarchyHistory.append(ps);    // sign not to clear HierarchyHistory

  QString s = QucsWorkDir.filePath(pc->Props.getFirst()->Value);
  if(!gotoPage(s)) {
    HierarchyHistory.remove();
    return;
  }

  *(HierarchyHistory.getLast()) = Doc->DocName; // remember for the way back
  popH->setEnabled(true);
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{
  editText->setHidden(true); // disable text edit of component property

  if(HierarchyHistory.count() == 0) return;

  QString Doc = *(HierarchyHistory.getLast());
  *(HierarchyHistory.last()) = "*";  // sign not to clear HierarchyHistory

  if(!gotoPage(Doc)) {
    *(HierarchyHistory.getLast()) = Doc;
    return;
  }

  HierarchyHistory.remove();
  if(HierarchyHistory.count() == 0)
    popH->setEnabled(false);
}

// --------------------------------------------------------------
void QucsApp::slotShowAll()
{
  editText->setHidden(true); // disable text edit of component property
  getDoc()->showAll();
}

// -----------------------------------------------------------
// Sets the scale factor to 1.
void QucsApp::slotShowOne()
{
  editText->setHidden(true); // disable text edit of component property
  getDoc()->showNoZoom();
}

// -----------------------------------------------------------
void QucsApp::slotZoomOut()
{
  editText->setHidden(true); // disable text edit of component property
  getDoc()->zoom(0.7f);
}

// -----------------------------------------------------------------------
// Is called when the simulate toolbar button is pressed.
void QucsApp::slotSimulate()
{
  editText->setHidden(true); // disable text edit of component property

  QucsDoc *Doc;
  QWidget *w = DocumentTab->currentPage();
  if(w->inherits("QTextEdit")) {
    Doc = (QucsDoc*)((TextDoc*)w);
    if(Doc->SimTime.isEmpty()) {
      DigiSettingsDialog *d = new DigiSettingsDialog((TextDoc*)Doc);
      if(d->exec() == QDialog::Rejected)
        return;
    }
  }
  else
    Doc = (QucsDoc*)((Schematic*)w);

  if(Doc->DocName.isEmpty()) // if document 'untitled' ...
    if(!saveAs()) return;    // ... save schematic before

  // Perhaps the document was modified from another program ?
  if(Doc->lastSaved.isValid()) {
    QFileInfo Info(Doc->DocName);
    if(Doc->lastSaved < Info.lastModified()) {
      int No = QMessageBox::warning(this, tr("Warning"),
               tr("The document was modified by another program !") + '\n' +
               tr("Do you want to reload or keep this version ?"),
               tr("Reload"), tr("Keep it"));
      if(No == 0)
        Doc->load();
    }
  }

  slotResetWarnings();
  SimMessage *sim = new SimMessage(w, this);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed !
  connect(sim, SIGNAL(SimulationEnded(int, SimMessage*)), this,
		SLOT(slotAfterSimulation(int, SimMessage*)));
  connect(sim, SIGNAL(displayDataPage(QString&, QString&)),
		this, SLOT(slotChangePage(QString&, QString&)));

  sim->show();
  if(!sim->startProcess()) return;

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), sim, SLOT(slotClose()));
}

// ------------------------------------------------------------------------
// Is called after the simulation process terminates.
void QucsApp::slotAfterSimulation(int Status, SimMessage *sim)
{
  if(Status != 0) return;  // errors ocurred ?

  if(sim->ErrText->lines() > 1)   // were there warnings ?
    slotShowWarnings();

  int i=0;
  QWidget *w;  // search, if page is still open
  while((w=DocumentTab->page(i++)) != 0)
    if(w == sim->DocWidget)
      break;

  if(sim->showBias == 0) {  // paint dc bias into schematic ?
    sim->slotClose();   // close and delete simulation window
    if(w) {  // schematic still open ?
      SweepDialog *Dia = new SweepDialog((Schematic*)sim->DocWidget);
      Dia->show();
    }
  }
  else if(sim->SimOpenDpl) {
    // switch to data display
    slotChangePage(sim->DocName, sim->DataDisplay);
    sim->slotClose();   // close and delete simulation window
  }
  else
    if(w) if(!sim->DocWidget->inherits("QTextEdit"))
      // load recent simulation data (if document is still open)
      ((Schematic*)sim->DocWidget)->reloadGraphs();

  if(!sim->DocWidget->inherits("QTextEdit"))
    ((Schematic*)sim->DocWidget)->viewport()->update();

  // put all dataset files into "Content"-ListView (update)
/*  QStringList Elements = ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
  for(it = Elements.begin(); it != Elements.end(); ++it)
    new QListViewItem(ConDatasets, (*it).ascii());*/
}

// ------------------------------------------------------------------------
void QucsApp::slotDCbias()
{
  getDoc()->showBias = 0;
  slotSimulate();
}

// ------------------------------------------------------------------------
// Changes to the corresponding data display page or vice versa.
void QucsApp::slotChangePage(QString& DocName, QString& DataDisplay)
{
  if(DataDisplay.isEmpty())  return;

  QFileInfo Info(DocName);
  QString Name = Info.dirPath() + QDir::separator() + DataDisplay;

  QWidget  *w = DocumentTab->currentPage();

  int z=0;  // search, if page is already loaded
  QucsDoc  *d;
  while((d=getDoc(z++)) != 0)
    if(QDir::convertSeparators (d->DocName) == QDir::convertSeparators (Name))
      break;

  if(d)
    DocumentTab->setCurrentPage(z-1);
  else {   // no open page found ?
    if(DataDisplay.section('.',1) != "vhdl")
      d = new Schematic(this, Name);
    else
      d = new TextDoc(this, Name);

    QFile file(Name);
    if(file.open(IO_ReadOnly)) {      // try to load document
      file.close();
      if(!d->load()) {
        delete d;
        view->drawn = false;
        return;
      }
    }
    else {
      if(file.open(IO_ReadWrite)) {  // if document doesn't exist, create
        new QListViewItem(ConDisplays, DataDisplay); // add new name
        d->DataDisplay = Info.fileName();
      }
      else {
        QMessageBox::critical(this, tr("Error"), tr("Cannot create ")+Name);
        return;
      }
      file.close();
    }

    d->becomeCurrent(true);
  }


  if(DocumentTab->currentPage() == w)      // if page not ...
    if(!w->inherits("QTextEdit"))
      ((Schematic*)w)->reloadGraphs();  // ... changes, reload here !

  TabView->setCurrentPage(2);   // switch to "Component"-Tab
  if(Name.right(4) == ".dpl") {
    int i = sizeof(ComponentGroups)/sizeof(pInfoFunc) - 2;
    CompChoose->setCurrentItem(i);   // switch to diagrams
    slotSetCompView(i);
  }
}

// -------------------------------------------------------------------
// Changes to the data display of current page.
void QucsApp::slotToPage()
{
  QucsDoc *d = getDoc();
  if(d->DataDisplay.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No page set !"));
    return;
  }

  slotChangePage(d->DocName, d->DataDisplay);
}

// -------------------------------------------------------------------
// Is called when a double-click is made in the content ListView.
void QucsApp::slotOpenContent(QListViewItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  if(item == 0) return;   // no item was double clicked
  if(item->parent() == 0) return; // no document, but item "schematic", ...

/*  
  QucsWorkDir.setPath(QucsHomeDir.path());
  QString p = ProjName+"_prj";
  if(!QucsWorkDir.cd(p)) {
    QMessageBox::critical(this, tr("Error"),
			  tr("Cannot access project directory: ")+
			  QucsWorkDir.path()+QDir::separator()+p);
    return;
  }*/

  QFileInfo Info(QucsWorkDir.filePath(item->text(0)));
  QString Suffix = Info.extension(false);

  if((Suffix == "sch") || (Suffix == "dpl") || (Suffix == "vhdl")) {
    gotoPage(Info.absFilePath());

    if(item->text(1).isEmpty())     // is subcircuit ?
      if(Suffix != "vhdl") return;  // is VHDL subcircuit ?

    select->blockSignals(true);  // switch on the 'select' action ...
    select->setOn(true);
    select->blockSignals(false);

    activeAction = select;
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    return;
  }

  if(Suffix == "dat") {
    editFile(Info.absFilePath());  // open datasets with text editor
    return;
  }


  // File is no Qucs file, so go through list and search a user
  // defined program to open it.
  QStringList com;
  QStringList::Iterator it = QucsSettings.FileTypes.begin();
  while(it != QucsSettings.FileTypes.end()) {
    if(Suffix == (*it).section('/',0,0)) {
      com = QStringList::split(" ", (*it).section('/',1,1));
      com << Info.absFilePath();
      QProcess *Program = new QProcess(com);
      Program->setCommunication(0);
      if(!Program->start()) {
        QMessageBox::critical(this, tr("Error"),
               tr("Cannot start \"%1\"!").arg(Info.absFilePath()));
        delete Program;
      }
      return;
    }
    it++;
  }

  // If no appropriate program was found, open as text file.
  editFile(Info.absFilePath());  // open datasets with text editor
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectSubcircuit(QListViewItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  if(item == 0) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    return;
  }


  bool isVHDL = true;
  if(item->parent() == 0) return;
  if(item->parent()->text(0) == tr("Schematics")) {
    if(item->text(1).isEmpty())
      return;   // return, if not a subcircuit
    isVHDL = false;
  }
  else if(item->parent()->text(0) != tr("VHDL"))
    return;

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem = 0;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  Component *Comp;
  if(isVHDL)
    Comp = new VHDL_File();
  else
    Comp = new Subcircuit();
  Comp->Props.first()->Value = item->text(0);
  Comp->recreate(0);
  view->selElem = Comp;

  if(view->drawn)
    ((QScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
}

// ---------------------------------------------------------
// This function is called if the document type changes, i.e.
// from schematic to text document or vice versa.
void QucsApp::switchSchematicDoc(bool SchematicMode)
{
  mainAccel->setEnabled(SchematicMode);

  if(!SchematicMode) {
    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = select;
    select->blockSignals(true);
    select->setOn(true);
    select->blockSignals(false);
  }
  else {
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }

  symEdit->setEnabled(SchematicMode);
  alignTop->setEnabled(SchematicMode);
  alignBottom->setEnabled(SchematicMode);
  alignLeft->setEnabled(SchematicMode);
  alignRight->setEnabled(SchematicMode);
  distrHor->setEnabled(SchematicMode);
  distrVert->setEnabled(SchematicMode);
  onGrid->setEnabled(SchematicMode);
  moveText->setEnabled(SchematicMode);
  editFind->setEnabled(!SchematicMode);
  editFindAgain->setEnabled(!SchematicMode);
  editRotate->setEnabled(SchematicMode);
  editMirror->setEnabled(SchematicMode);
  editMirrorY->setEnabled(SchematicMode);
  intoH->setEnabled(SchematicMode);
  dcbias->setEnabled(SchematicMode);
  insWire->setEnabled(SchematicMode);
  insLabel->setEnabled(SchematicMode);
  insPort->setEnabled(SchematicMode);
  insGround->setEnabled(SchematicMode);
  insEquation->setEnabled(SchematicMode);
  insEntity->setEnabled(!SchematicMode);
  setMarker->setEnabled(SchematicMode);
}

// ---------------------------------------------------------
void QucsApp::switchEditMode(bool SchematicMode)
{
  if(SchematicMode) {
    symEdit->setMenuText(tr("Edit Circuit Symbol"));
    symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
    symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  }
  else {
    symEdit->setMenuText(tr("Edit Schematic"));
    symEdit->setStatusTip(tr("Edits the schematic"));
    symEdit->setWhatsThis(tr("Edit Schematic\n\nEdits the schematic"));
  }


  fillComboBox(SchematicMode);
  slotSetCompView(0);

  editActivate->setEnabled(SchematicMode);
  changeProps->setEnabled(SchematicMode);
  insEquation->setEnabled(SchematicMode);
  insGround->setEnabled(SchematicMode);
  insPort->setEnabled(SchematicMode);
  insWire->setEnabled(SchematicMode);
  insLabel->setEnabled(SchematicMode);
  setMarker->setEnabled(SchematicMode);
  simulate->setEnabled(SchematicMode);
}

// ---------------------------------------------------------
void QucsApp::changeSchematicSymbolMode(Schematic *Doc)
{
  if(Doc->symbolMode) {
    // go into select modus to avoid placing a forbidden element
    select->setOn(true);

    switchEditMode(false);
  }
  else
    switchEditMode(true);
}

// ---------------------------------------------------------
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: Schematic and (subcircuit)
// symbol.
void QucsApp::slotSymbolEdit()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  Doc->switchPaintMode();   // twist the view coordinates
  changeSchematicSymbolMode(Doc);
  Doc->becomeCurrent(true);

  // This can only be true when switching to the symbol the first time.
  if(Doc->UndoSymbol.isEmpty()) {
    Doc->setChanged(false, true); // "not changed" state, but put on undo stack
    Doc->UndoSymbol.current()->at(1) = 'i';  // state of being unchanged
  }

  Doc->viewport()->update();
  view->drawn = false;
}

// -----------------------------------------------------------
void QucsApp::slotPowerMatching()
{
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

//  double Z0 = 50.0;
  QString Var = pm->pGraph->Var;
  double Imag = pm->VarPos[pm->nVarPos+1];
  if(Var == "Sopt")  // noise matching ?
    Imag *= -1.0;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->TwoCheck->setChecked(false);
  Dia->TwoCheck->setEnabled(false);
//  Dia->Ref1Edit->setText(QString::number(Z0));
  Dia->S11magEdit->setText(QString::number(pm->VarPos[pm->nVarPos]));
  Dia->S11degEdit->setText(QString::number(Imag));
  Dia->setFrequency(pm->VarPos[0]);

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
void QucsApp::slot2PortMatching()
{
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

  QString DataSet;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  int z = pm->pGraph->Var.find(':');
  if(z <= 0)  DataSet = Doc->DataSet;
  else  DataSet = pm->pGraph->Var.mid(z+1);
  double Freq = pm->VarPos[0];

  QFileInfo Info(Doc->DocName);
  DataSet = Info.dirPath()+QDir::separator()+DataSet;

  Diagram *Diag = new Diagram();

  Graph *pg = new Graph("S[1,1]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,1]."));
    return;
  }

  pg = new Graph("S[1,2]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,2]."));
    return;
  }

  pg = new Graph("S[2,1]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,1]."));
    return;
  }

  pg = new Graph("S[2,2]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,2]."));
    return;
  }

  DataX *Data = Diag->Graphs.getFirst()->cPointsX.first();
  if(Data->Var != "frequency") {
    QMessageBox::critical(0, tr("Error"), tr("Wrong dependency!"));
    return;
  }

  double *Value = Data->Points;
  // search for values for chosen frequency
  for(z=0; z<Data->count; z++)
    if(*(Value++) == Freq) break;

  // get S-parameters
  double S11real = *(Diag->Graphs.first()->cPointsY + 2*z);
  double S11imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S12real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S12imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S21real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S21imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S22real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S22imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);

  delete Diag;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->TwoCheck->setEnabled(false);
  Dia->setFrequency(Freq);
  Dia->S11magEdit->setText(QString::number(S11real));
  Dia->S11degEdit->setText(QString::number(S11imag));
  Dia->S12magEdit->setText(QString::number(S12real));
  Dia->S12degEdit->setText(QString::number(S12imag));
  Dia->S21magEdit->setText(QString::number(S21real));
  Dia->S21degEdit->setText(QString::number(S21imag));
  Dia->S22magEdit->setText(QString::number(S22real));
  Dia->S22degEdit->setText(QString::number(S22imag));

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
// Is called if the "edit" action is clicked on right mouse button menu.
void QucsApp::slotEditElement()
{
  if(view->focusMEvent)
    view->editElement((Schematic*)DocumentTab->currentPage(), view->focusMEvent);
}

// -----------------------------------------------------------
// Hides the edit for component property. Called e.g. if QLineEdit
// looses the focus.
void QucsApp::slotHideEdit()
{
  editText->setHidden(true);
}
