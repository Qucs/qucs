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

// Deal with user interactions with a doc
// inherit from this alongside a specialisation of QucsDoc

#ifndef QUCS_DOC_ACTIONS
#define QUCS_DOC_ACTIONS

#include "io_trace.h"
#include <QObject>

class ElementGraphics;
class QUndoCommand;
class MouseAction;
class QucsDoc;
class QAction;
class QMenu; // here?!


// must be QObject so it can receive/filter events (needed??!)
class MouseActions : public QObject {
	Q_OBJECT
public:
  typedef QList<ElementGraphics*> EGPList;
public:
  MouseActions(QucsDoc*);
  virtual ~MouseActions();

//  void setPainter(SchematicDoc*);
//  bool pasteElements(SchematicDoc*);
//  void editElement(SchematicDoc*, QMouseEvent*);
//  void editLabel(SchematicDoc*, WireLabel*);

  void setDrawn(bool b=true){_drawn = b;}
  bool wasDrawn() const{return _drawn;}

  // menu appearing by right mouse button click on component
  QMenu *ComponentMenu;

public:
	void slotToggle(QAction* sender);
	void slotTrigger(QAction* sender);

private:
  // former Schematic::select*
  // but that does not work, because ElementMouseAction lives here.
  // (does it matter?)
public:
  // Component* selectCompText(SchematicDoc*, int, int, int&, int&);
//  void     deselectElements(ElementMouseAction);
  void updateViewport(); // why?

public: // TODO. move into mouse actions
	//  void rightPressMenu(QMouseEvent*);

	bool eventFilter(QObject *obj, QEvent *event);
	virtual bool handle(QEvent*);
	//virtual void setParent(QWidget* ctx) = 0;
	virtual void setControls(QucsDoc* ctx) = 0;

	void executeCommand(QUndoCommand* c);
	MouseAction* activeAction(){ untested(); return _maCurrent; }
	MouseAction const* currentMode() const{ untested(); return _maCurrent; }
	void setCurrentMode(MouseAction* a);

public:
  void undo();
  void redo();

public:
	QucsDoc* doc();
	QucsDoc const* doc() const;
	void possiblyToggleAction(MouseAction* a, QObject* sender);

private:
	MouseAction* _maCurrent{nullptr};
	// QUndoStack* _undoStack; // Doc
  bool _drawn;  // indicates whether the scheme element was drawn last time
  bool isMoveEqual;

}; // MouseActions

#endif
