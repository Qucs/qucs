/***************************************************************************
                               qucs_actions.cpp
                              -----------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/* Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>        */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Qucs Toolbar

#include <QtCore>
#include <stdlib.h>
#include <limits.h>

#include <QProcess>
#include "qt_compat.h"
#include <QLineEdit>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QComboBox>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QMutableHashIterator>
#include <QListWidget>
#include <QDesktopServices>
#include <QUrl>

#include "projectView.h"
#include "qucs.h"
#include "textdoc.h"
#include "mouseactions.h"
#include "messagedock.h"

// BUG: untangle dialogs
#include "matchdialog.h"
#include "changedialog.h"
#include "searchdialog.h"
#include "librarydialog.h"
#include "loaddialog.h"
#include "importdialog.h"
#include "packagedialog.h"
#include "aboutdialog.h"
//

#include "module.h"
#include "misc.h"
#include "globals.h"
#include "actions.h"
#include "platform.h"

// -----------------------------------------------------------------------
// Is called, when "set on grid" action is triggered.
void QucsApp::slotOnGrid(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionOnGrid(on);
}

// -----------------------------------------------------------------------
// Is called when the rotate toolbar button is pressed.
void QucsApp::slotEditRotate(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditRotate(on);
}

// -----------------------------------------------------------------------
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorX(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditMirrorX(on);
}

// -----------------------------------------------------------------------
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorY(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditMirrorY(on);
}

// -----------------------------------------------------------------------
// Is called when the activate/deactivate toolbar button is pressed.
// It also comments out the selected text on a text document
// \todo update the status or tooltip message
void QucsApp::slotEditActivate(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditActivate(on);
}

// ------------------------------------------------------------------------
// Is called if "Delete"-Button is pressed.
// Toolbar::?
void QucsApp::slotEditDelete(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditDelete(on);
}

// -----------------------------------------------------------------------
// Is called if "Wire"-Button is pressed.
void QucsApp::slotSetWire(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionSetWire(on);
}

// -----------------------------------------------------------------------
void QucsApp::slotInsertLabel(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionInsertLabel(on);
}

// -----------------------------------------------------------------------
void QucsApp::slotSetMarker(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionSetMarker(on);
}

// -----------------------------------------------------------------------
// Is called, when "move component text" action is triggered.
void QucsApp::slotMoveText(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionMoveText(on);
}

// -----------------------------------------------------------------------
// Is called, when "Zoom in" action is triggered.
void QucsApp::slotZoomIn(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionZoomIn(on);
}

// -----------------------------------------------------------------------
void QucsApp::slotEscape()
{
    select->setChecked(true);
    slotSearchClear();
}

// -----------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsApp::slotSelect(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionSelect(on);
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->showMessage(tr("Cutting selection..."));
  slotHideEdit(); // disable text edit of component property

  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionCut();

  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------------
void QucsApp::slotEditCopy()
{
  statusBar()->showMessage(tr("Copying selection to clipboard..."));

  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionCopy();

  statusBar()->showMessage(tr("Ready."));
}

// -----------------------------------------------------------------------
void QucsApp::slotEditPaste(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditPaste(on);
}


// -----------------------------------------------------------------------
void QucsApp::slotInsertEntity ()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionInsertEntity();
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the equation toolbar button.
void QucsApp::slotInsertEquation(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionInsertEquation(on);
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the ground toolbar button.
void QucsApp::slotInsertGround(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionInsertGround(on);
}


// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the port toolbar button.
void QucsApp::slotInsertPort(bool on)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionInsertPort(on);
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditUndo()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditUndo();
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditRedo()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionEditRedo();
}

// --------------------------------------------------------------
// Is called, when "Align top" action is triggered.
void QucsApp::slotAlignTop()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionAlign(0);
}

// --------------------------------------------------------------
// Is called, when "Align bottom" action is triggered.
void QucsApp::slotAlignBottom()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionAlign(1);
}

// --------------------------------------------------------------
// Is called, when "Align left" action is triggered.
void QucsApp::slotAlignLeft()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionAlign(2);
}

// --------------------------------------------------------------
// Is called, when "Align right" action is triggered.
void QucsApp::slotAlignRight()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionAlign(3);
}

// --------------------------------------------------------------
// Is called, when "Distribute horizontally" action is triggered.
void QucsApp::slotDistribHoriz()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionDistrib(0);
}

