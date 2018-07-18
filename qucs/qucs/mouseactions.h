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

#include <Q3PtrList>

class Wire;
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

  bool pasteElements(Schematic*);
  void editElement(Schematic*, QMouseEvent*);
  void editLabel(Schematic*, WireLabel*);

  bool drawn;  // indicates whether the scheme element was drawn last time
  Element *selElem;  // component/diagram/painting selected in IconView
  Element *focusElement;
  QMouseEvent *focusMEvent;

  int  MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  Q3PtrList<Element> movingElements;
  int movingRotated;

  // menu appearing by right mouse button click on component
  QMenu *ComponentMenu;

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

  // FIXME several of the float, float are unused, deprecated
  void MPressSelect(Schematic*, QMouseEvent*, float, float);
  void MPressDelete(Schematic*, QMouseEvent*, float, float);
  void MPressActivate(Schematic*, QMouseEvent*, float, float);
  void MPressMirrorX(Schematic*, QMouseEvent*, float, float);
  void MPressMirrorY(Schematic*, QMouseEvent*, float, float);
  void MPressRotate(Schematic*, QMouseEvent*, float, float);
  void MPressElement(Schematic*, QMouseEvent*, float x, float y);
  void MPressLabel(Schematic*, QMouseEvent*, float, float);
  void MPressWire1(Schematic*, QMouseEvent*, float, float);
  void MPressWire2(Schematic*, QMouseEvent*, float, float);
  void MPressPainting(Schematic*, QMouseEvent*, float, float);
  void MPressMarker(Schematic*, QMouseEvent*, float, float);
  void MPressOnGrid(Schematic*, QMouseEvent*, float, float);
  void MPressMoveText(Schematic*, QMouseEvent*, float, float);
  void MPressZoomIn(Schematic*, QMouseEvent*, float, float);

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

#endif
