/***************************************************************************
    copyright            : (C) 2003, 2004, 2005, 2006 by Michael Margraf
                               2018, 2020 Felix Salfelder
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
# include "config.h"
#endif

#include <QModelIndex>
#include <QAction>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QShortcut>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QUrl>
#include <QSettings>
#include <QVariant>
#include <QDebug>
#include <QPlainTextEdit> // what?

#include "qucs.h"
#include "qucsdoc.h"
#include "mouseactions.h"
#include "messagedock.h"
#include "module.h"
#include "projectView.h"
#include "component_widget.h"

// BUG: cleanup dialogs. how?!
#include "savedialog.h"
#include "newprojdialog.h"
#include "settingsdialog.h"
#include "digisettingsdialog.h"
#include "vasettingsdialog.h"
#include "qucssettingsdialog.h"
#include "searchdialog.h"
#include "sweepdialog.h"
#include "labeldialog.h"
#include "matchdialog.h"
#include "simmessage.h"
#include "exportdialog.h"
#include "octave_window.h"
#include "printerwriter.h"
#include "imagewriter.h"

#include "settings.h"
#include "../qucs-lib/qucslib_common.h" // BUG
#include "misc.h"
#include "platform.h"
#include "globals.h"
#include "qucs_tabs.h"

#include "simulator.h"

struct iconCompInfoStruct{
  int catIdx; // index of the component Category
  int compIdx; // index of the component itself in the Category
};

Q_DECLARE_METATYPE(iconCompInfoStruct)

// declare extern variables
tQucsSettings QucsSettings;  // nearly everywhere used
QString lastDir;    // to remember last directory for several dialogs
QStringList qucsPathList;
VersionTriplet QucsVersion; // Qucs version string
QucsApp *QucsMain = 0;  // the Qucs application itself

typedef enum {
    Project = 0,
    Content,
    Components,
    Libraries,
} TabViewTabs;


/*!
 * \brief QucsApp::QucsApp main application
 */
QucsApp::QucsApp()
{itested();
  setWindowTitle("Qucs " PACKAGE_VERSION);

  QucsFileFilter =
    tr("Schematic") + " (*.sch);;" +
    tr("Data Display") + " (*.dpl);;" +
    tr("Qucs Documents") + " (*.sch *.dpl);;" +
    tr("VHDL Sources") + " (*.vhdl *.vhd);;" +
    tr("Verilog Sources") + " (*.v);;" +
    tr("Verilog-A Sources") + " (*.va);;" +
    tr("Octave Scripts") + " (*.m *.oct);;" +
    tr("Spice Files") + QString(" (") + QucsSettings.spiceExtensions.join(" ") + QString(");;") +
    tr("Any File")+" (*)";

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
  viewToolBar->setChecked(true);
  viewStatusBar->setChecked(true);
  viewBrowseDock->setChecked(true);
  slotViewOctaveDock(false);
  slotUpdateRecentFiles();
  initCursorMenu();
  Module::registerModules ();

  // instance of small text search dialog
  SearchDia = new SearchDialog(this);

  // creates a document called "untitled"
  DocumentTab->createEmptySchematic("");

  select->setChecked(true);  // switch on the 'select' action
  switchSchematicDoc(true);  // "untitled" document is schematic

  lastExportFilename = QDir::homePath() + QDir::separator() + "export.png";

  // load documents given as command line arguments
  // // ARGH. this is argv argc
  // TODO: move to main.cc...
  for(int z=1; z<qApp->arguments().size(); z++) {itested();
    QString arg = qApp->arguments()[z];
    QByteArray ba = arg.toLatin1();
    const char *c_arg= ba.data();
    if(*(c_arg) == '-') {itested();
        // do not open files with '-'
    }else{itested();
      QFileInfo Info(arg);
      QucsSettings.QucsWorkDir.setPath(Info.absoluteDir().absolutePath());
      arg = QucsSettings.QucsWorkDir.filePath(Info.fileName());
      gotoPage(arg);
    }
  }
}



QucsApp::~QucsApp()
{ untested();
  Module::unregisterModules ();
}


// #######################################################################
// ##########                                                   ##########
// ##########     Creates the working area (QTabWidget etc.)    ##########
// ##########                                                   ##########
// #######################################################################
/**
 * @brief QucsApp::initView Setup the layout of all widgets
 */
void QucsApp::initView()
{itested();
  // set application icon
  // APPLE sets the QApplication icon with Info.plist
#ifndef __APPLE__
  // BUG: platform.h
  setWindowIcon (QPixmap(":/bitmaps/big.qucs.xpm"));
#endif

  DocumentTab = new QucsTabWidget(this);
  assert(DocumentTab);
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

  dock = new QDockWidget(tr("Main Dock"),this);
  TabView = new QTabWidget(dock);
  TabView->setTabPosition(QTabWidget::West);

  connect(dock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleDock(bool)));

#if 0
  view = new MouseActions(*this);
#endif


  { // something about editText
    editText = new QLineEdit(this);  // for editing component properties
    editText->setFrame(false);
    editText->setHidden(true);

    misc::setWidgetBackgroundColor(editText, QucsSettings.BGColor);

    connect(editText, SIGNAL(returnPressed()), SLOT(slotApplyCompText()));
    connect(editText, SIGNAL(textChanged(const QString&)),
            SLOT(slotResizePropEdit(const QString&)));
    connect(editText, SIGNAL(lostFocus()), SLOT(slotHideEdit()));
  }

  // ----------------------------------------------------------
  // "Project Tab" of the left QTabWidget
  QWidget *ProjGroup = new QWidget();
  QVBoxLayout *ProjGroupLayout = new QVBoxLayout();
  QWidget *ProjButts = new QWidget();
  QPushButton *ProjNew = new QPushButton(tr("New"));
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotButtonProjNew()));
  QPushButton *ProjOpen = new QPushButton(tr("Open"));
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotButtonProjOpen()));
  QPushButton *ProjDel = new QPushButton(tr("Delete"));
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotButtonProjDel()));

  QHBoxLayout *ProjButtsLayout = new QHBoxLayout();
  ProjButtsLayout->addWidget(ProjNew);
  ProjButtsLayout->addWidget(ProjOpen);
  ProjButtsLayout->addWidget(ProjDel);
  ProjButts->setLayout(ProjButtsLayout);

  ProjGroupLayout->addWidget(ProjButts);

  Projects = new QListView();

  ProjGroupLayout->addWidget(Projects);
  ProjGroup->setLayout(ProjGroupLayout);

  int tabviewIndex;

  TabView->addTab(ProjGroup, tr("Projects"));
  tabviewIndex = TabView->insertTab(TabViewTabs::Project, ProjGroup, tr("Projects"));
  assert(tabviewIndex == TabViewTabs::Project);
  TabView->setTabToolTip(TabViewTabs::Project, tr("content of project directory"));


  connect(Projects, &QListView::doubleClicked, this, &QucsApp::slotListProjOpen);

  // ----------------------------------------------------------
  {itested();
    // "Content" Tab of the left QTabWidget
    Content = new ProjectView(this);
    Content->setContextMenuPolicy(Qt::CustomContextMenu);

    tabviewIndex = TabView->insertTab(TabViewTabs::Content, Content, tr("Content"));
    assert(tabviewIndex == TabViewTabs::Content);
    TabView->setTabToolTip(TabViewTabs::Content, tr("content of current project"));

    connect(Content, SIGNAL(clicked(const QModelIndex &)), 
            SLOT(slotSelectSubcircuit(const QModelIndex &)));

    connect(Content, SIGNAL(doubleClicked(const QModelIndex &)),
            SLOT(slotOpenContent(const QModelIndex &)));
  }

  // ----------------------------------------------------------
  // "Component" Tab of the left QTabWidget
  QWidget *CompGroup  = new QWidget();
  QVBoxLayout *CompGroupLayout = new QVBoxLayout();
  QHBoxLayout *CompSearchLayout = new QHBoxLayout();

  CompChoose = new QComboBox(this);
  CompComps = new ComponentWidget(this);
  CompComps->setViewMode(QListView::IconMode);
  CompComps->setGridSize(QSize(110,90));
  CompSearch = new QLineEdit(this);
  CompSearch->setPlaceholderText(tr("Search Components"));
  CompSearchClear = new QPushButton(tr("Clear"));

  CompGroupLayout->setSpacing(5);
  CompGroupLayout->addWidget(CompChoose);
  CompGroupLayout->addWidget(CompComps);
  CompGroupLayout->addLayout(CompSearchLayout);
  CompSearchLayout->addWidget(CompSearch);
  CompSearchLayout->addWidget(CompSearchClear);
  CompGroup->setLayout(CompGroupLayout);

  tabviewIndex = TabView->insertTab(TabViewTabs::Components, CompGroup,tr("Components"));
  assert(tabviewIndex == TabViewTabs::Components);
  TabView->setTabToolTip(TabViewTabs::Components, tr("components and diagrams"));
  fillComboBox(true);

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompComps, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompSearch, SIGNAL(textEdited(const QString &)), SLOT(slotSearchComponent(const QString &)));
  connect(CompSearchClear, SIGNAL(clicked()), SLOT(slotSearchClear()));

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
  QStringList headers;
  headers.clear ();
  headers << tr ("Libraries");
  libTreeWidget->setHeaderLabels (headers);

  LibGroupLayout->addWidget (libTreeWidget);
  LibGroup->setLayout (LibGroupLayout);

  fillLibrariesTreeView ();

  tabviewIndex = TabView->insertTab(TabViewTabs::Libraries, LibGroup, tr("Libraries"));
  assert(tabviewIndex == TabViewTabs::Libraries);
  TabView->setTabToolTip (TabViewTabs::Libraries, tr ("system and user component libraries"));

  connect(libTreeWidget, SIGNAL(itemPressed (QTreeWidgetItem*, int)),
           SLOT(slotSelectLibComponent (QTreeWidgetItem*)));

  // ----------------------------------------------------------
  // put the tab widget in the dock
  dock->setWidget(TabView);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->addDockWidget(Qt::LeftDockWidgetArea, dock);
  TabView->setCurrentIndex(TabViewTabs::Project);

  // ----------------------------------------------------------
  // Octave docking window
  octDock = new QDockWidget(tr("Octave Dock"));

  connect(octDock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleOctave(bool)));
  octave = new OctaveWindow(octDock);
  this->addDockWidget(Qt::BottomDockWidgetArea, octDock);
  this->setCorner(Qt::BottomLeftCorner  , Qt::LeftDockWidgetArea);

  // ............................................

  messageDock = new MessageDock(this);

  // initial projects directory model
  m_homeDirModel = new QucsFileSystemModel(this);
  m_proxyModel = new QucsSortFilterProxyModel();
  //m_proxyModel->setDynamicSortFilter(true);
  // show all directories (project and non-project)
  m_homeDirModel->setFilter(QDir::NoDot | QDir::AllDirs);

  // ............................................
  QString path = QucsSettings.QucsHomeDir.absolutePath();
  QDir ProjDir(path);
  // initial projects directory is the Qucs home directory
  QucsSettings.projsDir.setPath(path);

  // create home dir if not exist
  if(!ProjDir.exists()) { untested();
    if(!ProjDir.mkdir(path)) { untested();
      QMessageBox::warning(this, tr("Warning"),
          tr("Cannot create work directory !"));
      return;
    }
  }
  readProjects(); // reads all projects and inserts them into the ListBox
}

