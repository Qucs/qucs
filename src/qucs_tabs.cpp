/***************************************************************************
    copyright            : (C) 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qucs_tabs.h"
#include "qucsdoc.h"
#include "qucs_app.h" // gaah
#include "io_trace.h"
#include "qt_compat.h"
#include "qucs_globals.h"

#include <QTabBar>
#include <QMenu>
#include <QApplication> // BUG
#include <QClipboard>
#include <QDesktopServices>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void QucsTabWidget::setCurrentIndex_(int i)
{
	// BUG: currentIndex is wrong (why??). use _current instead.
	trace4("QucsTabWidget::setCurrentIndex", currentIndex(), i, _current, current());
	QTabWidget::setCurrentIndex(i);

	if(_current){
		// BUG: current may have gone and this crashes.
		_current->cleanup();
		_current = nullptr;
	}else{
	}
	
	if(_current == current()){
		// why?
		incomplete();
		unreachable(); // BUG
	}else{
		_current = current();

		if(_current){
			_current->becomeCurrent();
		}else{
		}
	}

}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotDCbias()
{ untested();
  current()->showBias = 0; //???
  incomplete();
  // slotSimulate();
  current()->slotDCbias();
}
/*--------------------------------------------------------------------------*/
QucsTabWidget::QucsTabWidget(QucsApp *parent) : QTabWidget(parent)
{itested();
	App = parent;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QucsTabWidget::customContextMenuRequested,
	        this, &QucsTabWidget::showContextMenu);
	connect(this, &QucsTabWidget::currentChanged,
	        this, &QucsTabWidget::setCurrentIndex_);
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::showContextMenu(const QPoint& point)
{ untested();
  if (point.isNull()) { untested();
	  incomplete();
    return;
  }else{
  }

  contextTabIndex = tabBar()->tabAt(point);
  if (contextTabIndex >= 0) { // clicked over a tab
    QMenu menu(this);

    // get the document where the context menu was opened
	 // yikes. why involve App?!
    QucsDoc *d = App->getDoc(contextTabIndex);
    // save the document name (full path)
    // docName = d->docName();

#define APPEND_MENU(action, slot, text)         \
  QAction *action = new QAction(tr(text), &menu);    \
  connect(action, SIGNAL(triggered()), SLOT(slot())); \
  menu.addAction(action);

  APPEND_MENU(ActionCxMenuClose, slotCxMenuClose, "Close")
  APPEND_MENU(ActionCxMenuCloseOthers, slotCxMenuCloseOthers, "Close all but this")
  APPEND_MENU(ActionCxMenuCloseLeft, slotCxMenuCloseLeft, "Close all to the left")
  APPEND_MENU(ActionCxMenuCloseRight, slotCxMenuCloseRight, "Close all to the right")
  APPEND_MENU(ActionCxMenuCloseAll, slotCxMenuCloseAll, "Close all")
  menu.addSeparator();
  APPEND_MENU(ActionCxMenuCopyPath, slotCxMenuCopyPath, "Copy full path")
  APPEND_MENU(ActionCxMenuOpenFolder, slotCxMenuOpenFolder, "Open containing folder")
#undef APPEND_MENU

    // a not-yet-saved document does not have a name/full path
    // so copying full path or opening containing folder does not make sense
    ActionCxMenuCopyPath->setEnabled(!docName().isEmpty());
    ActionCxMenuOpenFolder->setEnabled(!docName().isEmpty());

    menu.exec(tabBar()->mapToGlobal(point));
  }
}
/*--------------------------------------------------------------------------*/
QString QucsTabWidget::docName()
{
	if(current()){
		return current()->docName();
	}else{
		unreachable();
		return "notaqucsdoc";
	}
}
/*--------------------------------------------------------------------------*/
QucsDoc* QucsTabWidget::current()
{
	QWidget* w = currentWidget();
	auto d = dynamic_cast<QucsDoc*>(w);
	assert(d || !w);
	return d;
}
/*--------------------------------------------------------------------------*/
QucsDoc* QucsTabWidget::createEmptySchematic(const QString &name)
{itested();
  // create a schematic
  QFileInfo Info(name);
  assert(App);

  Widget* o = widget_dispatcher.clone("SchematicDoc");
//  e->setParam("name", name.toStdString());
  assert(o);
  QucsDoc* d = dynamic_cast<QucsDoc*>(o);
  assert(d);
  QWidget* w = dynamic_cast<QWidget*>(o);
  assert(w);
  d->setParent(this);
  trace1("setname??", name);
  d->setName(name); // it's actually the (full?) filename?

  int i = addTab(w, QPixmap(":/bitmaps/empty.xpm"), name.isEmpty() ? QObject::tr("untitled") : Info.fileName());
  setCurrentIndex(i);
  return d;
}
/*--------------------------------------------------------------------------*/
// duplicate in qucs_actions QucsApp::?!
QucsDoc* QucsTabWidget::createEmptyTextDoc(const QString &name)
{itested();
  // create a text document
  QFileInfo Info(name);
  // QucsDoc *d = newTextDoc(*App, name, this);
  Widget* o = widget_dispatcher.clone("TextDoc");
//  e->setParam("name", name.toStdString());
  assert(o);
  QucsDoc* d = dynamic_cast<QucsDoc*>(o);
  assert(d);
  QWidget* w = dynamic_cast<QWidget*>(o);
  assert(w);
  d->setParent(this);
  trace1("setname??", name);
  d->setName(name); // it's actually the (full?) filename?

  int i = addTab(w, QPixmap(":/bitmaps/empty.xpm"), name.isEmpty() ? QObject::tr("untitled") : Info.fileName());
  setCurrentIndex(i);
  return d;
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::setSaveIcon(bool state, int index)
{ untested();
  // set document tab icon to "smallsave.xpm" or "empty.xpm"
  QString icon = (state)? ":/bitmaps/smallsave.xpm" : ":/bitmaps/empty.xpm";
  if (index < 0) { untested();
    index = currentIndex();
  }
  setTabIcon(index, QPixmap(icon));
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuClose()
{ untested();
  // close tab where the context menu was opened
  App->slotFileClose(contextTabIndex);
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuCloseOthers()
{ untested();
  // close all tabs, except the one where the context menu was opened
  App->closeAllFiles(contextTabIndex);
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuCloseLeft()
{ untested();
  // close all tabs to the left of the current one
  App->closeAllLeft(contextTabIndex);
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuCloseRight()
{ untested();
  // close all tabs to the right of the current one
  App->closeAllRight(contextTabIndex);
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuCloseAll()
{ untested();
  App->slotFileCloseAll();
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuCopyPath()
{ untested();
  // copy the document full path to the clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(docName());
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotCxMenuOpenFolder()
{ untested();
  QFileInfo Info(docName());
  QDesktopServices::openUrl(QUrl::fromLocalFile(Info.canonicalPath()));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
