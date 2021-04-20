/***************************************************************************
    copyright            : (C) 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qucs_globals.h"
#include "qt_compat.h"
#include "qucs_app.h"
#include "qucs_tabs.h"
#include "qucsdoc.h"
//#include "octave_window.h"
#include "misc.h"

#include <QLabel>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTimer>

namespace qucs {

// obsolete.
void App::clearWorkToolbar()
{ untested();
	assert(_docToolBar);
	_docToolBar->clear();
}

// obsolete.
// void App::addWorkToolbarAction(QAction* a)
// { untested();
// 	assert(_docToolBar);
// 	_docToolBar->addAction(a);
// }

// why "AtStartup?!"
void App::openFileAtStartup(QString const& arg)
{
      QFileInfo Info(arg);
      QucsSettings.QucsWorkDir = Info.absoluteDir().absolutePath().toStdString(); // ?
      QString p = QString_(QucsSettings.QucsWorkDir) + QDir::separator() + Info.fileName();
      gotoPage(p);
}

// Change to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
// this is used "atStartup"...
bool App::gotoPage(const QString& Name)
{itested();
	// is Name the filename?!
	int No = DocumentTab->currentIndex();

	int i = 0;
	Doc* d = findDoc(Name, &i);  // search, if page is already loaded

	if(d) { untested();
		// open page found
		DocumentTab->setCurrentIndex(i);  // make new document the current
		return true; // BUG
	}else{itested();
	}

#if 0
	DocumentTab->load(Name);
#else

	QFileInfo Info(Name);
	bool is_schematic = false;

	std::string suffix = "." + Info.suffix().toStdString();
	for(auto i : language_dispatcher){
		trace3("a schematic?", i.first, i.second->short_label(), suffix);
		if(i.first == suffix){
			is_schematic = true;
		}else{
		}
	}

	// TODO. open schematic associated with document?
	if(is_schematic) {itested();
		d = DocumentTab->createEmptySchematic(Name);
	}else if(Info.suffix() == "sym") { untested();
		incomplete();
	} else { untested();
		d = DocumentTab->createEmptyTextDoc(Name);
	}

	if(!d->load()) {untested();
		delete d;
		DocumentTab->setCurrentIndex(No);
		// view->drawn = false;
		return false;
	}else{itested();
		slotChangeView(DocumentTab->current()); //?

		// if only an untitled document was open -> close it
		if(!getDoc(0)->docName().isEmpty()){ untested();
		}else if(!getDoc(0)->DocChanged){itested();
			delete DocumentTab->widget(0);
		}else{ untested();
		}

		//view->drawn = false;
		return true;
	}
#endif
}

// turn Toolbar on or off
void App::slotViewToolBar(bool toggle)
{
	if(fileToolbar){ untested();
	  fileToolbar->setVisible(toggle);
	}else{itested();
	}

	if(editToolbar){ untested();
	  editToolbar->setVisible(toggle);
	}else{itested();
	}

	if(viewToolbar){ untested();
	  viewToolbar->setVisible(toggle);
	}else{itested();
	}

	if(_docToolBar){ untested();
	  _docToolBar->setVisible(toggle);
	}else{itested();
	}
}

// ----------------------------------------------------------
// turn Statusbar on or off
void App::slotViewStatusBar(bool toggle)
{
  statusBar()->setVisible(toggle);
}

// ----------------------------------------------------------
// turn Brwose Dock Window on or off
void App::slotViewBrowseDock(bool toggle)
{
  dock->setVisible(toggle);
}

// ----------------------------------------------------------
void App::slotToggleDock(bool on)
{
  viewBrowseDock->blockSignals(true);
  viewBrowseDock->setChecked(on);
  viewBrowseDock->blockSignals(false);
}

// ----------------------------------------------------------
// turn Octave Dock Window on or off
void App::slotViewOctaveDock(bool toggle)
{
	if(octDock){
	  octDock->setVisible(toggle);
	  if (toggle) {
	//	 octave->startOctave();
	  }else{
	  }
	}else{
	}
}

// ----------------------------------------------------------
void App::slotToggleOctave(bool on)
{
  viewOctaveDock->blockSignals(true);
  viewOctaveDock->setChecked(on);
  viewOctaveDock->blockSignals(false);
}
// ----------------------------------------------------------
void App::printCursorPosition(int x, int y)
{
  PositionLabel->setText(QString::number(x)+" : "+QString::number(y));
  PositionLabel->setMinimumWidth(PositionLabel->width());
}

// --------------------------------------------------------------
// called by document, update undo state
void App::slotUpdateUndo(bool isEnabled)
{itested();
  assert(undo);
  undo->setEnabled(isEnabled);
}

// --------------------------------------------------------------
// called by document, update redo state
void App::slotUpdateRedo(bool isEnabled)
{itested();
  assert(redo);
  redo->setEnabled(isEnabled);
}
// ----------------------------------------------------------
// ----------------------------------------------------------
void App::slotResetWarnings()
{
  QFont f = WarningLabel->font();   // reset warning label
  f.setWeight(QFont::Normal);
  WarningLabel->setFont(f);
  misc::setWidgetForegroundColor(WarningLabel, Qt::black);
  WarningLabel->setText(tr("no warnings"));
}
// ----------------------------------------------------------
void App::slotShowWarnings()
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



} // qucs

#define ASSIGN_STUFF \
  Doc *qd = DocumentTab->current(); \
  assert(qd); \
  auto s = prechecked_cast<QAction*>(sender()); \
  assert(s);

#include <QListWidgetItem>
#include "component_widget.h"
#include <QProcess>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLineEdit>

namespace qucs {

// Is called when the mouse is clicked within the Component QIconView.
void App::slotSelectComponent(QListWidgetItem *item)
{
  slotHideEdit(); // disable text edit of component property

  Doc* qd = DocumentTab->current();
  assert(qd);
//  auto s = prechecked_cast<QAction*>(sender());
//  assert(s);

  if(auto ci=dynamic_cast<ComponentListWidgetItem*>(item)){ untested();
    qd->actionSelectElement(ci);
  }else{ untested();
  }

  // incomplete(); possibly
#if 0
  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem  = 0;   // no component/diagram/painting selected

  if(item == 0) {   // mouse button pressed not over an item ?
    CompComps->clearSelection();  // deselect component in ViewList
    return;
  }

  if(view->drawn){
    ((SchematicBase*)DocumentTab->currentWidget())->viewport()->update();
  }else{
  }
  view->drawn = false;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  int i = CompComps->row(item);
  QList<Module *> Comps;

  // if symbol mode, only paintings are enabled.
  Comps = Category::getModules(CompChoose->currentText());

  QString name = CompComps->item(i)->text();
  QString CompName;
  QString CompFile_qstr;
  char *CompFile_cptr;

  qDebug() << "pressed CompComps id" << i << name;
  QVariant v = CompComps->item(i)->data(Qt::UserRole);
  iconCompInfoStruct iconCompInfo = v.value<iconCompInfoStruct>();
  qDebug() << "slotSelectComponent()" << iconCompInfo.catIdx << iconCompInfo.compIdx;

  Category const* cat = Category::categories.at(iconCompInfo.catIdx);
  assert(cat);
  Module *mod = (*cat)[iconCompInfo.compIdx];
  assert(mod);
  qDebug() << "mod->info" << mod->element()->name();
  Element const* e = mod->element();
  if (e) {
    // static component
    view->selElem = prechecked_cast<Element*>(e->clone()); // BUG. memory leak
    assert(view->selElem);
  } else {
    incomplete();
#if 0 // pointless
    // Verilog-A component
    InfosVA = mod->infoVA;
    // get JSON file out of item name on widgetitem
    QString filename = Module::vaComponents[name];
    if (InfosVA) {
      view->selElem = (*InfosVA) (CompName, CompFile_qstr, true, filename);
    }
#endif
  }

#if 0 // what is this?
  if (Infos || InfosVA) {
    // change currently selected category, so the user will
    //   see where the component comes from
    CompChoose->setCurrentIndex(iconCompInfo.catIdx+1); // +1 due to the added "Search Results" item
    ccCurIdx = iconCompInfo.catIdx; // remember the category to select when exiting search
    //!! comment out the above two lines if you would like that the search
    //!!   returns back to the last selected category instead
  }
#endif
#endif
}

// -----------------------------------------------------------------------

// TODO?   comment out the selected text on a text document
// \todo update the status or tooltip message
void App::slotZoomIn()
{
  ASSIGN_STUFF
  qd->actionZoomIn(s);
}
// -----------------------------------------------------------------------
void App::slotEditCut()
{
  statusBar()->showMessage(tr("Cutting selection..."));
  slotHideEdit(); // disable text edit of component property

  ASSIGN_STUFF
  qd->actionCut(s);

  statusBar()->showMessage(tr("Ready."));
}
// --------------------------------------------------------------------
void App::slotEditCopy()
{
  statusBar()->showMessage(tr("Copying selection to clipboard..."));

  ASSIGN_STUFF
  qd->actionCopy(s);

  statusBar()->showMessage(tr("Ready."));
}
// -----------------------------------------------------------------------
void App::slotEditPaste()
{
  ASSIGN_STUFF
  qd->actionEditPaste(s);
}
// -----------------------------------------------------------------------
void App::slotEditDelete()
{
  ASSIGN_STUFF
  qd->slotEditDelete(s);
}
// -----------------------------------------------------------------------
void App::slotEditUndo()
{
  ASSIGN_STUFF
  qd->slotEditUndo(s);
}
// --------------------------------------------------------------
void App::slotEditRedo()
{
  ASSIGN_STUFF
  qd->slotEditRedo(s);
}
// --------------------------------------------------------------
// ---------------------------------------------------------------------

extern QString lastDirOpenSave; // to remember last directory and file
// ------------------------------------------------------------------------
///
/// \brief App::editFile
/// \param File is the filename, or empty for a new file
///
/// Called by :
/// - slotTextNew()
/// - slotShowLastMsg()
/// - slotShowLastNetlist()
/// - edit properties of components (such as spice, verilog)
///
//
//BUG. same in qucs_tabs
void App::editFile(const QString& File)
{
	incomplete();
	if ((QucsSettings.Editor == "qucs") || QucsSettings.Editor == "") {
		// The Editor is 'qucs' or empty, open a net document tab
		if (File.isEmpty()) {
			std::string name = "untitled";
//			TextDoc *d = new TextDoc(this, "", DocumentTab);

			Widget* o = widget_dispatcher.clone("TextDoc");
			//  e->setParam("name", name.toStdString());
			assert(o);
			Doc* d = dynamic_cast<Doc*>(o);
			assert(d);
			QWidget* w = dynamic_cast<QWidget*>(o);
			assert(w);
			d->setParent(this);
			trace1("setname??", name);
			d->setName(QString_(name)); // it's actually the (full?) filename? BUG: name vs label

			int i = DocumentTab->addTab(w, QPixmap(":/bitmaps/empty.xpm"), QObject::tr("untitled"));
			DocumentTab->setCurrentIndex(i);
		} else {
			slotHideEdit(); // disable text edit of component property
			statusBar()->showMessage(tr("Opening file..."));
			QFileInfo finfo(File);

			if(!finfo.exists()){
				statusBar()->showMessage(tr("Opening aborted, file not found."), 2000);
			}else{
				gotoPage(File);
				lastDirOpenSave = File;   // remember last directory and file
				statusBar()->showMessage(tr("Ready."));
			}
		}
	} else {
	}
#if 0
	{
		// use an external editor
		QString prog;
		QStringList args;

		QString editorPath = QucsSettings.Editor;
		QFileInfo editor(editorPath);
		prog = QDir::toNativeSeparators(editor.canonicalFilePath());

		if (!File.isEmpty()) {
			args << File;
		}

		QProcess *externalEditor = new QProcess();
		qDebug() << "taskElement: " << editorPath << args.join(" ");
		externalEditor->start(prog, args);

		if( !externalEditor->waitForStarted(1000) ) {
			QMessageBox::critical(this, tr("Error"), tr("Cannot start text editor: \n\n%1").arg(editorPath));
			delete externalEditor;
			return;
		}
		qDebug() << externalEditor->readAllStandardError();

		// to kill it before qucs ends
		connect(this, SIGNAL(signalKillEmAll()), externalEditor, SLOT(kill()));
	}
#endif
}

// ------------------------------------------------------------------------
#if 0 // BUG. works with qucsator only.
void App::slotShowLastMsg()
{
  editFile(QString_(QucsSettings.homeDir()) + QDir::separator() + "log.txt");
}
void App::slotShowLastNetlist()
{
  editFile(QDir_(QucsSettings.homeDir()).filePath("netlist.txt"));
}
#endif
// ------------------------------------------------------------------------
// Is called to start the text editor.
void App::slotCallEditor()
{
	// command?
  editFile(QString(""));
}

// ------------------------------------------------------------------------
void App::slotCallFilter()
{
  launchTool("qucsfilter", "filter synthesis");
}

void App::slotCallActiveFilter()
{
  launchTool("qucsactivefilter", "active filter synthesis");
}

void App::slotCallLine()
{
  launchTool("qucstrans", "line calculation");
}

void App::slotCallLibrary()
{
  launchTool("qucslib", "library");
}

void App::slotCallMatch()
{
  incomplete();
  // ask dispatcher
//  MatchDialog *d = new MatchDialog(this);
//  d->exec();
}

void App::slotCallAtt()
{
  launchTool("qucsattenuator", "attenuator calculation");
}

void App::slotCallRes()
{
  launchTool("qucsrescodes", "resistor color code calculation");
}

void App::slotCallPowerComb()
{
  launchTool("qucspowercombining", "Power combiner synthesis");
}


/*!
 * \brief launch an external application passing arguments
 *
 * \param prog  executable program name (will be transformed depending on OS)
 * \param progDesc  program description string (used for error messages)
 * \param args  arguments to pass to the executable
 */
