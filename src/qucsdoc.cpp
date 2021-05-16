/***************************************************************************
    copyright            : (C) 2003, 2004 by Michael Margraf
                               2019, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFileInfo>
#include <QFileDialog>
#include <QAction>
#include <QUndoStack>
#include <QMessageBox>

#include "qucs_globals.h"
#include "data.h"
#include "action.h"
#include "doc_actions.h"
#include "qio.h" // tmp?
#include "qt_compat.h"
#include "qucs_app.h"
#include "qucsdoc.h"
#include "element_list.h"
#include "sckt_base.h"
#include "simmessage.h"
#include "simulator.h"
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
namespace qucs {
/* -------------------------------------------------------------------------------- */
void Doc::setDocFile(std::string const& filename)
{ untested();
	_name = QString_(filename);
  QFileInfo Info(_name);
  if(!_name.isEmpty()) { untested();
    _name = Info.absoluteFilePath();

	 // properties.set("filename", _name); ??
    QString base = Info.completeBaseName();
    QString ext = Info.suffix();

    if(ext == "m" || ext == "oct")
      SimTime = "1";

    DataSet = base + ".dat";       // name of the default dataset
    Script = base + ".m";          // name of the default script
    if(ext != "dpl")
      DataDisplay = base + ".dpl"; // name of default data display
    else { untested();
      DataDisplay = base + ".sch"; // name of default schematic
      GridOn = false;              // data display without grid (per default)
    }
  }else{ untested();
  }
}
/* -------------------------------------------------------------------------------- */
Doc::Doc() : Widget() // (App* App_, const QString& Name_, QWidget* o)
{itested();
  GridOn = true;
  _name = "unnamed"; // name? filename?

  // setDocName(...)?

  SimOpenDpl = true;
  SimRunScript = false;

  DocChanged = false;
  showBias = -1;  // don't show DC bias (currently for "Schematic" only)
  // Scale = 1.0;
}
/* -------------------------------------------------------------------------------- */
Doc::~Doc()
{itested();
}
/* -------------------------------------------------------------------------------- */
#if 1
/* -------------------------------------------------------------------------------- */
void Doc::redo()
{
	incomplete();
}
/* -------------------------------------------------------------------------------- */
void Doc::undo()
{
	incomplete();
}
/* -------------------------------------------------------------------------------- */
#else
// really?!
// no. some qucsdocs come without undo stack. so undo/redo must be custom.
void Doc::undo()
{ untested();
	QUndoStack* u = undoStack();
	if(u){itested();
		u->undo();
		assert(_app);
		assert(_app->redo);
		_app->redo->setEnabled(true); // yikes.
	}else{ untested();
	}
}

void Doc::redo()
{ untested();
	QUndoStack* u = undoStack();
	if(u){ untested();
		u->redo();
	}else{ untested();
	}
}
#endif
/* -------------------------------------------------------------------------------- */
QString Doc::fileSuffix (const QString& Name)
{ untested();
  QFileInfo Info (Name);
  return Info.suffix();
}
/* -------------------------------------------------------------------------------- */
QString Doc::fileSuffix (void)
{ untested();
  return fileSuffix (_name);
}
/* -------------------------------------------------------------------------------- */
QString Doc::fileBase (const QString& Name)
{ untested();
  QFileInfo Info (Name);
  return Info.completeBaseName();
}
/* -------------------------------------------------------------------------------- */
QString Doc::fileBase (void)
{ untested();
  return fileBase (_name);
}
/* -------------------------------------------------------------------------------- */
#if 0
// cleanup debris
QAction* Doc::selectAction()
{ untested();
	unreachable();
	assert(_app);
  	return _app->select;
}
#endif

// set mode??
// void Doc::setActiveAction(MouseAction* a)
// { untested();
// 	if(eventHandler()){ untested();
// 		eventHandler()->setActive(a);
// 	}else{ untested();
// 	}
// }

#if 0
MouseActions const* Doc::mouseActions() const
{ untested();
	auto ma = const_cast<MouseActions*>(mouseActions());
	return ma;
}