// Put all available libraries into ComboBox.
// // BUG no here.
void QucsApp::fillLibrariesTreeView ()
{itested();
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

    QDir LibDir(QucsSettings.libDir());
    LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

    // create top level library items, base on the library names
    for(it = LibFiles.begin(); it != LibFiles.end(); it++) {itested();
        QString libPath(*it);
        libPath.chop(4); // remove extension

        ComponentLibrary parsedlibrary;

        int result = parseComponentLibrary (libPath , parsedlibrary);
        QStringList nameAndFileName;
        nameAndFileName.append (parsedlibrary.name);
        nameAndFileName.append (QucsSettings.libDir() + *it);

        QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)0, nameAndFileName);

        switch (result) {
            case QUCS_COMP_LIB_IO_ERROR:
            { untested();
                QString filename = getLibAbsPath(libPath);
                QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
                return;
            }
            case QUCS_COMP_LIB_CORRUPT:
                QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
                return;
            default:
                break;
        }

        for (int i = 0; i < parsedlibrary.components.count (); i++) {itested();
            QStringList compNameAndDefinition;

            compNameAndDefinition.append (parsedlibrary.components[i].name);

            QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

            s +=  "<Components>\n  " +
                  parsedlibrary.components[i].modelString + "\n" +
                  "</Components>\n";

            compNameAndDefinition.append (s);

            QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);

            // Silence warning from the compiler about unused variable newcompitem
            // we pass the pointer to the parent item in the constructor
            Q_UNUSED( newcompitem )
        }

        topitems.append (newlibitem);
    }


    // make the user libraries section header
    newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("User Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    newitem->setFont (0, sectionFont);
    topitems.append (newitem);

    QDir UserLibDir = QDir (QucsSettings.QucsHomeDir.canonicalPath () + "/user_lib/");

    LibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    int UserLibCount = LibFiles.count();

    if (UserLibCount > 0) { untested();
      // there are user libraries

        // create top level library itmes, base on the library names
        for(it = LibFiles.begin(); it != LibFiles.end(); it++) { untested();
            QString libPath(UserLibDir.absoluteFilePath(*it));
            libPath.chop(4); // remove extension

            ComponentLibrary parsedlibrary;

            int result = parseComponentLibrary (libPath, parsedlibrary);
            QStringList nameAndFileName;
            nameAndFileName.append (parsedlibrary.name);
            nameAndFileName.append (UserLibDir.absolutePath() +"/"+ *it);

            QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)0, nameAndFileName);

            switch (result) {
                case QUCS_COMP_LIB_IO_ERROR:
                { untested();
                    QString filename = getLibAbsPath(libPath);
                    QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
                    return;
                }
                case QUCS_COMP_LIB_CORRUPT:
                    QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
                    return;
                default:
                    break;
            }

            for (int i = 0; i < parsedlibrary.components.count (); i++) { untested();
                QStringList compNameAndDefinition;

                compNameAndDefinition.append (parsedlibrary.components[i].name);

                QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

                s +=  "<Components>\n  " +
                      parsedlibrary.components[i].modelString + "\n" +
                      "</Components>\n";

                compNameAndDefinition.append (s);


                QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);

                // Silence warning from the compiler about unused variable newcompitem
                // we pass the pointer to the parent item in the constructor
                Q_UNUSED( newcompitem )
            }

            topitems.append (newlibitem);
        }
        libTreeWidget->insertTopLevelItems(0, topitems);
    } else {itested();
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
{itested();
  QWidget *w;
  if(No < 0){ untested();
    return DocumentTab->current();
  } else{itested();
    w = DocumentTab->widget(No);
  }

  return dynamic_cast<QucsDoc*>(w);
}

// ---------------------------------------------------------------
/*!
 * \brief QucsApp::findDoc
 * Returns a pointer to the QucsDoc object whose file name is "Name".
 * \param File
 * \param Pos
 * \return
 */
QucsDoc * QucsApp::findDoc (QString File, int * Pos)
{itested();
  QucsDoc * d;
  int No = 0;
  File = QDir::toNativeSeparators(File);
  while ((d = getDoc (No++)) != 0){itested();
    if (QDir::toNativeSeparators(d->docName()) == File) { untested();
      if (Pos) { untested();
        *Pos = No - 1;
      }else{ untested();
      }
      return d;
    }else{itested();
    }
  }
  return 0;
}

// ---------------------------------------------------------------
// Put the component groups into the ComboBox. It is possible to
// only put the paintings in it, because of "symbol painting mode".

// if setAll, add all categories to combobox
// if not, set just paintings (symbol painting mode)
void QucsApp::fillComboBox (bool setAll)
{itested();
  //CompChoose->setMaxVisibleItems (13); // Increase this if you add items below.
  CompChoose->clear ();
  CompSearch->clear(); // clear the search box, in case search was active...

  if (!setAll) { untested();
    CompChoose->insertItem(CompChoose->count(), QObject::tr("paintings"));
  } else {itested();
    QStringList cats = Category::getCategories ();
    foreach (QString it, cats) {itested();
      CompChoose->insertItem(CompChoose->count(), it);
    }
  }
}

// ----------------------------------------------------------
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriate components.
//
// BUG: not here.
void QucsApp::slotSetCompView (int index)
{itested();
  trace1("QucsApp::slotSetCompView", index);

  editText->setHidden (true); // disable text edit of component property

  QList<Module *> Comps;
  if (CompChoose->count () <= 0) return;

  // was in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) { untested();
    if (index == 0) // user selected "Search results" item
      return;
    CompChoose->removeItem(0);
    CompSearch->clear(); // clear the search box
    --index; // adjust requested index since item 0 was removed
  }
  CompComps->clear ();   // clear the IconView

  // make sure the right index is selected
  //  (might have been called by a cleared search and not by user action)
  CompChoose->setCurrentIndex(index);
  int compIdx;
  iconCompInfoStruct iconCompInfo;
  QVariant v;
  QString item = CompChoose->itemText (index);
  int catIdx = Category::getModulesNr(item);
  //int catIdx = Category::categories.getModulesNr(item);

  Comps = Category::getModules(item);
  QString Name;

  // if something was registered dynamicaly, get and draw icons into dock
  if (item == QObject::tr("verilog-a user devices")) { untested();
    unreachable();
    incomplete();

    compIdx = 0;
    QMapIterator<QString, QString> i(Module::vaComponents);
    while (i.hasNext()) { untested();
      i.next();

      // default icon initally matches the module name
      //Name = i.key();

      // Just need path to bitmap, do not create an object
      QString Name, vaBitmap;
		incomplete();
/// what's this?
///       Component * c = (Component *)
///               vacomponent::info (Name, vaBitmap, false, i.value());
///       if (c) delete c;

      // check if icon exists, fall back to default
      QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

      QFile iconFile(iconPath);
      QPixmap vaIcon;

      if(iconFile.exists())
      { untested();
        // load bitmap defined on the JSON symbol file
        vaIcon = QPixmap(iconPath);
      }else{ untested();
        QMessageBox::information(this, tr("Info"),
                     tr("Default icon not found:\n %1.png").arg(vaBitmap));
        // default icon
        vaIcon = QPixmap(":/bitmaps/editdelete.png");
      }
      QListWidgetItem *icon = new QListWidgetItem(vaIcon, Name);
      icon->setToolTip(Name);
      iconCompInfo = iconCompInfoStruct{catIdx, compIdx};
      v.setValue(iconCompInfo);
      icon->setData(Qt::UserRole, v);
      CompComps->addItem(icon);
      compIdx++;
    }
  } else {itested();
    QString File;
    // Populate list of ComponentListWidgetItems
    // // BUG: not here.
    QList<Module *>::const_iterator it;
    for (it = Comps.constBegin(); it != Comps.constEnd(); it++) {itested();
      if (Element const* e = (*it)->element()) {itested();
        Name = e->description();
        File = e->iconBasename();
        trace1("adding icon?", File);
        QListWidgetItem *icon = new ComponentListWidgetItem(e);
      //   iconCompInfo = iconCompInfoStruct{catIdx, compIdx};
      //   v.setValue(iconCompInfo);
      //   icon->setData(Qt::UserRole, v);
        CompComps->addItem(icon); // takes ownership?
      }else{ untested();
        incomplete();
      }
    }
  }
}

// ------------------------------------------------------------------
/*!
 * \brief QucsApp::slotSearchComponent
 *  When CompSearch is being edited, create a temp page show the search result
 * \param searchText
 */
