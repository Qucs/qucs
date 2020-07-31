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

#ifndef MOUSEACTIONS_H
#define MOUSEACTIONS_H

#include "element.h"
#include "qt_compat.h"
#include "schematic_scene.h"
#include "element_graphics.h"
#include <QEvent>
#include <QAction>

class Label;
class MouseActions;
class QAction;
class QMenu;
class QMouseEvent;
class QPainter;
class QUndoCommand;
class Schematic;
class Wire;

// something happens to the mouse on a schematic
// BUG: wrong file. schematic_mouse.h maybe?
// FIXME: why Mouse? User??
class MouseAction {
public:
	typedef QUndoCommand cmd;
protected:
public:
	explicit MouseAction(MouseActions& ctx)
		:_ctx(ctx), _sender(nullptr){}
	MouseAction(MouseAction const&) = delete;

public:
	virtual ~MouseAction(){}

public:
	cmd* handle(QEvent*);

	virtual cmd* activate(QAction* sender);
	virtual cmd* deactivate();
// private: TODO
	// TODO: only use POS in those
	virtual cmd* move(QEvent*) { return nullptr; }
	virtual cmd* press(QEvent*) { return nullptr; }
	// virtual cmd* grab(QGraphicsSceneEvent*) { return nullptr; }
	virtual cmd* release(QMouseEvent*) { return nullptr; }
	virtual cmd* dblclk(QEvent*) { return nullptr; }

	virtual cmd* generic(QEvent*) { return nullptr; } // remove
	virtual cmd* enter(QEvent*) { untested(); return nullptr; }
	virtual cmd* leave(QEvent*) { untested(); return nullptr; }

	void uncheck();

protected:
	MouseActions& ctx(){return _ctx;}

protected: // bug. private
	SchematicDoc const& doc() const;

protected:
	void sceneAddItem(ElementGraphics*);
	void sceneRemoveItem(ElementGraphics*);

protected:
	SchematicDoc& doc(); // BUG _ctx.
	QList<ElementGraphics*> selectedItems(); // BUG. copies.
	QPointF mapToScene(QPoint const& p) const;
	void updateViewport(); // why?
	void setCursor(QCursor const& c);
   bool isNode(int fX, int fY) const;

private:
	MouseActions& _ctx;
	QAction* _sender;
};


// a mouse action on an element (first attempt)
// formerly, a mouse action was implemented by means of messing with element
// internals.
class ElementMouseAction {
public:
	explicit ElementMouseAction(ElementGraphics* e)
		: _e(e)
	{
	}
public:
	ElementGraphics* element() { return _e; }
	ElementGraphics const* element() const { return _e; }

	void clear(){
		_e=nullptr;
	}
public: // compat with old code
	bool operator==(ElementGraphics const* e) const{
		return _e==e;
	}
	bool operator!=(ElementGraphics const* e) const{
		return _e!=e;
	}
#ifndef USE_SCROLLVIEW
	bool operator==(Element const* e) const{
		return ::element(_e)==e;
	}
	bool operator!=(Element const* e) const{
		return *_e!=e;
	}
#endif
	operator bool() const{
		return _e;
	}
	void setSelected(bool x=true);
	bool isSelected() const;

	void toggleSelected() const{
		assert(_e);
		_e->toggleSelected();
	}

public: // access coordinates from old code.
	     // newer code might use Qt conventions...?
	int const& cx_() const { assert(_e); return _e->cx_(); }
	int const& cy_() const { assert(_e); return _e->cy_(); }
	int const& x1_() const { assert(_e); return _e->x1_(); }
	int const& y1_() const { assert(_e); return _e->y1_(); }
	int const& x2_() const { assert(_e); return _e->x2_(); }
	int const& y2_() const { assert(_e); return _e->y2_(); }
public:
	// don't use. just to compile
	int Type; // BUG BUG
#if 1
	ElementMouseAction* operator->(){
		unreachable(); // complain loudly.
		return this;
	}
#endif
	void setObsoleteType(int t){
		_action_type = t;
	}

private:
	int _action_type; // the legacy way.
	                  //  might need cleanup
  ElementGraphics* _e;
};

// enable access to attached elements.
// this might be temporary
inline ElementGraphics* element(ElementMouseAction e)
{
  return e.element();
}
inline Component* component(ElementMouseAction e)
{
  return component(element(e));
}
inline Wire* wire(ElementMouseAction e)
{
  return wire(element(e));
}
inline WireLabel* wireLabel(ElementMouseAction e)
{
  return wireLabel(element(e));
}
inline Diagram* diagram(ElementMouseAction e)
{
  return diagram(element(e));
}
inline Painting* painting(ElementMouseAction e)
{
  return painting(element(e));
}
inline Graph* graph(ElementMouseAction e)
{
  return graph(element(e));
}
inline Marker* marker(ElementMouseAction e)
{
  return marker(element(e));
}
inline Node* node(ElementMouseAction e)
{
  return node(element(e));
}
Label* label(ElementMouseAction e);


extern QAction *formerAction;

// must be QObject so it can receive/filter events
class MouseActions : public QObject {
	Q_OBJECT
public:
  typedef QList<ElementGraphics*> EGPList;
public:
  MouseActions(QucsDoc&);
  virtual ~MouseActions();

  void setPainter(SchematicDoc*);
  bool pasteElements(SchematicDoc*);
  void editElement(SchematicDoc*, QMouseEvent*);
  void editLabel(SchematicDoc*, WireLabel*);