// --------------------------------------------------------------
// Is called, when "Distribute vertically" action is triggered.
void QucsApp::slotDistribVert()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionDistrib(1);
}

// --------------------------------------------------------------
// Is called, when "Center horizontally" action is triggered.
void QucsApp::slotCenterHorizontal()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionAlign(4);
}

// --------------------------------------------------------------
// Is called, when "Center vertically" action is triggered.
void QucsApp::slotCenterVertical()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionAlign(5);
}

// ---------------------------------------------------------------------
// Is called when the "select all" action is triggered.
void QucsApp::slotSelectAll()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  hideEdit();
  qd->actionSelectAll();
}

// ---------------------------------------------------------------------
// Is called when the "select markers" action is triggered.
// schematic only
void QucsApp::slotSelectMarker()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionSelectMarker();
}


extern QString lastDirOpenSave; // to remember last directory and file

// ------------------------------------------------------------------------
///
/// \brief QucsApp::editFile
/// \param File is the filename, or empty for a new file
///
/// Called by :
/// - slotTextNew()
/// - slotShowLastMsg()
/// - slotShowLastNetlist()
/// - edit properties of components (such as spice, verilog)
///
void QucsApp::editFile(const QString& File)
{
    if ((QucsSettings.Editor.toLower() == "qucs") | QucsSettings.Editor.isEmpty())
    {
        // The Editor is 'qucs' or empty, open a net document tab
        if (File.isEmpty()) {
            TextDoc *d = new TextDoc(this, "");
            int i = DocumentTab->addTab(d, QPixmap(":/bitmaps/empty.xpm"), QObject::tr("untitled"));
            DocumentTab->setCurrentIndex(i);
        }
        else
        {
            slotHideEdit(); // disable text edit of component property

            statusBar()->showMessage(tr("Opening file..."));

            QFileInfo finfo(File);

            if(!finfo.exists())
                statusBar()->showMessage(tr("Opening aborted, file not found."), 2000);
            else {
                gotoPage(File);
                lastDirOpenSave = File;   // remember last directory and file
                statusBar()->showMessage(tr("Ready."));
            }
        }
    }
    else
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
      qDebug() << "Command: " << editorPath << args.join(" ");
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
}

// ------------------------------------------------------------------------
// Is called to show the output messages of the last simulation.
void QucsApp::slotShowLastMsg()
{
  editFile(QucsSettings.QucsHomeDir.filePath("log.txt"));
}

// ------------------------------------------------------------------------
// Is called to show the netlist of the last simulation.
void QucsApp::slotShowLastNetlist()
{
  editFile(QucsSettings.QucsHomeDir.filePath("netlist.txt"));
}

// ------------------------------------------------------------------------
// Is called to start the text editor.
void QucsApp::slotCallEditor()
{
  editFile(QString(""));
}

// ------------------------------------------------------------------------
// Is called to start the filter synthesis program.
void QucsApp::slotCallFilter()
{
  launchTool("qucsfilter", "filter synthesis");
}

void QucsApp::slotCallActiveFilter()
{
  launchTool("qucsactivefilter", "active filter synthesis");
}

// ------------------------------------------------------------------------
// Is called to start the transmission line calculation program.
void QucsApp::slotCallLine()
{
  launchTool("qucstrans", "line calculation");
}

// ------------------------------------------------------------------------
// Is called to start the component library program.
void QucsApp::slotCallLibrary()
{
  launchTool("qucslib", "library");
}

// --------------------------------------------------------------
// Is called to show a dialog for creating matching circuits.
void QucsApp::slotCallMatch()
{
  MatchDialog *d = new MatchDialog(this);
  d->exec();
}

// ------------------------------------------------------------------------
// Is called to start the attenuator calculation program.
void QucsApp::slotCallAtt()
{
  launchTool("qucsattenuator", "attenuator calculation");
}

// ------------------------------------------------------------------------
// Is called to start the resistor color code calculation program.
void QucsApp::slotCallRes()
{
  launchTool("qucsrescodes", "resistor color code calculation");
}

