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
#include <QtGui>
#include <QDebug>
#include <QtCore>
#include <QtSvg>
#include <limits.h>

#include <QProcess>
#include <Q3SyntaxHighlighter>
//Added by qt3to4:
#include <Q3PtrList>
#include <Q3TextEdit>

#include "main.h"
#include "qucs.h"
#include "qucsdoc.h"
#include "textdoc.h"
#include "syntax.h"
#include "schematic.h"
#include "mouseactions.h"
#include "messagedock.h"
#include "wire.h"
#include "module.h"
#include "components/components.h"
#include "paintings/paintings.h"
#include "diagrams/diagrams.h"
#include "dialogs/savedialog.h"
#include "dialogs/newprojdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/digisettingsdialog.h"
#include "dialogs/vasettingsdialog.h"
#include "dialogs/qucssettingsdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/sweepdialog.h"
#include "dialogs/labeldialog.h"
#include "dialogs/matchdialog.h"
#include "dialogs/simmessage.h"
#include "dialogs/exportdialog.h"
//#include "dialogs/vtabwidget.h"
//#include "dialogs/vtabbeddockwidget.h"
#include "octave_window.h"
#include "../qucs-lib/qucslib_common.h"

extern const char *empty_xpm[];


// IconView without dragging icon bitmap
/*
class myIconView : public Q3IconView
{
public:
  myIconView(QWidget* parent_) : Q3IconView(parent_, 0, 0) {};
 ~myIconView() {};

protected:
  Q3DragObject *dragObject() {
    Q3IconViewItem *Item = currentItem();
    if(!Item) return 0;

    // no picture during dragging, but bounding rectangles in QListView
    Q3IconDrag *DragPic = new Q3IconDrag( viewport() );
    DragPic->setPixmap( QPixmap(empty_xpm), QPoint(0, 0) );
    DragPic->append( Q3IconDragItem(),
        QRect( Item->pixmapRect().width() / -2,
               Item->pixmapRect().height() / -2,
               Item->pixmapRect().width(), Item->pixmapRect().height() ),
        QRect( Item->textRect().width() / -2,
               Item->pixmapRect().height() / 2 + 5,
               Item->textRect().width(), Item->textRect().height() ) );
    return DragPic;
  };
};
*/


QucsApp::QucsApp()
{
  setCaption("Qucs " PACKAGE_VERSION);

  spiceExtensions << "*.sp" << "*.cir" << "*.spc" << "*.spi";

  QucsFileFilter =
    tr("Schematic") + " (*.sch);;" +
    tr("Data Display") + " (*.dpl);;" +
    tr("Qucs Documents") + " (*.sch *.dpl);;" +
    tr("VHDL Sources") + " (*.vhdl *.vhd);;" +
    tr("Verilog Sources") + " (*.v);;" +
    tr("Verilog-A Sources") + " (*.va);;" +
    tr("Octave Scripts") + " (*.m *.oct);;" +
    tr("Spice Files") + getSpiceFileFilter() +
    tr("Any File")+" (*)";
  //QucsSettings.QucsWorkDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  //QucsSettings.QucsHomeDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));

  updateSchNameHash();
  updateSpiceNameHash();

  move  (QucsSettings.x,  QucsSettings.y);
  resize(QucsSettings.dx, QucsSettings.dy);

  MouseMoveAction = 0;
  MousePressAction = 0;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  initView();
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();
  viewToolBar->setOn(true);
  viewStatusBar->setOn(true);
  viewBrowseDock->setOn(true);
  slotViewOctaveDock(false);
  initCursorMenu();
  HierarchyHistory.setAutoDelete(true);
  Module::registerModules ();

  // default settings of the printer
  Printer = new QPrinter(QPrinter::HighResolution);
#if defined (QT_VERSION) && QT_VERSION > 0x030200
  Printer->setOptionEnabled(QPrinter::PrintSelection, true);
  Printer->setOptionEnabled(QPrinter::PrintPageRange, false);
  Printer->setOptionEnabled(QPrinter::PrintToFile, true);
#endif
  Printer->setColorMode(QPrinter::Color);
  Printer->setFullPage(true);

  // instance of small text search dialog
  SearchDia = new SearchDialog(this);

  // creates a document called "untitled"
  // it configures itself and get appended to App->DocumentTab
  new Schematic(this, "");

  select->setOn(true);  // switch on the 'select' action
  switchSchematicDoc(true);  // "untitled" document is schematic

  lastExportFilename = QDir::homePath() + QDir::separator() + "export.png";

  // load documents given as command line arguments
  for(int z=1; z<qApp->argc(); z++) {
    QString arg = qApp->argv()[z];
    if(*(arg) != '-') {
      // allow uri's: file:/home/linuxuser/Desktop/example.sch
      //TODO
      //if(arg.contains(":/")) {
        //QString f = QDir::convertSeparators(Q3UriDrag::uriToLocalFile(arg));
      //  if(f.isEmpty()) f = arg;
      //  gotoPage(f);
      //} else {
        // get and set absolute path, QucsWorkDir now finds subcircuits
        QFileInfo Info(arg);
        QucsSettings.QucsWorkDir.setPath(Info.absoluteDir().absolutePath());
        arg = QucsSettings.QucsWorkDir.filePath(Info.fileName());
        gotoPage(arg);
      //}
    }
  }
}

QucsApp::~QucsApp()
{
  Module::unregisterModules ();
  delete Printer;
}


// #######################################################################
// ##########                                                   ##########
// ##########     Creates the working area (QTabWidget etc.)    ##########
// ##########                                                   ##########
// #######################################################################
void QucsApp::initContentListView()
{

  Content->clear();

  ConOthers = new QTreeWidgetItem(Content);
  ConOthers->setText(0, tr("Others"));
  ConDatasets = new QTreeWidgetItem(Content);
  ConDatasets->setText(0, tr("Datasets"));
  ConDisplays = new QTreeWidgetItem(Content);
  ConDisplays->setText(0, tr("Data Displays"));
  ConOctave = new QTreeWidgetItem(Content);
  ConOctave->setText(0, tr("Octave"));
  ConVerilog = new QTreeWidgetItem(Content);
  ConVerilog->setText(0, tr("Verilog"));
  ConVerilogA = new QTreeWidgetItem(Content);
  ConVerilogA->setText(0, tr("Verilog-A"));
  ConSources = new QTreeWidgetItem(Content);
  ConSources->setText(0, tr("VHDL"));
  ConSchematics = new QTreeWidgetItem(Content);
  ConSchematics->setText(0, tr("Schematics"));

}

/**
 * @brief QucsApp::initView Setup the layour of all widgets
 */
