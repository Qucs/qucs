/***************************************************************************
                                qucsdoc.cpp
                               -------------
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
#include "qucs.h"
#include <QUndoStack>
#include "io.h" // tmp?


QucsDoc::QucsDoc(QucsApp &App_, const QString& Name_)
	: App(&App_),
     _app(App_)
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
  Scale = 1.0;
}

// vtable here?
QucsDoc::~QucsDoc()
{
}

// really?!
void QucsDoc::undo()
{
	QUndoStack* u = undoStack();
	if(u){
		u->undo();
	}else{
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
	assert(App);
  	return App->select;
}

void QucsDoc::setActiveAction(MouseAction* a)
{
	if(mouseActions()){ untested();
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
	if(!sender){
		unreachable();
	}else if(!sender->isCheckable()){ untested();
		cmd = a->activate(sender);
	}else if(sender->isChecked()){ untested();

		cmd = a->activate(sender);

		if(cmd){ untested();
			sender->setChecked(false);
			// possible 'delete' after select.
			// don't do anything else
			a->deactivate();
		}else{ untested();
			// sender->setChecked(true); // assert?
			setActiveAction(a);
		}
	}else{ untested();
		setActiveAction(nullptr);
	}

	if(cmd){ untested();
		executeCommand(cmd);
	}else{ untested();
	}
}

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
MouseAction const* QucsDoc::activeAction() const
{
	auto d = const_cast<QucsDoc*>(this);
	return d->activeAction();
}

void QucsDoc::executeCommand(QUndoCommand* c)
{
	if(mouseActions()){
		mouseActions()->executeCommand(c);
	}else{
	}
}
