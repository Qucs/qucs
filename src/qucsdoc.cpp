/***************************************************************************
    copyright            : (C) 2003, 2004 by Michael Margraf
                               2019 Felix Salfelder / QUCS team
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
#include <QAction>

#include "qucsdoc.h"
#include "mouseactions.h"
#include "simmessage.h"
#include "qucs_app.h"
#include <QUndoStack>
#include "qio.h" // tmp?
#include "simulator.h"
#include "sckt_base.h"


QucsDoc::QucsDoc(QucsApp* App_, const QString& Name_, QWidget* o)
   : _app(App_),
	  _owner(o)
{
  GridOn = true;
  DocName = Name_;
  QFileInfo Info(DocName);
  if(!DocName.isEmpty()) {
    DocName = Info.absoluteFilePath();
    QString base = Info.completeBaseName();
    QString ext = Info.suffix();

    if(ext == "m" || ext == "oct")
      SimTime = "1";

    DataSet = base + ".dat";       // name of the default dataset
    Script = base + ".m";          // name of the default script
    if(ext != "dpl")
      DataDisplay = base + ".dpl"; // name of default data display
    else {
      DataDisplay = base + ".sch"; // name of default schematic
      GridOn = false;              // data display without grid (per default)
    }
  }
  SimOpenDpl = true;
  SimRunScript = false;

  DocChanged = false;
  showBias = -1;  // don't show DC bias (currently for "Schematic" only)
  // Scale = 1.0;
}

QucsDoc::~QucsDoc()
{
}

// really?!
void QucsDoc::undo()
{
	QUndoStack* u = undoStack();
	if(u){itested();
		u->undo();
		assert(_app);
		assert(_app->redo);
		_app->redo->setEnabled(true); // yikes.
	}else{ untested();
	}
}

void QucsDoc::redo()
{
	QUndoStack* u = undoStack();
	if(u){
		u->redo();
	}else{
	}
}

QString QucsDoc::fileSuffix (const QString& Name)
{
  QFileInfo Info (Name);
  return Info.suffix();
}

QString QucsDoc::fileSuffix (void)
{
  return fileSuffix (DocName);
}

QString QucsDoc::fileBase (const QString& Name)
{
  QFileInfo Info (Name);
  return Info.completeBaseName();
}

QString QucsDoc::fileBase (void)
{
  return fileBase (DocName);
}

// cleanup debris
QAction* QucsDoc::selectAction()
{
	unreachable();
	assert(_app);
  	return _app->select;
}

void QucsDoc::setActiveAction(MouseAction* a)
{
	if(mouseActions()){itested();
		mouseActions()->setActive(a);
	}else{ untested();
	}
}

MouseActions const* QucsDoc::mouseActions() const
{ untested();
	auto ma = const_cast<MouseActions*>(mouseActions());
	return ma;
}

MouseActions* QucsDoc::mouseActions()
{ untested();
	return nullptr;
}

// similar to former Schematic::performtoggleaction. but take care of actions,
// and deal with undoable commands.
// https://www.walletfox.com/course/qundocommandexample.php?
// https://stackoverflow.com/questions/32973326/qgraphicsscene-how-to-map-item-movements-into-qundocommand
void QucsDoc::possiblyToggleAction(MouseAction* a, QAction* sender)
{
	QUndoCommand* cmd = nullptr;
	assert(a);
	if(!sender){ untested();
		setActiveAction(nullptr);
		// cmd = a->activate(sender);
		setActiveAction(a);
	}else if(!sender->isCheckable()){ untested();
		cmd = a->activate(sender);
	}else if(sender->isChecked()){itested();
		cmd = a->activate(sender);

		if(cmd){itested();
			sender->blockSignals(true);
			sender->setChecked(false);
			sender->blockSignals(false);
			// possible 'delete' after select.
			// don't do anything else
			a->deactivate();
		}else{itested();
			// sender->setChecked(true); // assert?
			setActiveAction(a);
		}
	}else{itested();
		setActiveAction(nullptr);
	}

	if(cmd){itested();
		executeCommand(cmd);
	}else{itested();
	}
}
/* -------------------------------------------------------------------------------- */
// maybe this only works for SchematicDoc.
// SchematicDoc has input modes coupled to "MouseActions" that deal with user input.
// TextDoc also has modes, but somehow redirects input to another widget.
MouseAction* QucsDoc::activeAction()
{
	if(mouseActions()){ untested();
		return mouseActions()->activeAction();
	}else{ untested();
		return nullptr;
	}
}
/* -------------------------------------------------------------------------------- */
MouseAction const* QucsDoc::activeAction() const
{
	auto d = const_cast<QucsDoc*>(this);
	return d->activeAction();
}
/* -------------------------------------------------------------------------------- */
void QucsDoc::executeCommand(QUndoCommand* c)
{
	if(mouseActions()){itested();
		mouseActions()->executeCommand(c);
		// setChanged();
		if(!DocChanged){
			emit signalFileChanged(true);
		}else{
			// TODO: unset Changed in undo.
		}
		DocChanged = c;

		showBias = -1;   // schematic changed => bias points may be invalid
	}else{
	}
}
/* -------------------------------------------------------------------------------- */
CommonData* QucsDoc::qucsData(std::string const& /*key*/)
{
	return nullptr; // _data[key];
}
/* -------------------------------------------------------------------------------- */
void QucsDoc::slotDCbias()
{
	incomplete();
}
/* -------------------------------------------------------------------------------- */
void QucsDoc::slotSimulate()
{
  incomplete();
} // QucsDoc::slotSimulate
/* -------------------------------------------------------------------------------- */
// move around more.
#include <QFileDialog>
#include <QMessageBox>
bool QucsDoc::saveAs()
{
	int n = -1;
	QString s;
	QString Filter = QWidget::tr("Schematic") + " (*.sch);;" + QWidget::tr("Any File")+" (*)";
	QFileInfo Info;
	while(true) { untested();
		s = docName();
		Info.setFile(s);
		// which is default directory ?
		if(!s.isEmpty()) {
#if 0
		}else if(!ProjName.isEmpty()) { untested();
			s = QucsSettings.QucsWorkDir.path();
		}else if(!lastDirOpenSave.isEmpty()){
			s = lastDirOpenSave;
#endif
		} else{
			s = QDir::currentPath();
		}

		// list of known file extensions
		QString ext = "vhdl;vhd;v;va;sch;dpl;m;oct;net;qnet;txt";
		QStringList extlist = ext.split (';');

		auto w = dynamic_cast<QWidget*>(this);
		assert(w);

		s = QFileDialog::getSaveFileName(w, QWidget::tr("Enter a Document Name"),
				QDir(QString::fromStdString(QucsSettings.QucsWorkDir)).absolutePath(),
				Filter);

		if(s.isEmpty()) {
			return false;
		}else{
		}

		Info.setFile(s);               // try to guess the best extension ...
		ext = Info.suffix();

		if(ext.isEmpty() || !extlist.contains(ext)) { untested();
			incomplete();
			// if no extension was specified or is unknown
		}else{
		}

		Info.setFile(s);
		if(QFile::exists(s)) { untested();
			n = QMessageBox::warning(w, QWidget::tr("Warning"),
					QWidget::tr("The file '")+Info.fileName()+QWidget::tr("' already exists!\n")+
					QWidget::tr("Saving will overwrite the old one! Continue?"),
					QWidget::tr("No"), QWidget::tr("Yes"), QWidget::tr("Cancel"));
			if(n == 2){
				return false;    // cancel
			}else if(n == 0) {
				// try again?
				continue;
			}else{
			}
		}else{
		}

#if 0 // incomplete
		// search, if document is open
		QucsDoc * d = findDoc (s);
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

	if(auto o=prechecked_cast<QTabWidget*>(_owner)){
		o->setTabText(o->indexOf(W), s); // misc::properFileName(s));
		// lastDirOpenSave = Info.absolutePath();  // remember last directory and file
	}else{
	}


	n = save();   // SAVE
	if(n < 0){
		return false;
	}else{
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
void Simulator::attachDoc(QucsDoc /* const?? */ * d)
{
	_doc = d;
}
/* -------------------------------------------------------------------------------- */
#if 1
// FIXME.
Simulator* QucsDoc::simulatorInstance(std::string const& which)
{
	if (!scope()){
		return nullptr;
	}else{
	}
	auto i = scope()->find_(which);
	Simulator* sim=nullptr;

	if(i==scope()->end()){ untested();

		Simulator const* proto = QucsSettings.simulator();
		if(which!=""){ untested();
			auto d = dataDispatcher[which];
			proto = dynamic_cast<Simulator*>(d);
		}else{
		}

		if(proto){
			sim = proto->clone();
			sim->set_owner(root());
			scope()->push_back(sim);
		}else{
			incomplete();
		}
	}else if(auto s=dynamic_cast<Simulator*>(*i)){ untested();
		sim = s;
	}else{
	}

	return sim;
}
#endif
/* -------------------------------------------------------------------------------- */
void setParameter(std::string const&, std::string const&)
{
	incomplete();
}
/* -------------------------------------------------------------------------------- */
SchematicModel* QucsDoc::scope()
{
	if(root()){
		return root()->scope();
	}else{
		return nullptr;
	}
}
/* -------------------------------------------------------------------------------- */
SchematicModel const* QucsDoc::scope() const
{
	auto d = const_cast<QucsDoc*>(this);
	return d->scope();
}
/* -------------------------------------------------------------------------------- */
SubcktBase const* QucsDoc::root() const
{
	auto d = const_cast<QucsDoc*>(this);
	return d->root();
}
/* -------------------------------------------------------------------------------- */
#if 0
// don't know how this works in legacy qucs. the tabs suggest that circuits
// from different "projects" can be openened simultaneously.
// Now a project might have prototypes (e.g. user library) that are not global,
// they are stashed here.
void QucsDoc::installElement(Element const* e)
{
	trace1("installElement", e->label());
	auto& p = _protos[e->label()];
	if(p){
		delete p;
	}else{
	}
	p = e;
}
#endif
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