void QucsApp::initView()
{


  // set application icon
  // APPLE sets the QApplication icon with Info.plist
#ifndef __APPLE__
  setWindowIcon (QPixmap(":/bitmaps/big.qucs.xpm"));
#endif

  DocumentTab = new QTabWidget(this);
  setCentralWidget(DocumentTab);

  connect(DocumentTab,
          SIGNAL(currentChanged(QWidget*)), SLOT(slotChangeView(QWidget*)));

  // Give every tab a close button, and connect the button's signal to
  // slotFileClose
  DocumentTab->setTabsClosable(true);
  connect(DocumentTab,
          SIGNAL(tabCloseRequested(int)), SLOT(slotFileClose(int)));
#ifdef HAVE_QTABWIDGET_SETMOVABLE
  // make tabs draggable if supported
  DocumentTab->setMovable (true);
#endif

  //dock = new VTabbedDockWidget(Q3DockWindow::InDock, this);
  dock = new QDockWidget(this);
  //TabView = new VTabWidget(VTabInterface::TabLeft,dock);  // tabs on the left side
  TabView = new QTabWidget(dock);
  TabView->setTabPosition(QTabWidget::West);

  connect(dock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleDock(bool)));

  view = new MouseActions(this);

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
  QWidget *ProjGroup = new QWidget();
  QVBoxLayout *ProjGroupLayout = new QVBoxLayout();
  QWidget *ProjButts = new QWidget();
  QPushButton *ProjNew = new QPushButton(tr("New"));
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotProjNewButt()));
  QPushButton *ProjOpen = new QPushButton(tr("Open"));
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotProjOpenButt()));
  QPushButton *ProjDel = new QPushButton(tr("Delete"));
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotProjDelButt()));

  QHBoxLayout *ProjButtsLayout = new QHBoxLayout();
  ProjButtsLayout->addWidget(ProjNew);
  ProjButtsLayout->addWidget(ProjOpen);
  ProjButtsLayout->addWidget(ProjDel);
  ProjButts->setLayout(ProjButtsLayout);

  ProjGroupLayout->addWidget(ProjButts);

  Projects = new QListWidget();

  ProjGroupLayout->addWidget(Projects);
  ProjGroup->setLayout(ProjGroupLayout);

  TabView->addTab(ProjGroup, tr("Projects"));
  TabView->setTabToolTip(TabView->indexOf(ProjGroup), tr("content of project directory"));

  connect(Projects, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(slotOpenProject(QListWidgetItem*)));

  // ----------------------------------------------------------
  // "Content" Tab of the left QTabWidget
  Content = new QTreeWidget(this);
  Content->setColumnCount(2);
  QStringList headers;
  headers << tr("Content of") << tr("Note");
  Content->setHeaderLabels(headers);
  Content->setSortingEnabled(false);
  Content->setColumnWidth(0,150);

  // allow for a custom context menu
  Content->setContextMenuPolicy(Qt::CustomContextMenu);

  initContentListView();

  TabView->addTab(Content,tr("Content"));
  TabView->setTabToolTip(TabView->indexOf(Content), tr("content of current project"));


  connect(Content, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		   SLOT(slotOpenContent(QTreeWidgetItem*)));

  connect(Content, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
           SLOT(slotSelectSubcircuit(QTreeWidgetItem*)));

  // ----------------------------------------------------------
  // "Component" Tab of the left QTabWidget
  QWidget *CompGroup  = new QWidget();
  QVBoxLayout *CompGroupLayout = new QVBoxLayout();

  CompChoose = new QComboBox(this);
  CompComps = new QListWidget(this);
  CompComps->setViewMode(QListView::IconMode);
  CompComps->setGridSize(QSize(110,90));

  CompGroupLayout->addWidget(CompChoose);
  CompGroupLayout->addWidget(CompComps);
  CompGroup->setLayout(CompGroupLayout);

  TabView->addTab(CompGroup,tr("Components"));
  TabView->setTabToolTip(TabView->indexOf(CompGroup), tr("components and diagrams"));
  fillComboBox(true);

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompComps, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));

  // ----------------------------------------------------------
  // "Libraries" Tab of the left QTabWidget

  QWidget *LibGroup = new QWidget ();
  QVBoxLayout *LibGroupLayout = new QVBoxLayout ();
  QWidget *LibButts = new QWidget ();
  QPushButton *LibManage = new QPushButton (tr ("Manage Libraries"));
  connect(LibManage, SIGNAL(clicked()), SLOT(slotCallLibrary()));

  QHBoxLayout *LibButtsLayout = new QHBoxLayout();
  LibButtsLayout->addWidget (LibManage);
  LibButts->setLayout(LibButtsLayout);

  LibGroupLayout->addWidget(LibButts);


  libTreeWidget = new QTreeWidget (this);
  libTreeWidget->setColumnCount (1);
  headers.clear ();
  headers << tr ("Libraries");
  libTreeWidget->setHeaderLabels (headers);

  LibGroupLayout->addWidget (libTreeWidget);
  LibGroup->setLayout (LibGroupLayout);

  fillLibrariesTreeView ();

  TabView->addTab (LibGroup, tr("Libraries"));
  TabView->setTabToolTip (TabView->indexOf (CompGroup), tr ("system and user component libraries"));

  connect(libTreeWidget, SIGNAL(itemPressed (QTreeWidgetItem*, int)),
           SLOT(slotSelectLibComponent (QTreeWidgetItem*)));

  // ----------------------------------------------------------
  // put the tab widget in the dock
  dock->setWidget(TabView);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->addDockWidget(Qt::LeftDockWidgetArea, dock);
  TabView->setCurrentPage(0);

  // ----------------------------------------------------------
  // Octave docking window
  //octDock = new Q3DockWindow(Q3DockWindow::InDock, this);
  //octDock->setCloseMode(Q3DockWindow::Always);
  octDock = new QDockWidget();

  connect(octDock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleOctave(bool)));
  octave = new OctaveWindow(octDock);
  this->addDockWidget(Qt::BottomDockWidgetArea, octDock);
  this->setCorner(Qt::BottomLeftCorner  , Qt::LeftDockWidgetArea);
  //| Qt::BottomRightCorner

  // ............................................

  messageDock = new MessageDock(this);

  // ............................................
  readProjects(); // reads all projects and inserts them into the ListBox
}

// Put all available libraries into ComboBox.
void QucsApp::fillLibrariesTreeView ()
{
    QStringList LibFiles;
    QStringList::iterator it;
    QList<QTreeWidgetItem *> topitems;

    libTreeWidget->clear();

    // make the system libraries section header
    QTreeWidgetItem* newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("System Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    QFont sectionFont = newitem->font(0);
    sectionFont.setItalic (true);
    sectionFont.setBold (true);
    newitem->setFont (0, sectionFont);
//    newitem->setBackground
    topitems.append (newitem);

    QDir LibDir(QucsSettings.LibDir);
    LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

    // create top level library itmes, base on the library names
    for(it = LibFiles.begin(); it != LibFiles.end(); it++)
    {
        ComponentLibrary parsedlibrary;

        int result = parseComponentLibrary (QucsSettings.LibDir + *it , parsedlibrary);
        QStringList nameAndFileName;
        nameAndFileName.append (parsedlibrary.name);
        nameAndFileName.append (QucsSettings.LibDir + *it);

        QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)0, nameAndFileName);

        switch (result)
        {
            case QUCS_COMP_LIB_IO_ERROR:
                QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (*it));
                return;
            case QUCS_COMP_LIB_CORRUPT:
                QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
                return;
            default:
                break;
        }

        for (int i = 0; i < parsedlibrary.components.count (); i++)
        {
            QStringList compNameAndDefinition;

            compNameAndDefinition.append (parsedlibrary.components[i].name);

            QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

            s +=  "<Components>\n  " +
                  parsedlibrary.components[i].modelString + "\n" +
                  "</Components>\n";

            compNameAndDefinition.append (s);
            // The following may produce a warning from the compiler about
            // unused variable newcompitem, ignore it, we pass the pointer
            // to the parent item in the constructor
            QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);
        }

        topitems.append (newlibitem);
    }


    // make the user libraries section header
    newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("User Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    newitem->setFont (0, sectionFont);
    topitems.append (newitem);

    QDir UserLibDir = QDir (QucsSettings.QucsHomeDir.canonicalPath () + "/user_lib/");

    // if there are user libraries, add them too
    if(UserLibDir.exists ())
    {
        //LibFiles = UserLibDir.entryList("*.lib", QDir::Files, QDir::Name);
        LibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
        int UserLibCount = LibFiles.count();

        if (UserLibCount > 0)
        {

            // create top level library itmes, base on the library names
            for(it = LibFiles.begin(); it != LibFiles.end(); it++)
            {
                ComponentLibrary parsedlibrary;

                int result = parseComponentLibrary (UserLibDir.absolutePath() +"/"+ *it , parsedlibrary);
                QStringList nameAndFileName;
                nameAndFileName.append (parsedlibrary.name);
                nameAndFileName.append (UserLibDir.absolutePath() +"/"+ *it);

                QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)0, nameAndFileName);

                switch (result)
                {
                    case QUCS_COMP_LIB_IO_ERROR:
                        QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (UserLibDir.absolutePath()+"/" +*it));
                        return;
                    case QUCS_COMP_LIB_CORRUPT:
                        QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
                        return;
                    default:
                        break;
                }

                for (int i = 0; i < parsedlibrary.components.count (); i++)
                {
                    QStringList compNameAndDefinition;

                    compNameAndDefinition.append (parsedlibrary.components[i].name);

                    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

                    s +=  "<Components>\n  " +
                          parsedlibrary.components[i].modelString + "\n" +
                          "</Components>\n";

                    compNameAndDefinition.append (s);
                    // The following may produce a warning from the compiler about
                    // unused variable newcompitem, ignore it, we pass the pointer
                    // to the parent item in the constructor
                    QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);
                }

                topitems.append (newlibitem);
            }
            libTreeWidget->insertTopLevelItems(0, topitems);
        }
        else
        {
            // make the user libraries section header
            newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("No User Libraries"));
            sectionFont.setBold (false);
            newitem->setFont (0, sectionFont);
            topitems.append (newitem);
        }
    }
    else
    {
        // make the user libraries section header
        newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("No User Libraries"));
        sectionFont.setBold (false);
        newitem->setFont (0, sectionFont);
        topitems.append (newitem);
    }

    libTreeWidget->insertTopLevelItems(0, topitems);
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
    if(isTextDocument (w))
      return (QucsDoc*) ((TextDoc*)w);
    else
      return (QucsDoc*) ((Schematic*)w);
  }

  return 0;
}

// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose file name is "Name".
QucsDoc * QucsApp::findDoc (QString File, int * Pos)
{
  QucsDoc * d;
  int No = 0;
  File = QDir::convertSeparators (File);
  while ((d = getDoc (No++)) != 0)
    if (QDir::convertSeparators (d->DocName) == File) {
      if (Pos) *Pos = No - 1;
      return d;
    }
  return 0;
}

// ---------------------------------------------------------------
// Put the component groups into the ComboBox. It is possible to
// only put the paintings in it, because of "symbol painting mode".

// if setAll, add all categories to combobox
// if not, set just paintings (symbol painting mode)
void QucsApp::fillComboBox (bool setAll)
{
  //CompChoose->setMaxVisibleItems (13); // Increase this if you add items below.
  CompChoose->clear ();

  QStringList cats = Category::getCategories ();
  for (QStringList::Iterator it = cats.begin (); it != cats.end (); ++it) {
    if (setAll)
      CompChoose->insertItem (*it);
    else
      if (*it == QObject::tr("paintings"))
        CompChoose->insertItem (*it);
  }
}