MouseActions* Doc::mouseActions()
{ untested();
	return nullptr;
}
#endif
/* -------------------------------------------------------------------------------- */
App* Doc::app()
{itested();
	QObject* w = dynamic_cast<QWidget*>(this);
	while(w){itested();
		if(auto a = dynamic_cast<App*>(w)){itested();
			return a;
		}else{itested();
			w = w->parent();
		}
	}
	unreachable();
	return nullptr;
}
/* -------------------------------------------------------------------------------- */
void Doc::printCursorPosition(int x, int y)
{
	if(app()){itested();
	// BUG. signal status bar?
		app()->printCursorPosition(x, y);
	}else{ untested();
	}
}
/* -------------------------------------------------------------------------------- */
void Doc::cleanup()
{
	if(!app()){
		unreachable();
	}else if(_toolbar){itested();
		// assert(_toolbar->parent()==app()); // what??
		// app()->removeToolBar(_toolbar);
		_toolbar->setVisible(false);
//		assert(_toolbar->parent()==nullptr);
	}else{
	}
}
/* -------------------------------------------------------------------------------- */
void Doc::becomeCurrent()
{
	QWidget* p = dynamic_cast<QWidget*>(this);
	assert(p);

	trace1("Doc::becomeCurrent", _toolbar);
	if(_eventHandler){
		_eventHandler->setControls(this);
	}else{
	}

	if(!app()){
		incomplete();
		assert(false);
	}else if(_toolbar){itested();
		app()->addToolBar(_toolbar); // BUG: add upfront.
		assert(_toolbar->parent()==app());

		_toolbar->setVisible(true);

	}else{ untested();
	}

	// not here.
	if(!app()){
		// _undo_redo_bar = newToolBar();
	}else if(_eventHandler){
	  QObject::connect(app()->undo, SIGNAL(triggered()), _eventHandler, SLOT(slotUndo()));
	  QObject::connect(app()->redo, SIGNAL(triggered()), _eventHandler, SLOT(slotRedo()));
	}
}
/* -------------------------------------------------------------------------------- */
void Doc::addElement(Element*)
{ untested();
	incomplete();
}
/* -------------------------------------------------------------------------------- */
ElementList* Doc::model()
{ untested();
	incomplete();
	return nullptr;
}
/* -------------------------------------------------------------------------------- */
void Doc::executeCommand(QUndoCommand* c)
{itested();
	// TODO: what if there are multiple views to a scene?
	// is mouseActions == scene?

	if(eventHandler()){itested();
		eventHandler()->executeCommand(c); // really?
		// setChanged();
		if(!DocChanged){itested();
			emit signalFileChanged(true);
		}else{ untested();
			// TODO: unset Changed in undo.
		}
		DocChanged = c;

		showBias = -1;   // schematic changed => bias points may be invalid
	}else{ untested();
	}
}
/* -------------------------------------------------------------------------------- */
void Doc::addToolBar(QToolBar* t)
{itested();
	assert(!_toolbar);
	// assert(!t->parent());
	_toolbar = t;
}
/* -------------------------------------------------------------------------------- */
CommonData* Doc::qucsData(std::string const& /*key*/)
{ untested();
	return nullptr; // _data[key];
}
/* -------------------------------------------------------------------------------- */
void Doc::slotDCbias()
{ untested();
	incomplete();
}
/* -------------------------------------------------------------------------------- */
void Doc::slotSimulate()
{ untested();
  incomplete();
} // Doc::slotSimulate
/* -------------------------------------------------------------------------------- */
// move around more.
#include <QFileDialog>
#include <QMessageBox>
bool Doc::saveAs()
{ untested();
	int n = -1;
	QString s;
	QString Filter = QWidget::tr("Schematic") + " (*.sch);;" + QWidget::tr("Any File")+" (*)";
	QFileInfo Info;
	while(true) { untested();
		s = docName();
		Info.setFile(s);
		// which is default directory ?
		if(!s.isEmpty()) { untested();
#if 0
		}else if(!ProjName.isEmpty()) { untested();
			s = QucsSettings.QucsWorkDir.path();
		}else if(!lastDirOpenSave.isEmpty()){ untested();
			s = lastDirOpenSave;
#endif
		} else{ untested();
			s = QDir::currentPath();
		}

		QString known_ext = "vhdl;vhd;v;va;sch;dpl;m;oct;net;qnet;txt";
		QStringList extlist = known_ext.split (';');

		auto w = dynamic_cast<QWidget*>(this);
		assert(w);

		s = QFileDialog::getSaveFileName(w, QWidget::tr("Enter a Document Name"),
				QDir(QString::fromStdString(QucsSettings.QucsWorkDir)).absolutePath(),
				Filter);

		if(s.isEmpty()) { untested();
			return false;
		}else{ untested();
		}

		Info.setFile(s);               // try to guess the best extension ...
		QString ext = Info.suffix();

		if(ext.isEmpty() || !extlist.contains(ext)) { untested();
			incomplete();
			// if no extension was specified or is unknown
		}else{ untested();
		}

		Info.setFile(s);
		if(QFile::exists(s)) { untested();
			n = QMessageBox::warning(w, QWidget::tr("Warning"),
					QWidget::tr("The file '")+Info.fileName()+QWidget::tr("' already exists!\n")+
					QWidget::tr("Saving will overwrite the old one! Continue?"),
					QWidget::tr("No"), QWidget::tr("Yes"), QWidget::tr("Cancel"));
			if(n == 2){ untested();
				return false;    // cancel
			}else if(n == 0) { untested();
				// try again?
				continue;
			}else{ untested();
			}
		}else{ untested();
		}

#if 0 // incomplete
		// search, if document is open
		Doc * d = findDoc (s);
		if(d) { untested();
			QMessageBox::information(this, QWidget::tr("Info"),
					QWidget::tr("Cannot overwrite an open document"));
			return false;
		}
#endif

		break;
	}
	setName(s);

	auto W = dynamic_cast<QWidget*>(this);

	if(auto o=prechecked_cast<QTabWidget*>(_owner)){ untested();
		o->setTabText(o->indexOf(W), s); // misc::properFileName(s));
		// lastDirOpenSave = Info.absolutePath();  // remember last directory and file
	}else{ untested();
	}


	n = save();   // SAVE
	if(n < 0){ untested();
		return false;
	}else{ untested();
	}

//	updatePortNumber(Doc, n);
//
//	// signal?
//	updateRecentFilesList(s);
	return true;
}
/* -------------------------------------------------------------------------------- */
#include "qucs_globals.h"
#include "simulator.h"
/* -------------------------------------------------------------------------------- */
void Simulator::attachDoc(Doc /* const?? */ * d)
{ untested();
	incomplete();
	_doc = d;
}
/* -------------------------------------------------------------------------------- */
#if 1
// FIXME.
Simulator* Doc::simulatorInstance(std::string const& which)
{ untested();
	if (!scope()){ untested();
		return nullptr;
	}else{ untested();
	}
	auto i = scope()->find_(which);
	Simulator* sim=nullptr;

	if(i==scope()->end()){ untested();

		Simulator const* proto = QucsSettings.simulator();
		if(which!=""){ untested();
			Data* d = data_dispatcher[which];
			proto = dynamic_cast<Simulator*>(d);
		}else{ untested();
		}

		if(proto){ untested();
			sim = proto->clone();
			sim->set_owner(root());
			scope()->push_back(sim);
		}else{ untested();
			incomplete();
		}
	}else if(auto s=dynamic_cast<Simulator*>(*i)){ untested();
		sim = s;
	}else{ untested();
	}

	return sim;
}
#endif
/* -------------------------------------------------------------------------------- */
void setParameter(std::string const&, std::string const&)
{ untested();
	incomplete();
}
/* -------------------------------------------------------------------------------- */
ElementList* Doc::scope()
{ untested();
	if(root()){ untested();
		return root()->scope();
	}else{ untested();
		return nullptr;
	}
}
/* -------------------------------------------------------------------------------- */
ElementList const* Doc::scope() const
{ untested();
	auto d = const_cast<Doc*>(this);
	return d->scope();
}
/* -------------------------------------------------------------------------------- */
SubcktBase const* Doc::root() const
{ untested();
	auto d = const_cast<Doc*>(this);
	return d->root();
}
/* -------------------------------------------------------------------------------- */
} // qucs
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