//-------------------------------------------------------------------
// It starts the power combining synthesis tool
void QucsApp::slotCallPowerComb()
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
void QucsApp::launchTool(const QString& prog, const QString& progDesc, const QString& args) {
  QProcess *tool = new QProcess();

#ifdef __MINGW32__
  QString cmd = QDir::toNativeSeparators("\""+QucsSettings.BinDir + prog + ".exe\"");
#elif __APPLE__
  QString cmd = QDir::toNativeSeparators(QucsSettings.BinDir + prog + ".app/Contents/MacOS/" + prog);
#else
  QString cmd = QDir::toNativeSeparators(QucsSettings.BinDir + prog);
#endif

  char const* var = getenv("QUCS_USE_PATH");
  if(var != NULL) {
    // just use PATH. this is currently bound to a contition, to maintain
    // backwards compatibility with QUCSDIR
    qDebug() << "QUCS_USE_PATH";
    cmd = prog;
  }else{
    // don't know what this is about. doesn't make sense in the case above.
    // (and in all other cases i can think of)
    tool->setWorkingDirectory(QucsSettings.BinDir);
  }

  qDebug() << "Command :" << cmd + " " + args;

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

/**
 * @brief QucsApp::slotHelpOnline
 * Open default browser poining at the Qucs-Help website.
 */
void QucsApp::slotHelpOnline()
{
  QString link = "http://qucs-help.readthedocs.io/";
  QDesktopServices::openUrl(QUrl(link));
}

// --------------------------------------------------------------
void QucsApp::showHTML(const QString& Page)
{
  launchTool("qucshelp", "help", Page);
}



// ---------------------------------------------------------------------
// Is called when the find action is triggered.
void QucsApp::slotEditFind()
{
  SearchDia->initSearch(DocumentTab->currentWidget(),
      ((TextDoc *)DocumentTab->currentWidget())->textCursor().selectedText(), false);
}

// --------------------------------------------------------------
void QucsApp::slotChangeProps()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionChangeProps();
}

// --------------------------------------------------------------
void QucsApp::slotAddToProject()
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
    destFile.setFileName(QucsSettings.QucsWorkDir.absolutePath() +
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

#if 0
static Marker const* marker(Element const* e)
{
  return dynamic_cast<Marker const*>(e);
}
#endif

// -----------------------------------------------------------
void QucsApp::slotCursor(arrow_dir_t dir)
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionCursor(dir);
}

// -----------------------------------------------------------
// Is called if user clicked on component text of if return is
// pressed in the component text QLineEdit.
// In "view->MAx3" is the number of the current property.
void QucsApp::slotApplyCompText()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  qd->actionApplyCompText();
}


// -----------------------------------------------------------
// Is called if the text of the property edit changed, to match
// the width of the edit field.
void QucsApp::slotResizePropEdit(const QString& t)
{
  editText->resize(editText->fontMetrics().width(t)+4,
                   editText->fontMetrics().lineSpacing());
}

// -----------------------------------------------------------
void QucsApp::slotCreateLib()
{
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project with subcircuits!"));
    return;
  }

  LibraryDialog *d = new LibraryDialog(this);
  d->fillSchematicList(Content->exportSchematic());
  d->exec();
}

// -----------------------------------------------------------
void QucsApp::slotImportData()
{
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project first!"));
    return;
  }

  ImportDialog *d = new ImportDialog(this);
  d->exec();
}

// -----------------------------------------------------------
void QucsApp::slotExportSchematic()
{
  //incomplete();

  // dlg A;
  // for netlisters n
  // A.somehow_add(n)
  //
  // dlg.show
  //
  // if dlg.selected_netlister->has_options
  //
  // display(optionsdialog(dlg.selected_netlister.options)
  
  // unless abort
  // do_the_export(netlister,filename);
}
// -----------------------------------------------------------
// // BUG this is a diagram slot.
void QucsApp::slotExportGraphAsCsv()
{
  QWidget *w=DocumentTab->currentWidget();
  QucsDoc *qd=dynamic_cast<QucsDoc*>(w);
  assert(qd);

  hideEdit();
  qd->actionExportGraphAsCsv();
}


// ----------------------------------------------------------
void QucsApp::slotCreatePackage()
{
  slotHideEdit(); // disable text edit of component property

  PackageDialog *d = new PackageDialog(this, true);
  d->exec();
}

// ----------------------------------------------------------
void QucsApp::slotExtractPackage()
{
  slotHideEdit(); // disable text edit of component property
  PackageDialog *d = new PackageDialog(this, false);
  d->show();
  d->extractPackage();
  readProjects();
}

