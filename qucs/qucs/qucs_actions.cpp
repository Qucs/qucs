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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*! \file qucs_actions.cpp
 *  \brief Actions implementation for the GUI menu items
 */

#include <QtCore>
#include <stdlib.h>
#include <limits.h>

#include <QProcess>
#include <QRegExpValidator>
#include <QLineEdit>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QComboBox>
#include <QDockWidget>
#include <QTreeWidgetItem>

#include "projectView.h"
#include "main.h"
#include "qucs.h"
#include "textdoc.h"

// for editing component name on schematic
QRegExp  Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);

// -----------------------------------------------------------------------
// Is called, when "set on grid" action is triggered.
void QucsApp::slotOnGrid(bool on)
{
}

// -----------------------------------------------------------------------
// Is called when the rotate toolbar button is pressed.
void QucsApp::slotEditRotate(bool on)
{
}

// -----------------------------------------------------------------------
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorX(bool on)
{
}

// -----------------------------------------------------------------------
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorY(bool on)
{
}

// -----------------------------------------------------------------------
// Is called when the activate/deactivate toolbar button is pressed.
// It also comments out the selected text on a text document
// \todo update the status or tooltip message
void QucsApp::slotEditActivate (bool on)
{
}

// ------------------------------------------------------------------------
// Is called if "Delete"-Button is pressed.
void QucsApp::slotEditDelete(bool on)
{
}

// -----------------------------------------------------------------------
// Is called if "Wire"-Button is pressed.
void QucsApp::slotSetWire(bool on)
{
}

// -----------------------------------------------------------------------
void QucsApp::slotInsertLabel(bool on)
{
}

// -----------------------------------------------------------------------
void QucsApp::slotSetMarker(bool on)
{
}

// -----------------------------------------------------------------------
// Is called, when "move component text" action is triggered.
void QucsApp::slotMoveText(bool on)
{
}

// -----------------------------------------------------------------------
// Is called, when "Zoom in" action is triggered.
void QucsApp::slotZoomIn(bool on)
{
}


void QucsApp::slotEscape()
{
    select->setChecked(true);
    slotSearchClear();
}

// -----------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsApp::slotSelect(bool on)
{
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));
  slotHideEdit(); // disable text edit of component property

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
void QucsApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  statusBar()->message(tr("Ready."));
}

// -----------------------------------------------------------------------
void QucsApp::slotEditPaste(bool on)
{
  // get the current document
  QWidget *Doc = DocumentTab->currentPage();
}

// -----------------------------------------------------------------------
void QucsApp::slotInsertEntity ()
{
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the equation toolbar button.
void QucsApp::slotInsertEquation(bool on)
{
  slotHideEdit(); // disable text edit of component property
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the ground toolbar button.
void QucsApp::slotInsertGround(bool on)
{
  slotHideEdit(); // disable text edit of component property
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the port toolbar button.
void QucsApp::slotInsertPort(bool on)
{
  slotHideEdit(); // disable text edit of component property
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditUndo()
{
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditRedo()
{
}

// --------------------------------------------------------------
// Is called, when "Align top" action is triggered.
void QucsApp::slotAlignTop()
{
}

// --------------------------------------------------------------
// Is called, when "Align bottom" action is triggered.
void QucsApp::slotAlignBottom()
{
}

// --------------------------------------------------------------
// Is called, when "Align left" action is triggered.
void QucsApp::slotAlignLeft()
{
}

// --------------------------------------------------------------
// Is called, when "Align right" action is triggered.
void QucsApp::slotAlignRight()
{
}

// --------------------------------------------------------------
// Is called, when "Distribute horizontally" action is triggered.
void QucsApp::slotDistribHoriz()
{
}

// --------------------------------------------------------------
// Is called, when "Distribute vertically" action is triggered.
void QucsApp::slotDistribVert()
{
}

// --------------------------------------------------------------
// Is called, when "Center horizontally" action is triggered.
void QucsApp::slotCenterHorizontal()
{
}

// --------------------------------------------------------------
// Is called, when "Center vertically" action is triggered.
void QucsApp::slotCenterVertical()
{
}

// ---------------------------------------------------------------------
// Is called when the "select all" action is triggered.
void QucsApp::slotSelectAll()
{
}

// ---------------------------------------------------------------------
// Is called when the "select markers" action is triggered.
void QucsApp::slotSelectMarker()
{
}


extern QString lastDirOpenSave; // to remember last directory and file

// ------------------------------------------------------------------------
// Is called by slotShowLastMsg(), by slotShowLastNetlist() and from the
// component edit dialog.
void QucsApp::editFile(const QString& File)
{
}

// ------------------------------------------------------------------------
// Is called to show the output messages of the last simulation.
void QucsApp::slotShowLastMsg()
{
}

// ------------------------------------------------------------------------
// Is called to show the netlist of the last simulation.
void QucsApp::slotShowLastNetlist()
{
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

/*!
 * \brief launch an external application passing arguments
 *
 * \param prog  executable program name (will be transformed depending on OS)
 * \param progDesc  program description string (used for error messages)
 * \param args  arguments to pass to the executable
 */
void QucsApp::launchTool(const QString& prog, const QString& progDesc, const QString& args) {
}


// --------------------------------------------------------------
void QucsApp::slotHelpIndex()
{
  showHTML("index.html");
}

// --------------------------------------------------------------
void QucsApp::slotGettingStarted()
{
  showHTML("start.html");
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
}

// --------------------------------------------------------------
void QucsApp::slotChangeProps()
{
}

// --------------------------------------------------------------
void QucsApp::slotAddToProject()
{
  slotHideEdit(); // disable text edit of component property
  statusBar()->message(tr("Ready."));
}

// -----------------------------------------------------------
void QucsApp::slotCursorLeft()
{
}

// -----------------------------------------------------------
void QucsApp::slotCursorRight()
{
}

// -----------------------------------------------------------
void QucsApp::slotCursorUp()
{
}

// -----------------------------------------------------------
void QucsApp::slotCursorDown()
{
}

// -----------------------------------------------------------
// Is called if user clicked on component text of if return is
// pressed in the component text QLineEdit.
// In "view->MAx3" is the number of the current property.
void QucsApp::slotApplyCompText()
{
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
}

// -----------------------------------------------------------
void QucsApp::slotImportData()
{
  slotHideEdit(); // disable text edit of component property
}

// -----------------------------------------------------------
void QucsApp::slotExportGraphAsCsv()
{
  slotHideEdit(); // disable text edit of component property
}

// ----------------------------------------------------------
void QucsApp::slotCreatePackage()
{
  slotHideEdit(); // disable text edit of component property
}

// ----------------------------------------------------------
void QucsApp::slotExtractPackage()
{
  slotHideEdit(); // disable text edit of component property
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
}

void QucsApp::slotClearRecentFiles()
{
  slotUpdateRecentFiles();
}

/*!
 * \brief QucsApp::slotLoadModule launches the dialog to select dynamic modueles
 */
void QucsApp::slotLoadModule()
{
  qDebug() << "slotLoadModule";
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
}