void QucsApp::slotSearchComponent(const QString &searchText)
{ untested();
  incomplete();
  qDebug() << "User search: " << searchText;
  CompComps->clear ();   // clear the IconView

  // not already in "search mode"
  if (CompChoose->itemText(0) != tr("Search results")) { untested();
    ccCurIdx = CompChoose->currentIndex(); // remember current panel
    // insert "Search results" at the beginning, so that it is visible
    CompChoose->insertItem(-1, tr("Search results"));
    CompChoose->setCurrentIndex(0);
  }

  if (searchText.isEmpty()) { untested();
    slotSetCompView(CompChoose->currentIndex());
  } else { untested();
    CompChoose->setCurrentIndex(0); // make sure the "Search results" category is selected
    editText->setHidden (true); // disable text edit of component property

    //traverse all component and match searchText with name
    QList<Module *> Comps;
    iconCompInfoStruct iconCompInfo;
    QVariant v;

    QStringList cats = Category::getCategories ();
    int catIdx = 0;
    foreach(QString it, cats) { untested();
      // this will go also over the "verilog-a user devices" category, if present
      //   but since modules there have no 'info' function it won't handle them
      Comps = Category::getModules(it);
      QList<Module *>::const_iterator modit;
      int compIdx = 0;
      for (modit = Comps.constBegin(); modit != Comps.constEnd(); modit++) { untested();
        if (Element const* e = (*modit)->element()) { untested();
          QString File = e->iconBasename();
          auto Name = QString::fromStdString(e->label());

          if((Name.indexOf(searchText, 0, Qt::CaseInsensitive)) != -1) { untested();
            incomplete();
            //match
            QListWidgetItem *icon = new QListWidgetItem(QPixmap(":/bitmaps/" + File + ".png"), Name);
            icon->setToolTip(it + ": " + Name);
            // add component category and module indexes to the icon
            iconCompInfo = iconCompInfoStruct{catIdx, compIdx};
            v.setValue(iconCompInfo);
            icon->setData(Qt::UserRole, v);
            CompComps->addItem(icon);
          }
        }
        compIdx++;
      }
      catIdx++;
    }
    // the "verilog-a user devices" is the last category, if present
    QMapIterator<QString, QString> i(Module::vaComponents);
    int compIdx = 0;
    while (i.hasNext()) { untested();
      i.next();
      // Just need path to bitmap, do not create an object
      QString Name, vaBitmap;

      incomplete();
      // what's this? vacomponent::info (Name, vaBitmap, false, i.value());

      if((Name.indexOf(searchText, 0, Qt::CaseInsensitive)) != -1) { untested();
        //match

        // check if icon exists, fall back to default
        QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

        QFile iconFile(iconPath);
        QPixmap vaIcon;

        if(iconFile.exists())
        { untested();
          // load bitmap defined on the JSON symbol file
          vaIcon = QPixmap(iconPath);
        }
        else
        { untested();
          // default icon
          vaIcon = QPixmap(":/bitmaps/editdelete.png");
        }

        // Add icon an name tag to dock
        QListWidgetItem *icon = new QListWidgetItem(vaIcon, Name);
        icon->setToolTip(tr("verilog-a user devices") + ": " + Name);
        // Verilog-A is the last category
        iconCompInfo = iconCompInfoStruct{catIdx-1, compIdx};
        v.setValue(iconCompInfo);
        icon->setData(Qt::UserRole, v);
        CompComps->addItem(icon);
      }
      compIdx++;
    }
  }
}

// ------------------------------------------------------------------
void QucsApp::slotSearchClear()
{ untested();
  // was in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) { untested();
    CompChoose->removeItem(0); // remove the added "Search results" item
    CompSearch->clear();
    // go back to the panel selected before search started
    slotSetCompView(ccCurIdx);
    // the added "Search results" panel text will be removed by slotSetCompView()
  }
}

// ------------------------------------------------------------------

// ####################################################################
// #####  Functions for the menu that appears when right-clicking #####
// #####  on a file in the "Content" ListView.                    #####
// ####################################################################

void QucsApp::initCursorMenu()
{itested();
  ContentMenu = new QMenu(this);
#define APPEND_MENU(action, slot, text) \
  { \
  action = new QAction(tr(text), ContentMenu); \
  connect(action, SIGNAL(triggered()), SLOT(slot())); \
  ContentMenu->addAction(action); \
  }
  
  APPEND_MENU(ActionCMenuOpen, slotCMenuOpen, "Open")
  APPEND_MENU(ActionCMenuCopy, slotCMenuCopy, "Copy file")
  APPEND_MENU(ActionCMenuRename, slotCMenuRename, "Rename")
  APPEND_MENU(ActionCMenuDelete, slotCMenuDelete, "Delete")
  APPEND_MENU(ActionCMenuInsert, slotCMenuInsert, "Insert")

#undef APPEND_MENU
  connect(Content, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotShowContentMenu(const QPoint&)));
}

// ----------------------------------------------------------
// Shows the menu.
void QucsApp::slotShowContentMenu(const QPoint& pos) 
{ untested();
  QModelIndex idx = Content->indexAt(pos);
  if (idx.isValid() && idx.parent().isValid()) { untested();
    ActionCMenuInsert->setVisible(
        idx.sibling(idx.row(), 1).data().toString().contains(tr("-port"))
    );
    ContentMenu->popup(Content->mapToGlobal(pos));
  }    
}

