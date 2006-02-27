/***************************************************************************
                               qucsview.h
                              ------------
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
  bool  pasteElements();
  void  enlargeView(int, int, int, int);
  void  setPainter(QPainter*, QucsDoc*);
  void  eraseCross();
  void  editLabel(WireLabel*);

  Element  *selElem;  // component/diagram/painting selected in IconView

  bool    drawn;  // indicates whether the scheme element was drawn last time
  QString ProjName;
  QLineEdit *editText;  // for edit component properties on schematic

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
  void contentsDropEvent(QDropEvent*);
  void contentsDragEnterEvent(QDragEnterEvent*);
  void contentsDragLeaveEvent(QDragLeaveEvent*);
  void contentsDragMoveEvent(QDragMoveEvent*);

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
  void slotPowerMatching();
  void slot2PortMatching();

public:
  void MMoveSelect(QMouseEvent*);
  void MMoveElement(QMouseEvent*);
  void MMoveWire1(QMouseEvent*);
  void MMoveWire2(QMouseEvent*);
  void MMoveMoving(QMouseEvent*);
  void MMoveMoving2(QMouseEvent*);
  void MMovePaste(QMouseEvent*);
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

  void MPressSelect(QMouseEvent*, QucsDoc*, int, int);
  void MPressDelete(QMouseEvent*, QucsDoc*, int, int);
  void MPressActivate(QMouseEvent*, QucsDoc*, int, int);
  void MPressMirrorX(QMouseEvent*, QucsDoc*, int, int);
  void MPressMirrorY(QMouseEvent*, QucsDoc*, int, int);
  void MPressRotate(QMouseEvent*, QucsDoc*, int, int);
  void MPressElement(QMouseEvent*, QucsDoc*, int, int);
  void MPressLabel(QMouseEvent*, QucsDoc*, int, int);
  void MPressWire1(QMouseEvent*, QucsDoc*, int, int);
  void MPressWire2(QMouseEvent*, QucsDoc*, int, int);
  void MPressPainting(QMouseEvent*, QucsDoc*, int, int);
  void MPressMarker(QMouseEvent*, QucsDoc*, int, int);
  void MPressOnGrid(QMouseEvent*, QucsDoc*, int, int);
  void MPressMoveText(QMouseEvent*, QucsDoc*, int, int);
  void MPressZoomIn(QMouseEvent*, QucsDoc*, int, int);
  void (QucsView::*MousePressAction) (QMouseEvent*, QucsDoc*, int, int);

  void MDoubleClickSelect(QMouseEvent*);
  void MDoubleClickWire2(QMouseEvent*);
  void (QucsView::*MouseDoubleClickAction) (QMouseEvent*);

  void MReleaseSelect(QMouseEvent*, QucsDoc*);
  void MReleaseSelect2(QMouseEvent*, QucsDoc*);
  void MReleaseActivate(QMouseEvent*, QucsDoc*);
  void MReleaseMoving(QMouseEvent*, QucsDoc*);
  void MReleaseResizeDiagram(QMouseEvent*, QucsDoc*);
  void MReleasePaste(QMouseEvent*, QucsDoc*);
  void MReleaseResizePainting(QMouseEvent*, QucsDoc*);
  void MReleaseMoveText(QMouseEvent*, QucsDoc*);
  void MReleaseZoomIn(QMouseEvent*, QucsDoc*);
  void (QucsView::*MouseReleaseAction) (QMouseEvent*, QucsDoc*);

  void MovingElements();
  void endElementMoving();

private:
  void editElement(QMouseEvent*);
  void rightPressMenu(QMouseEvent*, QucsDoc*, int, int);

  int  MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  bool isMoveEqual, dragIsOkay;
  Element *focusElement;
  QMouseEvent *focusMEvent;
  Wire *labeledWire;     // remember the wire whose label is moving
  ViewPainter Painter;
};

#endif
