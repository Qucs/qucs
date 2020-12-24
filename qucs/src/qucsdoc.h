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
#include <QMouseEvent>
#include <QTextStream> // tmp: used as baseclass, should be member.
#include <assert.h>

#include "object.h"
#include "actions.h"
#include "io_trace.h"

class QucsApp;
class QAction;
class QPrinter;
class QPainter;
class MouseActions;
class MouseAction;
class QUndoStack;
class QUndoCommand;
class Element;
class QucsData;
class Simulator;

class QucsDoc : public Object {
protected:
	QucsDoc(const QucsDoc&);
public:
  explicit QucsDoc(QucsApp*, const QString&, QWidget* owner);
  virtual ~QucsDoc();
 Object::Type type() const override {return Object::Type::QucsDoc;}

public:
  bool saveAs();

  virtual void  setName(const QString&) {};
  virtual bool  load() { return true; };
  virtual int   save() { return 0; };
  virtual void  print(QPrinter*, QPainter*, bool, bool) {};
  virtual void  becomeCurrent(bool){ unreachable(); }
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

//  float Scale;
  QucsApp* _app;
  bool DocChanged;
  bool SimOpenDpl;   // open data display after simulation ?
  bool SimRunScript; // run script after simulation ?
  int  showBias;     // -1=no, 0=calculation running, >0=show DC bias points
  bool GridOn;
  int  tmpPosX, tmpPosY;

  void installElement(Element const*);
  Element const* find_proto(std::string const& name) const;

protected:
  Simulator* simulatorInstance(std::string const& which="");

protected: // why not directly connect to undostack slots?!
  virtual void undo();
  virtual void redo();
  virtual void signalFileChanged(bool){incomplete();}

protected:
//  void toggleAction(QAction* sender);

public: // actions: These somehow correspond to buttons.
        // needs cleanup... better use qt signals, but where?
	virtual void actionSelect(QAction*) { unreachable(); }
	virtual void actionCopy(QAction*) { unreachable(); }
	virtual void actionCut(QAction*) { unreachable(); }
	virtual void actionEditActivate(QAction*) { unreachable(); }
	virtual void actionEditUndo(QAction*) { unreachable(); }
	virtual void actionEditRedo(QAction*) { unreachable(); }
	virtual void actionSelectAll(QAction*) { unreachable(); }
	virtual void actionChangeProps(QAction*) { unreachable(); }

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
	virtual void actionZoomIn(QAction*) { unreachable(); }
	virtual void actionSelectElement(QObject*) {untested(); }

	void uncheckActive();

	virtual void slotSimulate(); // why "slot"? maybe later.
	virtual void slotDCbias(); // why "slot"? maybe later.

protected: // cleaning up debris
	QAction* selectAction();
	virtual MouseActions* mouseActions();
public:
	MouseActions const* mouseActions() const;
	virtual QMouseEvent snapToGrid(QMouseEvent* e) const{
		assert(e);
		return QMouseEvent(*e);
	}
	void executeCommand(QUndoCommand*);
private:
	void setActiveAction(MouseAction* a);
	MouseAction* activeAction();

public: // hmm
	virtual QUndoStack* undoStack(){return nullptr;}
   virtual void updateViewport() {}
   virtual void reloadGraphs() {} // fix later.

public:
	void possiblyToggleAction(MouseAction* a, QAction* sender);
	MouseAction const* activeAction() const;

	QucsData* qucsData(std::string const& key);
	QWidget* ownerWidget(){return _owner;}
	void setOwner(QWidget* o){_owner=o;}

	void setParameter(std::string const&, std::string const&);

private:
	friend class Simulator;

	std::map<std::string, Element const*> _protos;
	std::map<std::string, QucsData*> _data;
	std::map<std::string, Simulator*> _simulators;

	QWidget* _owner;
}; // QucsDoc

#endif
