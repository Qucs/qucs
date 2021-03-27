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
