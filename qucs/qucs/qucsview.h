/***************************************************************************
                          qucsview.h  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
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

#ifndef QUCSVIEW_H
#define QUCSVIEW_H

// include files for QT
#include <qscrollview.h>
#include <qwidget.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qevent.h>
#include <qdir.h>


// application specific includes
#include "components/component.h"
#include "wire.h"
#include "qucsdoc.h"
#include "main.h"



// **************************************************************************
// ******* This class provides an incomplete base for the application *******
// ******* view. (scematics, data displays etc.)                      *******
// **************************************************************************

class QucsView : public QScrollView
{
  Q_OBJECT

public:
  QucsView(QWidget *parent=0);
  ~QucsView();

  double Zoom(double s);
  bool   pasteElements();
  void   enlargeView(int x1, int y1, int x2, int y2);
  void   setPainter(QPainter *p, QucsDoc *d);
  void   eraseCross();
  void   editLabel(WireLabel *pl);

  Component *selComp;   // component selected in IconView
  Diagram   *selDiag;   // diagram selected in IconView
  Painting  *selPaint;  // painting selected in IconView

  bool    drawn;  // indicates whether the scheme element was drawn last time
  QString ProjName;

  QPtrList<Element> movingElements;

  // -------------------------------------------------------------------
  QPtrList<QucsDoc>  Docs;  // instances of the documents (schematics, data displays)

protected:
  void drawContents(QPainter *p, int clipx, int clipy, int clipw, int cliph);
  void contentsMouseMoveEvent(QMouseEvent *Event);
  void contentsMousePressEvent(QMouseEvent *Event);
  void contentsMouseDoubleClickEvent(QMouseEvent *Event);
  void contentsMouseReleaseEvent(QMouseEvent *Event);
  void contentsWheelEvent(QWheelEvent *Event);

  bool ScrollUp(int step);
  bool ScrollDown(int step);
  bool ScrollLeft(int step);
  bool ScrollRight(int step);

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();

public slots:
  void slotCursorLeft();
  void slotCursorRight();
  void slotCursorUp();
  void slotCursorDown();

public:
  void MouseDoNothing(QMouseEvent*);
  void MMoveSelect(QMouseEvent*);
  void MMoveComponent(QMouseEvent*);
  void MMoveDiagram(QMouseEvent*);
  void MMoveWire1(QMouseEvent*);
  void MMoveWire2(QMouseEvent*);
  void MMoveMoving(QMouseEvent*);
  void MMoveMoving2(QMouseEvent*);
  void MMovePaste(QMouseEvent*);
  void MMovePainting(QMouseEvent*);
  void MMoveDelete(QMouseEvent*);
  void MMoveLabel(QMouseEvent*);
  void MMoveMarker(QMouseEvent*);
  void MMoveMirrorY(QMouseEvent*);
  void MMoveMirrorX(QMouseEvent*);
  void MMoveRotate(QMouseEvent*);
  void MMoveActivate(QMouseEvent*);
  void MMoveOnGrid(QMouseEvent*);
  void MMoveResizePainting(QMouseEvent*);
  void (QucsView::*MouseMoveAction) (QMouseEvent*);// current mouse move method

  void MPressSelect(QMouseEvent*);
  void MPressDelete(QMouseEvent*);
  void MPressActivate(QMouseEvent*);
  void MPressMirrorX(QMouseEvent*);
  void MPressMirrorY(QMouseEvent*);
  void MPressRotate(QMouseEvent*);
  void MPressComponent(QMouseEvent*);
  void MPressDiagram(QMouseEvent*);
  void MPressLabel(QMouseEvent*);
  void MPressWire1(QMouseEvent*);
  void MPressWire2(QMouseEvent*);
  void MPressPainting(QMouseEvent*);
  void MPressMarker(QMouseEvent*);
  void MPressOnGrid(QMouseEvent*);
  void (QucsView::*MousePressAction) (QMouseEvent*); // mouse press method

  void MDoubleClickSelect(QMouseEvent*);
  void MDoubleClickWire2(QMouseEvent*);
  void (QucsView::*MouseDoubleClickAction) (QMouseEvent*);

  void MReleaseSelect(QMouseEvent*);
  void MReleaseSelect2(QMouseEvent*);
  void MReleaseActivate(QMouseEvent*);
  void MReleaseMoving(QMouseEvent*);
  void MReleaseResizeDiagram(QMouseEvent*);
  void MReleasePaste(QMouseEvent*);
  void MReleaseResizePainting(QMouseEvent*);
  void (QucsView::*MouseReleaseAction) (QMouseEvent*);

  void MovingElements();
  void endElementMoving();

signals:
//  void CompsSelected(bool);
//  void AreaMouseMove(QMouseEvent *Event);
//  void AreaMousePress(QMouseEvent *Event);

private:
  int MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  bool isMoveEqual;
  Element *focusElement;
  Wire *labeledWire;     // remember the wire whose label is moving
};

#endif