void App::launchTool(const QString& prog, const QString& progDesc, const QString& args) {
  QProcess *tool = new QProcess();

// platform.h? config.h??
///#ifdef __MINGW32__
///  QString cmd = QDir::toNativeSeparators("\""+QucsSettings.BinDir + prog + ".exe\"");
///#elif __APPLE__
///  QString cmd = QDir::toNativeSeparators(QucsSettings.BinDir + prog + ".app/Contents/MacOS/" + prog);
///#else
///#endif

  QString cmd = QDir::toNativeSeparators(QString_(QucsSettings.BinDir) + prog);

  char const* var = getenv("QUCS_USE_PATH");
  if(var != NULL) {
    // just use PATH. this is currently bound to a contition, to maintain
    // backwards compatibility with QUCSDIR
    qDebug() << "QUCS_USE_PATH";
    cmd = prog;
  }else{
    // don't know what this is about. doesn't make sense in the case above.
    // (and in all other cases i can think of)
    tool->setWorkingDirectory(QString_(QucsSettings.BinDir));
  }

  qDebug() << "taskElement :" << cmd + " " + args;

  // FIXME: use start(const QString& program, const QStringList& arguments);
  tool->start(cmd + " " + args);
  
  // BUG: use signals. see QUCSATOR invocation
  if(!tool->waitForStarted(1000) ) {
    QMessageBox::critical(this, tr("Error"),
	tr("Cannot start %1 program! \n\n(%2)").arg(progDesc, cmd + " " + args));
    delete tool;
    return;
  }

  // to kill the application first before qucs finishes exiting
  connect(this, SIGNAL(signalKillEmAll()), tool, SLOT(kill()));
}

