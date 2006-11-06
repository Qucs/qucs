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

#include <qptrlist.h>


class Wire;
class Schematic;
class QPainter;
class QPopupMenu;
class QMouseEvent;


class MouseActions {
public:
  MouseActions();
  virtual ~MouseActions();

  void setPainter(Schematic*, QPainter*);
  bool pasteElements(Schematic*);
  void editElement(Schematic*, QMouseEvent*);
  void editLabel(Schematic*, WireLabel*);

  bool drawn;  // indicates whether the scheme element was drawn last time
  Element *selElem;  // component/diagram/painting selected in IconView
  Element *focusElement;
  QMouseEvent *focusMEvent;

  int  MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  QPtrList<Element> movingElements;

  // menu appearing by right mouse button click on component
  QPopupMenu *ComponentMenu;

private:
  bool isMoveEqual;

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

  void MPressSelect(Schematic*, QMouseEvent*, int, int);
  void MPressDelete(Schematic*, QMouseEvent*, int, int);
  void MPressActivate(Schematic*, QMouseEvent*, int, int);
  void MPressMirrorX(Schematic*, QMouseEvent*, int, int);
  void MPressMirrorY(Schematic*, QMouseEvent*, int, int);
  void MPressRotate(Schematic*, QMouseEvent*, int, int);
  void MPressElement(Schematic*, QMouseEvent*, int, int);
  void MPressLabel(Schematic*, QMouseEvent*, int, int);
  void MPressWire1(Schematic*, QMouseEvent*, int, int);
  void MPressWire2(Schematic*, QMouseEvent*, int, int);
  void MPressPainting(Schematic*, QMouseEvent*, int, int);
  void MPressMarker(Schematic*, QMouseEvent*, int, int);
  void MPressOnGrid(Schematic*, QMouseEvent*, int, int);
  void MPressMoveText(Schematic*, QMouseEvent*, int, int);
  void MPressZoomIn(Schematic*, QMouseEvent*, int, int);

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

  void moveElements(QPtrList<Element>*, int, int);
  void endElementMoving(Schematic*, QPtrList<Element>*);
  void rightPressMenu(Schematic*, QMouseEvent*, int, int);
};

#endif
