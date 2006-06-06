/***************************************************************************
                              schematic.cpp
                             ---------------
    begin                : Sat Mar 3 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <limits.h>

#include <qimage.h>
#include <qaction.h>
#include <qregexp.h>
#include <qiconset.h>
#include <qprinter.h>
#include <qlineedit.h>
#include <qfileinfo.h>
#include <qiconview.h>
#include <qtabwidget.h>
#include <qdragobject.h>

#include "qucs.h"
#include "schematic.h"
#include "mouseactions.h"
#include "viewpainter.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "main.h"
#include "node.h"

// just dummies for empty lists
QPtrList<Wire>      SymbolWires;
QPtrList<Node>      SymbolNodes;
QPtrList<Diagram>   SymbolDiags;
QPtrList<Component> SymbolComps;


Schematic::Schematic(QucsApp *App_, const QString& Name_)
    : QucsDoc(App_, Name_)
{
  symbolMode = false;

  // ...........................................................
  GridX  = GridY  = 10;
  ViewX1=ViewY1=0;
  ViewX2=ViewY2=800;
  UsedX1 = UsedY1 = INT_MAX;
  UsedX2 = UsedY2 = INT_MIN;

  tmpPosX = tmpPosY = -100;
  tmpUsedX1 = tmpUsedY1 = tmpViewX1 = tmpViewY1 = -200;
  tmpUsedX2 = tmpUsedY2 = tmpViewX2 = tmpViewY2 =  200;
  tmpScale = 1.0;

  DocComps.setAutoDelete(true);
  DocWires.setAutoDelete(true);
  DocNodes.setAutoDelete(true);
  DocDiags.setAutoDelete(true);
  DocPaints.setAutoDelete(true);
  SymbolPaints.setAutoDelete(true);

  UndoStack.setAutoDelete(true);
  UndoSymbol.setAutoDelete(true);
  // The 'i' means state for being unchanged.
  UndoStack.append(new QString(" i\n</>\n</>\n</>\n</>\n"));

  QFileInfo Info(Name_);
  if(App) {
    if(Name_.isEmpty())
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            QObject::tr("untitled"));
    else
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            Info.fileName());

    // calls indirectly "becomeCurrent"
    App->DocumentTab->setCurrentPage(App->DocumentTab->indexOf(this));

    setVScrollBarMode(QScrollView::AlwaysOn);
    setHScrollBarMode(QScrollView::AlwaysOn);
    viewport()->setPaletteBackgroundColor(QucsSettings.BGColor);
    viewport()->setMouseTracking(true);
    viewport()->setAcceptDrops(true);  // enable drag'n drop

    connect(horizontalScrollBar(),
		SIGNAL(prevLine()), SLOT(slotScrollLeft()));
    connect(horizontalScrollBar(),
		SIGNAL(nextLine()), SLOT(slotScrollRight()));
    connect(verticalScrollBar(),
		SIGNAL(prevLine()), SLOT(slotScrollUp()));
    connect(verticalScrollBar(),
		SIGNAL(nextLine()), SLOT(slotScrollDown()));

    // ...........................................................

    // to repair some strange  scrolling artefacts
    connect(this, SIGNAL(horizontalSliderReleased()),
		viewport(), SLOT(update()));
    connect(this, SIGNAL(verticalSliderReleased()),
		viewport(), SLOT(update()));

    // to prevent user from editing something that he doesn't see
    connect(this, SIGNAL(horizontalSliderPressed()), App, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()), App, SLOT(slotHideEdit()));
  } // of "if(App)"
}

Schematic::~Schematic()
{
  if(App) {
    App->editText->reparent(App, 0, QPoint(0, 0));
    App->DocumentTab->removePage(this);  // delete tab in TabBar
  }
}

// ---------------------------------------------------
bool Schematic::createSubcircuitSymbol()
{
  // If the number of ports is not equal, remove or add some.
  unsigned int countPort = adjustPortNumbers();

  // If a symbol does not yet exist, create one.
  if(SymbolPaints.count() != countPort)
    return false;

  int h = 30*((countPort-1)/2) + 10;
  SymbolPaints.prepend(new ID_Text(-20, h+4));

  SymbolPaints.append(
     new GraphicLine(-20, -h, 40,  0, QPen(QPen::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine( 20, -h,  0,2*h, QPen(QPen::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine(-20,  h, 40,  0, QPen(QPen::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine(-20, -h,  0,2*h, QPen(QPen::darkBlue,2)));

  unsigned int i=0, y = 10-h;
  while(i<countPort) {
    i++;
    SymbolPaints.append(
       new GraphicLine(-30, y, 10, 0, QPen(QPen::darkBlue,2)));
    SymbolPaints.at(i)->setCenter(-30,  y);

    if(i == countPort)  break;
    i++;
    SymbolPaints.append(
       new GraphicLine( 20, y, 10, 0, QPen(QPen::darkBlue,2)));
    SymbolPaints.at(i)->setCenter(30,  y);
    y += 60;
  }
  return true;
}

// ---------------------------------------------------
void Schematic::becomeCurrent(bool update)
{
  QString *ps;
  App->printCursorPosition(0, 0);

  if(symbolMode) {
    Nodes = &SymbolNodes;
    Wires = &SymbolWires;
    Diagrams = &SymbolDiags;
    Paintings = &SymbolPaints;
    Components = &SymbolComps;

    // if no symbol yet exists -> create one
    if(createSubcircuitSymbol())
      sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
    
    ps = UndoSymbol.current();
    if(ps != UndoSymbol.getFirst())  App->undo->setEnabled(true);
    else  App->undo->setEnabled(false);
    if(ps != UndoSymbol.getLast())  App->redo->setEnabled(true);
    else  App->redo->setEnabled(false);
  }
  else {
    Nodes = &DocNodes;
    Wires = &DocWires;
    Diagrams = &DocDiags;
    Paintings = &DocPaints;
    Components = &DocComps;

    ps = UndoStack.current();
    if(ps != UndoStack.getFirst())  App->undo->setEnabled(true);
    else  App->undo->setEnabled(false);
    if(ps != UndoStack.getLast())  App->redo->setEnabled(true);
    else  App->redo->setEnabled(false);

    if(update)
      reloadGraphs();   // load recent simulation data
  }
}

// ---------------------------------------------------
void Schematic::setName(const QString& Name_)
{
  DocName = Name_;

  QFileInfo Info(DocName);
  if (App)
    App->DocumentTab->setTabLabel(this, Info.fileName());

  DataSet = Info.baseName()+".dat";
  if(Info.extension(false) != "dpl")
    DataDisplay = Info.baseName()+".dpl";
  else
    DataDisplay = Info.baseName()+".sch";
}

// ---------------------------------------------------
// Sets the document to be changed or not to be changed.
void Schematic::setChanged(bool c, bool fillStack, char Op)
{
  if((!DocChanged) && c)
    App->DocumentTab->setTabIconSet(this, QPixmap(smallsave_xpm));
  else if(DocChanged && (!c))
    App->DocumentTab->setTabIconSet(this, QPixmap(empty_xpm));
  DocChanged = c;

  showBias = -1;   // schematic changed => bias points may be invalid

  if(!fillStack)
    return;


  // ................................................
  if(symbolMode) {  // for symbol edit mode
    QString *Curr = UndoSymbol.current();
    while(Curr != UndoSymbol.last())
      UndoSymbol.remove();   // remove "Redo" items

    UndoSymbol.append(new QString(createSymbolUndoString(Op)));

    if(!App->undo->isEnabled()) App->undo->setEnabled(true);
    if(App->redo->isEnabled())  App->redo->setEnabled(false);

    while(UndoSymbol.count() > QucsSettings.maxUndo) { // "while..." because
      UndoSymbol.removeFirst();    // "maxUndo" could be decreased meanwhile
      UndoSymbol.last();
    }
    return;
  }

  // ................................................
  // for schematic edit mode
  QString *Curr = UndoStack.current();
  while(Curr != UndoStack.last())
    UndoStack.remove();   // remove "Redo" items

  if(Op == 'm')   // only one for move marker
    if(UndoStack.current()->at(0) == Op)
      UndoStack.remove();

  UndoStack.append(new QString(createUndoString(Op)));

  if(!App->undo->isEnabled()) App->undo->setEnabled(true);
  if(App->redo->isEnabled())  App->redo->setEnabled(false);

  while(UndoStack.count() > QucsSettings.maxUndo) { // "while..." because
    UndoStack.removeFirst();    // "maxUndo" could be decreased meanwhile
    UndoStack.last();
  }
}

// -----------------------------------------------------------
// Is called when the content (schematic or data display) has to be drawn.
void Schematic::drawContents(QPainter *p, int, int, int, int)
{
  ViewPainter Painter;
  Painter.init(p, Scale, -ViewX1, -ViewY1, contentsX(), contentsY());

  paintGrid(&Painter, contentsX(), contentsY(),
		visibleWidth(), visibleHeight());

  for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    pc->paint(&Painter);

  for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next()) {
    pw->paint(&Painter);
    if(pw->Label)
      pw->Label->paint(&Painter);  // separate because of paintSelected
  }

  Node *pn;
  for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    pn->paint(&Painter);
    if(pn->Label)
      pn->Label->paint(&Painter);  // separate because of paintSelected
  }

  for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    pd->paint(&Painter);

  for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
    pp->paint(&Painter);

  if(showBias > 0) {  // show DC bias points in schematic ?
    int x, y, z;
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
      if(pn->Name.isEmpty()) continue;
      x = pn->cx;
      y = pn->cy + 4;
      z = pn->x1;
      if(z & 1) x -= Painter.Painter->fontMetrics().width(pn->Name);
      if(!(z & 2)) {
        y -= (Painter.LineSpacing>>1) + 4;
        if(z & 1) x -= 4;
        else x += 4;
      }
      if(z & 0x10)
        Painter.Painter->setPen(QPen::darkGreen);  // green for currents
      else
        Painter.Painter->setPen(QPen::blue);   // blue for voltages
      Painter.drawText(pn->Name, x, y);
    }
  }
}

// ---------------------------------------------------
void Schematic::contentsMouseMoveEvent(QMouseEvent *Event)
{
  if(App->MouseMoveAction)
    (App->view->*(App->MouseMoveAction))(this, Event);
}

// -----------------------------------------------------------
void Schematic::contentsMousePressEvent(QMouseEvent *Event)
{
  App->editText->setHidden(true); // disable text edit of component property
  if(App->MouseReleaseAction == &MouseActions::MReleasePaste)
    return;
  
  int x = int(float(Event->pos().x())/Scale) + ViewX1;
  int y = int(float(Event->pos().y())/Scale) + ViewY1;

  if(Event->button() != Qt::LeftButton) {
    if(App->MousePressAction == &MouseActions::MPressElement)
      App->view->MPressElement(this, Event, x, y);
    else if(App->MousePressAction == &MouseActions::MPressWire2)
      App->view->MPressWire2(this, Event, x, y);
    else {
      App->view->rightPressMenu(this, Event, x, y); // show menu on right mouse button
      if(App->MouseReleaseAction)
        (App->view->*(App->MouseReleaseAction))(this, Event); // not called (menu has focus)
    }
    return;
  }

  if(App->MousePressAction)
    (App->view->*(App->MousePressAction))(this, Event, x, y);
}

// -----------------------------------------------------------
void Schematic::contentsMouseReleaseEvent(QMouseEvent *Event)
{
  if(App->MouseReleaseAction)
    (App->view->*(App->MouseReleaseAction))(this, Event);
}

// -----------------------------------------------------------
void Schematic::contentsMouseDoubleClickEvent(QMouseEvent *Event)
{
  if(App->MouseDoubleClickAction)
    (App->view->*(App->MouseDoubleClickAction))(this, Event);
}

// -----------------------------------------------------------
void Schematic::print(QPrinter *Printer, bool printAll)
{
  QPainter painter(Printer);
  if(!painter.device())   // valid device available ?
    return;

  bool selected;
  ViewPainter p;
  p.init(&painter, 1.0, 0, 0, UsedX1-20, UsedY1-20);

  for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    if(pc->isSelected || printAll) {
      selected = pc->isSelected;
      pc->isSelected = false;
      pc->print(&p);   // paint all selected components
      pc->isSelected = selected;
    }

  for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next()) {
    if(pw->isSelected || printAll) {
      selected = pw->isSelected;
      pw->isSelected = false;
      pw->paint(&p);   // paint all selected wires
      pw->isSelected = selected;
    }
    if(pw->Label)
      if(pw->Label->isSelected || printAll) {
        selected = pw->Label->isSelected;
        pw->Label->isSelected = false;
        pw->Label->paint(&p);
        pw->Label->isSelected = selected;
      }
  }

  Element *pe;
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    for(pe = pn->Connections.first(); pe != 0; pe = pn->Connections.next())
      if(pe->isSelected || printAll) {
	pn->paint(&p); // paint all nodes with selected elements
	break;
      }
    if(pn->Label)
      if(pn->Label->isSelected || printAll) {
        selected = pn->Label->isSelected;
        pn->Label->isSelected = false;
        pn->Label->paint(&p);
        pn->Label->isSelected = selected;
      }
  }

  Graph  *pg;
  Marker *pm;
  for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    if(pd->isSelected || printAll) {
      // if graph or marker is selected, deselect during printing
      for(pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next()) {
	if(pg->isSelected)  pg->Type |= 1;  // remember selection
	pg->isSelected = false;
	for(pm = pg->Markers.first(); pm != 0; pm = pg->Markers.next()) {
	  if(pm->isSelected)  pm->Type |= 1;  // remember selection
	  pm->isSelected = false;
	}
      }

      selected = pd->isSelected;
      pd->isSelected = false;
      pd->paint(&p);  // paint all selected diagrams with graphs and markers
      pd->isSelected = selected;

      // revert selection of graphs and markers
      for(pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next()) {
	if(pg->Type & 1)  pg->isSelected = true;
	pg->Type &= -2;
	for(pm = pg->Markers.first(); pm != 0; pm = pg->Markers.next()) {
	  if(pm->Type & 1)  pm->isSelected = true;
	  pm->Type &= -2;
	}
      }
    }

  for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
    if(pp->isSelected || printAll) {
      selected = pp->isSelected;
      pp->isSelected = false;
      pp->paint(&p);   // paint all selected paintings
      pp->isSelected = selected;
    }
}

// -----------------------------------------------------------
float Schematic::zoom(float s)
{
  Scale *= s;
  if(Scale > 10.0) Scale = 10.0f;
  if(Scale < 0.01) Scale = 0.01f;

  resizeContents(int(Scale*float(ViewX2 - ViewX1)),
                 int(Scale*float(ViewY2 - ViewY1)));
  viewport()->update();
  App->view->drawn = false;
  return Scale;
}

// ---------------------------------------------------
void Schematic::showAll()
{
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(UsedX1 == 0)
    if(UsedX2 == 0)
      if(UsedY1 == 0)
        if(UsedY2 == 0) {
    UsedX1 = UsedY1 = INT_MAX;
    UsedX2 = UsedY2 = INT_MIN;
    return;
  }

  float xScale = float(visibleWidth()) / float(UsedX2-UsedX1+80);
  float yScale = float(visibleHeight()) / float(UsedY2-UsedY1+80);
  if(xScale > yScale) xScale = yScale;
  xScale /= Scale;

  ViewX1 = UsedX1 - 40;
  ViewY1 = UsedY1 - 40;
  ViewX2 = UsedX2 + 40;
  ViewY2 = UsedY2 + 40;
  zoom(xScale);
}

// ---------------------------------------------------
void Schematic::showNoZoom()
{
  Scale = 1.0;

  int x1 = UsedX1;
  int y1 = UsedY1;
  int x2 = UsedX2;
  int y2 = UsedY2;

//  sizeOfAll(x1, y1, x2, y2);
  if(x2==0) if(y2==0) if(x1==0) if(y1==0) x2 = y2 = 800;

  ViewX1 = x1-40;
  ViewY1 = y1-40;
  ViewX2 = x2+40;
  ViewY2 = y2+40;
  resizeContents(x2-x1+80, y2-y1+80);
  viewport()->update();
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceed the
// visible area.
void Schematic::enlargeView(int x1, int y1, int x2, int y2)
{
  int dx=0, dy=0;
  if(x1 < UsedX1) UsedX1 = x1;
  if(y1 < UsedY1) UsedY1 = y1;
  if(x2 > UsedX2) UsedX2 = x2;
  if(y2 > UsedY2) UsedY2 = y2;

  if(x1 < ViewX1) {
    dx = int(Scale * float(ViewX1-x1+40));
    ViewX1 = x1-40;
  }
  if(y1 < ViewY1) {
    dy = int(Scale * float(ViewY1-y1+40));
    ViewY1 = y1-40;
  }
  if(x2 > ViewX2) ViewX2 = x2+40;
  if(y2 > ViewY2) ViewY2 = y2+40;

  resizeContents(int(Scale*float(ViewX2 - ViewX1)),
		int(Scale*float(ViewY2 - ViewY1)));
  scrollBy(dx,dy);
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
void Schematic::setOnGrid(int& x, int& y)
{
  if(x<0) x -= (GridX >> 1) - 1;
  else x += GridX >> 1;
  x -= x % GridX;

  if(y<0) y -= (GridY >> 1) - 1;
  else y += GridY >> 1;
  y -= y % GridY;
}

// ---------------------------------------------------
void Schematic::paintGrid(ViewPainter *p, int cX, int cY, int Width, int Height)
{
  if(!GridOn) return;

  p->Painter->setPen(QPen(QPen::black,1));
  int dx = -int(Scale*float(ViewX1)) - cX;
  int dy = -int(Scale*float(ViewY1)) - cY;
  p->Painter->drawLine(-3+dx, dy, 4+dx, dy); // small cross at origin
  p->Painter->drawLine( dx,-3+dy, dx, 4+dy);


  int x1  = int(float(cX)/Scale) + ViewX1;
  int y1  = int(float(cY)/Scale) + ViewY1;

  // setOnGrid(x1, y1) for 2*Grid
  if(x1<0) x1 -= GridX - 1;
  else x1 += GridX;
  x1 -= x1 % (GridX << 1);

  if(y1<0) y1 -= GridY - 1;
  else y1 += GridY;
  y1 -= y1 % (GridY << 1);

  float X, Y, Y0, DX, DY;
  X = float(x1)*Scale + p->DX;
  Y = Y0 = float(y1)*Scale + p->DY;
  x1 = X > 0.0 ? int(X + 0.5) : int(X - 0.5);
  y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);


  int xEnd = x1 + Width;
  int yEnd = y1 + Height;
  DX = float(GridX << 1) * Scale;   // every second grid a point
  DY = float(GridY << 1) * Scale;
  while(DX <= 8.0)  DX *= 1.5;  // if too narrow, every third grid a point
  while(DY <= 8.0)  DY *= 1.5;  // if too narrow, every third grid a point

  while(x1 < xEnd) {
    Y = Y0;
    y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);
    while(y1 < yEnd) {
      p->Painter->drawPoint(x1, y1);    // paint grid
      Y += DY;
      y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);
    }
    X += DX;
    x1 = X > 0.0 ? int(X + 0.5) : int(X - 0.5);
  }
}

// ---------------------------------------------------
// Correction factor for unproportional font scaling.
float Schematic::textCorr()
{
  QFont Font = QucsSettings.font;
  Font.setPointSizeFloat( Scale * float(Font.pointSize()) );
  QFontMetrics  metrics(Font);
  return (Scale / float(metrics.lineSpacing()));
}

// ---------------------------------------------------
void Schematic::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax)
{
  xmin=INT_MAX;
  ymin=INT_MAX;
  xmax=INT_MIN;
  ymax=INT_MIN;
  Component *pc;
  Diagram *pd;
  Wire *pw;
  WireLabel *pl;
  Painting *pp;

  if(Components->isEmpty())
    if(Wires->isEmpty())
      if(Diagrams->isEmpty())
        if(Paintings->isEmpty()) {
          xmin = xmax = 0;
          ymin = ymax = 0;
          return;
        }


  float Corr = textCorr();
  int x1, y1, x2, y2;
  // find boundings of all components
  for(pc = Components->first(); pc != 0; pc = Components->next()) {
    pc->entireBounds(x1, y1, x2, y2, Corr);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  // find boundings of all wires
  for(pw = Wires->first(); pw != 0; pw = Wires->next()) {
    if(pw->x1 < xmin) xmin = pw->x1;
    if(pw->x2 > xmax) xmax = pw->x2;
    if(pw->y1 < ymin) ymin = pw->y1;
    if(pw->y2 > ymax) ymax = pw->y2;

    pl = pw->Label;
    if(pl) {     // check position of wire label
      if(pl->x1 < xmin)  xmin = pl->x1;
      if((pl->x1+pl->x2) > xmax)  xmax = pl->x1 + pl->x2;
      if(pl->y1 > ymax)  ymax = pl->y1;
      if((pl->y1-pl->y2) < ymin)  ymin = pl->y1 - pl->y2;
    }
  }

  // find boundings of all node labels
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    pl = pn->Label;
    if(pl) {     // check position of node label
      if(pl->x1 < xmin)  xmin = pl->x1;
      if((pl->x1+pl->x2) > xmax)  xmax = pl->x1 + pl->x2;
      if(pl->y1 > ymax)  ymax = pl->y1;
      if((pl->y1-pl->y2) < ymin)  ymin = pl->y1 - pl->y2;
    }
  }

  // find boundings of all diagrams
  for(pd = Diagrams->first(); pd != 0; pd = Diagrams->next()) {
    pd->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;

    for(Graph *pg = pd->Graphs.first(); pg!=0; pg = pd->Graphs.next())
      // test all markers of diagram
      for(Marker *pm = pg->Markers.first(); pm!=0; pm = pg->Markers.next()) {
        pm->Bounding(x1, y1, x2, y2);
        if(x1 < xmin) xmin = x1;
        if(x2 > xmax) xmax = x2;
        if(y1 < ymin) ymin = y1;
        if(y2 > ymax) ymax = y2;
      }
  }

  // find boundings of all Paintings
  for(pp = Paintings->first(); pp != 0; pp = Paintings->next()) {
    pp->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
}

// ---------------------------------------------------
// Rotates all selected components around their midpoint.
bool Schematic::rotateElements()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1=INT_MAX, y1=INT_MAX;
  int x2=INT_MIN, y2=INT_MIN;
  QPtrList<Element> ElementCache;
  copyLabels(x1, y1, x2, y2, &ElementCache);   // must be first of all !
  copyComponents(x1, y1, x2, y2, &ElementCache);
  copyWires(x1, y1, x2, y2, &ElementCache);
  copyPaintings(x1, y1, x2, y2, &ElementCache);
  if(y1 == INT_MAX) return false;   // no element selected

  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // center for rotation
  y1 = (y1+y2) >> 1;
  setOnGrid(x1, y1);


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  for(Element *pe = ElementCache.first(); pe != 0; pe = ElementCache.next())
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->rotate();   //rotate component !before! rotating its center
        pc->setCenter(pc->cy - y1 + x1, x1 - pc->cx + y1);
        insertRawComponent(pc);
        break;

      case isWire:
        pw = (Wire*)pe;
        x2 = pw->x1;
        pw->x1 = pw->y1 - y1 + x1;
        pw->y1 = x1 - x2 + y1;
        x2 = pw->x2;
        pw->x2 = pw->y2 - y1 + x1;
        pw->y2 = x1 - x2 + y1;
        pl = pw->Label;
        if(pl) {
          x2 = pl->cx;
          pl->cx = pl->cy - y1 + x1;
          pl->cy = x1 - x2 + y1;
          if(pl->Type == isHWireLabel)
            pl->Type = isVWireLabel;
          else pl->Type = isHWireLabel;
        }
        insertWire(pw);
        break;

      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	x2 = pl->x1;
	pl->x1 = pl->y1 - y1 + x1;
	pl->y1 = x1 - x2 + y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0) {
	  x2 = pl->x1;
	  pl->x1 = pl->y1 - y1 + x1;
	  pl->y1 = x1 - x2 + y1;
	}
	x2 = pl->cx;
	pl->cx = pl->cy - y1 + x1;
	pl->cy = x1 - x2 + y1;
	insertNodeLabel(pl);
	break;

      case isPainting:
        pp = (Painting*)pe;
        pp->rotate();   // rotate painting !before! rotating its center
        pp->getCenter(x2, y2);
        pp->setCenter(y2-y1 + x1, x1-x2 + y1);
        Paintings->append(pp);
        break;
      default: ;
    }

  ElementCache.clear();

  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components.
// First copy them to 'ElementCache', then mirror and insert again.
bool Schematic::mirrorXComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QPtrList<Element> ElementCache;
  if(!copyComps2WiresPaints(x1, y1, x2, y2, &ElementCache))
    return false;
  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  y1 = (y1+y2) >> 1;   // axis for mirroring
  setOnGrid(y2, y1);
  y1 <<= 1;


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  for(Element *pe = ElementCache.first(); pe != 0; pe = ElementCache.next())
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
	pc = (Component*)pe;
	pc->mirrorX();   // mirror component !before! mirroring its center
	pc->setCenter(pc->cx, y1 - pc->cy);
	insertRawComponent(pc);
	break;
      case isWire:
	pw = (Wire*)pe;
	pw->y1 = y1 - pw->y1;
	pw->y2 = y1 - pw->y2;
	pl = pw->Label;
	if(pl)  pl->cy = y1 - pl->cy;
	insertWire(pw);
	break;
      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	pl->y1 = y1 - pl->y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0)
	  pl->y1 = y1 - pl->y1;
	pl->cy = y1 - pl->cy;
	insertNodeLabel(pl);
	break;
      case isPainting:
	pp = (Painting*)pe;
	pp->getCenter(x2, y2);
	pp->mirrorX();   // mirror painting !before! mirroring its center
	pp->setCenter(x2, y1 - y2);
	Paintings->append(pp);
	break;
      default: ;
    }

  ElementCache.clear();
  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components. First copy them to 'ElementCache', then mirror and insert again.
bool Schematic::mirrorYComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QPtrList<Element> ElementCache;
  if(!copyComps2WiresPaints(x1, y1, x2, y2, &ElementCache))
    return false;
  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // axis for mirroring
  setOnGrid(x1, x2);
  x1 <<= 1;

  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  for(Element *pe = ElementCache.first(); pe != 0; pe = ElementCache.next())
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->mirrorY();   // mirror component !before! mirroring its center
        pc->setCenter(x1 - pc->cx, pc->cy);
        insertRawComponent(pc);
        break;
      case isWire:
        pw = (Wire*)pe;
        pw->x1 = x1 - pw->x1;
        pw->x2 = x1 - pw->x2;
        pl = pw->Label;
        if(pl)  pl->cx = x1 - pl->cx;
        insertWire(pw);
        break;
      case isHWireLabel:
      case isVWireLabel:
        pl = (WireLabel*)pe;
        pl->x1 = x1 - pl->x1;
        break;
      case isNodeLabel:
        pl = (WireLabel*)pe;
        if(pl->pOwner == 0)
          pl->x1 = x1 - pl->x1;
        pl->cx = x1 - pl->cx;
        insertNodeLabel(pl);
        break;
      case isPainting:
        pp = (Painting*)pe;
        pp->getCenter(x2, y2);
        pp->mirrorY();   // mirror painting !before! mirroring its center
        pp->setCenter(x1 - x2, y2);
        Paintings->append(pp);
        break;
      default: ;
    }

  ElementCache.clear();
  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Updates the graph data of all diagrams (load from data files).
void Schematic::reloadGraphs()
{
  QFileInfo Info(DocName);
  for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    pd->loadGraphData(Info.dirPath()+QDir::separator()+DataSet);
}

// ---------------------------------------------------
// Performs copy or cut functions for clipboard.
QString Schematic::copySelected(bool cut)
{
  QString s = createClipboardFile();
  if(cut) deleteElements();   // delete selected elements if wanted
  return s;
}

// ---------------------------------------------------
// Performs paste function from clipboard
bool Schematic::paste(QTextStream *stream, QPtrList<Element> *pe)
{
  return pasteFromClipboard(stream, pe);
}

// ---------------------------------------------------
// Loads this Qucs document.
bool Schematic::load()
{
  DocComps.clear();
  DocWires.clear();
  DocNodes.clear();
  DocDiags.clear();
  DocPaints.clear();
  SymbolPaints.clear();

  if(!loadDocument()) return false;
  lastSaved = QDateTime::currentDateTime();
  UndoStack.clear();
  setChanged(false, true); // "not changed" state, but put on undo stack
  UndoStack.current()->at(1) = 'i';  // state of being unchanged

  // The undo stack of the circuit symbol is initialized when first
  // entering its edit mode.

  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(ViewX1 > UsedX1)  ViewX1 = UsedX1;
  if(ViewY1 > UsedY1)  ViewY1 = UsedY1;
  if(ViewX2 > UsedX2)  ViewX2 = UsedX2;
  if(ViewY2 > UsedY2)  ViewY2 = UsedY2;
  zoom(1.0f);
  return true;
}

// ---------------------------------------------------
// Saves this Qucs document. Returns the number of subcircuit ports.
int Schematic::save()
{
  int result = adjustPortNumbers();// same port number for schematic and symbol
  saveDocument();
  lastSaved = QDateTime::currentDateTime();
  if(result >= 0) {
    setChanged(false);
    QString *p, *ps = UndoStack.current();
    for(p = UndoStack.first(); p != 0; p = UndoStack.next())
      p->at(1) = ' ';  // state of being changed
    ps->at(1) = 'i';   // state of being unchanged
    UndoStack.findRef(ps);  // back to current

    ps = UndoSymbol.current();
    if(!ps)  return result;
    for(p = UndoSymbol.first(); p != 0; p = UndoSymbol.next())
      p->at(1) = ' ';  // state of being changed
    ps->at(1) = 'i';   // state of being unchanged
    UndoSymbol.findRef(ps);  // back to current
  }
  return result;
}

// ---------------------------------------------------
// If the port number of the schematic and of the symbol are not
// equal add or remove some in the symbol.
int Schematic::adjustPortNumbers()
{
  int x1, x2, y1, y2;
  // get size of whole symbol to know where to place new ports
  if(symbolMode)  sizeOfAll(x1, y1, x2, y2);
  else {
    Components = &SymbolComps;
    Wires      = &SymbolWires;
    Nodes      = &SymbolNodes;
    Diagrams   = &SymbolDiags;
    Paintings  = &SymbolPaints;
    sizeOfAll(x1, y1, x2, y2);
    Components = &DocComps;
    Wires      = &DocWires;
    Nodes      = &DocNodes;
    Diagrams   = &DocDiags;
    Paintings  = &DocPaints;
  }
  x1 += 40;
  y2 += 20;
  setOnGrid(x1, y2);


  Painting *pp;
  // delete all port names in symbol
  for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
    if(pp->Name == ".PortSym ")
      ((PortSymbol*)pp)->nameStr = "";

  QString Str;
  int countPort = 0;
  for(Component *pc = DocComps.first(); pc!=0; pc = DocComps.next())
    if(pc->Model == "Port") {
      countPort++;

      Str = pc->Props.getFirst()->Value;
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
        if(pp->Name == ".PortSym ")
          if(((PortSymbol*)pp)->numberStr == Str)  break;

      if(pp)  ((PortSymbol*)pp)->nameStr = pc->Name;
      else {
        SymbolPaints.append(new PortSymbol(x1, y2, Str, pc->Name));
        y2 += 40;
      }
    }


  // delete not accounted port symbols
  for(pp = SymbolPaints.first(); pp!=0; ) {
    if(pp->Name == ".PortSym ")
      if(((PortSymbol*)pp)->nameStr.isEmpty()) {
        SymbolPaints.remove();
        pp = SymbolPaints.current();
        continue;
      }
    pp = SymbolPaints.next();
  }

  return countPort;
}

// ---------------------------------------------------
bool Schematic::undo()
{
  if(symbolMode) {
    if(UndoSymbol.current() == UndoSymbol.getFirst())  return false;

    rebuildSymbol(UndoSymbol.prev());
    adjustPortNumbers();  // set port names

    QString *ps = UndoSymbol.current();
    if(ps != UndoSymbol.getFirst())  App->undo->setEnabled(true);
    else  App->undo->setEnabled(false);
    if(ps != UndoSymbol.getLast())  App->redo->setEnabled(true);
    else  App->redo->setEnabled(false);

    if(ps->at(1) == 'i')
      if(UndoStack.current()->at(1) == 'i') {
	setChanged(false, false);
	return true;
      }

    setChanged(true, false);
    return true;
  }


  // ...... for schematic edit mode .......
  if(UndoStack.current() == UndoStack.getFirst())  return false;

  rebuild(UndoStack.prev());
  reloadGraphs();  // load recent simulation data

  QString *ps = UndoStack.current();
  if(ps != UndoStack.getFirst())  App->undo->setEnabled(true);
  else  App->undo->setEnabled(false);
  if(ps != UndoStack.getLast())  App->redo->setEnabled(true);
  else  App->redo->setEnabled(false);

  if(ps->at(1) == 'i')
    if(UndoSymbol.isEmpty()) {
      setChanged(false, false);
      return true;
    }
    else if(UndoSymbol.current()->at(1) == 'i') {
      setChanged(false, false);
      return true;
    }

  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
bool Schematic::redo()
{
  if(symbolMode) {
    if(UndoSymbol.current() == UndoSymbol.getLast())  return false;

    rebuildSymbol(UndoSymbol.next());
    adjustPortNumbers();  // set port names

    QString *ps = UndoSymbol.current();
    if(ps != UndoSymbol.getFirst())  App->undo->setEnabled(true);
    else  App->undo->setEnabled(false);
    if(ps != UndoSymbol.getLast())  App->redo->setEnabled(true);
    else  App->redo->setEnabled(false);

    if(ps->at(1) == 'i')
      if(UndoStack.current()->at(1) == 'i') {
	setChanged(false, false);
	return true;
      }

    setChanged(true, false);
    return true;
  }


  // ...... for schematic edit mode .......
  if(UndoStack.current() == UndoStack.getLast())  return false;

  rebuild(UndoStack.next());
  reloadGraphs();  // load recent simulation data

  QString *ps = UndoStack.current();
  if(ps != UndoStack.getFirst())  App->undo->setEnabled(true);
  else  App->undo->setEnabled(false);
  if(ps != UndoStack.getLast())  App->redo->setEnabled(true);
  else  App->redo->setEnabled(false);

  if(ps->at(1) == 'i')
    if(UndoSymbol.isEmpty()) {
      setChanged(false, false);
      return true;
    }
    else if(UndoSymbol.current()->at(1) == 'i') {
      setChanged(false, false);
      return true;
    }

  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
// Sets selected elements on grid.
bool Schematic::elementsOnGrid()
{
  int x, y, No;
  bool count = false;
  Port *pp;
  WireLabel *pl, *pLabel;
  QPtrList<WireLabel> LabelCache;

  // test all components
  Components->setAutoDelete(false);
  for(Component *pc = Components->last(); pc != 0; pc = Components->prev())
    if(pc->isSelected) {

      // rescue non-selected node labels
      for(pp = pc->Ports.first(); pp != 0; pp = pc->Ports.next())
        if(pp->Connection->Label)
          if(pp->Connection->Connections.count() < 2) {
            LabelCache.append(pp->Connection->Label);
            pp->Connection->Label->pOwner = 0;
            pp->Connection->Label = 0;
          }

      x = pc->cx;
      y = pc->cy;
      No = Components->at();
      deleteComp(pc);
      setOnGrid(pc->cx, pc->cy);
      insertRawComponent(pc);
      Components->at(No);   // restore current list position
      pc->isSelected = false;
      count = true;

      x -= pc->cx;
      y -= pc->cy;    // re-insert node labels and correct position
      for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next()) {
        pl->cx -= x;
        pl->cy -= y;
        insertNodeLabel(pl);
      }
      LabelCache.clear();
    }
  Components->setAutoDelete(true);

  Wires->setAutoDelete(false);
  // test all wires and wire labels
  for(Wire *pw = Wires->last(); pw != 0; pw = Wires->prev()) {
    pl = pw->Label;
    pw->Label = 0;

    if(pw->isSelected) {
      // rescue non-selected node label
      pLabel = 0;
      if(pw->Port1->Label) {
        if(pw->Port1->Connections.count() < 2) {
            pLabel = pw->Port1->Label;
            pw->Port1->Label = 0;
        }
      }
      else if(pw->Port2->Label) {
        if(pw->Port2->Connections.count() < 2) {
            pLabel = pw->Port2->Label;
            pw->Port2->Label = 0;
        }
      }

      No = Wires->at();
      deleteWire(pw);
      setOnGrid(pw->x1, pw->y1);
      setOnGrid(pw->x2, pw->y2);
      insertWire(pw);
      Wires->at(No);   // restore current list position
      pw->isSelected = false;
      count = true;
      if(pl)
        setOnGrid(pl->cx, pl->cy);

      if(pLabel) {
        setOnGrid(pLabel->cx, pLabel->cy);
        insertNodeLabel(pLabel);
      }
    }

    if(pl) {
      pw->Label = pl;
      if(pl->isSelected) {
        setOnGrid(pl->x1, pl->y1);
        pl->isSelected = false;
        count = true;
      }
    }
  }
  Wires->setAutoDelete(true);

  // test all node labels
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label)
      if(pn->Label->isSelected) {
        setOnGrid(pn->Label->x1, pn->Label->y1);
        pn->Label->isSelected = false;
        count = true;
      }

  // test all diagrams
  for(Diagram *pd = Diagrams->last(); pd != 0; pd = Diagrams->prev()) {
    if(pd->isSelected) {
      setOnGrid(pd->cx, pd->cy);
      pd->isSelected = false;
      count = true;
    }

    for(Graph *pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next())
      // test markers of diagram
      for(Marker *pm = pg->Markers.first(); pm != 0; pm = pg->Markers.next())
        if(pm->isSelected) {
	  x = pm->x1 + pd->cx;
	  y = pm->y1 + pd->cy;
	  setOnGrid(x, y);
	  pm->x1 = x - pd->cx;
	  pm->y1 = y - pd->cy;
	  pm->isSelected = false;
	  count = true;
        }
  }

  // test all paintings
  for(Painting *pa = Paintings->last(); pa != 0; pa = Paintings->prev())
    if(pa->isSelected) {
      setOnGrid(pa->cx, pa->cy);
      pa->isSelected = false;
      count = true;
    }

  if(count) setChanged(true, true);
  return count;
}

// ---------------------------------------------------
void Schematic::switchPaintMode()
{
  symbolMode = !symbolMode;  // change mode

  int tmp, t2;
  float temp;
  temp = Scale; Scale  = tmpScale;  tmpScale  = temp;
  tmp = contentsX();
  t2  = contentsY();
  setContentsPos(tmpPosX, tmpPosY);
  tmpPosX = tmp;
  tmpPosY = t2;
  tmp = ViewX1; ViewX1 = tmpViewX1; tmpViewX1 = tmp;
  tmp = ViewY1; ViewY1 = tmpViewY1; tmpViewY1 = tmp;
  tmp = ViewX2; ViewX2 = tmpViewX2; tmpViewX2 = tmp;
  tmp = ViewY2; ViewY2 = tmpViewY2; tmpViewY2 = tmp;
  tmp = UsedX1; UsedX1 = tmpUsedX1; tmpUsedX1 = tmp;
  tmp = UsedY1; UsedY1 = tmpUsedY1; tmpUsedY1 = tmp;
  tmp = UsedX2; UsedX2 = tmpUsedX2; tmpUsedX2 = tmp;
  tmp = UsedY2; UsedY2 = tmpUsedY2; tmpUsedY2 = tmp;
}


// *********************************************************************
// **********                                                 **********
// **********      Function for serving mouse wheel moving    **********
// **********                                                 **********
// *********************************************************************
void Schematic::contentsWheelEvent(QWheelEvent *Event)
{
  App->editText->setHidden(true);  // disable edit of component property
  int delta = Event->delta() >> 1;     // use smaller steps

  // ...................................................................
  if((Event->state() & Qt::ShiftButton) ||
     (Event->orientation() == Horizontal)) { // scroll horizontally ?
      if(delta > 0) { if(scrollLeft(delta)) scrollBy(-delta, 0); }
      else { if(scrollRight(delta)) scrollBy(-delta, 0); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }
  // ...................................................................
  else if(Event->state() & Qt::ControlButton) {  // use mouse wheel to zoom ?
      float Scaling;
      if(delta < 0) Scaling = float(-delta)/60.0/1.1;
      else Scaling = 1.1*60.0/float(delta);
      Scaling = zoom(Scaling);
//      center(Event->x(), Event->y());
  }
  // ...................................................................
  else {     // scroll vertically !
      if(delta > 0) { if(scrollUp(delta)) scrollBy(0, -delta); }
      else { if(scrollDown(delta)) scrollBy(0, -delta); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }

  Event->accept();   // QScrollView must not handle this event
}

// -----------------------------------------------------------
// Scrolls the visible area upwards and enlarges or reduces the view
// area accordingly.
bool Schematic::scrollUp(int step)
{
  int diff;

  diff = contentsY() - step;
  if(diff < 0) {     // scroll outside the active area ?  (upwards)
    resizeContents(contentsWidth(), contentsHeight()-diff);
    ViewY1 += diff;
    scrollBy(0, diff);
    return false;
  }

  diff = ViewY2 - UsedY2 - 20;    // keep border of 20
  if(diff > 0) {      // make active area smaller ?
    if(step < diff) diff = step;
    resizeContents(contentsWidth(), contentsHeight()-diff);
    ViewY2 -= diff;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area downwards and enlarges or reduces the view
// area accordingly. ("step" must be negative!)
bool Schematic::scrollDown(int step)
{
  int diff;

  diff = contentsHeight() - contentsY()-visibleHeight() + step;
  if(diff < 0) {     // scroll outside the active area ?  (downwards)
    resizeContents(contentsWidth(), contentsHeight()-diff);
    ViewY2 -= diff;
    scrollBy(0, -step);
    return false;
  }

  diff = ViewY1 - UsedY1 + 20;    // keep border of 20
  if(diff < 0) {      // make active area smaller ?
    if(step > diff) diff = step;
    resizeContents(contentsWidth(), contentsHeight()+diff);
    ViewY1 -= diff;
    return false;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the left and enlarges or reduces the view
// area accordingly.
bool Schematic::scrollLeft(int step)
{
  int diff;

  diff = contentsX() - step;
  if(diff < 0) {     // scroll outside the active area ?  (to the left)
    resizeContents(contentsWidth()-diff, contentsHeight());
    ViewX1 += diff;
    scrollBy(diff, 0);
    return false;
  }

  diff = ViewX2 - UsedX2 - 20;    // keep border of 20
  if(diff > 0) {      // make active area smaller ?
    if(step < diff) diff = step;
    resizeContents(contentsWidth()-diff, contentsHeight());
    ViewX2 -= diff;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the right and enlarges or reduces the
// view area accordingly. ("step" must be negative!)
bool Schematic::scrollRight(int step)
{
  int diff;

  diff = contentsWidth() - contentsX()-visibleWidth() + step;
  if(diff < 0) {     // scroll outside the active area ?  (to the right)
    resizeContents(contentsWidth()-diff, contentsHeight());
    ViewX2 -= diff;
    scrollBy(-step, 0);
    return false;
  }

  diff = ViewX1 - UsedX1 + 20;    // keep border of 20
  if(diff < 0) {      // make active area smaller ?
    if(step > diff) diff = step;
    resizeContents(contentsWidth()+diff, contentsHeight());
    ViewX1 -= diff;
    return false;
  }

  return true;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollUp()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollUp(verticalScrollBar()->lineStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollDown()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollDown(-verticalScrollBar()->lineStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollLeft()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollLeft(horizontalScrollBar()->lineStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollRight()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollRight(-horizontalScrollBar()->lineStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}


// *********************************************************************
// **********                                                 **********
// **********        Function for serving drag'n drop         **********
// **********                                                 **********
// *********************************************************************

// Is called if an object is dropped (after drag'n drop).
void Schematic::contentsDropEvent(QDropEvent *Event)
{
  if(dragIsOkay) {
    QStrList List;
    QUriDrag::decode(Event, List);

    // URI:  file:/home/linuxuser/Desktop/example.sch
    for(unsigned int i=0; i < List.count(); i++)
      App->gotoPage(QString(List.at(i)).section(':',1));

    return;
  }


  QMouseEvent e(QEvent::MouseButtonPress, Event->pos(),
                Qt::LeftButton, Qt::NoButton);
  int x = int(Event->pos().x()/Scale) + ViewX1;
  int y = int(Event->pos().y()/Scale) + ViewY1;

  App->view->MPressElement(this, &e, x, y);

  if(App->view->selElem) delete App->view->selElem;
  App->view->selElem = 0;  // no component selected

  if(App->view->labeledWire)
    ((QAction*)App->view->labeledWire)->setOn(true);  // restore old action
}

// ---------------------------------------------------
void Schematic::contentsDragEnterEvent(QDragEnterEvent *Event)
{
  dragIsOkay = false;
  if(Event->provides("text/uri-list"))  // file dragged in ?
    if(QUriDrag::canDecode(Event)) {
      dragIsOkay = true;
      Event->accept();
      return;
    }


  if(Event->format(1) == 0)   // only one MIME type ?
    if(Event->provides("application/x-qiconlist")) {
      QIconViewItem *Item = App->CompComps->currentItem();
      if(Item) {
	App->view->labeledWire = (Wire*)App->activeAction; // misuse variable
	App->slotSelectComponent(Item);  // also sets drawn=false
	App->MouseMoveAction = 0;
	App->MousePressAction = 0;

	Event->accept();
	return;
      }
    }

  Event->ignore();
}

// ---------------------------------------------------
void Schematic::contentsDragLeaveEvent(QDragLeaveEvent*)
{
  if(App->view->selElem)
    if(App->view->selElem->Type & isComponent)
      if(App->view->drawn) {

        QPainter painter(viewport());
        App->view->setPainter(this, &painter);
        ((Component*)App->view->selElem)->paintScheme(&painter);
        App->view->drawn = false;
      }

  if(App->view->labeledWire)
    ((QAction*)App->view->labeledWire)->setOn(true);  // restore old action
}

// ---------------------------------------------------
void Schematic::contentsDragMoveEvent(QDragMoveEvent *Event)
{
  if(!dragIsOkay) {
    if(App->view->selElem == 0) {
      Event->ignore();
      return;
    }

    QMouseEvent e(QEvent::MouseMove, Event->pos(), Qt::NoButton, Qt::NoButton);
    App->view->MMoveElement(this, &e);
  }

  Event->accept();
}