void App::slotHelpOnline()
{
  QString link = "http://qucs-help.readthedocs.io/";
  QDesktopServices::openUrl(QUrl(link));
}

void App::showHTML(const QString& Page)
{
  launchTool("qucshelp", "help", Page);
}

// ---------------------------------------------------------------------
// Is called when the find action is triggered.
void App::slotEditFind()
{
#if 0
  SearchDia->initSearch(DocumentTab->current(),
      ((TextDoc *)DocumentTab->current())->textCursor().selectedText(), false);
#else
  incomplete();
//  display SearchDialog widget
#endif
}

// --------------------------------------------------------------
void App::slotChangeProps()
{
  ASSIGN_STUFF
  qd->actionChangeProps(s);
}

// --------------------------------------------------------------
void App::slotAddToProject()
{
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No project open!"));
    return;
  }


  QStringList List = QFileDialog::getOpenFileNames(this, tr("Select files to copy"),
    lastDir.isEmpty() ? QString(".") : lastDir, QucsFileFilter);

  if(List.isEmpty()) {
    statusBar()->showMessage(tr("No files copied."), 2000);
    return;
  }


  char *Buffer = (char*)malloc(0x10000);
  if(!Buffer) return;  // should never happen

  QStringList FileList = List;  // make a copy as recommended by Qt
  QStringList::Iterator it = FileList.begin();
  QFileInfo Info(*it);
  lastDir = Info.absolutePath();  // remember last directory

  // copy all files to project directory
  int Num;
  QFile origFile, destFile;
  while(it != FileList.end()) {
    Info.setFile(*it);
    origFile.setFileName(*it);
    destFile.setFileName(QDir_(QucsSettings.QucsWorkDir).absolutePath() +
                     QDir::separator() + Info.fileName());

    if(!origFile.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot open \"%1\" !").arg(*it));
      it++;
      continue;
    }

    if(destFile.exists())
      if(QMessageBox::information(this, tr("Overwrite"),
           tr("File \"%1\" already exists.\nOverwrite ?").arg(*it), QMessageBox::Yes,
           QMessageBox::No|QMessageBox::Default|QMessageBox::Escape)
         != QMessageBox::Yes) {
        origFile.close();
        it++;
        continue;
      }

    if(!destFile.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot create \"%1\" !").arg(*it));
      origFile.close();
      it++;
      continue;
    }

    // copy data
    do {
      Num = origFile.read(Buffer, 0x10000);
      if(Num < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot read \"%1\" !").arg(*it));
        break;
      }
      Num = destFile.write(Buffer, Num);
      if(Num < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot write \"%1\" !").arg(*it));
        break;
      }
    } while(Num == 0x10000);

    origFile.close();
    destFile.close();
    it++;
  }

  free(Buffer);
  statusBar()->showMessage(tr("Ready."));
}

