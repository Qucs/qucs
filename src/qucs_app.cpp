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

#include <QToolBar>

// obsolete.
void QucsApp::clearWorkToolbar()
{ untested();
	assert(_docToolBar);
	_docToolBar->clear();
}

// obsolete.
// void QucsApp::addWorkToolbarAction(QAction* a)
// { untested();
// 	assert(_docToolBar);
// 	_docToolBar->addAction(a);
// }

// why "AtStartup?!"
void QucsApp::openFileAtStartup(QString const& arg)
{
      QFileInfo Info(arg);
      QucsSettings.QucsWorkDir = Info.absoluteDir().absolutePath().toStdString(); // ?
      QString p = QString_(QucsSettings.QucsWorkDir) + QDir::separator() + Info.fileName();
      gotoPage(p);
}

// Change to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
// this is used "atStartup"...
bool QucsApp::gotoPage(const QString& Name)
{itested();
	// is Name the filename?!
	int No = DocumentTab->currentIndex();

	int i = 0;
	QucsDoc * d = findDoc (Name, &i);  // search, if page is already loaded

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
	for(auto i : languageDispatcher){
		trace3("a schematic?", i.first, i.second->label(), suffix);
		if(i.first == suffix){
			is_schematic = true;
		}else{
		}
	}

	// TODO. open schematic associated with document?
	if(is_schematic) { untested();
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
		}else if(!getDoc(0)->DocChanged){ untested();
			delete DocumentTab->widget(0);
		}else{ untested();
		}

		//view->drawn = false;
		return true;
	}
#endif
}