void QucsApp::slotOpenRecent()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    gotoPage(action->data().toString());
    updateRecentFilesList(action->data().toString());
  }
}

void QucsApp::slotUpdateRecentFiles()
{
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
}

void QucsApp::slotClearRecentFiles()
{
  QucsSettings.RecentDocs.clear();
  slotUpdateRecentFiles();
}

/*!
 * \brief QucsApp::slotLoadModule launches the dialog to select dynamic modueles
 */
void QucsApp::slotLoadModule()
{
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
        QucsApp::fillComboBox(true);
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

}


/*!
 * \brief QucsApp::slotBuildModule runs admsXml, C++ compiler to build library
 *
 * Run the va2cpp
 * Run the cpp2lib
 *
 * TODO
 * - split into two actions, elaborate and compile?
 * - collect, parse and display output of make
 *
 */
void QucsApp::slotBuildModule()
{
    qDebug() << "slotBuildModule";

    // reset message dock on entry
    messageDock->reset();

    messageDock->builderTabs->setTabIcon(0,QPixmap());
    messageDock->builderTabs->setTabIcon(1,QPixmap());


    QString make;

#ifdef __MINGW32__
    make = "mingw32-make.exe";    // must be on the path!
#else
    make = "make";                // must be on the path!
#endif

    QDir prefix = QDir(QucsSettings.BinDir+"../");

    QDir include = QDir(QucsSettings.BinDir+"../include/qucs-core");

    QString workDir = QucsSettings.QucsWorkDir.absolutePath();

    // need to cd into project to make sure output is droped there?
    // need to cd - into previous location?
    QDir::setCurrent(workDir);

    QProcess *builder = new QProcess();
    builder->setProcessChannelMode(QProcess::MergedChannels);
    // get current va document
    QucsDoc *Doc = getDoc();
    QString vaModule = Doc->fileBase(Doc->docName());

    QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#ifdef __MINGW32__
    admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
#else
    admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
#endif

    // admsXml emmits C++
    QStringList Arguments;
    Arguments << "-f" <<  QDir::toNativeSeparators(include.absoluteFilePath("va2cpp.makefile"))
              << QString("ADMSXML=%1").arg(admsXml)
              << QString("PREFIX=%1").arg(QDir::toNativeSeparators(prefix.absolutePath()))
              << QString("MODEL=%1").arg(vaModule);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    builder->setProcessEnvironment(env);

    // prepend command to log
    QString cmdString = QString("%1 %2\n").arg(make, Arguments.join(" "));
    messageDock->admsOutput->appendPlainText(cmdString);

    qDebug() << "Command :" << make << Arguments.join(" ");
    builder->start(make, Arguments);

    // admsXml seems to communicate all via stdout, or is it because of make?
    QString vaStatus;
    if (!builder->waitForFinished()) {
        vaStatus = builder->errorString();
        qDebug() << "Make failed:" << vaStatus;
    }
    else {
        vaStatus = builder->readAll();
        qDebug() << "Make stdout"  << vaStatus;
    }

    //build libs
    qDebug() << "\nbuild libs\n";

    Arguments.clear();

    Arguments << "-f" <<  QDir::toNativeSeparators(include.absoluteFilePath("cpp2lib.makefile"))
              << QString("PREFIX=\"%1\"").arg(QDir::toNativeSeparators(prefix.absolutePath()))
              << QString("PROJDIR=\"%1\"").arg(QDir::toNativeSeparators(workDir))
              << QString("MODEL=%1").arg(vaModule);

    // prepend command to log
    cmdString = QString("%1 %2\n").arg(make, Arguments.join(" "));
    messageDock->cppOutput->appendPlainText(cmdString);

    builder->start(make, Arguments);

    QString cppStatus;

    if (!builder->waitForFinished()) {
        cppStatus = builder->errorString();
        qDebug() << "Make failed:" << cppStatus;
    }
    else {
        cppStatus = builder->readAll();
        qDebug() << "Make output:" << cppStatus;
    }
    delete builder;

    // push make output to message dock
    messageDock->admsOutput->appendPlainText(vaStatus);
    messageDock->cppOutput->appendPlainText(cppStatus);

    // shot the message docks
    messageDock->msgDock->show();

}

// ----------------------------------------------------------
void QucsApp::slotHelpAbout()
{
  AboutDialog *ad = new AboutDialog(this);
  ad->exec();
}

// vim:ts=8:sw=2:noet
