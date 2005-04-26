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

#include "components/component.h"
#include "qucsdoc.h"
#include "viewpainter.h"
#include "main.h"

#include <qscrollview.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qevent.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <qlineedit.h>


class Wire;
class QWidget;
class QPainter;
class QPopupMenu;


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

  float Zoom(float);
  bool   pasteElements();
  void   enlargeView(int, int, int, int);
  void   setPainter(QPainter*, QucsDoc*);
  void   eraseCross();
  void   editLabel(WireLabel*);

  Component *selComp;   // component selected in IconView
  Diagram   *selDiag;   // diagram selected in IconView
  Painting  *selPaint;  // painting selected in IconView

  bool    drawn;  // indicates whether the scheme element was drawn last time
  QString ProjName;
  QLineEdit *editText;  // for edit component properties

  QPtrList<Element> movingElements;

  // menu appearing by right mouse button click on component
  QPopupMenu *ComponentMenu;

  // -------------------------------------------------------------------
  QPtrList<QucsDoc>  Docs; // document instances (schematics, data displays)

protected:
  void drawContents(QPainter*, int, int, int, int);
  void contentsMouseMoveEvent(QMouseEvent*);
  void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseDoubleClickEvent(QMouseEvent*);
  void contentsMouseReleaseEvent(QMouseEvent*);
  void contentsWheelEvent(QWheelEvent*);

  bool ScrollUp(int);
  bool ScrollDown(int);
  bool ScrollLeft(int);
  bool ScrollRight(int);

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();
  void slotApplyCompText();

public slots:
  void slotCursorLeft();
  void slotCursorRight();
  void slotCursorUp();
  void slotCursorDown();

  void slotHideEdit();
  void slotEditElement();

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
  void MMoveMoveText(QMouseEvent*);
  void MMoveMoveTextB(QMouseEvent*);
  void MMoveZoomIn(QMouseEvent*);
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
  void MPressMoveText(QMouseEvent*);
  void MPressZoomIn(QMouseEvent*);
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
  void MReleaseMoveText(QMouseEvent*);
  void MReleaseZoomIn(QMouseEvent*);
  void (QucsView::*MouseReleaseAction) (QMouseEvent*);

  void MovingElements();
  void endElementMoving();

private:
  void editElement(QMouseEvent*);
  void rightPressMenu(QMouseEvent*);
  void PressLabel(QMouseEvent*);

  int MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  bool isMoveEqual;
  Element *focusElement;
  QMouseEvent *focusMEvent;
  Wire *labeledWire;     // remember the wire whose label is moving
  ViewPainter Painter;

  QRegExp Expression;
  QRegExpValidator *Validator;
};

#endif