// -----------------------------------------------------------
// BUG/feature? why does the widget not receive key presses?!
void App::slotCursor(arrow_dir_t dir)
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionCursor(dir);
}

// -----------------------------------------------------------
// Is called if user clicked on component text of if return is
// pressed in the component text QLineEdit.
// In "view->MAx3" is the number of the current property.
void App::slotApplyCompText()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  incomplete(); // edit text differently with graphics stuff

//  qd->actionApplyCompText();
}


// -----------------------------------------------------------
// Is called if the text of the property edit changed, to match
// the width of the edit field.
void App::slotResizePropEdit(const QString& t)
{
  editText->resize(editText->fontMetrics().horizontalAdvance(t)+4,
                   editText->fontMetrics().lineSpacing());
}

// -----------------------------------------------------------
void App::slotCreateLib()
{
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project with subcircuits!"));
    return;
  }

  incomplete(); // ask dispatcher.
  // LibraryDialog *d = new LibraryDialog(this);
  // d->fillSchematicList(Content->exportSchematic());
  // d->exec();
}

// -----------------------------------------------------------
void App::slotImportData()
{
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project first!"));
    return;
  }else{
  }

  incomplete(); // ask dispatcher
  // ImportDialog *d = new ImportDialog(this);
  // d->exec();
}

