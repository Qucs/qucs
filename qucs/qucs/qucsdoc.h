/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCSDOC_H
#define QUCSDOC_H

#include "object.h"
#include <QString>
#include <QDateTime>
#include <QTextStream> // tmp: used as baseclass, should be member.
#include <assert.h>

#include "object.h"
#include "actions.h"
#include "trace.h"

class QucsApp;
class QAction;
class QPrinter;
class QPainter;
class MouseActions;
class MouseAction;
class QUndoStack;
class QUndoCommand;

class QucsDoc {
protected:
	QucsDoc(const QucsDoc&);
public:
  explicit QucsDoc(QucsApp&, const QString&);
  virtual ~QucsDoc();

public:
  virtual void  setName(const QString&) {};
  virtual bool  load() { return true; };
  virtual int   save() { return 0; };
  virtual void  print(QPrinter*, QPainter*, bool, bool) {};
  virtual void  becomeCurrent(bool) = 0;
  virtual float zoomBy(float) { return 1.0; };
  virtual void  showAll() {};
  virtual void  showNoZoom() {};

  static QString fileSuffix (const QString&);
  QString fileSuffix (void);
  static QString fileBase (const QString&);
  QString fileBase (void);

private:
  QString DocName;

public:
  void setDocName(QString x){ DocName=x; }
  QString docName() const{ return DocName; }

  QString DataSet;     // name of the default dataset
  QString DataDisplay; // name of the default data display
  QString Script;
  QString SimTime;     // used for VHDL simulation, but stored in datadisplay
  QDateTime lastSaved;

  float Scale;
  QucsApp* App; // TODO: delete.
  QucsApp &_app;
  bool DocChanged;
  bool SimOpenDpl;   // open data display after simulation ?
  bool SimRunScript; // run script after simulation ?
  int  showBias;     // -1=no, 0=calculation running, >0=show DC bias points
  bool GridOn;
  int  tmpPosX, tmpPosY;

protected: // why not directly connect to undostack slots?!
  virtual void undo();
  virtual void redo();

protected:
//  void toggleAction(QAction* sender);

public: // actions: These somehow correspond to buttons.
        // needs cleanup...
	virtual void actionSelect(QAction* sender) = 0;
	virtual void actionCopy(QAction* sender) = 0;
	virtual void actionCut(QAction* sender) = 0;
	virtual void actionEditActivate(QAction* sender) = 0;
	virtual void actionEditUndo(QAction* sender) = 0;
	virtual void actionEditRedo(QAction* sender) = 0;
	virtual void actionSelectAll(QAction* sender) = 0;
	virtual void actionChangeProps(QAction* sender) = 0;

	// these are not implemented for qucstext, not called perhaps?
	virtual void actionApplyCompText() { unreachable(); }
	virtual void actionAlign(int) {unreachable();}
	virtual void actionDistrib(int) {unreachable();}
	virtual void actionSelectMarker() {unreachable();}
	virtual void actionCursor(arrow_dir_t) { unreachable();}
	virtual void actionExportGraphAsCsv(){ unreachable();}

	virtual void actionOnGrid(QAction*) {unreachable();}
	virtual void actionEditRotate(QAction*) {unreachable();}
	virtual void actionEditMirrorX(QAction*) {unreachable();}
	virtual void actionEditMirrorY(QAction*) {unreachable();}
	virtual void actionEditDelete(QAction*) {unreachable();}
	virtual void actionEditPaste(QAction*) {unreachable();}
	virtual void actionSetWire(QAction*) {unreachable();}
	virtual void actionInsertLabel(QAction*) {unreachable();}
	virtual void actionInsertEquation(QAction*) {unreachable();}
	virtual void actionInsertEntity(QAction*) {unreachable();}
	virtual void actionInsertPort(QAction*) {unreachable();}
	virtual void actionInsertGround(QAction*) {unreachable();}
	virtual void actionSetMarker(QAction*) {unreachable();}
	virtual void actionMoveText(QAction*) {unreachable();}
	virtual void actionZoomIn(QAction*) = 0;

	void uncheckActive();

protected: // cleaning up debris
	QAction* selectAction();
	virtual MouseActions* mouseActions();
public:
	MouseActions const* mouseActions() const;
private:
	void setActiveAction(MouseAction* a);
	void executeCommand(QUndoCommand*);
	MouseAction* activeAction();

public: // hmm
	virtual QUndoStack* undoStack(){return nullptr;}

public:
	void possiblyToggleAction(MouseAction* a, QAction* sender);
	MouseAction const* activeAction() const;
}; // QucsDoc

#endif