// ----------------------------------------------------------
QString QucsApp::fileType (const QString& Ext)
{ untested();
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

void QucsApp::slotCMenuOpen()
{ untested();
  slotOpenContent(Content->currentIndex());
}

void QucsApp::slotCMenuCopy()
{ untested();
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QDir dir(QucsSettings.QucsWorkDir);
  QString file(dir.filePath(filename));
  QFileInfo fileinfo(file);

  //check changed file save
  int z = 0; //search if the doc is loaded
  QucsDoc *d = findDoc(file, &z);
  if (d != NULL && d->DocChanged) { untested();
    DocumentTab->setCurrentIndex(z);
    int ret = QMessageBox::question(this, tr("Copying Qucs document"), 
        tr("The document contains unsaved changes!\n") + 
        tr("Do you want to save the changes before copying?"),
        tr("&Ignore"), tr("&Save"), 0, 1);
    if (ret == 1) { untested();
      d->save();
    }
  }

  QString suffix = fileinfo.suffix();
  QString base = fileinfo.completeBaseName();
  if(base.isEmpty()) { untested();
    base = filename;
  }

  bool exists = true;   //generate unique name
  int i = 0;
  QString defaultName;
  while (exists) { untested();
    ++i;
    defaultName = base + "_copy" + QString::number(i) + "." + suffix;
    exists = QFile::exists(dir.filePath(defaultName));
  }

  bool ok;
  QString s = QInputDialog::getText(this, tr("Copy file"), tr("Enter new name:"), QLineEdit::Normal, defaultName, &ok);

  if(ok && !s.isEmpty()) { untested();
    if (!s.endsWith(suffix)) { untested();
      s += QString(".") + suffix;
    }

    if (QFile::exists(dir.filePath(s))) {  //check New Name exists
      QMessageBox::critical(this, tr("error"), tr("Cannot copy file to identical name: %1").arg(filename));
      return;
    }

    if (!QFile::copy(dir.filePath(filename), dir.filePath(s))) { untested();
      QMessageBox::critical(this, tr("Error"), tr("Cannot copy schematic: %1").arg(filename));
      return;
    }
    //TODO: maybe require disable edit here

    // refresh the schematic file path
    this->updateSchNameHash();
    this->updateSpiceNameHash();
  }
}

void QucsApp::slotCMenuRename()
{ untested();
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString file(QucsSettings.QucsWorkDir.filePath(filename));
  QFileInfo fileinfo(file);

  if (findDoc(file)) { untested();
    QMessageBox::critical(this, tr("Error"),
        tr("Cannot rename an open file!"));
    return;
  }

  QString suffix = fileinfo.suffix();
  QString base = fileinfo.completeBaseName();
  if(base.isEmpty()) { untested();
    base = filename;
  }

  bool ok;
  QString s = QInputDialog::getText(this, tr("Rename file"), tr("Enter new filename:"), QLineEdit::Normal, base, &ok);

  if(ok && !s.isEmpty()) { 
    if (!s.endsWith(suffix)) { untested();
      s += QString(".") + suffix;
    }
    QDir dir(QucsSettings.QucsWorkDir.path());
    if(!dir.rename(filename, s)) { untested();
      QMessageBox::critical(this, tr("Error"), tr("Cannot rename file: %1").arg(filename));
      return;
    }
  }
}

void QucsApp::slotCMenuDelete()
{ untested();
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString file(QucsSettings.QucsWorkDir.filePath(filename));

  if (findDoc (file)) { untested();
    QMessageBox::critical(this, tr("Error"), tr("Cannot delete an open file!"));
    return;
  }

  int No;
  No = QMessageBox::warning(this, tr("Warning"),
      tr("This will delete the file permanently! Continue ?"),
      tr("No"), tr("Yes"));
  if(No == 1) { untested();
    if(!QFile::remove(file)) { untested();
      QMessageBox::critical(this, tr("Error"),
      tr("Cannot delete file: %1").arg(filename));
      return;
    }
  }
}

void QucsApp::slotCMenuInsert()
{ untested();
  slotSelectSubcircuit(Content->currentIndex());
}

// ################################################################
// #####    Functions that handle the project operations.     #####
// ################################################################

// Checks for qucs directory and reads all existing Qucs projects.
void QucsApp::readProjects()
{itested();
  QString path = QucsSettings.projsDir.absolutePath();
  QString homepath = QucsSettings.QucsHomeDir.absolutePath();

  if (path == homepath) {itested();
    // in Qucs Home, disallow further up in the dirs tree
    m_homeDirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
  } else { untested();
    m_homeDirModel->setFilter(QDir::NoDot | QDir::AllDirs);
  }

  // set the root path
  QModelIndex rootModelIndex = m_homeDirModel->setRootPath(path);
  // assign the model to the proxy and the proxy to the view
  m_proxyModel->setSourceModel(m_homeDirModel);
  m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
  // sort by first column (file name, only column show in the QListView)
  m_proxyModel->sort(0);
  Projects->setModel(m_proxyModel);
  // fix the listview on the root path of the model
  Projects->setRootIndex(m_proxyModel->mapFromSource(rootModelIndex));
}

// ----------------------------------------------------------
// Is called, when "Create New Project" button is pressed.
void QucsApp::slotButtonProjNew()
{ untested();
  slotHideEdit(); // disable text edit of component property

  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir projDir(QucsSettings.projsDir.path());
  QString name = d->ProjName->text();
  bool open = d->OpenProj->isChecked();

  if (!name.endsWith("_prj")) { untested();
    name += "_prj";
  }

  if(!projDir.mkdir(name)) { untested();
    QMessageBox::information(this, tr("Info"),
        tr("Cannot create project directory !"));
  }
  if(open) { untested();
    openProject(QucsSettings.projsDir.filePath(name));
  }
}

// ----------------------------------------------------------
/*!
 * \brief QucsApp::openProject
 * Opens an existing project.
 * \param Path
 */
void QucsApp::openProject(const QString& Path)
{ untested();
  slotHideEdit(); // disable text edit of component property

  QDir ProjDir(QDir::cleanPath(Path)); // the full path
  QString openProjName = ProjDir.dirName(); // only the project directory name

  if(!ProjDir.exists() || !ProjDir.isReadable()) { // check project directory
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: %1").arg(Path));
    return;
  }

  if (!openProjName.endsWith("_prj")) { // should not happen
    QMessageBox::critical(this, tr("Error"),
                          tr("Project directory name does not end in '_prj'(%1)").arg(openProjName));
    return;
  }

  if(!closeAllFiles()){
    untested();
    // close files and ask for saving them
  }
    // As soon as all files are closed, open a new file.
  // TODO: maybe it is better to open no project or?

  DocumentTab->createEmptySchematic("");

  //  view->drawn = false;

    slotResetWarnings();

    QucsSettings.QucsWorkDir.setPath(ProjDir.path());
    octave->adjustDirectory();

    Content->setProjPath(QucsSettings.QucsWorkDir.absolutePath());

    TabView->setCurrentIndex(TabViewTabs::Content);

    openProjName.chop(4); // remove "_prj" from name
    ProjName = openProjName;   // remember the name of project

    // show name in title of main window
    setWindowTitle("Qucs " PACKAGE_VERSION + tr(" - Project: ")+ProjName);
}

// ----------------------------------------------------------
// Is called when the open project menu is called.
void QucsApp::slotMenuProjOpen()
{ untested();
  QString d = QFileDialog::getExistingDirectory(
      this, tr("Choose Project Directory for Opening"),
      QucsSettings.projsDir.path(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if(d.isEmpty()) return;

  openProject(d);
}

// ----------------------------------------------------------
// Is called, when "Open Project" button is pressed.
void QucsApp::slotButtonProjOpen()
{ untested();
  slotHideEdit();

  QModelIndex idx = Projects->currentIndex();
  if (!idx.isValid()) { untested();
    QMessageBox::information(this, tr("Info"),
				tr("No project is selected !"));
  } else { untested();
    slotListProjOpen(idx);
  }
}

// ----------------------------------------------------------
/*!
 * \brief QucsApp::slotListProjOpen
 * Is called when project is double-clicked to open it.
 * \param idx Index of the item on which was clicked
 */
void QucsApp::slotListProjOpen(const QModelIndex &idx)
{
  QString dName = idx.data().toString();
  if (dName.endsWith("_prj")) { // it's a Qucs project
    openProject(QucsSettings.projsDir.filePath(dName));
  } else { // it's a normal directory
    untested();
    // change projects directory to the selected one
    QucsSettings.projsDir.setPath(QucsSettings.projsDir.filePath(dName));
    readProjects();
    //repaint();
  }
}

// ----------------------------------------------------------
// Is called when the close project menu is called.
void QucsApp::slotMenuProjClose()
{ untested();
  slotHideEdit(); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  DocumentTab->createEmptySchematic("");

  // view->drawn = false;

  slotResetWarnings();
  setWindowTitle("Qucs " PACKAGE_VERSION + tr(" - Project: "));
  QucsSettings.QucsWorkDir.setPath(QDir::homePath()+QDir::toNativeSeparators("/.qucs"));
  octave->adjustDirectory();

  Content->setProjPath("");

  TabView->setCurrentIndex(TabViewTabs::Project);
  ProjName = "";
}

// remove a directory recursively
bool QucsApp::recurRemove(const QString &Path)
{ untested();
  bool result = true;
  QDir projDir = QDir(Path);

  if (projDir.exists(Path)) { untested();
    Q_FOREACH(QFileInfo info, 
        projDir.entryInfoList(
            QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::AllEntries, QDir::DirsFirst)) { untested();
      if (info.isDir()) { untested();
        result = recurRemove(info.absoluteFilePath());
        if (!result) { untested();
          QMessageBox::information(this, tr("Info"),
              tr("Cannot remove directory: %1").arg(Path));
          return false;
        }
      }
      else if(info.isFile()) { untested();
        result = QFile::remove(info.absoluteFilePath());
        if (!result) { untested();
          QMessageBox::information(this, tr("Info"),
              tr("Cannot delete file: %1").arg(info.fileName()));
          return false;
        }
      }
    }
    result = projDir.rmdir(Path);
  }
  return result;
}

// ----------------------------------------------------------
bool QucsApp::deleteProject(const QString& Path)
{ untested();
  slotHideEdit();

  if(Path.isEmpty()) return false;

  QString delProjName = QDir(Path).dirName(); // only project directory name

  if (!delProjName.endsWith("_prj")) { // should not happen
    QMessageBox::critical(this, tr("Error"),
                          tr("Project directory name does not end in '_prj' (%1)").arg(delProjName));
    return false;
  }

  delProjName.chop(4); // remove "_prj" from name

  if(delProjName == ProjName) { untested();
    QMessageBox::information(this, tr("Info"),
        tr("Cannot delete an open project !"));
    return false;
  }

  // first ask, if really delete project ?
  if(QMessageBox::warning(this, tr("Warning"),
      tr("This will destroy all the project files permanently ! Continue ?"),
      tr("&Yes"), tr("&No"), 0,1,1))  return false;

  if (!recurRemove(Path)) { untested();
    QMessageBox::information(this, tr("Info"),
        tr("Cannot remove project directory!"));
    return false;
  }
  return true;
}

// ----------------------------------------------------------
// Is called, when "Delete Project" menu is activated.
void QucsApp::slotMenuProjDel()
{ untested();
  QString d = QFileDialog::getExistingDirectory(
      this, tr("Choose Project Directory for Deleting"),
      QucsSettings.projsDir.path(),
      QFileDialog::ShowDirsOnly
      | QFileDialog::DontResolveSymlinks);

  deleteProject(d);
}

// ----------------------------------------------------------
// Is called, when "Delete Project" button is pressed.
void QucsApp::slotButtonProjDel()
{ untested();
  QModelIndex idx = Projects->currentIndex();
  if(!idx.isValid()) { untested();
    QMessageBox::information(this, tr("Info"),
        tr("No project is selected!"));
    return;
  }

  deleteProject(QucsSettings.projsDir.filePath(idx.data().toString()));
}


// ################################################################
// #####  Functions that handle the file operations for the   #####
// #####  documents.                                          #####
// ################################################################

void QucsApp::slotFileNew(bool)
{ untested();
  statusBar()->showMessage(tr("Creating new schematic..."));
  slotHideEdit(); // disable text edit of component property

  DocumentTab->createEmptySchematic("");
  // d->SimOpenDpl = enableOpenDpl;

  statusBar()->showMessage(tr("Ready."));
}

void QucsApp::slotFileNewNoDD()
{ untested();
  slotFileNew(false);
}

// --------------------------------------------------------------
void QucsApp::slotTextNew()
{ untested();
  statusBar()->showMessage(tr("Creating new text editor..."));
  slotHideEdit(); // disable text edit of component property

  editFile("");

  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------
// Changes to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
bool QucsApp::gotoPage(const QString& Name)
{itested();
    // is Name the filename?!
  int No = DocumentTab->currentIndex();

  int i = 0;
  QucsDoc * d = findDoc (Name, &i);  // search, if page is already loaded

  if(d) { untested();
    // open page found
    d->becomeCurrent(true);
    DocumentTab->setCurrentIndex(i);  // make new document the current
    return true;
  }else{itested();
  }

  QFileInfo Info(Name);
  if(Info.suffix() == "sch" || Info.suffix() == "dpl" ||
     Info.suffix() == "sym") {itested();
    d = DocumentTab->createEmptySchematic(Name);
  } else {itested();
    d = DocumentTab->createEmptyTextDoc(Name);
  }

     // load document if possible
  if(!d->load()) {itested();
    delete d;
    DocumentTab->setCurrentIndex(No);
    // view->drawn = false;
    return false;
  }else{itested();
    slotChangeView(DocumentTab->current());

    // if only an untitled document was open -> close it
    if(getDoc(0)->docName().isEmpty())
      if(!getDoc(0)->DocChanged)
        delete DocumentTab->widget(0);

    //view->drawn = false;
    return true;
  }
}

QString lastDirOpenSave; // to remember last directory and file

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{ untested();
  slotHideEdit(); // disable text edit of component property

  statusBar()->showMessage(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(this, tr("Enter a Schematic Name"),
    lastDirOpenSave.isEmpty() ? QString(".") : lastDirOpenSave, QucsFileFilter);

  if(s.isEmpty())
    statusBar()->showMessage(tr("Opening aborted"), 2000);
  else { untested();
    updateRecentFilesList(s);

    gotoPage(s);
    lastDirOpenSave = s;   // remember last directory and file

    statusBar()->showMessage(tr("Ready."));
  }
}

// --------------------------------------------------------------
bool QucsApp::saveFile(QucsDoc *Doc)
{ untested();
  if(!Doc)
    Doc = getDoc();

  if(Doc->docName().isEmpty())
    return saveAs(Doc);

  int Result = Doc->save();
  if(Result < 0)  return false;

  updatePortNumber(Doc, Result);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSave()
{ untested();
  statusBar()->showMessage(tr("Saving file..."));
  DocumentTab->blockSignals(true);   // no user interaction during that time
  slotHideEdit(); // disable text edit of component property

  if(!saveFile()) { untested();
    DocumentTab->blockSignals(false);
    statusBar()->showMessage(tr("Saving aborted"), 2000);
    statusBar()->showMessage(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------
bool QucsApp::saveAs()
{ untested();
  QucsDoc *w = DocumentTab->current();
  return saveAs(w);
}

bool QucsApp::saveAs(QucsDoc* doc)
{ untested();

  return doc->saveAs();
}

// --------------------------------------------------------------
void QucsApp::slotFileSaveAs()
{ untested();
  statusBar()->showMessage(tr("Saving file under new filename..."));
  DocumentTab->blockSignals(true);   // no user interaction during the time
  slotHideEdit(); // disable text edit of component property

  if(!saveAs()) { untested();
    DocumentTab->blockSignals(false);
    statusBar()->showMessage(tr("Saving aborted"), 3000);
    statusBar()->showMessage(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->showMessage(tr("Ready."));

  // refresh the schematic file path
  slotRefreshSchPath();
}


// --------------------------------------------------------------
void QucsApp::slotFileSaveAll()
{ untested();
  statusBar()->showMessage(tr("Saving all files..."));
  slotHideEdit(); // disable text edit of component property
  DocumentTab->blockSignals(true);   // no user interaction during the time

  int No=0;
  QucsDoc *Doc;  // search, if page is already loaded
  while((Doc=getDoc(No++)) != 0) { untested();
    if(Doc->docName().isEmpty())  // make document the current ?
      DocumentTab->setCurrentIndex(No-1);
    // Hack! TODO: Maybe it's better to let slotFileChanged() know about Tab number?
    //   if saving was successful, turn off "saving needed" icon
    DocumentTab->setSaveIcon(!saveFile(Doc), No-1);
  }

  DocumentTab->blockSignals(false);
  // Call update() to update subcircuit symbols in current Schematic document.
  // TextDoc has no viewport, it needs no update.
  // //???
//  QString tabType = DocumentTab->currentWidget()->metaObject()->className();

  Doc = prechecked_cast<QucsDoc*>(DocumentTab->current());
  assert(Doc);
  Doc->updateViewport();

  //view->drawn = false;//??
  statusBar()->showMessage(tr("Ready."));

  // refresh the schematic file path
  slotRefreshSchPath();
}

// --------------------------------------------------------------
// Close the currently active file tab
void QucsApp::slotFileClose()
{ untested();
    // Using file index -1 closes the currently active file window
    closeFile(-1);
}

// --------------------------------------------------------------
// Close the file tab specified by its index
void QucsApp::slotFileClose(int index)
{ untested();
    // Call closeFile with a specific tab index
    closeFile(index);
}

// Close all documents except the current one
void QucsApp::slotFileCloseOthers()
{ untested();
  closeAllFiles(DocumentTab->currentIndex());
}

// Close all documents to the left of the current one
void QucsApp::slotFileCloseAllLeft()
{ untested();
  closeAllLeft(DocumentTab->currentIndex());
}

// Close all documents to the right of the current one
void QucsApp::slotFileCloseAllRight()
{ untested();
  closeAllRight(DocumentTab->currentIndex());
}

// Close all documents
void QucsApp::slotFileCloseAll()
{ untested();
  // close all tabs
  closeAllFiles();
  // create empty schematic
  slotFileNew();
}

//
// --------------------------------------------------------------
// Common function to close a file tab specified by its index
// checking for changes in the file before doing so. If called
// index == -1, the active document will be closed
void QucsApp::closeFile(int index)
{ untested();
    statusBar()->showMessage(tr("Closing file..."));

    slotHideEdit(); // disable text edit of component property

    QucsDoc *Doc = getDoc(index);
    if(Doc->DocChanged) { untested();
      int m = QMessageBox::warning(this,tr("Closing Qucs document"),
        tr("The document contains unsaved changes!\n")+
        tr("Do you want to save the changes before closing?"),
        tr("&Save"), tr("&Discard"), tr("Cancel"), 0, 2);
        switch (m) {
        case 0 : slotFileSave();
                 break;
        case 2 : return;
      }
    }

    DocumentTab->removeTab(index);
    delete Doc;

    if(DocumentTab->count() < 1) { // if no document left, create an untitled
      DocumentTab->createEmptySchematic("");
    }

    statusBar()->showMessage(tr("Ready."));
}


/**
 * @brief close all open documents - except a specified one, optionally
 * @param exceptTab tab to leave open, none if not specified
 *
 * @return true if all files were succesfully closed
 */
bool QucsApp::closeAllFiles(int exceptTab)
{itested();
  if (DocumentTab->count() == 0) // no open tabs
    return true;

  return closeTabsRange(0, DocumentTab->count()-1, exceptTab);
}

/**
 * @brief close Tabs in a specified range, optionally skipping a specified one
 * @param startTab first tab to be closed
 * @param stoptTab last tab to be closed
 * @param exceptTab tab to leave open, none if not specified
 *
 * @return true if all requested tabs were succesfully closed
 */
bool QucsApp::closeTabsRange(int startTab, int stopTab, int exceptTab)
{itested();
  if (stopTab < startTab)
    return false;
  // document to keep open, if any
  QucsDoc *docToKeep = 0;
  if (exceptTab >= 0) { untested();
    docToKeep = getDoc(exceptTab);
  }

  SaveDialog *sd = new SaveDialog(this);
  sd->setApp(this);
  Q_ASSERT(startTab >= 0);
  Q_ASSERT(stopTab < DocumentTab->count());

  for(int i=startTab; i <= stopTab; ++i) {itested();
    QucsDoc *doc = getDoc(i);
    if ((doc->DocChanged) && (doc != docToKeep))
      sd->addUnsavedDoc(doc);
  }
  int Result = SaveDialog::DontSave;
  if(!sd->isEmpty())
     Result = sd->exec();
  delete sd;
  if(Result == SaveDialog::AbortClosing)
    return false;
  // remove documents
  QucsDoc *doc = 0;
  QucsDoc *stopDoc = getDoc(stopTab);
  int i = 0;
  do {itested();
    doc = getDoc(startTab+i);
    if (doc == docToKeep) { untested();
      i++; // skip to next doc
    } else {itested();
      delete doc;
    }
  } while (doc != stopDoc);

  switchEditMode(true);
  return true;
}

/**
 * @brief close all documents to the left of the specified one
 * @param index reference tab
 */
bool QucsApp::closeAllLeft(int index)
{ untested();
  return closeTabsRange(0, index-1);
}

/**
 * @brief close all documents to the right of the specified one
 * @param index reference tab
 */
bool QucsApp::closeAllRight(int index)
{ untested();
  return closeTabsRange(index+1, DocumentTab->count()-1);
}

void QucsApp::slotFileExamples()
{ untested();
  statusBar()->showMessage(tr("Open examples directory..."));
  // pass the QUrl representation of a local file
  QDesktopServices::openUrl(QUrl::fromLocalFile(QucsSettings.ExamplesDir));
  statusBar()->showMessage(tr("Ready."));
}

void QucsApp::slotHelpTutorial()
{ untested();
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/tutorial/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpTechnical()
{ untested();
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/technical/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpReport()
{ untested();
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/report/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}



// --------------------------------------------------------------
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(QucsDoc *w)
{itested();

  if(w==NULL){
    //what??
    return;
  }else{itested();
  }
  editText->setHidden (true); // disable text edit of component property
  auto Doc = w;
  // for text documents
  if (isTextDocument (w)) { untested();
    TextDoc *d = (TextDoc*)w;
    Doc = (QucsDoc*)d;
    // update menu entries, etc. if neccesary
    magAll->setDisabled(true); // here?
    if(cursorLeft->isEnabled()){ untested();
      switchSchematicDoc (false); // Doc->expose??
    }else{ untested();
    }
#if 0
  } else if(auto d=dynamic_cast<SchematicDoc*>(w)){ untested();
    incomplete();
    Doc = (QucsDoc*)d;
    magAll->setDisabled(false);
    // already in schematic?
    if(cursorLeft->isEnabled()) { untested();
      // which mode: schematic or symbol editor ?
      if((CompChoose->count() > 1) == d->isSymbolMode())
        changeSchematicSymbolMode (d);
    }
    else { untested();
      switchSchematicDoc(true);
      changeSchematicSymbolMode(d);
    }
#endif
  }else{itested();
  }

  assert(Doc);
  Doc->becomeCurrent(true);
  // view->drawn = false;

  HierarchyHistory.clear();
  popH->setEnabled(false);
}

// --------------------------------------------------------------
void QucsApp::slotFileSettings ()
{ untested();
  editText->setHidden (true); // disable text edit of component property

  QucsDoc* w = DocumentTab->current();
  if (isTextDocument (w)) { untested();
    QucsDoc * Doc = (QucsDoc *) ((TextDoc *) w);
    QString ext = Doc->fileSuffix ();
    // Octave properties
    if (ext == "m" || ext == "oct") { untested();
    }else if (ext == "va") { untested();
    // Verilog-A properties
      VASettingsDialog * d = new VASettingsDialog ((TextDoc *) w);
      d->exec ();
    }
    // VHDL and Verilog-HDL properties
    else { untested();
      DigiSettingsDialog * d = new DigiSettingsDialog ((TextDoc *) w);
      d->exec ();
    }
  }else{ untested();
    SettingsDialog * d = new SettingsDialog ((SchematicDoc *) w);
    d->exec ();
  }
  // view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotApplSettings()
{ untested();
  slotHideEdit(); // disable text edit of component property

  QucsSettingsDialog *d = new QucsSettingsDialog(this);
  d->exec();
  //view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotRefreshSchPath()
{ untested();
  this->updateSchNameHash();
  this->updateSpiceNameHash();

  statusBar()->showMessage(tr("The schematic search path has been refreshed."), 2000);
}

// --------------------------------------------------------------
void QucsApp::updatePortNumber(QucsDoc *currDoc, int No)
{ untested();
  (void) currDoc;
  (void) No;
  incomplete();
#if 0
  if(No<0) return;

  QString pathName = currDoc->docName();
  QString ext = currDoc->fileSuffix ();
  QFileInfo Info (pathName);
  QString Model, File, Name = Info.fileName();

  if (ext == "sch") { untested();
    Model = "Sub";
  }
  else if (ext == "vhdl" || ext == "vhd") { untested();
    Model = "VHDL";
  }
  else if (ext == "v") { untested();
    Model = "Verilog";
  }

  // update all occurencies of subcircuit in all open documents
  No = 0;
  QWidget *w;
  Component *pc_tmp;
  while((w=DocumentTab->widget(No++)) != 0) { untested();
    if(isTextDocument (w))  continue;

    // start from the last to omit re-appended components
    SchematicDoc *Doc = (SchematicDoc*)w;
    for(Component *pc=Doc->components().last(); pc!=0; ) { untested();
      if(pc->obsolete_model_hack() == Model) { // BUG
        File = pc->Props.getFirst()->Value;
        if((File == pathName) || (File == Name)) { untested();
          pc_tmp = Doc->components().prev();
          Doc->recreateSymbol(pc);  // delete and re-append component
          if(!pc_tmp)  break;
          Doc->components().findRef(pc_tmp);
          pc = Doc->components().current();
          continue;
        }
      }
      pc = Doc->components().prev();
    }
  }
#endif
}

// --------------------------------------------------------------
// printCurrentDocument: call printerwriter to print document
void QucsApp::printCurrentDocument(bool fitToPage)
{ untested();
  statusBar()->showMessage(tr("Printing..."));
  slotHideEdit(); // disable text edit of component property

  PrinterWriter *writer = new PrinterWriter();
  writer->setFitToPage(fitToPage);
  writer->print(DocumentTab->current());
  delete writer;

  statusBar()->showMessage(tr("Ready."));
  return;
}

// --------------------------------------------------------------
void QucsApp::slotFilePrint()
{ untested();
  printCurrentDocument(false);
}

// --------------------------------------------------------------
// Fit printed content to page size.
void QucsApp::slotFilePrintFit()
{ untested();
  printCurrentDocument(true);
}

// --------------------------------------------------------------------
// Exits the application.
void QucsApp::slotFileQuit()
{itested();
  statusBar()->showMessage(tr("Exiting application..."));
  slotHideEdit(); // disable text edit of component property

  int exit = QMessageBox::information(this,
      tr("Quit..."), tr("Do you really want to quit?"),
      tr("Yes"), tr("No"));

  if(exit == 0)
    if(closeAllFiles()) { untested();
      emit signalKillEmAll();   // kill all subprocesses
      qApp->quit();
    }

  statusBar()->showMessage(tr("Ready."));
}

//-----------------------------------------------------------------
// To get all close events.
void QucsApp::closeEvent(QCloseEvent* Event)
{ untested();
    qDebug()<<"x"<<pos().x()<<" ,y"<<pos().y();
    qDebug()<<"dx"<<size().width()<<" ,dy"<<size().height();
    QucsSettings.x=pos().x();
    QucsSettings.y=pos().y();
    QucsSettings.dx=size().width();
    QucsSettings.dy=size().height();
    saveApplSettings();

   if(closeAllFiles()) { untested();
      emit signalKillEmAll();   // what?
      Event->accept();
      qApp->quit();
   }else{ untested();
      Event->ignore();
   }
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{ untested();
  slotHideEdit(); // disable text edit of component property

  incomplete();
#if 0
  SchematicDoc *Doc = (SchematicDoc*)DocumentTab->currentWidget();
  Component *pc = Doc->searchSelSubcircuit();
  if(pc == 0) { return; }

  QString s = pc->getSubcircuitFile();
  if(!gotoPage(s)) { return; }

  // BUG: this complains about some malloc in qvector (not without a reason)
  HierarchyHistory.push(Doc->docName()); //remember for the way back
  popH->setEnabled(true);
#endif
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{ untested();
  slotHideEdit(); // disable text edit of component property

  if(HierarchyHistory.size() == 0) return;

  QString Doc = HierarchyHistory.pop();

  if(!gotoPage(Doc)) { untested();
    HierarchyHistory.push(Doc);
    return;
  }

  if(HierarchyHistory.size() == 0) { untested();
    popH->setEnabled(false);
  }
}

// --------------------------------------------------------------
void QucsApp::slotShowAll()
{ untested();
  slotHideEdit(); // disable text edit of component property
  getDoc()->showAll();
}

// -----------------------------------------------------------
// Sets the scale factor to 1.
void QucsApp::slotShowOne()
{ untested();
  slotHideEdit(); // disable text edit of component property
  getDoc()->showNoZoom();
}

// -----------------------------------------------------------
void QucsApp::slotZoomOut()
{ untested();
  slotHideEdit(); // disable text edit of component property
  getDoc()->zoomBy(0.5f);
}


// call Doc->simulate (instead)?
void QucsApp::slotSimulate()
{ untested();
  slotHideEdit(); // disable text edit of component property

  QucsDoc *Doc = DocumentTab->current();
  assert(Doc);
  Doc->slotSimulate();
}

// ------------------------------------------------------------------------
// Is called after the simulation process terminates.
// why is this needed? just signal names of refreshed datasets to doc.
void QucsApp::slotAfterSimulation(Simulator const* sim)
{ untested();
  if(sim->state() != 0){ untested();
    return;  // errors ocurred ?
  }else{ untested();
  }

//  if(sim->ErrText->document()->lineCount() > 1){ untested();
//    // possibly warnings why here?
//    slotShowWarnings();
//  }else{ untested();
//  }

//  int i=0;
//  QWidget *w = sim->docWidget();
//  (void)w;

  // what?
//   if(sim->showBias == 0) {  // paint dc bias into schematic ?
//     if(w) { untested();
//       // schematic still open ?
//       SweepDialog *Dia = new SweepDialog((SchematicDoc*)sim->DocWidget);
// 
//       // silence warning about unused variable.
//       Q_UNUSED(Dia)
//     }else{ untested();
//     }
//   }else{ untested();
//   }
#if 0 // not here
  else { untested();
    if(sim->SimRunScript) { untested();
      // run script
      octave->startOctave();
      octave->runOctaveScript(sim->Script);
    }else{ untested();
    }
#endif
#if 0 // not here
    if(sim->SimOpenDpl) { untested();
      // switch to data display
      if(sim->DataDisplay.right(2) == ".m" ||
	 sim->DataDisplay.right(4) == ".oct") {  // Is it an Octave script?
	octave->startOctave();
	octave->runOctaveScript(sim->DataDisplay);
      } else{ untested();
	slotChangePage(sim->DocName, sim->DataDisplay);
      }
      sim->slotClose();   // close and delete simulation window
    }else{ untested();
      if(w) if(!isTextDocument (sim->DocWidget))
	// load recent simulation data (if document is still open)
	((SchematicDoc*)sim->DocWidget)->reloadGraphs();
    }
  }

  if(!isTextDocument (sim->DocWidget))
    ((SchematicDoc*)sim->DocWidget)->viewport()->update();
#endif

}

// ------------------------------------------------------------------------
// Changes to the corresponding data display page or vice versa.
void QucsApp::slotChangePage(QString& DocName, QString& DataDisplay)
{ untested();
  if(DataDisplay.isEmpty())  return;

  QFileInfo Info(DocName);
  QString Name = Info.path() + QDir::separator() + DataDisplay;

//  QWidget *w = DocumentTab->currentWidget();

  int z = 0;  // search, if page is already loaded
  QucsDoc * d = findDoc (Name, &z);

  if(d)
    DocumentTab->setCurrentIndex(z);
  else {   // no open page found ?
    QString ext = QucsDoc::fileSuffix (DataDisplay);

    if (ext != "vhd" && ext != "vhdl" && ext != "v" && ext != "va" &&
	ext != "oct" && ext != "m") { untested();
      d = DocumentTab->createEmptySchematic(Name);
    } else { untested();
      d = DocumentTab->createEmptyTextDoc(Name);
    }

    QFile file(Name);
    if(file.open(QIODevice::ReadOnly)) {      // try to load document
      file.close();
      if(!d->load()) { untested();
        delete d;
        // view->drawn = false;
        return;
      }
    }
    else { untested();
      if(file.open(QIODevice::ReadWrite)) {  // if document doesn't exist, create
        d->DataDisplay = Info.fileName();
      }
      else { untested();
        QMessageBox::critical(this, tr("Error"), tr("Cannot create ")+Name);
        return;
      }
      file.close();
    }

    d->becomeCurrent(true);
  }


  incomplete();
//  if(DocumentTab->currentWidget() == w)      // if page not ...
//    if(!isTextDocument (w))
//      ((QucsDoc*)w)->reloadGraphs();  // ... changes, reload here !

  TabView->setCurrentIndex(TabViewTabs::Components);
  if (Name.right(4) == ".dpl") { untested();
    int i = Category::getModulesNr (QObject::tr("diagrams"));
    CompChoose->setCurrentIndex(i);   // switch to diagrams
    slotSetCompView (i);
  }
}

// -------------------------------------------------------------------
// Changes to the data display of current page.
void QucsApp::slotToPage()
{ untested();
  QucsDoc *d = getDoc();
  if(d->DataDisplay.isEmpty()) { untested();
    QMessageBox::critical(this, tr("Error"), tr("No page set !"));
    return;
  }

  if(d->docName().right(2) == ".m" ||
     d->docName().right(4) == ".oct") { untested();
    slotViewOctaveDock(true);
  }else{ untested();
      QString dn=d->docName();
    slotChangePage(dn, d->DataDisplay);
    d->setDocName(dn);
  }
}

// -------------------------------------------------------------------
/*!
 * \brief QucsApp::slotOpenContent
 * Called when file name in Project View is double-clicked
 * or open is selected in the context menu
 * \param idx
 */
void QucsApp::slotOpenContent(const QModelIndex &idx)
{ untested();
  editText->setHidden(true); // disable text edit of component property

  //test the item is valid
  if (!idx.isValid()) { return; }
  if (!idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString note = idx.sibling(idx.row(), 1).data().toString();
  QFileInfo Info(QucsSettings.QucsWorkDir.filePath(filename));
  QString extName = Info.suffix();

  if (extName == "sch" || extName == "dpl") { untested();

    // BUG BUG BUG
    gotoPage(Info.absoluteFilePath());
    updateRecentFilesList(Info.absoluteFilePath());
    slotUpdateRecentFiles();

    if(note.isEmpty())     // is subcircuit ?
      if(extName == "sch") return;

    select->blockSignals(true);  // switch on the 'select' action ...
    select->setChecked(true);
    select->blockSignals(false);

    activeAction = select;
    MouseMoveAction = 0;
#if 0
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#else
    // MouseAction = actionSelect;
#endif
    return;
  }

  // open text files with text editor
  if(extName == "dat" || extName == "vhdl" ||
     extName == "vhd" || extName == "v" || extName == "va" ||
     extName == "m" || extName == "oct") { untested();
    editFile(Info.absoluteFilePath());
    return;
  }


  // File is no Qucs file, so go through list and search a user
  // defined program to open it.
  QStringList com;
  QStringList::const_iterator it = QucsSettings.FileTypes.constBegin();
  while(it != QucsSettings.FileTypes.constEnd()) { untested();
    if(extName == (*it).section('/',0,0)) { untested();
      QString progName = (*it).section('/',1,1);
      com = progName.split(" ");
      com << Info.absoluteFilePath();

      QProcess *Program = new QProcess();
      //Program->setCommunication(0);
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      Program->setProcessEnvironment(env);
      Program->start(com.join(" "));
      if(Program->state()!=QProcess::Running&&
              Program->state()!=QProcess::Starting) { untested();
        QMessageBox::critical(this, tr("Error"),
               tr("Cannot start \"%1\"!").arg(Info.absoluteFilePath()));
        delete Program;
      }
      return;
    }
    it++;
  }

  // If no appropriate program was found, open as text file.
  editFile(Info.absoluteFilePath());  // open datasets with text editor
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
// (where?)
void QucsApp::slotSelectSubcircuit(const QModelIndex &idx)
{ untested();
  incomplete();
#if 0
  editText->setHidden(true); // disable text edit of component property

  if(!idx.isValid()) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    return;
  }

  bool isVHDL = false;
  bool isVerilog = false;
  QModelIndex parentIdx = idx.parent();
  if(!parentIdx.isValid()) { return; }

  QString category = parentIdx.data().toString();

  if(category == tr("Schematics")) { untested();
    if(idx.sibling(idx.row(), 1).data().toString().isEmpty())
      return;   // return, if not a subcircuit
  }
  else if(category == tr("VHDL"))
    isVHDL = true;
  else if(category == tr("Verilog"))
    isVerilog = true;
  else
    return;

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString note = idx.sibling(idx.row(), 1).data().toString();
  int idx_pag = DocumentTab->currentIndex();
  QString tab_titl = "";
  if (idx_pag>=0) tab_titl = DocumentTab->tabText(idx_pag);
  if (filename == tab_titl ) return; // Forbid to paste subcircuit into itself.

  // delete previously selected elements
#if 0
  if(view->selElem != 0){ untested();
    delete view->selElem;
  }
  view->selElem = 0;
#endif

  // toggle last toolbar button off
  //  toolbar->deactivate();
  if(activeAction) { untested();
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0; //??

#if 0
  Symbol* Symb;
  if(isVHDL){ untested();
	  Symb = symbol_dispatcher.clone("VHDL_legacy");
  }else if(isVerilog){ untested();
	  Symb = symbol_dispatcher.clone("Verilog_legacy");
  }else{ untested();
	  Symb = symbol_dispatcher.clone("Subcircuit_legacy");
  }
#endif

#if 0 // later.
  Component *Comp = prechecked_cast<Component*>(Symb);
  assert(Comp);
  Comp->Props.first()->Value = idx.sibling(idx.row(), 0).data().toString();
#endif
  incomplete();
  // Symb->recreate(Scktmodel);
  // view->selElem = Comp;

///   if(view->drawn){ untested();
  QucsDoc *Doc = DocumentTab->current();
  assert(Doc);
  Doc->updateViewport();
///   }else{ untested();
///   }
///   view->drawn = false;
  MouseMoveAction = nullptr; // &MouseActions::MMoveElement;
//  MousePressAction = &MouseActions::MPressElement;
  incomplete();
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
#endif
  (void) idx;
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
// (where?)
void QucsApp::slotSelectLibComponent(QTreeWidgetItem *item)
{ untested();
    incomplete();
    (void) item;

#if 0
    SchematicDoc *Doc = (SchematicDoc*)DocumentTab->currentWidget();
    // if the current document is a schematic activate the paste
    if(!isTextDocument(Doc)) { untested();
        // if theres not a higher level item, this is a top level item,
        // not a component item so return
        if(item->parent() == 0) return;
        if(item->text(1).isEmpty()) return;   // return, if not a subcircuit

        // copy the subcircuit schematic to the clipboard
        QClipboard *cb = QApplication::clipboard();
        cb->setText(item->text(1));

        // activate the paste command
        slotEditPaste (); // who is the sender now??
    }else{ untested();
    }
#endif
}


// ---------------------------------------------------------
// This function is called if the document type changes, i.e.
// from schematic to text document or vice versa.
void QucsApp::switchSchematicDoc (bool SchematicMode)
{itested();
  // switch our scroll key actions on/off according to SchematicMode
  cursorLeft->setEnabled(SchematicMode);
  cursorRight->setEnabled(SchematicMode);
  cursorUp->setEnabled(SchematicMode);
  cursorDown->setEnabled(SchematicMode);

  // text document
  if (!SchematicMode) { untested();
    if (activeAction) { untested();
      activeAction->blockSignals (true); // do not call toggle slot
      activeAction->setChecked(false);       // set last toolbar button off
      activeAction->blockSignals (false);
    }
    activeAction = select;
    select->blockSignals (true);
    select->setChecked(true);
    select->blockSignals (false);
  }
  // schematic document
  else {itested();
#if 0
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#else
    // MouseAction = actionSelect;
#endif
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
  insEntity->setEnabled (!SchematicMode);

  buildModule->setEnabled(!SchematicMode); // only build if VA document
}

// ---------------------------------------------------------
void QucsApp::switchEditMode(bool SchematicMode)
{ untested();
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
  // no search in "symbol painting mode" as only paintings should be used
  CompSearch->setEnabled(SchematicMode);
  CompSearchClear->setEnabled(SchematicMode);
}

// ---------------------------------------------------------
// // obsolete
void QucsApp::changeSchematicSymbolMode(SchematicDoc *)
{ untested();

  // TODO: close currentwidget->document and reopen schematicSymbol (or the
  // other way round).
  // (SymbolDoc is no longer a "mode", but a document type).
#if 0
  if(Doc->isSymbolMode()) { untested();
    // go into select modus to avoid placing a forbidden element
    select->setChecked(true);

    switchEditMode(false);
  }else{ untested();
    switchEditMode(true);
  }
#endif
}

// ---------------------------------------------------------
bool QucsApp::isTextDocument(QucsDoc const*w)
{itested();
  return dynamic_cast<QPlainTextEdit const*>(w);
}

// ---------------------------------------------------------
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: SchematicDoc and (subcircuit)
// symbol.
// // TODO: open a symbol document
void QucsApp::slotSymbolEdit()
{ untested();
  incomplete();
#if 0
  QucsDoc *Doc = DocumentTab->current();
  assert(Doc);

  // in a text document (e.g. VHDL)
  if (isTextDocument (Doc)) { untested();
    TextDoc *TDoc = (TextDoc*)Doc;
    // set 'DataDisplay' document of text file to symbol file
    QFileInfo Info(Doc->docName());
    QString sym = Info.completeBaseName()+".sym";
#if 0
    TDoc->DataDisplay = sym;
#else
    // TODO: Doc->displayData(sym); // what is this?
#endif

    // symbol file already loaded?
    int paint_mode = 0;
    if (!findDoc (QucsSettings.QucsWorkDir.filePath(sym)))
      paint_mode = 1;

    // change current page to appropriate symbol file
    QString s = Doc->docName();

    incomplete(); // what does this do?
//    slotChangePage(s, TDoc->DataDisplay);
    Doc->setDocName(s);

    // set 'DataDisplay' document of symbol file to original text file
#if 0
//    SchematicDoc *SDoc = (SchematicDoc*)DocumentTab->currentWidget();
//    SDoc->DataDisplay = Info.fileName();

    // change into symbol mode
    if (paint_mode) // but only switch coordinates if newly loaded
      SDoc->switchPaintMode(); // toggles SymbolMode (wtf?)
#endif

    incomplete(); // TODO: symbolDoc
#if 0
    SDoc->setSymbolMode(true);
    changeSchematicSymbolMode(SDoc);
    SDoc->becomeCurrent(true);
    SDoc->viewport()->update();
#endif
    // view->drawn = false;
  }else{ untested();
    // in a normal schematic, data display or symbol file
    SchematicDoc *SDoc = (SchematicDoc*)Doc;
    // in a symbol file
    //
#if 0
    if(SDoc->docName().right(4) == ".sym") { untested();
      QString dn=SDoc->docName();
      slotChangePage(dn, SDoc->DataDisplay);
      SDoc->setDocName(dn);
    }else{ untested();
    // in a normal schematic
      slotHideEdit(); // disable text edit of component property
      SDoc->switchPaintMode(); // toggles SymbolMode (wtf?)
      changeSchematicSymbolMode(SDoc);
      SDoc->becomeCurrent(true);
      SDoc->viewport()->update();
      // view->drawn = false;
    }
#endif
  }
#endif
}

// -----------------------------------------------------------
void QucsApp::slotPowerMatching()
{ untested();
#if 0

	// not here.

  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

//  double Z0 = 50.0;
  QString Var = pm->pGraph->Var;
  double Imag = pm->powImag();
  if(Var == "Sopt")  // noise matching ?
    Imag *= -1.0;

  MatchDialog *Dia = new MatchDialog(this);
//  Dia->Ref1Edit->setText(QString::number(Z0));
  Dia->setS11LineEdits(pm->powReal(), Imag);
  Dia->setFrequency(pm->powFreq());
  Dia->setTwoPortMatch(false); // will also cause the corresponding impedance LineEdit to be updated

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
#endif
}

// -----------------------------------------------------------
void QucsApp::slot2PortMatching()
{ untested();
#if 0

	// not here
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

  QString DataSet;
  SchematicDoc *Doc = (SchematicDoc*)DocumentTab->currentWidget();
  int z = pm->pGraph->Var.indexOf(':');
  if(z <= 0)  DataSet = Doc->DataSet;
  else  DataSet = pm->pGraph->Var.mid(z+1);
  double Freq = pm->powFreq();

  QFileInfo Info(Doc->DocName);
  DataSet = Info.path()+QDir::separator()+DataSet;

  Diagram *Diag = new Diagram(); //?! which one?

  // FIXME: use normal Diagrams.
  Graph *pg = new Graph(Diag, "S[1,1]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) { untested();
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,1]."));
    return;
  }

  pg = new Graph(Diag, "S[1,2]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) { untested();
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,2]."));
    return;
  }

  pg = new Graph(Diag, "S[2,1]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) { untested();
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,1]."));
    return;
  }

  pg = new Graph(Diag, "S[2,2]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) { untested();
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,2]."));
    return;
  }

  DataX const *Data = Diag->Graphs.first()->axis(0);
  if(Data->Var != "frequency") { untested();
    QMessageBox::critical(0, tr("Error"), tr("Wrong dependency!"));
    return;
  }

  double *Value = Data->Points;
  // search for values for chosen frequency
  for(z=0; z<Data->count; z++)
    if(*(Value++) == Freq) break;

  // get S-parameters
  double S11real = *(Diag->Graphs.at(0)->cPointsY + 2*z);
  double S11imag = *(Diag->Graphs.at(0)->cPointsY + 2*z + 1);
  double S12real = *(Diag->Graphs.at(1)->cPointsY + 2*z);
  double S12imag = *(Diag->Graphs.at(1)->cPointsY + 2*z + 1);
  double S21real = *(Diag->Graphs.at(2)->cPointsY + 2*z);
  double S21imag = *(Diag->Graphs.at(2)->cPointsY + 2*z + 1);
  double S22real = *(Diag->Graphs.at(3)->cPointsY + 2*z);
  double S22imag = *(Diag->Graphs.at(3)->cPointsY + 2*z + 1);
  delete Diag;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->setTwoPortMatch(true);
  Dia->setFrequency(Freq);
  Dia->setS11LineEdits(S11real, S11imag);
  Dia->setS12LineEdits(S12real, S12imag);
  Dia->setS21LineEdits(S21real, S21imag);
  Dia->setS22LineEdits(S22real, S22imag);

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;

#endif
}

// -----------------------------------------------------------
// Is called if the "edit" action is clicked on right mouse button menu.
void QucsApp::slotEditElement()
{ untested();
  incomplete();
#if 0 // SchematicDoc?
  if(view->focusMEvent){ untested();
    view->editElement((SchematicDoc*)DocumentTab->currentWidget(), view->focusMEvent);
  }else{ untested();
	 // ?!
  }
#endif
}

// -----------------------------------------------------------
// Hides the edit for component property. Called e.g. if QLineEdit
// looses the focus.
void QucsApp::slotHideEdit()
{itested();
  editText->setParent(this, 0);
  editText->setHidden(true);
}

// -----------------------------------------------------------
// set document tab icon to "smallsave.xpm" or "empty.xpm"
void QucsApp::slotFileChanged(bool changed)
{ untested();
  DocumentTab->setSaveIcon(changed);
}

// -----------------------------------------------------------
// Searches the qucs path list for all schematic files and creates
// a hash for lookup later
void QucsApp::updateSchNameHash(void)
{itested();
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

    foreach (QString qucspath, qucsPathList) { untested();
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        foreach (QFileInfo schfile, schfilesList) { untested();
            QString bn = schfile.completeBaseName();
            schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    foreach (QFileInfo schfile, schfilesList) { untested();
        schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
    }
}

// -----------------------------------------------------------
// Searches the qucs path list for all spice files and creates
// a hash for lookup later
void QucsApp::updateSpiceNameHash(void)
{itested();
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

    foreach (QString qucspath, qucsPathList) { untested();
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList spicefilesList = thispath.entryInfoList( QucsSettings.spiceExtensions, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        foreach (QFileInfo spicefile, spicefilesList) { untested();
            QString bn = spicefile.completeBaseName();
            schNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList spicefilesList = thispath.entryInfoList( QucsSettings.spiceExtensions, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    foreach (QFileInfo spicefile, spicefilesList) { untested();
        spiceNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
    }
}

// -----------------------------------------------------------
// update the list of paths, pruning non-existing paths
void QucsApp::updatePathList(void)
{itested();
    // check each path actually exists, if not remove it
    QMutableListIterator<QString> i(qucsPathList);
    while (i.hasNext()) { untested();
        i.next();
        QDir thispath(i.value());
        if (!thispath.exists())
        { untested();
            // the path does not exist, remove it from the list
            i.remove();
        }
    }
}

// replace the old path list with a new one
void QucsApp::updatePathList(QStringList newPathList)
{ untested();
    // clear out the old path list
    qucsPathList.clear();

    // copy the new path into the path list
    foreach(QString path, newPathList)
    { untested();
        qucsPathList.append(path);
    }
    // do the normal path update operations
    updatePathList();
}


void QucsApp::updateRecentFilesList(QString s)
{ untested();
  QSettings* settings = new QSettings("qucs","qucs");
  QucsSettings.RecentDocs.removeAll(s);
  QucsSettings.RecentDocs.prepend(s);
  if (QucsSettings.RecentDocs.size() > MaxRecentFiles) { untested();
    QucsSettings.RecentDocs.removeLast();
  }
  settings->setValue("RecentDocs",QucsSettings.RecentDocs.join("*"));
  delete settings;
  slotUpdateRecentFiles();
}

void QucsApp::slotSaveDiagramToGraphicsFile()
{ untested();
  slotSaveSchematicToGraphicsFile(true);
}

void QucsApp::slotSaveSchematicToGraphicsFile(bool diagram)
{ untested();
  ImageWriter *writer = new ImageWriter(lastExportFilename);
  writer->setDiagram(diagram);
  if (!writer->print(DocumentTab->current())) { untested();
    lastExportFilename = writer->getLastSavedFile();
    statusBar()->showMessage(QObject::tr("Successfully exported"), 2000);
  }
  delete writer;
}

// #########################################################################


QucsDoc* newSchematicDoc(QucsApp*, QString const&, QWidget*); // tmp hack.

QucsDoc *QucsTabWidget::createEmptySchematic(const QString &name)
{itested();
  // create a schematic
  QFileInfo Info(name);
  assert(App);
  QucsDoc *d = newSchematicDoc(App, name, this);
  QWidget* w = dynamic_cast<QWidget*>(d);
  assert(w);
  int i = addTab(w, QPixmap(":/bitmaps/empty.xpm"), name.isEmpty() ? QObject::tr("untitled") : Info.fileName());
  setCurrentIndex(i);
  return d;
}

QucsDoc* newTextDoc(QucsApp&, QString const&, QWidget*); // tmp hack.

QucsDoc *QucsTabWidget::createEmptyTextDoc(const QString &name)
{itested();
  // create a text document
  QFileInfo Info(name);
  QucsDoc *d = newTextDoc(*App, name, this);
  QWidget* w = dynamic_cast<QWidget*>(d);
  assert(w);
  int i = addTab(w, QPixmap(":/bitmaps/empty.xpm"), name.isEmpty() ? QObject::tr("untitled") : Info.fileName());
  setCurrentIndex(i);
  return d;
}

void QucsTabWidget::setSaveIcon(bool state, int index)
{ untested();
  // set document tab icon to "smallsave.xpm" or "empty.xpm"
  QString icon = (state)? ":/bitmaps/smallsave.xpm" : ":/bitmaps/empty.xpm";
  if (index < 0) { untested();
    index = currentIndex();
  }
  setTabIcon(index, QPixmap(icon));
}

void QucsTabWidget::slotCxMenuClose()
{ untested();
  // close tab where the context menu was opened
  App->slotFileClose(contextTabIndex);
}

void QucsTabWidget::slotCxMenuCloseOthers()
{ untested();
  // close all tabs, except the one where the context menu was opened
  App->closeAllFiles(contextTabIndex);
}

void QucsTabWidget::slotCxMenuCloseLeft()
{ untested();
  // close all tabs to the left of the current one
  App->closeAllLeft(contextTabIndex);
}

void QucsTabWidget::slotCxMenuCloseRight()
{ untested();
  // close all tabs to the right of the current one
  App->closeAllRight(contextTabIndex);
}

void QucsTabWidget::slotCxMenuCloseAll()
{ untested();
  App->slotFileCloseAll();
}

void QucsTabWidget::slotCxMenuCopyPath()
{ untested();
  // copy the document full path to the clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(docName);
}

void QucsTabWidget::slotCxMenuOpenFolder()
{ untested();
  QFileInfo Info(docName);
  QDesktopServices::openUrl(QUrl::fromLocalFile(Info.canonicalPath()));
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings()
{ untested();
    QSettings settings ("qucs","qucs");

    settings.setValue("x", QucsSettings.x);
    settings.setValue("y", QucsSettings.y);
    settings.setValue("dx", QucsSettings.dx);
    settings.setValue("dy", QucsSettings.dy);
    settings.setValue("font", QucsSettings.font.toString());
    // store LargeFontSize as a string, so it will be also human-readable in the settings file (will be a @Variant() otherwise)
    settings.setValue("LargeFontSize", QString::number(QucsSettings.largeFontSize));
    settings.setValue("maxUndo", QucsSettings.maxUndo);
    settings.setValue("NodeWiring", QucsSettings.NodeWiring);
    settings.setValue("BGColor", QucsSettings.BGColor.name());
    settings.setValue("Editor", QucsSettings.Editor);
    settings.setValue("FileTypes", QucsSettings.FileTypes);
    settings.setValue("Language", QucsSettings.Language);
    settings.setValue("Comment", QucsSettings.Comment.name());
    settings.setValue("String", QucsSettings.String.name());
    settings.setValue("Integer", QucsSettings.Integer.name());
    settings.setValue("Real", QucsSettings.Real.name());
    settings.setValue("Character", QucsSettings.Character.name());
    settings.setValue("Type", QucsSettings.Type.name());
    settings.setValue("Attribute", QucsSettings.Attribute.name());
    settings.setValue("Directive", QucsSettings.Directive.name());
    settings.setValue("Task", QucsSettings.Task.name());
    //settings.setValue("Qucsator", QucsSettings.Qucsator);
    //settings.setValue("BinDir", QucsSettings.BinDir);
    //settings.setValue("LangDir", QucsSettings.LangDir);
    //settings.setValue("LibDir", QucsSettings.LibDir);
    settings.setValue("AdmsXmlBinDir", QucsSettings.AdmsXmlBinDir.canonicalPath());
    settings.setValue("AscoBinDir", QucsSettings.AscoBinDir.canonicalPath());
    //settings.setValue("OctaveDir", QucsSettings.OctaveDir);
    //settings.setValue("ExamplesDir", QucsSettings.ExamplesDir);
    //settings.setValue("DocDir", QucsSettings.DocDir);
    // settings.setValue("OctaveBinDir", QucsSettings.OctaveBinDir.canonicalPath());
    settings.setValue("OctaveExecutable",QucsSettings.OctaveExecutable);
    settings.setValue("QucsHomeDir", QucsSettings.QucsHomeDir.canonicalPath());
    settings.setValue("IgnoreVersion", QucsSettings.IgnoreFutureVersion);
    settings.setValue("GraphAntiAliasing", QucsSettings.GraphAntiAliasing);
    settings.setValue("TextAntiAliasing", QucsSettings.TextAntiAliasing);
    settings.setValue("Editor", QucsSettings.Editor);
    settings.setValue("ShowDescription", QucsSettings.ShowDescriptionProjectTree);

    // Copy the list of directory paths in which Qucs should
    // search for subcircuit schematics from qucsPathList
    settings.remove("Paths");
    settings.beginWriteArray("Paths");
    int i = 0;
    foreach(QString path, qucsPathList) { untested();
         settings.setArrayIndex(i);
         settings.setValue("path", path);
         i++;
     }
     settings.endArray();

  return true;

}

QVariant QucsFileSystemModel::data( const QModelIndex& index, int role ) const
{ untested();
  if (role == Qt::DecorationRole) { // it's an icon
    QString dName = fileName(index);
    if (dName.endsWith("_prj")) { // it's a Qucs project
      // for some reason SVG does not always work on Windows, so use PNG
      return QIcon(":bitmaps/hicolor/128x128/apps/qucs.png");
    }
  }
  // return default system icon
  return QFileSystemModel::data(index, role);
}

// function below is adapted from https://stackoverflow.com/questions/10789284/qfilesystemmodel-sorting-dirsfirst
bool QucsSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{ untested();
  // if sorting by file names column
  if (sortColumn() == 0) { untested();
    QucsFileSystemModel *model = qobject_cast<QucsFileSystemModel*>(sourceModel());
    // get the current sort order (do we need this ?)
    bool asc = sortOrder() == Qt::AscendingOrder ? true : false;

    QFileInfo leftFileInfo = model->fileInfo(left);
    QFileInfo rightFileInfo = model->fileInfo(right);
    QString leftFileName = model->fileName(left);
    QString rightFileName = model->fileName(right);

    // If DotAndDot move in the beginning
    if (sourceModel()->data(left).toString() == ".."){ untested();
      return asc;
    }else if (sourceModel()->data(right).toString() == ".."){ untested();
      return !asc;
    }

    // move dirs upper
    if (!leftFileInfo.isDir() && rightFileInfo.isDir()) { untested();
      return !asc;
    }else if (leftFileInfo.isDir() && !rightFileInfo.isDir()) { untested();
      return asc;
    } else if (!leftFileInfo.isDir() || rightFileInfo.isDir()) { untested();
       //?
    }else if (!leftFileName.endsWith("_prj") && rightFileName.endsWith("_prj")) { untested();
      return !asc;
    }else if (leftFileName.endsWith("_prj") && !rightFileName.endsWith("_prj")) { untested();
      return asc;
    }
  }else{ untested();
  }

  return QSortFilterProxyModel::lessThan(left, right);
}

// vim:ts=8:sw=2:et
