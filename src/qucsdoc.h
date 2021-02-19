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

// Baseclass for documents

#ifndef QUCSDOC_H
#define QUCSDOC_H

#include <QString>
#include <QToolBar>
#include <QDateTime>
#include <QMouseEvent>
#include <QTextStream> // tmp: used as baseclass, should be member.
#include <assert.h>

#include "actions.h"
#include "io_trace.h"
#include "object.h"
#include "widget.h"

class QAction;
class QPainter;
class QPrinter;
class QucsApp;
class QUndoCommand;
class QUndoStack;

class CommonData;
class Element;
class MouseAction;
class MouseActions;
class SchematicModel;
class Simulator;
class SubcktBase;

class QucsDoc : public Widget {
protected:
	QucsDoc(const QucsDoc&);
	explicit QucsDoc(); // QucsApp*, const QString&, QWidget* owner);
public:
//  explicit QucsDoc(QucsApp*, const QString&, QWidget* owner);
	virtual ~QucsDoc();
	virtual void setParent(QWidget*) = 0;

public:
	bool saveAs();

	virtual void  setName(const QString&) {incomplete();}
	virtual void  setFileName(const QString&x) {incomplete(); _name=x;}
	virtual bool  load() { return true; }
	virtual int   save() { return 0; }
	virtual void  print(QPrinter*, QPainter*, bool, bool) const {}
	virtual float zoomBy(float) { return 1.0; }
	virtual void  showAll() {}
	virtual void  showNoZoom() {}
	virtual bool handleMouseActions(QEvent*) {return false;}
	virtual void addElement(Element*);

	virtual SchematicModel* model();
	void printCursorPosition(int x, int y);

	static QString fileSuffix (const QString&);
	QString fileSuffix (void);
	static QString fileBase (const QString&);
	QString fileBase (void);

	virtual QAction* createUndoAction(){ return nullptr; }
	virtual QAction* createRedoAction(){ return nullptr; }

private:
  QucsApp* app();

protected:
	void setEventHandler(MouseActions* a) { _eventHandler = a; }
	MouseActions* eventHandler() { return _eventHandler;}
	void showEvent(QShowEvent*);
	void hideEvent(QHideEvent*);

public:
	QToolBar* newToolBar();
	void addToolBarAction(QAction* a);

private:
  QString _name; // actually, filename?! use label. either way.

public:
  void setDocFile(std::string const& x);
  QString docName() const{ return _name; }
  virtual QPoint gridSize() const{ unreachable(); return QPoint(0, 0); }
  virtual QPoint setOnGrid(int x, int y) const{ unreachable(); return QPoint(x, y); }

  QString DataSet;     // name of the default dataset
  QString DataDisplay; // name of the default data display
  QString Script;
  QString SimTime;     // used for VHDL simulation, but stored in datadisplay
  QDateTime lastSaved;

//  float Scale;
  QucsApp* _app{nullptr}; /// yikes.
  bool DocChanged;
  bool SimOpenDpl;   // open data display after simulation ?
  bool SimRunScript; // run script after simulation ?
  int  showBias;     // -1=no, 0=calculation running, >0=show DC bias points
  bool GridOn;
  int  tmpPosX, tmpPosY;

protected:
	Simulator* simulatorInstance(std::string const& which="");

protected: // why not directly connect to undostack slots?!
  virtual void undo();
  virtual void redo();
  virtual void signalFileChanged(bool){incomplete();}

protected:
//  void toggleAction(QAction* sender);

public: // actions: These somehow correspond to buttons.
	void slotToolBar(QAction* a);


#if 1	// obsolete
        // needs cleanup... better use qt signals, but where?
	virtual void actionSelect(QAction*) { unreachable(); }
	virtual void actionCopy(QAction*) { unreachable(); }
	virtual void actionCut(QAction*) { unreachable(); }

	virtual void slotEditUndo(QAction*) = 0;
	virtual void slotEditRedo(QAction*) = 0;

	virtual void actionSelectAll(QAction*) { unreachable(); }
	virtual void actionChangeProps(QAction*) { unreachable(); }

	// these are not implemented for qucstext, not called perhaps?
//	virtual void actionApplyCompText() { unreachable(); }
//	virtual void actionAlign(int) {unreachable();}
//	virtual void actionDistrib(int) {unreachable();}
//	virtual void actionSelectMarker() {unreachable();}
//	virtual void actionExportGraphAsCsv(){ unreachable();}
//
//	virtual void actionOnGrid(QAction*) {unreachable();}
//	virtual void actionSetWire(QAction*) {unreachable();}
//	virtual void actionInsertLabel(QAction*) {unreachable();}
//	virtual void actionInsertEquation(QAction*) {unreachable();}
//	virtual void actionInsertPort(QAction*) {unreachable();}
//	virtual void actionInsertGround(QAction*) {unreachable();}
//	virtual void actionSetMarker(QAction*) {unreachable();}
//	virtual void actionMoveText(QAction*) {unreachable();}
	virtual void actionCursor(arrow_dir_t) { unreachable();}
	virtual void actionEditPaste(QAction*) {unreachable();}
	virtual void actionZoomIn(QAction*) { unreachable(); }
#endif

	// textdoc?
	virtual void actionInsertEntity(QAction*) {unreachable();}

	// this one has to do with the left "component toolbox"
	virtual void actionSelectElement(QObject*) {untested(); }
	virtual void slotEditDelete(QAction*) = 0;
	virtual void slotEditActivate(QAction*) = 0;

	void uncheckActive();

	virtual void slotSimulate(); // why "slot"? maybe later.
	virtual void slotDCbias(); // why "slot"? maybe later.

protected: // cleaning up debris
//	QAction* selectAction(); really??
	// virtual MouseActions* mouseActions();
public:
//	MouseActions const* mouseActions() const;
	virtual QMouseEvent snapToGrid(QMouseEvent* e) const{
		assert(e);
		return QMouseEvent(*e);
	}
	void executeCommand(QUndoCommand*);

public: // hmm
	virtual QUndoStack* undoStack(){return nullptr;}
   virtual void updateViewport() {}
   virtual void reloadGraphs() {} // fix later.

public:

	CommonData* qucsData(std::string const& key);
	QWidget* ownerWidget(){return _owner;}
	void setOwner(QWidget* o){_owner=o;}

	void setParameter(std::string const&, std::string const&);

public:
	SubcktBase const* root() const;
	SchematicModel const* scope() const;

public: // really??
	virtual SubcktBase* root() {return nullptr;}
	SchematicModel* scope();

private:
	QWidget* _owner;
	MouseActions* _eventHandler{nullptr};
}; // QucsDoc

#endif