  void setDrawn(bool b=true){_drawn = b;}
  bool wasDrawn() const{return _drawn;}
// private: BUG.
  Element *selElem;  // component/diagram/painting selected in IconView
  ElementMouseAction focusElement; // BUG: use focusMEvent instead
  QMouseEvent *focusMEvent;

public: // really?
  bool hasElem(){ return selElem; }
  Element* getElem(){assert(selElem); return selElem; }
  void setElem(Element* e){selElem=e;}

private:
public: // BUG? called from MouseAction.
  void Set1(QMouseEvent*, SchematicDoc*ignore=nullptr);
  void Set2(QMouseEvent*, SchematicDoc*ignore=nullptr);
  void Set3(QMouseEvent*, SchematicDoc*ignore=nullptr);
public: // BUG
  int MAx1, MAy1, MAx2, MAy2;
  int MAx3, MAy3;
private:
  QList<ElementGraphics*> movingElements;
public:
  int movingRotated;

  // menu appearing by right mouse button click on component
  QMenu *ComponentMenu;

private:
  // former Schematic::select*
  // but that does not work, because ElementMouseAction lives here.
  // (does it matter?)
public:
  ElementMouseAction selectElement(QPoint const&, bool, int *index=0);
  Component* selectCompText(SchematicDoc*, int, int, int&, int&);
  void     deselectElements(ElementMouseAction);

public: // really?
  QucsDoc& doc();
  void updateViewport();

public:
#define Schematic SchematicDoc
  void MMoveSelect(Schematic*, QMouseEvent*);
  void MMoveElement(Schematic*, QMouseEvent*);
//  void MMoveWire1(Schematic*, QMouseEvent*);
//  void MMoveWire2(Schematic*, QMouseEvent*);
//  void MMoveMoving(Schematic*, QMouseEvent*);
//  void MMoveMoving2(Schematic*, QMouseEvent*);
  void MMovePaste(Schematic*, QMouseEvent*);
//  void MMoveDelete(Schematic*, QMouseEvent*);
  void MMoveLabel(Schematic*, QMouseEvent*);
  void MMoveMarker(Schematic*, QMouseEvent*);
  void MMoveMirrorY(Schematic*, QMouseEvent*);
  void MMoveMirrorX(Schematic*, QMouseEvent*);
  void MMoveRotate(Schematic*, QMouseEvent*);
  void MMoveActivate(Schematic*, QMouseEvent*);
  void MMoveOnGrid(Schematic*, QMouseEvent*);
  void MMoveResizePainting(Schematic*, QMouseEvent*);
  void MMoveMoveText(Schematic*, QMouseEvent*);
  void MMoveMoveTextB(Schematic*, QMouseEvent*);
  void MMoveZoomIn(Schematic*, QMouseEvent*);
  void MMoveScrollBar(Schematic*, QMouseEvent*);

  void MPressSelect(QMouseEvent*);
//  void MPressDelete(Schematic*, QMouseEvent*);
  void MPressActivate(Schematic*, QMouseEvent*);
  void MPressMirrorX(Schematic*, QMouseEvent*);
  void MPressMirrorY(Schematic*, QMouseEvent*);
  void MPressRotate(Schematic*, QMouseEvent*);
//  void MPressElement(Schematic*, QMouseEvent*);
  void MPressLabel(Schematic*, QMouseEvent*);
//  void MPressWire1(Schematic*, QMouseEvent*);
//  void MPressWire2(Schematic*, QMouseEvent*);
  void MPressPainting(Schematic*, QMouseEvent*);
  void MPressMarker(Schematic*, QMouseEvent*);
  void MPressOnGrid(Schematic*, QMouseEvent*);
  void MPressMoveText(Schematic*, QMouseEvent*);
  void MPressZoomIn(Schematic*, QMouseEvent*);

  void MDoubleClickSelect(Schematic*, QMouseEvent*);
  void MDoubleClickWire2(Schematic*, QMouseEvent*);

  void MReleaseSelect(Schematic*, QMouseEvent*);
  void MReleaseSelect2(Schematic*, QMouseEvent*);
  void MReleaseActivate(Schematic*, QMouseEvent*);
  void MReleaseMoving(Schematic*, QMouseEvent*);
  void MReleaseResizeDiagram(Schematic*, QMouseEvent*);
  void MReleasePaste(Schematic*, QMouseEvent*);
  void MReleaseResizePainting(Schematic*, QMouseEvent*);
  void MReleaseMoveText(Schematic*, QMouseEvent*);
  void MReleaseZoomIn(Schematic*, QMouseEvent*);

  void paintElementsScheme(Schematic*);
//  void rotateElements(Schematic*, int&, int&);
//  void moveElements(Schematic*, int&, int&);
  void moveElements(QList<ElementGraphics*>&, int, int);
  void endElementMoving(Schematic*, EGPList*);
  void rightPressMenu(QMouseEvent*);
#undef Schematic

  bool eventFilter(QObject *obj, QEvent *event);
  virtual bool handle(QEvent*);
  void executeCommand(QUndoCommand* c);
  MouseAction* activeAction(){ return _maCurrent; }
  void setActive(MouseAction* a);

public:
  void undo();
  void redo();

protected:
	MouseAction* _maCurrent;
private:
	// QUndoStack* _undoStack; // Doc
  bool _drawn;  // indicates whether the scheme element was drawn last time
private:
  bool isMoveEqual;
protected:
  QucsDoc& _doc;
}; // MouseActions

class Label;

// really?
ElementGraphics* element(ElementMouseAction);
Component* component(ElementMouseAction);
Wire* wire(ElementMouseAction);
WireLabel* wireLabel(ElementMouseAction);
Diagram* diagram(ElementMouseAction);
Painting* painting(ElementMouseAction);
Graph* graph(ElementMouseAction);
Marker* marker(ElementMouseAction);
Node* node(ElementMouseAction);
Label* label(ElementMouseAction);
WireLabel* wireLabel(ElementMouseAction);

#endif
