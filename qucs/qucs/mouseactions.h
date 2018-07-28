/***************************************************************************
                              mouseactions.h
                             ----------------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MOUSEACTIONS_H
#define MOUSEACTIONS_H

#include "element.h"
#include "qt_compat.h"


// a mouse action on an element.
// formerly, a mouse action was implemented by means of messing with element
// internals.
class ElementMouseAction {
public:
	explicit ElementMouseAction(Element* e)
		: _e(e)
	{
	}
public:
	Element* element() { return _e; }
	Element const* element() const { return _e; }

	void clear(){
		_e=nullptr;
	}
public: // compat with old code
	bool operator==(Element const* e) const{
		return _e==e;
	}
	bool operator!=(Element const* e) const{
		return _e!=e;
	}
	operator bool() const{
		return _e;
	}
	void setSelected() const{
		incomplete();
	}
	bool isSelected() const{
		incomplete();
		return false;
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
	ElementMouseAction* operator->(){
		unreachable(); // complain loudly.
		return this;
	}
	void setObsoleteType(int t){
		_action_type = t;
	}

private:
	int _action_type; // the legacy way.
	                  //  might need cleanup
  Element* _e;
};

class Wire;
class Label;

// enable access to attached elements.
// this might be temporary
inline Element* element(ElementMouseAction e)
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

class Schematic;
class QPainter;
class QMenu;
class QMouseEvent;
class QucsApp;

extern QAction *formerAction;

class MouseActions {
public:
  MouseActions(QucsApp*);
  virtual ~MouseActions();

  void setPainter(Schematic*);
  bool pasteElements(Schematic*);
  void editElement(Schematic*, QMouseEvent*);
  void editLabel(Schematic*, WireLabel*);

  bool drawn;  // indicates whether the scheme element was drawn last time
  Element *selElem;  // component/diagram/painting selected in IconView
  ElementMouseAction focusElement; // BUG: use focusMEvent instead
  QMouseEvent *focusMEvent;

  int  MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  Q3PtrList<Element> movingElements;
  int movingRotated;

  // menu appearing by right mouse button click on component
  QMenu *ComponentMenu;

private:
  // former Schematic::select*
  // but that does not work, because ElementMouseAction lives here.
  ElementMouseAction selectElement(Schematic*, float, float, bool, int *index=0);
  Component* selectCompText(Schematic*, int, int, int&, int&);
  void     deselectElements(Schematic*, ElementMouseAction);

private:
  bool isMoveEqual;
  QucsApp* App;

  // -------------------------------------------------------------------
public:
  void MMoveSelect(Schematic*, QMouseEvent*);
  void MMoveElement(Schematic*, QMouseEvent*);
  void MMoveWire1(Schematic*, QMouseEvent*);
  void MMoveWire2(Schematic*, QMouseEvent*);
  void MMoveMoving(Schematic*, QMouseEvent*);
  void MMoveMoving2(Schematic*, QMouseEvent*);
  void MMovePaste(Schematic*, QMouseEvent*);
  void MMoveDelete(Schematic*, QMouseEvent*);
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

  void MPressSelect(Schematic*, QMouseEvent*);
  void MPressDelete(Schematic*, QMouseEvent*);
  void MPressActivate(Schematic*, QMouseEvent*);
  void MPressMirrorX(Schematic*, QMouseEvent*);
  void MPressMirrorY(Schematic*, QMouseEvent*);
  void MPressRotate(Schematic*, QMouseEvent*);
  void MPressElement(Schematic*, QMouseEvent*);
  void MPressLabel(Schematic*, QMouseEvent*);
  void MPressWire1(Schematic*, QMouseEvent*);
  void MPressWire2(Schematic*, QMouseEvent*);
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
  void rotateElements(Schematic*, int&, int&);
  void moveElements(Schematic*, int&, int&);
  void moveElements(Q3PtrList<Element>*, int, int);
  void endElementMoving(Schematic*, Q3PtrList<Element>*);
  void rightPressMenu(Schematic*, QMouseEvent*);
};

Element* element(ElementMouseAction);
Component* component(ElementMouseAction);
Wire* wire(ElementMouseAction);
WireLabel* wireLabel(ElementMouseAction);
Diagram* diagram(ElementMouseAction);
Painting* painting(ElementMouseAction);
Graph* graph(ElementMouseAction);
Marker* marker(ElementMouseAction);
Node* node(ElementMouseAction);

#endif