// ----------------------------------------------------------
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriate components.
void QucsApp::slotSetCompView (int index)
{
//  qDebug() << "QucsApp::slotSetCompView (int index)";

  editText->setHidden (true); // disable text edit of component property

  Q3PtrList<Module> Comps;
  CompComps->clear ();   // clear the IconView
  if (CompChoose->count () <= 0) return;

  QString item = CompChoose->text (index);

  /*
   * The following test is used as only paintings are allowed in
   * symbol editing mode, previously paintings had to be the last
   * category in modules.cpp.
   * Howver, before symbol edit is started it is requested that only
   * 'paintigs' gets added to the 'compChoose' combobox.
   * See: QucsApp::fillComboBox
   *
   */

  if (item == QObject::tr("paintings"))
  {
    // if index beyond count of combobox, assume it is paintings...
    Comps = Category::getModules (QObject::tr("paintings"));
  }
  else
  {
    // get list of components, will add the icons to dock
    Comps = Category::getModules (item);
  }

  QString Name;

  // if something was registered dynamicaly, get and draw icons into dock
  if (item == QObject::tr("verilog-a user devices")) {

    QListWidgetItem *icon;
    QMapIterator<QString, QString> i(Module::vaComponents);
    while (i.hasNext()) {
      i.next();

      // default icon initally matches the module name
      //Name = i.key();

      // Just need path to bitmap, do not create an object
      QString Name, vaBitmap;
      Component * c = (Component *)
              vacomponent::info (Name, vaBitmap, false, i.value());
      if (c) delete c;

      // check if icon exists, fall back to default
      QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

      QFile iconFile(iconPath);
      QPixmap vaIcon;

      if(iconFile.exists())
      {
        // load bitmap defined on the JSON symbol file
        vaIcon = QPixmap(iconPath);
      }
      else
      {
        QMessageBox::information(this, tr("Info"),
                     tr("Default icon not found:\n %1.png").arg(vaBitmap));
        // default icon
        vaIcon = QPixmap(":/bitmaps/editdelete.png");
      }

      // Add icon an name tag to dock
      icon = new QListWidgetItem(vaIcon, Name);
      icon->setToolTip(Name);
      CompComps->addItem(icon);
    }
  }
  else {
    char * File;
    Module * Mod;
    // Populate list of component bitmaps
    for (Mod = Comps.first(); Mod; Mod = Comps.next ()) {
      if (Mod->info) {
        *(Mod->info) (Name, File, false);
        QListWidgetItem *icon = new QListWidgetItem(QPixmap(":/bitmaps/" + QString (File) + ".png"), Name);
        icon->setToolTip(Name);
        CompComps->addItem(icon);
      }
    }
  }
}

// ------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QListWidgetItem *item)
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
    ((Q3ScrollView*)DocumentTab->currentPage())->viewport()->update();
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

  pInfoVAFunc InfosVA = 0;

  int i = CompComps->row(item);
  Q3PtrList<Module> Comps;

  // if symbol mode, only paintings are enabled.
  if (CompChoose->currentText() == QObject::tr("paintings"))
    Comps = Category::getModules (QObject::tr("paintings"));
  else {
    Comps = Category::getModules (CompChoose->currentText ());
    qDebug() << "pressed CompComps id" << i;
    qDebug() << CompComps->item(i)->toolTip(); //Name;

  }

  // handle static and dynamic components
  if (CompChoose->currentText() == QObject::tr("verilog-a user devices")){
    InfosVA = Comps.at(i)->infoVA;

    // get JSON file out of item name on toolTip
    QString name = CompComps->item(i)->toolTip();
    QString filename = Module::vaComponents[name];

    QString Dummy1;
    QString Dummy2;
    if (InfosVA) {
      qDebug() <<  " slotSelectComponent, view->selElem" ;
      view->selElem = (*InfosVA) (Dummy1, Dummy2, true, filename);
    }

  }
  else {
    Infos = Comps.at(i)->info;

    char * Dummy2;
    QString Dummy1;
    if (Infos)
      view->selElem = (*Infos) (Dummy1, Dummy2, true);
  }
}

// ####################################################################
// #####  Functions for the menu that appears when right-clicking #####
// #####  on a file in the "Content" ListView.                    #####
// ####################################################################

void QucsApp::initCursorMenu()
{

  // TODO -> The contentmenu is also shown when the user right-clicks on a category...
  ContentMenu = new QMenu(this);

  ActionCMenuOpen = new QAction(tr("Open"), ContentMenu);
  connect(ActionCMenuOpen, SIGNAL(triggered()), this, SLOT(slotCMenuOpen()));
  ContentMenu->addAction(ActionCMenuOpen);

  ActionCMenuCopy = new QAction(tr("Duplicate"), ContentMenu);
  connect(ActionCMenuCopy, SIGNAL(triggered()), this, SLOT(slotCMenuCopy()));
  ContentMenu->addAction(ActionCMenuCopy);

  ActionCMenuRename = new QAction(tr("Rename"), ContentMenu);
  connect(ActionCMenuRename, SIGNAL(triggered()), this, SLOT(slotCMenuRename()));
  ContentMenu->addAction(ActionCMenuRename);

  ActionCMenuDelete = new QAction(tr("Delete"), ContentMenu);
  connect(ActionCMenuDelete, SIGNAL(triggered()), this, SLOT(slotCMenuDelete()));
  ContentMenu->addAction(ActionCMenuDelete);

  ActionCMenuInsert = new QAction(tr("Insert"), ContentMenu);
  connect(ActionCMenuInsert, SIGNAL(triggered()), this, SLOT(slotCMenuInsert()));
  ContentMenu->addAction(ActionCMenuInsert);


  // TODO -> not implemented yet...
  //ActionCMenuDelGroup = new QAction(tr("Delete Group"), ContentMenu);
  //connect(ActionCMenuDelGroup, SIGNAL(triggered()), this, SLOT(slotCMenuDelGroup()));
  //Content->addAction(ActionCMenuDelGroup);


  connect(Content, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotShowContentMenu(const QPoint&)));
}

// ----------------------------------------------------------
// Shows the menu.
void QucsApp::slotShowContentMenu(const QPoint& pos) {

  QTreeWidgetItem *item = Content->currentItem();
  if(item->text(1  ).contains(tr("-port")))
  {
      ActionCMenuInsert->setVisible(true);
  }
  else
  {
      ActionCMenuInsert->setVisible(false);
  }

  // only show contentmenu when child is selected...
  if(item->parent()!= 0) {
    ContentMenu->popup(Content->mapToGlobal(pos));
  }

}
/* OLD Version
void QucsApp::slotShowContentMenu(Q3ListViewItem *item, const QPoint& point, int)
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
*/
// ----------------------------------------------------------
void QucsApp::slotCMenuOpen()
{
  QTreeWidgetItem *Item = Content->currentItem();
  if(Item == 0) return;

  slotOpenContent(Item);
}

// ----------------------------------------------------------
void QucsApp::slotCMenuCopy()
{
  QTreeWidgetItem *Item = Content->currentItem();
  if(Item == 0) return;

  QString Name = Item->text(0);
  QString currentPath = QucsSettings.QucsWorkDir.filePath(Name);
  QString Path = currentPath.section(QDir::separator(), 0, -2);

  //check changed file save
  int z = 0; //search if the doc is loaded
  QucsDoc *d = findDoc(currentPath, &z);
  if (d != NULL && d->DocChanged) {
    DocumentTab->setCurrentPage(z);
    int ret = QMessageBox::question(this, tr("Copying Qucs document"), 
          tr("The document contains unsaved changes!\n") + 
          tr("Do you want to save the changes before copying?"),
          tr("&Ignore"), tr("&Save"), 0, 1);
    if (ret == 1) {
      d->save();
    }
  }

  QString Suffix = Name.section('.',-1);   // remember suffix
  QString Base   = Name.section('.',0,-2);
  if(Base.isEmpty()) Base = Name;

  bool exists = true;   //generate unique name
  int i = 0;
  QString defaultName;
  while (exists) {
    ++i;
    defaultName = Base + "_copy" + QString::number(i) + "." + Suffix;
    exists = QFile::exists (Path + QDir::separator() + defaultName);
  }

  bool ok;
  QString s = QInputDialog::getText(tr("Copy file"), tr("Enter new name:"),
		QLineEdit::Normal, defaultName, &ok, this);
  if(!ok) return;
  if(s.isEmpty()) return;

  QString NewName;
  if(s.contains('.'))
    NewName = s;
  else
    NewName = s+"."+Suffix;

  if (QFile::exists(Path + QDir::separator() + NewName)) {  //check New Name exists
    QMessageBox::critical(this, tr("error"), tr("Cannot copy file to identical name: ") + Name);
    return;
  }

  if (!QFile::copy(Path + QDir::separator() + Name,
      Path + QDir::separator() + NewName)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot copy schematic: ")+Name);
    return;
  }
  //TODO: maybe require disable edit here

  // refresh the schematic file path
  this->updateSchNameHash();
  this->updateSpiceNameHash();

  if(!ProjName.isEmpty())
    readProjectFiles();  // re-read the content ListView
}