// -----------------------------------------------------------
// // BUG this is a diagram slot.
void App::slotExportGraphAsCsv()
{
  incomplete();
  Doc* qd = DocumentTab->current();
  assert(qd);

  hideEdit();
//  qd->actionExportGraphAsCsv();
}


// ----------------------------------------------------------
void App::slotCreatePackage()
{
  slotHideEdit(); // disable text edit of component property

  incomplete(); // ask dispatcher
  ///PackageDialog *d = new PackageDialog(this, true);
  ///d->exec();
}

// ----------------------------------------------------------
void App::slotExtractPackage()
{
  slotHideEdit(); // disable text edit of component property

  incomplete(); //ask dispatcher
  // PackageDialog *d = new PackageDialog(this, false);
  // d->show();
  // d->extractPackage();
  // readProjects();
}

void App::slotOpenRecent()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    gotoPage(action->data().toString());
    updateRecentFilesList(action->data().toString());
  }
}

void App::slotUpdateRecentFiles()
{
incomplete();
#if 0
  QMutableStringListIterator it(QucsSettings.RecentDocs);
  while(it.hasNext()) {
    if (!QFile::exists(it.next())) {
        it.remove();
    }
  }

  for (int i = 0; i < MaxRecentFiles; ++i) {
    if (i < QucsSettings.RecentDocs.size()) {
      fileRecentAction[i]->setText(QucsSettings.RecentDocs[i]);
      fileRecentAction[i]->setData(QucsSettings.RecentDocs[i]);
      fileRecentAction[i]->setVisible(true);
    } else {
      fileRecentAction[i]->setVisible(false);
    }
  }
#endif
}

