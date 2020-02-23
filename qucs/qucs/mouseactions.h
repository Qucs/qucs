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

#include "graphicitem.h"
#include "qt_compat.h"

class Wire;
class SchematicView;
class QPainter;
class QMenu;
class QMouseEvent;
class QucsApp;

extern QAction *formerAction;

class MouseActions {
public:
  MouseActions(QucsApp*);
  virtual ~MouseActions();

  bool pasteElements(SchematicView*);
  void editElement(SchematicView*, QMouseEvent*);
  void editLabel(SchematicView*, WireLabel*);

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
  void MMoveSelect(SchematicView*, QMouseEvent*);
  void MMoveElement(SchematicView*, QMouseEvent*);
  void MMoveWire1(SchematicView*, QMouseEvent*);
  void MMoveWire2(SchematicView*, QMouseEvent*);
  void MMoveMoving(SchematicView*, QMouseEvent*);
  void MMoveMoving2(SchematicView*, QMouseEvent*);
  void MMovePaste(SchematicView*, QMouseEvent*);
  void MMoveDelete(SchematicView*, QMouseEvent*);
  void MMoveLabel(SchematicView*, QMouseEvent*);
  void MMoveMarker(SchematicView*, QMouseEvent*);
  void MMoveMirrorY(SchematicView*, QMouseEvent*);
  void MMoveMirrorX(SchematicView*, QMouseEvent*);
  void MMoveRotate(SchematicView*, QMouseEvent*);
  void MMoveActivate(SchematicView*, QMouseEvent*);
  void MMoveOnGrid(SchematicView*, QMouseEvent*);
  void MMoveResizePainting(SchematicView*, QMouseEvent*);
  void MMoveMoveText(SchematicView*, QMouseEvent*);
  void MMoveMoveTextB(SchematicView*, QMouseEvent*);
  void MMoveZoomIn(SchematicView*, QMouseEvent*);
  void MMoveScrollBar(SchematicView*, QMouseEvent*);

  // FIXME several of the float, float are unused, deprecated
  void MPressSelect(SchematicView*, QMouseEvent*);
  void MPressDelete(SchematicView*, QMouseEvent*);
  void MPressActivate(SchematicView*, QMouseEvent*);
  void MPressMirrorX(SchematicView*, QMouseEvent*);
  void MPressMirrorY(SchematicView*, QMouseEvent*);
  void MPressRotate(SchematicView*, QMouseEvent*);
  void MPressElement(SchematicView*, QMouseEvent*);
  void MPressLabel(SchematicView*, QMouseEvent*);
  void MPressWire1(SchematicView*, QMouseEvent*);
  void MPressWire2(SchematicView*, QMouseEvent*);
  //unused void MPressPainting(Schematic*, QMouseEvent*, float, float);
  void MPressMarker(SchematicView*, QMouseEvent*);
  void MPressOnGrid(SchematicView*, QMouseEvent*);
  void MPressMoveText(SchematicView*, QMouseEvent*);
  void MPressZoomIn(SchematicView*, QMouseEvent*);

  void MDoubleClickSelect(SchematicView*, QMouseEvent*);
  void MDoubleClickWire2(SchematicView*, QMouseEvent*);

  void MReleaseSelect(SchematicView*, QMouseEvent*);
  void MReleaseSelect2(SchematicView*, QMouseEvent*);
  void MReleaseActivate(SchematicView*, QMouseEvent*);
  void MReleaseMoving(SchematicView*, QMouseEvent*);
  void MReleaseResizeDiagram(SchematicView*, QMouseEvent*);
  void MReleasePaste(SchematicView*, QMouseEvent*);
  void MReleaseResizePainting(SchematicView*, QMouseEvent*);
  void MReleaseMoveText(SchematicView*, QMouseEvent*);
  void MReleaseZoomIn(SchematicView*, QMouseEvent*);

  void paintElementsScheme(SchematicView*);
  void rotateElements(SchematicView*, int&, int&);
  void moveElements(SchematicView*, int&, int&);
  void moveElements(Q3PtrList<Element>*, int, int);
  void endElementMoving(SchematicView*, Q3PtrList<Element>*);
  void rightPressMenu(SchematicView*, QMouseEvent*);
};

#endif