// ----------------------------------------------------------
void QucsApp::slotCMenuRename()
{
  QTreeWidgetItem *Item = Content->currentItem();
  if(!Item) return;

  QString Name = Item->text(0);
  if (findDoc (QucsSettings.QucsWorkDir.filePath(Name))) {
    QMessageBox::critical(this, tr("Error"),
			        tr("Cannot rename an open file!"));
    return;
  }

  QString Suffix = Name.section('.',-1);   // remember suffix
  QString Base   = Name.section('.',0,-2);
  if(Base.isEmpty()) Base = Name;

  bool ok;
  QString s = QInputDialog::getText(tr("Rename file"), tr("Enter new name:"),
		QLineEdit::Normal, Base, &ok, this);
  if(!ok) return;
  if(s.isEmpty()) return;

  QString NewName;
  if(s.contains('.'))
    NewName = s;
  else
    NewName = s+"."+Suffix;
  QDir file(QucsSettings.QucsWorkDir.path());
  if(!file.rename(Name, NewName)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot rename file: ")+Name);
    return;
  }
  Item->setText(0, NewName);
}

// ----------------------------------------------------------
void QucsApp::slotCMenuDelete()
{
  QTreeWidgetItem *item = Content->currentItem();
  if(item == 0) return;
  QString FileName = QucsSettings.QucsWorkDir.filePath(item->text(0));

  if (findDoc (FileName)) {
    QMessageBox::critical(this, tr("Error"),
			        tr("Cannot delete an open file!"));
    return;
  }

  int No;
  No = QMessageBox::warning(this, tr("Warning"),
             tr("This will delete the file permanently! Continue ?"),
             tr("No"), tr("Yes"));
  if(No != 1) return;

  if(!QFile::remove(FileName)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete schematic: ")+item->text(0));
    return;
  }

  delete item;
}

// ----------------------------------------------------------
QString QucsApp::fileType (const QString& Ext)
{
  QString Type = tr("unknown");
  if (Ext == "v")
    Type = tr("Verilog source");
  else if (Ext == "va")
    Type = tr("Verilog-A source");
  else if (Ext == "vhd" || Ext == "vhdl")
    Type = tr("VHDL source");
  else if (Ext == "dat")
    Type = tr("data file");
  else if (Ext == "dpl")
    Type = tr("data display");
  else if (Ext == "sch")
    Type = tr("schematic");
  else if (Ext == "sym")
    Type = tr("symbol");
  else if (Ext == "vhdl.cfg" || Ext == "vhd.cfg")
    Type = tr("VHDL configuration");
  else if (Ext == "cfg")
    Type = tr("configuration");
  return Type;
}

// ----------------------------------------------------------
// TODO -> not implemented yet
// Deletes all files with that name (and suffix sch, dpl, dat, vhdl, etc.).
void QucsApp::slotCMenuDelGroup ()
{
  QTreeWidgetItem *item = Content->currentItem();
  if (item == 0)
    return;
  QString s = item->text (0);
  s = QucsDoc::fileBase (s); // cut off suffix from file name

  const char * extensions[] =
    { "sch", "dpl", "dat", "vhdl", "vhd", "v", "sym",
      "vhdl.cfg", "vhd.cfg", "va", 0 };

  int i;
  for (i = 0; extensions[i] != 0; i++) {
    QString Short = s + "." + extensions[i];
    QString Name = QucsSettings.QucsWorkDir.filePath (Short);
    // search, if files are open
    if (findDoc (Name)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot delete the open file \"%1\"!").arg(Short));
      return;
    }
  }



  // check existence of files
  QString Str = "\n";
  for (i = 0; extensions[i] != 0; i++) {
    QString Short = s + "." + extensions[i];
    QString Long = QucsSettings.QucsWorkDir.filePath (Short);
    bool exists = QFile::exists (Long);
    if (exists)
      Str += Short + "\n";
  }
  int No;
  No = QMessageBox::warning (this, tr("Warning"),
	tr("This will delete the files%1permanently! Continue ?").arg(Str),
	tr("No"), tr("Yes"));
  if (No != 1)
    return;

  // file removal
  for (i = 0; extensions[i] != 0; i++) {
    QString Short = s + "." + extensions[i];
    QString Name = QucsSettings.QucsWorkDir.filePath (Short);
    bool exists = QFile::exists (Name);
    if (exists) {
      // remove files
      if (!QFile::remove (Name)) {
	       QMessageBox::critical(this, tr("Error"),	tr("Cannot delete %1: \"%2\"!").arg(fileType (extensions[i])).
	        arg(Short));
	       continue;
      }
      // remove items from listview
      //item = Content->findItem (Short, 0);
      if (item) {
	     // TODO???
       //item->parent()->takeItem (item);
	     delete item;
      }
    }
  }
}


// ----------------------------------------------------------
// Inserts the selected subschematic in the schematic
void QucsApp::slotCMenuInsert ()
{
    slotSelectSubcircuit(Content->currentItem());
}

// ################################################################
// #####    Functions that handle the project operations.     #####
// ################################################################

// Checks for qucs directory and reads all existing Qucs projects.
void QucsApp::readProjects()
{
  QDir ProjDir(QucsSettings.QucsHomeDir);
  if(!ProjDir.cd(QucsSettings.QucsHomeDir.absolutePath())) { // work directory exists ?
    if(!ProjDir.mkdir(QucsSettings.QucsHomeDir.absolutePath())) { // no, then create it
      QMessageBox::warning(this, tr("Warning"),
                   tr("Cannot create work directory !"));
      return;
    }
    ProjDir.cd(QucsSettings.QucsHomeDir.absolutePath());
  }

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
       Projects->addItem(*it);
     }
}

// ----------------------------------------------------------
// Is called, when "Create New Project" button is pressed.
void QucsApp::slotProjNewButt()
{
  editText->setHidden(true); // disable text edit of component property

  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir projDir(QucsSettings.QucsHomeDir.path());
  if(projDir.mkdir(d->ProjName->text()+"_prj")) {
    Projects->insertItem(0, d->ProjName->text());  // at first position
    if(d->OpenProj->isChecked())
      slotOpenProject(Projects->item(0));
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
  if(!file.open(QIODevice::ReadOnly)) {
    return -1;
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();
  file.close();
  QTextStream stream(&FileString, QIODevice::ReadOnly);


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

  Line = Line.mid(16, Line.length()-17);
  if(!checkVersion(Line)) { // wrong version number ?
      if (!QucsSettings.IgnoreFutureVersion) {
          file.close();
          return -4;
      }
    //file.close();
    //return -4;
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
    QString s = Line.section(' ',0,0);    // component type
    if(s == "<Port") z++;
  }
  return -5;  // component field not closed
}

// ----------------------------------------------------------
// Reads all files in the project directory and sort them into the
// content ListView
void QucsApp::readProjectFiles()
{
  // Delete the content files, but don't delete the parent items !!!
/* TODO
  while(ConSchematics->firstChild())
    delete ConSchematics->firstChild();
  while(ConDisplays->firstChild())
    delete ConDisplays->firstChild();
  while(ConDatasets->firstChild())
    delete ConDatasets->firstChild();
  while(ConSources->firstChild())
    delete ConSources->firstChild();
  while(ConVerilog->firstChild())
    delete ConVerilog->firstChild();
  while(ConVerilogA->firstChild())
    delete ConVerilogA->firstChild();
  while(ConOthers->firstChild())
    delete ConOthers->firstChild();
  while(ConOctave->firstChild())
    delete ConOctave->firstChild();
*/

  //Is this OK instead of the above??
  initContentListView();

  int n;
  // put all files into "Content"-ListView
  QStringList Elements = QucsSettings.QucsWorkDir.entryList("*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  QString Str;
  ConSchematics->setExpanded(true);
  for(it = Elements.begin(); it != Elements.end(); ++it) {
    Str = QucsDoc::fileSuffix (*it);
    if(Str == "sch") {
      n = testFile(QucsSettings.QucsWorkDir.filePath((*it).ascii()));
      if(n >= 0) {
        if(n > 0) {
          QTreeWidgetItem *temp = new QTreeWidgetItem(ConSchematics);
          temp->setText(0, (*it).ascii());
          temp->setText(1, QString::number(n)+tr("-port"));
        }
        else {
          QTreeWidgetItem *temp = new QTreeWidgetItem(ConSchematics);
          temp->setText(0, (*it).ascii());
        }
      }
    }
    else if(Str == "dpl") {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConDisplays);
      temp->setText(0, (*it).ascii());
    }
    else if(Str == "dat") {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConDatasets);
      temp->setText(0, (*it).ascii());
    }
    else if((Str == "vhdl") || (Str == "vhd")) {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConSources);
      temp->setText(0, (*it).ascii());
    }
    else if(Str == "v") {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConVerilog);
      temp->setText(0, (*it).ascii());
    }
    else if(Str == "va") {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConVerilogA);
      temp->setText(0, (*it).ascii());
    }
    else if((Str == "m") || (Str == "oct")) {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConOctave);
      temp->setText(0, (*it).ascii());
    }
    else {
      QTreeWidgetItem *temp = new QTreeWidgetItem(ConOthers);
      temp->setText(0, (*it).ascii());
    }
  }
}