void App::slotClearRecentFiles()
{
  QucsSettings.RecentDocs.clear();
  slotUpdateRecentFiles();
}

// launch the dialog to select dynamic modules
// redo?
void App::slotLoadModule()
{
#if 0
    qDebug() << "slotLoadModule";

    LoadDialog *ld = new LoadDialog(this);
    ld->setApp(this);

    // fech list of _symbol.json
    // \todo fetch timestamp of VA, JSON, if VA newer, need to reload.

    QDir projDir = QucsSettings.QucsWorkDir.absolutePath();

    QStringList files;
    QString fileSuffix = "*_symbol.json";

    files = projDir.entryList(QStringList(fileSuffix),
                                 QDir::Files | QDir::NoSymLinks);

    // no JSON files or no a project?
    if (!files.size()){
        QMessageBox::critical(this, tr("Error"),
                     tr("Symbol files not found in: %1\n\n"
                        "Is the project open?\n"
                        "Have you saved the Verilog-A symbols?")
                       .arg(QString(projDir.absolutePath())));
        return;
    }

    // initialize dialog

    // pass list of potential symbol files
    ld->symbolFiles << files;
    ld->projDir = projDir;
    ld->initDialog();

    // \todo check what is already loaded, offer skip, reload

    //pass stuff to ld dialog
    // run, let user do the selections

    if (ld->exec() == QDialog::Accepted) {

      Module::vaComponents = ld->selectedComponents;

      // dialog write new bitmap into JSON
      // load, unload, reload
      // inform if symbol changed
      // populate Module::vaComponents
      // vaComponents are selected with the dialog
      // dialog should populate acording to checkboxes
      // build vaComponents QMap

      // remove all previously registered modules
      QMutableHashIterator<QString, Module *> it( Module::Modules );
      while(it.hasNext()) {
        it.next();

	// BUG: what is this?
	// ( it's not needed for loading modules.)
//        if (it.value()->category == QObject::tr("verilog-a user devices")) {
//          it.remove();
//        }
      }

      if (! Module::vaComponents.isEmpty()) {
        // Register whatever is in Module::vaComponents
	incomplete();
	// components register themselves upon loading.
        // Module::registerDynamicComponents();

        // update the combobox, set new category in view
        // pick up new category 'verilog-a user components' from `Module::category`
        //set new category into view
        App::fillComboBox(true);
        CompChoose->setCurrentIndex(CompChoose->count()-1);
        slotSetCompView(CompChoose->count()-1);

        // icons of dynamically registered components ready to be dragged
      }
      else {

	// don't know what this is about.
	incomplete();

        // remove any previously registerd icons from the listview
        int foundCat = CompChoose->findText(QObject::tr("verilog-a user devices"));
        if (foundCat != -1) {
          CompChoose->setCurrentIndex(foundCat);
          CompComps->clear();
        }
      }
    }

    delete ld;

#endif
}

void App::slotBuildModule()
{
// // BUG this is part of qucsator or qucsator driver
	incomplete();
}

// ----------------------------------------------------------
void App::slotExportSchematic()
{
  incomplete();
}

} // qucs