// ----------------------------------------------------------
// Opens an existing project.
void QucsApp::openProject(const QString& Path, const QString& Name)
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
  QucsSettings.QucsWorkDir.setPath(ProjDir.path());
  octave->adjustDirectory();

  QStringList headers;
  headers << tr("Content of ") + Name << tr("Note");
  Content->setHeaderLabels(headers);

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
  QString d = QFileDialog::getExistingDirectory(this, tr("Choose Project Directory for Opening"),
                                                 QucsSettings.QucsHomeDir.path(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
  QString s = d;
  if(s.isEmpty()) return;

  s = s.left(s.length()-1);   // cut off trailing '/'
  int i = s.findRev('/');
  if(i > 0) s = s.mid(i+1);   // cut out the last subdirectory
  s.remove("_prj");
  openProject(d, s);
}

// ----------------------------------------------------------
// Is called, when "Open Project" button is pressed.
void QucsApp::slotProjOpenButt()
{
  editText->setHidden(true); // disable text edit of component property

  QListWidgetItem *item = Projects->currentItem();
  if(item) slotOpenProject(item);
  else QMessageBox::information(this, tr("Info"),
				tr("No project is selected !"));
}

// ----------------------------------------------------------
// Is called when project is double-clicked to open it.
void QucsApp::slotOpenProject(QListWidgetItem *item)
{
  openProject(QucsSettings.QucsHomeDir.filePath(item->text()+"_prj"), item->text());
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
  QucsSettings.QucsWorkDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  octave->adjustDirectory();

  QStringList headers;
  headers << tr("Content of") << tr("Note");
  Content->setHeaderLabels(headers);

  initContentListView();

  TabView->setCurrentPage(0);   // switch to "Projects"-Tab
  ProjName = "";
}

// ----------------------------------------------------------
bool QucsApp::deleteDirectoryContent(QDir& Dir)
{
  // removes every file, remove("*") does not work
  QStringList Files = Dir.entryList("*", QDir::Files|QDir::Hidden);  // all files
  QStringList::iterator it;
  for(it = Files.begin(); it != Files.end(); it++) {
     if(!Dir.remove(*it)) {
       QMessageBox::information(this, tr("Info"),
				tr("Cannot delete file: ")+(*it));
       return false;
     }
  }

  QDir myDir(Dir);
  // Remove all directories recursively.
  Files = Dir.entryList("*", QDir::Dirs);
  Files.pop_front();  // delete "." from list
  Files.pop_front();  // delete ".." from list
  for(it = Files.begin(); it != Files.end(); it++) {
     myDir.cd(*it);
     if(!deleteDirectoryContent(myDir))
       return false;
     myDir.cdUp();
     if(!myDir.rmdir(*it)) {
       QMessageBox::information(this, tr("Info"),
				tr("Cannot remove directory: ")+(*it));
       return false;
    }
  }
  return true;
}

// ----------------------------------------------------------
bool QucsApp::deleteProject(const QString& Path, const QString& Name)
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
  if(!deleteDirectoryContent(projDir))
    return false;

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

  QString d = QFileDialog::getExistingDirectory(this, tr("Choose Project Directory for Deleting"),
                                                 QucsSettings.QucsHomeDir.path(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
  QString s = d;

  if(s.isEmpty()) return;

  if (s.endsWith(QDir::separator())) {
      s = s.left(s.length()-1);  // cut off trailing '/'
  }
  int i = s.findRev(QDir::separator());
  if(i > 0) s = s.mid(i+1);  // cut out the last subdirectory
  s.chop(4); // remove "_prj" from name
  deleteProject(d, s);
  readProjects();   // re-reads all projects and inserts them into the ListBox
}

// ----------------------------------------------------------
// Is called, when "Delete Project" button is pressed.
void QucsApp::slotProjDelButt()
{
  QListWidgetItem *item = Projects->currentItem();
  if(!item) {
    QMessageBox::information(this, tr("Info"),
			     tr("No project is selected !"));
    return;
  }

  if(!deleteProject(QucsSettings.QucsHomeDir.filePath(item->text()+"_prj"),
	item->text()))  return;
  Projects->takeItem(Projects->currentRow());  // remove from project list
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

  int i = 0;
  QucsDoc * d = findDoc (Name, &i);  // search, if page is already loaded

  if(d) {   // open page found ?
    d->becomeCurrent(true);
    DocumentTab->setCurrentPage(i);  // make new document the current
    return true;
  }

  QFileInfo Info(Name);
  if(Info.extension(false) == "sch" || Info.extension(false) == "dpl" ||
     Info.extension(false) == "sym")
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
  if(getDoc(0)->DocName.isEmpty())
    if(!getDoc(0)->DocChanged)
      delete DocumentTab->page(0);

  view->drawn = false;
  return true;
}

QString lastDirOpenSave; // to remember last directory and file

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  editText->setHidden(true); // disable text edit of component property

  statusBar()->message(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(this, tr("Enter a Schematic Name"),
    lastDirOpenSave.isEmpty() ? QString(".") : lastDirOpenSave, QucsFileFilter);

  if(s.isEmpty())
    statusBar()->message(tr("Opening aborted"), 2000);
  else {




    updateRecentFilesList(s);
    slotUpdateRecentFiles();

    gotoPage(s);
    lastDirOpenSave = s;   // remember last directory and file


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

  if(!ProjName.isEmpty())
    readProjectFiles();  // re-read the content ListView
}

// --------------------------------------------------------------
bool QucsApp::saveAs()
{
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
        if(lastDirOpenSave.isEmpty())  s = QDir::currentDirPath();
        else  s = lastDirOpenSave;
      }
      else s = QucsSettings.QucsWorkDir.path();
    }

    // list of known file extensions
    QString ext = "vhdl;vhd;v;va;sch;dpl;m;oct;net;qnet;txt";
    QStringList extlist = QStringList::split (';', ext);

    if(isTextDocument (w))
      Filter = tr("VHDL Sources")+" (*.vhdl *.vhd);;" +
	       tr("Verilog Sources")+" (*.v);;"+
	       tr("Verilog-A Sources")+" (*.va);;"+
	       tr("Octave Scripts")+" (*.m *.oct);;"+
	       tr("Qucs Netlist")+" (*.net *.qnet);;"+
	       tr("Plain Text")+" (*.txt);;"+
	       tr("Any File")+" (*)";
    else
      Filter = QucsFileFilter;

    s = QFileDialog::getSaveFileName(this, tr("Enter a Document Name"),
                                     QucsSettings.QucsWorkDir.absPath(),
                                     Filter);
    if(s.isEmpty())  return false;
    Info.setFile(s);               // try to guess the best extension ...
    ext = Info.extension(false);

    if(ext.isEmpty() || !extlist.contains(ext))
    {
      // if no extension was specified or is unknown
      if (!isTextDocument (w))
      {
        // assume it is a schematic
        s += ".sch";
      }
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
    QucsDoc * d = findDoc (s);
    if(d) {
      QMessageBox::information(this, tr("Info"),
		tr("Cannot overwrite an open document"));
      return false;
    }

    break;
  }
  Doc->setName(s);
  lastDirOpenSave = Info.dirPath(true);  // remember last directory and file

  if(intoView) {    // insert new name in Content ListView ?
    if(Info.dirPath(true) == QucsSettings.QucsWorkDir.absPath())
      if(!ProjName.isEmpty()) {
        s = Info.fileName();  // remove path from file name
	QString ext = Info.extension (false);
  /*
        if(ext == "sch")
          Content->setSelected(new Q3ListViewItem(ConSchematics, s), true);
        else if(ext == "dpl")
          Content->setSelected(new Q3ListViewItem(ConDisplays, s), true);
        else if(ext == "dat")
          Content->setSelected(new Q3ListViewItem(ConDatasets, s), true);
        else if((ext == "vhdl") || (ext == "vhd"))
          Content->setSelected(new Q3ListViewItem(ConSources, s), true);
        else if(ext == "v")
          Content->setSelected(new Q3ListViewItem(ConVerilog, s), true);
        else if(ext == "va")
          Content->setSelected(new Q3ListViewItem(ConVerilogA, s), true);
        else if(ext == "m" || ext == "oct")
          Content->setSelected(new Q3ListViewItem(ConOctave, s), true);
        else
          Content->setSelected(new Q3ListViewItem(ConOthers, s), true);
*/
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

  // refresh the schematic file path
  this->updateSchNameHash();
  this->updateSpiceNameHash();

  if(!ProjName.isEmpty())
    readProjectFiles();  // re-read the content ListView
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
  // Call update() to update subcircuit symbols in current Schematic document.
  // TextDoc has no viewport, it needs no update.
  QString tabType = DocumentTab->currentWidget()->className();
  if (tabType == "Schematic") {
    ((Q3ScrollView*)DocumentTab->currentPage())->viewport()->update();
  }
  view->drawn = false;
  statusBar()->message(tr("Ready."));

  // refresh the schematic file path
  this->updateSchNameHash();
  this->updateSpiceNameHash();
}

// --------------------------------------------------------------
// Close the currently active file tab
void QucsApp::slotFileClose()
{
    // Using file index -1 closes the currently active file window
    closeFile(-1);
}

// --------------------------------------------------------------
// Close the file tab specified by its index
void QucsApp::slotFileClose(int index)
{
    // Call closeFile with a specific tab index
    closeFile(index);
}

// --------------------------------------------------------------
// Common function to close a file tab specified by its index
// checking for changes in the file before doing so. If called
// index == -1, the active document will be closed
void QucsApp::closeFile(int index)
{
    statusBar()->message(tr("Closing file..."));
    editText->setHidden(true); // disable text edit of component property

    QucsDoc *Doc = getDoc(index);
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
  SaveDialog *sd = new SaveDialog(this);
  sd->setApp(this);
  for(int i=0; i < DocumentTab->count(); ++i) {
    QucsDoc *doc = getDoc(i);
    if(doc->DocChanged)
      sd->addUnsavedDoc(doc);
  }
  int Result = SaveDialog::DontSave;
  if(!sd->isEmpty())
     Result = sd->exec();
  delete sd;
  if(Result == SaveDialog::AbortClosing)
    return false;
  QucsDoc *doc = 0;
  while((doc = getDoc()) != 0)
	delete doc;


  switchEditMode(true);   // set schematic edit mode
  return true;
}


void QucsApp::slotFileExamples()
{
  statusBar()->message(tr("Open examples directory..."));
  QString path = QDir::toNativeSeparators(QucsSettings.ExamplesDir);
  QDesktopServices::openUrl(QUrl("file:///" + path.replace("\\","/")));
  statusBar()->message(tr("Ready."));
}

void QucsApp::slotHelpTutorial()
{
  QString path = QDir::toNativeSeparators(QucsSettings.DocDir);
  QUrl url = QUrl("file:///" + path.replace("\\","/") + "tutorial/" + QObject::sender()->objectName());
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpTechnical()
{
  QString path = QDir::toNativeSeparators(QucsSettings.DocDir);
  QUrl url = QUrl("file:///" + path.replace("\\","/") + "technical/" + QObject::sender()->objectName());
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpReport()
{
  QString path = QDir::toNativeSeparators(QucsSettings.DocDir);
  QUrl url = QUrl("file:///" + path.replace("\\","/") + "report/" + QObject::sender()->objectName());
  QDesktopServices::openUrl(url);
}



// --------------------------------------------------------------
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(QWidget *w)
{

  editText->setHidden (true); // disable text edit of component property
  QucsDoc * Doc;
  if(w==NULL)return;
  // for text documents
  if (isTextDocument (w)) {
    TextDoc *d = (TextDoc*)w;
    Doc = (QucsDoc*)d;
    // update menu entries, etc. if neccesary
    magAll->setDisabled(true);
    if(cursorLeft->isEnabled())
      switchSchematicDoc (false);
  }
  // for schematic documents
  else {
    Schematic *d = (Schematic*)w;
    Doc = (QucsDoc*)d;
    magAll->setDisabled(false);
    // already in schematic?
    if(cursorLeft->isEnabled()) {
      // which mode: schematic or symbol editor ?
      if((CompChoose->count() > 1) == d->symbolMode)
        changeSchematicSymbolMode (d);
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
void QucsApp::slotFileSettings ()
{
  editText->setHidden (true); // disable text edit of component property

  QWidget * w = DocumentTab->currentPage ();
  if (isTextDocument (w)) {
    QucsDoc * Doc = (QucsDoc *) ((TextDoc *) w);
    QString ext = Doc->fileSuffix ();
    // Octave properties
    if (ext == "m" || ext == "oct") {
    }
    // Verilog-A properties
    else if (ext == "va") {
      VASettingsDialog * d = new VASettingsDialog ((TextDoc *) w);
      d->exec ();
    }
    // VHDL and Verilog-HDL properties
    else {
      DigiSettingsDialog * d = new DigiSettingsDialog ((TextDoc *) w);
      d->exec ();
    }
  }
  // schematic properties
  else {
    SettingsDialog * d = new SettingsDialog ((Schematic *) w);
    d->exec ();
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
void QucsApp::slotRefreshSchPath()
{
  this->updateSchNameHash();
  this->updateSpiceNameHash();

  QMessageBox msgBox;
  msgBox.setText("The schematic file path has been refreshed.");
  msgBox.exec();
}


// --------------------------------------------------------------
void QucsApp::updatePortNumber(QucsDoc *currDoc, int No)
{
  if(No<0) return;

  QString pathName = currDoc->DocName;
  QString ext = currDoc->fileSuffix ();
  QFileInfo Info (pathName);
  QString Model, File, Name = Info.fileName();

  if (ext == "sch") {
    Model = "Sub";

    // enter new port number into ListView
    // TODO I'm not sure if I do things correctly here -> RECHECK!!!
    QTreeWidgetItem *p;
    //for(p = ConSchematics->firstChild(); p!=0; p = p->nextSibling()) {
    for(int i=0; i<ConSchematics->childCount(); i++) {
      p = ConSchematics->child(i);
      if(p->text(0) == Name) {
        if(No == 0) p->setText(1,"");
        else p->setText(1,QString::number(No)+tr("-port"));
        break;
      }
    }
  }
  else if (ext == "vhdl" || ext == "vhd") {
    Model = "VHDL";
  }
  else if (ext == "v") {
    Model = "Verilog";
  }

  // update all occurencies of subcircuit in all open documents
  No = 0;
  QWidget *w;
  Component *pc_tmp;
  while((w=DocumentTab->page(No++)) != 0) {
    if(isTextDocument (w))  continue;

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
// TODO -> in case of textdocument, cast to QPlainTextEdit & print
void QucsApp::printCurrentDocument(bool fitToPage)
{
  statusBar()->message(tr("Printing..."));
  editText->setHidden(true); // disable text edit of component property

  if(isTextDocument (DocumentTab->currentPage())) {
    QWidget *w;
    w = DocumentTab->currentPage();
    QPlainTextEdit *temp =  (QPlainTextEdit*)w;

    QPrintDialog *dialog = new QPrintDialog(Printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
     if (dialog->exec() == QDialog::Accepted)
         temp->print(Printer);

  }
    //Printer->setOrientation(QPrinter::Portrait);
  else {
    Printer->setOrientation(QPrinter::Landscape);

  //Printer->setPrintRange(QPrinter::AllPages);

  if(Printer->setup(this)) {   // printer dialog

    QPainter Painter(Printer);
    if(!Painter.device())      // valid device available ?
      goto Error;

    for(int z=Printer->numCopies(); z>0 ; z--) {
      if(Printer->aborted())
        break;

      getDoc()->print(Printer, &Painter,
              Printer->printRange() == QPrinter::AllPages, fitToPage);
      if(z > 1)
        if(!Printer->newPage())
          goto Error;
    }
  }
  }
  statusBar()->message(tr("Ready."));
  return;

Error:
  statusBar()->message(tr("Printer Error."));
}

// --------------------------------------------------------------
void QucsApp::slotFilePrint()
{
  printCurrentDocument(false);
}

// --------------------------------------------------------------
// Fit printed content to page size.
void QucsApp::slotFilePrintFit()
{
  printCurrentDocument(true);
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
    qDebug()<<"x"<<pos().x()<<" ,y"<<pos().y();
    qDebug()<<"dx"<<size().width()<<" ,dy"<<size().height();
    QucsSettings.x=pos().x();
    QucsSettings.y=pos().y();
    QucsSettings.dx=size().width();
    QucsSettings.dy=size().height();
    saveApplSettings(this);

   if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
      Event->accept();
      qApp->quit();
   }
   else
      Event->ignore();
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(isTextDocument (Doc)) {
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
  if(isTextDocument (Doc)) {
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

  QString s = pc->getSubcircuitFile();
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
  getDoc()->zoomBy(0.5f);
}


/*!
 * \brief QucsApp::slotSimulate
 *  is called when the simulate toolbar button is pressed.
 */
void QucsApp::slotSimulate()
{
  editText->setHidden(true); // disable text edit of component property

  QucsDoc *Doc;
  QWidget *w = DocumentTab->currentPage();
  if(isTextDocument (w)) {
    Doc = (QucsDoc*)((TextDoc*)w);
    if(Doc->SimTime.isEmpty() && ((TextDoc*)Doc)->simulation) {
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
  QFileInfo Info(Doc->DocName);
  if(Doc->lastSaved.isValid()) {
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

  if(Info.extension(false) == "m" || Info.extension(false) == "oct") {
    // It is an Octave script.
    if(Doc->DocChanged)
      Doc->save();
    slotViewOctaveDock(true);
    octave->runOctaveScript(Doc->DocName);
    return;
  }

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

  if(sim->ErrText->document()->lineCount() > 1)   // were there warnings ?
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
    }
  }
  else {
    if(sim->SimRunScript) {
      // run script
      octave->startOctave();
      octave->runOctaveScript(sim->Script);
    }
    if(sim->SimOpenDpl) {
      // switch to data display
      if(sim->DataDisplay.right(2) == ".m" ||
	 sim->DataDisplay.right(4) == ".oct") {  // Is it an Octave script?
	octave->startOctave();
	octave->runOctaveScript(sim->DataDisplay);
      }
      else
	slotChangePage(sim->DocName, sim->DataDisplay);
      sim->slotClose();   // close and delete simulation window
    }
    else
      if(w) if(!isTextDocument (sim->DocWidget))
	// load recent simulation data (if document is still open)
	((Schematic*)sim->DocWidget)->reloadGraphs();
  }

  if(!isTextDocument (sim->DocWidget))
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

  int z = 0;  // search, if page is already loaded
  QucsDoc * d = findDoc (Name, &z);

  if(d)
    DocumentTab->setCurrentPage(z);
  else {   // no open page found ?
    QString ext = QucsDoc::fileSuffix (DataDisplay);
    if (ext != "vhd" && ext != "vhdl" && ext != "v" && ext != "va" &&
	ext != "oct" && ext != "m")
      d = new Schematic (this, Name);
    else
      d = new TextDoc (this, Name);

    QFile file(Name);
    if(file.open(QIODevice::ReadOnly)) {      // try to load document
      file.close();
      if(!d->load()) {
        delete d;
        view->drawn = false;
        return;
      }
    }
    else {
      if(file.open(QIODevice::ReadWrite)) {  // if document doesn't exist, create
        //TODO RECHECK!! new Q3ListViewItem(ConDisplays, DataDisplay); // add new name
        QTreeWidgetItem *temp = new QTreeWidgetItem(ConDisplays);
        temp->setText(0,DataDisplay);
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
    if(!isTextDocument (w))
      ((Schematic*)w)->reloadGraphs();  // ... changes, reload here !

  TabView->setCurrentPage (2);   // switch to "Component"-Tab
  if (Name.right(4) == ".dpl") {
    int i = Category::getModulesNr (QObject::tr("diagrams"));
    CompChoose->setCurrentItem (i);   // switch to diagrams
    slotSetCompView (i);
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

  if(d->DocName.right(2) == ".m" ||
     d->DocName.right(4) == ".oct")
    slotViewOctaveDock(true);
  else
    slotChangePage(d->DocName, d->DataDisplay);
}

// -------------------------------------------------------------------
// Is called when a double-click is made in the content ListView.
void QucsApp::slotOpenContent(QTreeWidgetItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  if(item == 0) return;   // no item was double clicked
  if(item->parent() == 0) return; // no document, but item "schematic", ...

/*
  QucsSettings.QucsWorkDir.setPath(QucsSettings.QucsHomeDir.path());
  QString p = ProjName+"_prj";
  if(!QucsSettings.QucsWorkDir.cd(p)) {
    QMessageBox::critical(this, tr("Error"),
			  tr("Cannot access project directory: ")+
              QucsSettings.QucsWorkDir.path()+QDir::separator()+p);
    return;
  }*/

  QFileInfo Info(QucsSettings.QucsWorkDir.filePath(item->text(0)));
  QString Suffix = Info.extension(false);

  if (Suffix == "sch" || Suffix == "dpl" || Suffix == "vhdl" ||
      Suffix == "vhd" || Suffix == "v" || Suffix == "va" ||
      Suffix == "m" || Suffix == "oct") {
    gotoPage(Info.absFilePath());
    updateRecentFilesList(Info.absFilePath());
    slotUpdateRecentFiles();

    if(item->text(1).isEmpty())     // is subcircuit ?
      if(Suffix == "sch") return;

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
      QProcess *Program = new QProcess();
      //Program->setCommunication(0);
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      env.insert("PATH", env.value("PATH") );
      Program->setProcessEnvironment(env);
      Program->start(com.join(" "));
      if(Program->state()!=QProcess::Running&&
              Program->state()!=QProcess::Starting) {
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
void QucsApp::slotSelectSubcircuit(QTreeWidgetItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  if(item == 0) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    return;
  }


  bool isVHDL = false;
  bool isVerilog = false;
  if(item->parent() == 0) return;
  if(item->parent()->text(0) == tr("Schematics")) {
    if(item->text(1).isEmpty())
      return;   // return, if not a subcircuit
  }
  else if(item->parent()->text(0) == tr("VHDL"))
    isVHDL = true;
  else if(item->parent()->text(0) == tr("Verilog"))
    isVerilog = true;
  else
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
  else if(isVerilog)
    Comp = new Verilog_File();
  else
    Comp = new Subcircuit();
  Comp->Props.first()->Value = item->text(0);
  Comp->recreate(0);
  view->selElem = Comp;

  if(view->drawn)
    ((Q3ScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
}


// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectLibComponent(QTreeWidgetItem *item)
{
    // get the current document
    Schematic *Doc = (Schematic*)DocumentTab->currentPage();

    // if the current document is a schematic activate the paste
    if(!Doc->inherits("QPlainTextEdit"))
    {

        if(item == 0)
        {
            // mouse button pressed not over an item ?
            Content->clearSelection();  // deselect component in ListView
            return;
        }

        // if theres not a higher level item, this is a top level item,
        // not a component item so return
        if(item->parent() == 0) return;

        if(item->text(1).isEmpty()) return;   // return, if not a subcircuit

        // copy the subcircuit schematic to the clipboard
        QClipboard *cb = QApplication::clipboard();
        cb->setText(item->text(1));

        // activate the paste command
        slotEditPaste (true);
    }

}


// ---------------------------------------------------------
// This function is called if the document type changes, i.e.
// from schematic to text document or vice versa.
void QucsApp::switchSchematicDoc (bool SchematicMode)
{
  // switch our scroll key actions on/off according to SchematicMode
  cursorLeft->setEnabled(SchematicMode);
  cursorRight->setEnabled(SchematicMode);
  cursorUp->setEnabled(SchematicMode);
  cursorDown->setEnabled(SchematicMode);

  // text document
  if (!SchematicMode) {
    if (activeAction) {
      activeAction->blockSignals (true); // do not call toggle slot
      activeAction->setOn (false);       // set last toolbar button off
      activeAction->blockSignals (false);
    }
    activeAction = select;
    select->blockSignals (true);
    select->setOn (true);
    select->blockSignals (false);
  }
  // schematic document
  else {
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }

  selectMarker->setEnabled (SchematicMode);
  alignTop->setEnabled (SchematicMode);
  alignBottom->setEnabled (SchematicMode);
  alignLeft->setEnabled (SchematicMode);
  alignRight->setEnabled (SchematicMode);
  centerHor->setEnabled (SchematicMode);
  centerVert->setEnabled (SchematicMode);
  distrHor->setEnabled (SchematicMode);
  distrVert->setEnabled (SchematicMode);
  onGrid->setEnabled (SchematicMode);
  moveText->setEnabled (SchematicMode);
  filePrintFit->setEnabled (SchematicMode);
  editRotate->setEnabled (SchematicMode);
  editMirror->setEnabled (SchematicMode);
  editMirrorY->setEnabled (SchematicMode);
  intoH->setEnabled (SchematicMode);
  popH->setEnabled (SchematicMode);
  dcbias->setEnabled (SchematicMode);
  insWire->setEnabled (SchematicMode);
  insLabel->setEnabled (SchematicMode);
  insPort->setEnabled (SchematicMode);
  insGround->setEnabled (SchematicMode);
  insEquation->setEnabled (SchematicMode);
  setMarker->setEnabled (SchematicMode);

  exportAsImage->setEnabled (SchematicMode); // only export schematic, no text

  editFind->setEnabled (!SchematicMode);
  editFindAgain->setEnabled (!SchematicMode);
  insEntity->setEnabled (!SchematicMode);

  buildModule->setEnabled(!SchematicMode); // only build if VA document
}

// ---------------------------------------------------------
void QucsApp::switchEditMode(bool SchematicMode)
{
  fillComboBox(SchematicMode);
  slotSetCompView(0);

  intoH->setEnabled(SchematicMode);
  popH->setEnabled(SchematicMode);
  editActivate->setEnabled(SchematicMode);
  changeProps->setEnabled(SchematicMode);
  insEquation->setEnabled(SchematicMode);
  insGround->setEnabled(SchematicMode);
  insPort->setEnabled(SchematicMode);
  insWire->setEnabled(SchematicMode);
  insLabel->setEnabled(SchematicMode);
  setMarker->setEnabled(SchematicMode);
  selectMarker->setEnabled(SchematicMode);
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
bool QucsApp::isTextDocument(QWidget *w) {
  if (w->inherits("QPlainTextEdit"))
    return true;
  return false;
}

// ---------------------------------------------------------
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: Schematic and (subcircuit)
// symbol.
void QucsApp::slotSymbolEdit()
{
  QWidget *w = DocumentTab->currentPage();

  // in a text document (e.g. VHDL)
  if (isTextDocument (w)) {
    TextDoc *TDoc = (TextDoc*)w;
    // set 'DataDisplay' document of text file to symbol file
    QFileInfo Info(TDoc->DocName);
    QString sym = Info.baseName(true)+".sym";
    TDoc->DataDisplay = sym;

    // symbol file already loaded?
    int paint_mode = 0;
    if (!findDoc (QucsSettings.QucsWorkDir.filePath(sym)))
      paint_mode = 1;

    // change current page to appropriate symbol file
    slotChangePage(TDoc->DocName,TDoc->DataDisplay);

    // set 'DataDisplay' document of symbol file to original text file
    Schematic *SDoc = (Schematic*)DocumentTab->currentPage();
    SDoc->DataDisplay = Info.fileName();

    // change into symbol mode
    if (paint_mode) // but only switch coordinates if newly loaded
      SDoc->switchPaintMode();
    SDoc->symbolMode = true;
    changeSchematicSymbolMode(SDoc);
    SDoc->becomeCurrent(true);
    SDoc->viewport()->update();
    view->drawn = false;
  }
  // in a normal schematic, data display or symbol file
  else {
    Schematic *SDoc = (Schematic*)w;
    // in a symbol file
    if(SDoc->DocName.right(4) == ".sym") {
      slotChangePage(SDoc->DocName, SDoc->DataDisplay);
    }
    // in a normal schematic
    else {
      editText->setHidden(true); // disable text edit of component property
      SDoc->switchPaintMode();   // twist the view coordinates
      changeSchematicSymbolMode(SDoc);
      SDoc->becomeCurrent(true);
      SDoc->viewport()->update();
      view->drawn = false;
    }
  }
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

// -----------------------------------------------------------
// Searches the qucs path list for all schematic files and creates
// a hash for lookup later
void QucsApp::updateSchNameHash(void)
{
    // update the list of paths to search in qucsPathList, this
    // removes nonexisting entries
    updatePathList();

    // now go through the paths creating a map to all the schematic files
    // found in the directories. Note that we go through the list of paths from
    // first index to last index. Since keys are unique it means schematic files
    // in directories at the end of the list take precendence over those at the
    // start of the list, we should warn about shadowing of schematic files in
    // this way in the future
    QStringList nameFilter;
    nameFilter << "*.sch";

    // clear out any existing hash table entriess
    schNameHash.clear();

    foreach (QString qucspath, qucsPathList) {
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        foreach (QFileInfo schfile, schfilesList) {
            QString bn = schfile.completeBaseName();
            schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    foreach (QFileInfo schfile, schfilesList) {
        schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
    }
}

// --------------------------------------------------------
// Produces a name filter suitible for file dialogs from the
// list of recognised spice extensions
QString QucsApp::getSpiceFileFilter (void)
{
    QString spexts = " (";

    for (int i = 0; i < spiceExtensions.count (); i++)
    {
        spexts += "*" + spiceExtensions[i] + " ";
    }

    spexts += ");;";

    return spexts;
}

// -----------------------------------------------------------
// Searches the qucs path list for all spice files and creates
// a hash for lookup later
void QucsApp::updateSpiceNameHash(void)
{
    // update the list of paths to search in qucsPathList, this
    // removes nonexisting entries
    updatePathList();

    // now go through the paths creating a map to all the spice files
    // found in the directories. Note that we go through the list of paths from
    // first index to last index. Since keys are unique it means spice files
    // in directories at the end of the list take precendence over those at the
    // start of the list, we should warn about shadowing of spice files in
    // this way in the future

    // clear out any existing hash table entriess
    spiceNameHash.clear();

    foreach (QString qucspath, qucsPathList) {
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList spicefilesList = thispath.entryInfoList( spiceExtensions, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        foreach (QFileInfo spicefile, spicefilesList) {
            QString bn = spicefile.completeBaseName();
            schNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList spicefilesList = thispath.entryInfoList( spiceExtensions, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    foreach (QFileInfo spicefile, spicefilesList) {
        spiceNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
    }
}

// -----------------------------------------------------------
// update the list of paths, pruning non-existing paths
void QucsApp::updatePathList(void)
{
    // check each path actually exists, if not remove it
    QMutableListIterator<QString> i(qucsPathList);
    while (i.hasNext()) {
        i.next();
        QDir thispath(i.value());
        if (!thispath.exists())
        {
            // the path does not exist, remove it from the list
            i.remove();
        }
    }
}

// replace the old path list with a new one
void QucsApp::updatePathList(QStringList newPathList)
{
    // clear out the old path list
    qucsPathList.clear();

    // copy the new path into the path list
    foreach(QString path, newPathList)
    {
        qucsPathList.append(path);
    }
    // do the normal path update operations
    updatePathList();
}


void QucsApp::updateRecentFilesList(QString s)
{
    QSettings* settings = new QSettings("qucs","qucs");
    QucsSettings.numRecentDocs++;
    if (!QucsSettings.RecentDocs.contains(s)) {
        QucsSettings.RecentDocs.append(s);
    } else {
        QucsSettings.RecentDocs.remove(s);
        QucsSettings.RecentDocs.append(s);
    }
    if (QucsSettings.RecentDocs.count()>8) {
        QucsSettings.RecentDocs.removeFirst();
    }

    settings->setValue("RecentDocs",QucsSettings.RecentDocs.join("*"));
    delete settings;
}

void QucsApp::slotSaveDiagramToGraphicsFile()
{
    slotSaveSchematicToGraphicsFile(true);
}

void QucsApp::slotSaveSchematicToGraphicsFile(bool diagram)
{
    Schematic *sch = (Schematic*)DocumentTab->currentPage();

    const int bourder = 30;

    int w,h,wsel,hsel,
        xmin, ymin, xmin_sel, ymin_sel;

    sch->getSchWidthAndHeight(w, h, xmin, ymin);
    sch->getSelAreaWidthAndHeight(wsel, hsel, xmin_sel, ymin_sel);
    w += bourder;
    h += bourder;
    wsel += bourder;
    hsel += bourder;

    bool noselect;
    if ((wsel==(bourder+1))&&(hsel==(bourder+1))) noselect = true;
    else noselect = false;

    ExportDialog* dlg = new ExportDialog(w,h,wsel,hsel,lastExportFilename,noselect,this);

    if (diagram) dlg->setDiagram();

    if (dlg->exec()) {

        QString filename = dlg->FileToSave();
        lastExportFilename = filename;

        bool exportAll;
        if (dlg->isExportSelected()) {
            exportAll = false;
            w=wsel;
            h=hsel;
            xmin = xmin_sel;
            ymin = ymin_sel;
        } else {
            exportAll = true;
        }

        float scal;
        if (!dlg->isOriginalSize()) {
            scal = (float) dlg->Xpixels()/w;
            w = round(w*scal);
            h = round(h*scal);
        } else {
            scal = 1.0;
        }

        if (dlg->isValidFilename()) {
            if (!dlg->isSvg()) {

                QImage* img;

                switch (dlg->getImgFormat()) {
                case ExportDialog::Coloured : img = new QImage(w,h,QImage::Format_RGB888);
                    break;
                case ExportDialog::Monochrome : img = new QImage(w,h,QImage::Format_Mono);
                    break;
                default : break;
                }

                QPainter* p = new QPainter(img);
                p->fillRect(0,0,w,h,Qt::white);
                ViewPainter* vp = new ViewPainter(p);
                vp->init(p,scal,0,0,xmin*scal-bourder/2,ymin*scal-bourder/2,scal,scal);

                sch->paintSchToViewpainter(vp,exportAll,true);

                img->save(filename);

                delete vp;
                delete p;
                delete img;
            } else {
                QSvgGenerator* svg1 = new QSvgGenerator();
                svg1->setResolution(this->physicalDpiX());

                if (dlg->needsInkscape()) {
                    svg1->setFileName(filename+".tmp.svg");
                } else {
                    svg1->setFileName(filename);
                }

                //svg1->setSize(QSize(1.12*w,1.1*h));
                svg1->setSize(QSize(1.12*w,h));
                QPainter *p = new QPainter(svg1);
                p->fillRect(0,0,svg1->size().width(),svg1->size().height(),Qt::white);
                ViewPainter *vp = new ViewPainter(p);
                vp->init(p,1.0,0,0,xmin-bourder/2,ymin-bourder/2,1.0,1.0);

                sch->paintSchToViewpainter(vp,exportAll,true);

                delete vp;
                delete p;
                delete svg1;

                if (dlg->needsInkscape()) {
                    QString cmd = "inkscape -z -D --file=";
                    cmd += filename+".tmp.svg ";

                    if (dlg->isPdf_Tex()) {
                        QString tmp = filename;
                        tmp.chop(4);
                        cmd = cmd + "--export-pdf="+ tmp + " --export-latex";
                    }

                    if (dlg->isPdf()) {
                        cmd = cmd + "--export-pdf=" + filename;
                    }

                    if (dlg->isEps()) {
                        cmd = cmd + "--export-eps=" + filename;
                    }

                    int result = QProcess::execute(cmd);

                    if (result!=0) {
                        QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,tr("Export to image"),
                                                            tr("Inkscape start error!"),
                                                            QMessageBox::Ok);
                        msg->exec();
                        delete msg;
                    }
                    QFile::remove(filename+".tmp.svg");
                }
            }

            successExportMessages(QFile::exists(filename));

        } else {
            QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,tr("Export to image"),
                                                tr("Unsupported format of graphics file. \n"
                                                "Use PNG, JPEG or SVG graphics!"),
                                                QMessageBox::Ok);
            msg->exec();
            delete msg;
        }


    }

}



void QucsApp::successExportMessages(bool ok)
{
    if (ok) {
        QMessageBox* msg =  new QMessageBox(QMessageBox::Information,tr("Export to image"),
                                        tr("Sucessfully exported!"),
                                        QMessageBox::Ok);
        msg->exec();
        delete msg;
    } else {
        QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,tr("Export to image"),
                                        tr("Disk write error!"),
                                        QMessageBox::Ok);
        msg->exec();
        delete msg;
    }
}
