/*
 * schematicview.cpp - implement schematic view
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2019, Guilherme Brondani Torri, guitorri@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <limits.h>

#include <QFileInfo>
#include <QMimeData>
#include <QPrinter>
#include <QPaintDevice>
#include <QDir>
#include <QTextStream>
#include <QDragLeaveEvent>
#include "qt_compat.h"
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QAction>
#include <QLineEdit>
#include <QUrl>
#include <QListWidget>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QScrollBar>
#include <QUndoStack>

#include "qucs.h"
#include "schematicview.h"
#include "schematicscene.h"
#include "node.h"
#include "textdoc.h"
#include "mouseactions.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/vafile.h"
#include "misc.h"

// just dummies for empty lists
WireList      SymbolWires;
NodeList      SymbolNodes;
DiagramList   SymbolDiags;
ComponentList SymbolComps;


SchematicView::SchematicView(QucsApp *App_, const QString& Name_)
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

  isVerilog = false;
  creatingLib = false;

  panMode= false;

  // create and set current scene
  scene = new SchematicScene(this);
  this->setScene(scene);

  // to be able to scroll, scene must be larger than view
  /// \todo for the intinite schematic, scene mulst alwas be larger than view
  /// when items are added on the border, increase size of scene/view
  /// move to constructor
  setSceneRect(-2000, -2000, 4000, 4000);


  // add a Frame to the schematic
  schematicFrame =  new Frame();
  /// \todo Frame should be part of the drawBackground (?)
  /// Having it as an item messes up the item count.
  //scene->addItem(schematicFrame);


  // add a MouseCursor to the scene
  mouseCursor = new MouseCursor();
  /// \todo MouseCursor should return a QCursor from a pixmap.
  /// Otherwise, with a QGraphicsItem it does not update fast enought.
  //scene->addItem(mouseCursor);

  undoStack = new QUndoStack(this);

  setDragMode(QGraphicsView::RubberBandDrag);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setAcceptDrops(true);
  setViewportUpdateMode(SmartViewportUpdate);
  setTransformationAnchor(QGraphicsView::NoAnchor);
  setMouseTracking(true);

  misc::setWidgetBackgroundColor(viewport(), QucsSettings.BGColor);

  TODO("Repair scroll connect");
  // to repair some strange  scrolling artefacts
  /** \todo
  connect(this, SIGNAL(horizontalSliderReleased()),
      viewport(), SLOT(update()));
  connect(this, SIGNAL(verticalSliderReleased()),
      viewport(), SLOT(update()));
  */

  if (App_) {

    connect(this, SIGNAL(signalCursorPosChanged(int, int)),
        App_, SLOT(printCursorPosition(int, int)));
    /** \todo
    connect(this, SIGNAL(horizontalSliderPressed()), 
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()),
        App_, SLOT(slotHideEdit()));
    */
    connect(this, SIGNAL(signalUndoState(bool)),
        App_, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        App_, SLOT(slotUpdateRedo(bool)));
    connect(this, SIGNAL(signalFileChanged(bool)),
        App_, SLOT(slotFileChanged(bool)));
  }
}

SchematicView::~SchematicView()
{
}

// ---------------------------------------------------
bool SchematicView::createSubcircuitSymbol()
{
  // If the number of ports is not equal, remove or add some.
  unsigned int countPort = adjustPortNumbers();

  // If a symbol does not yet exist, create one.
  if(SymbolPaints.count() != countPort)
    return false;

  int h = 30*((countPort-1)/2) + 10;
  SymbolPaints.prepend(new ID_Text(-20, h+4));

  SymbolPaints.append(
     new GraphicLine(-20, -h, 40,  0, QPen(Qt::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine( 20, -h,  0,2*h, QPen(Qt::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine(-20,  h, 40,  0, QPen(Qt::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine(-20, -h,  0,2*h, QPen(Qt::darkBlue,2)));

  unsigned int i=0, y = 10-h;
  while(i<countPort) {
    i++;
    SymbolPaints.append(
       new GraphicLine(-30, y, 10, 0, QPen(Qt::darkBlue,2)));
    SymbolPaints.at(i)->setCenter(-30,  y);

    if(i == countPort)  break;
    i++;
    SymbolPaints.append(
       new GraphicLine( 20, y, 10, 0, QPen(Qt::darkBlue,2)));
    SymbolPaints.at(i)->setCenter(30,  y);
    y += 60;
  }
  return true;
}

// ---------------------------------------------------
void SchematicView::becomeCurrent(bool update)
{
  emit signalCursorPosChanged(0, 0);

  // update appropriate menu entry
  if (symbolMode) {
    if (DocName.right(4) == ".sym") {
      App->symEdit->setText(tr("Edit Text"));
      App->symEdit->setStatusTip(tr("Edits the Text"));
      App->symEdit->setWhatsThis(tr("Edit Text\n\nEdits the text file"));
    }
    else {
      App->symEdit->setText(tr("Edit Schematic"));
      App->symEdit->setStatusTip(tr("Edits the schematic"));
      App->symEdit->setWhatsThis(tr("Edit Schematic\n\nEdits the schematic"));
    }
  }
  else {
    App->symEdit->setText(tr("Edit Circuit Symbol"));
    App->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
    App->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  }

  if(symbolMode) {
    Nodes = &SymbolNodes;
    Wires = &SymbolWires;
    Diagrams = &SymbolDiags;
    Paintings = &SymbolPaints;
    Components = &SymbolComps;

    // if no symbol yet exists -> create one
    if(createSubcircuitSymbol()) {
      sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
      setChanged(true, true);
    }


  }
  else {
    Nodes = &DocNodes;
    Wires = &DocWires;
    Diagrams = &DocDiags;
    Paintings = &DocPaints;
    Components = &DocComps;


    if(update)
      reloadGraphs();   // load recent simulation data
  }
}

// ---------------------------------------------------
void SchematicView::setName (const QString& Name_)
{
  DocName = Name_;
  QFileInfo Info (DocName);
  QString base = Info.completeBaseName ();
  QString ext = Info.suffix();
  DataSet = base + ".dat";
  Script = base + ".m";
  if (ext != "dpl")
    DataDisplay = base + ".dpl";
  else
    DataDisplay = base + ".sch";
}

// ---------------------------------------------------
// Sets the document to be changed or not to be changed.
/// \todo fillstack and Op are no longer used
void SchematicView::setChanged(bool c, bool fillStack, char Op)
{
  if((!DocChanged) && c)
    emit signalFileChanged(true);
  else if(DocChanged && (!c))
    emit signalFileChanged(false);
  DocChanged = c;

  showBias = -1;   // schematic changed => bias points may be invalid

  if(!fillStack)
    return;
  
  return;
}

/*!
 * \brief Schematic::mouseMoveEvent
 * \param Event
 *
 * Event handler for mouse movements over the View.
 * Send signal to update the cursor position on the status bar.
 * Cursor position is reported in Scene coordinates.
 */
void SchematicView::mouseMoveEvent(QMouseEvent *Event)
{
  if(panMode) {
    QPointF d = mapToScene(Event->pos()) - panStartPosition;
    translate(d.x(), d.y());
    panStartPosition = mapToScene(Event->pos());
  }

  QPoint pos = mapToScene(Event->pos()).toPoint();
  emit signalCursorPosChanged(pos.x(), pos.y());

  QGraphicsView::mouseMoveEvent(Event);
}

/*!
 * \brief Schematic::mousePressEvent
 * \param Event
 *
 * Event handler for mouse press.
 * Set current mouse press (function pointer) handler.
 * Pass position in Scene coordinates to handler.
 */
void SchematicView::mousePressEvent(QMouseEvent *Event)
{
  App->editText->setHidden(true); // disable text edit of component property

  if(Event->button() == Qt::MiddleButton) {
    panMode = true;
    panStartPosition = mapToScene(Event->pos());
    setCursor(Qt::ClosedHandCursor);
    Event->accept();
    return;
  }
  
  QGraphicsView::mousePressEvent(Event);
}

// -----------------------------------------------------------
void SchematicView::mouseReleaseEvent(QMouseEvent *Event)
{
  if(Event->button() == Qt::MiddleButton) {
    panMode = false;
    setCursor(Qt::ArrowCursor);
  }
  
  QGraphicsView::mouseReleaseEvent(Event);
}

// -----------------------------------------------------------
void SchematicView::mouseDoubleClickEvent(QMouseEvent *Event)
{
  TODO("check mouseDoubleClickEvent");
  if(App->MouseDoubleClickAction)
    (App->view->*(App->MouseDoubleClickAction))(this, Event);
}

// -----------------------------------------------------------
void SchematicView::print(QPrinter*, QPainter *Painter, bool printAll, bool fitToPage)
{
  QPaintDevice *pdevice = Painter->device();
  float printerDpiX = (float)pdevice->logicalDpiX();
  float printerDpiY = (float)pdevice->logicalDpiY();
  float printerW = (float)pdevice->width();
  float printerH = (float)pdevice->height();
  QPainter pa(viewport());
  float screenDpiX = (float)pa.device()->logicalDpiX();
  float screenDpiY = (float)pa.device()->logicalDpiY();
  float PrintScale = 0.5;
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  int marginX = (int)(40 * printerDpiX / screenDpiX);
  int marginY = (int)(40 * printerDpiY / screenDpiY);

  if(fitToPage) {

    float ScaleX = float((printerW - 2*marginX) /
                   float((UsedX2-UsedX1) * printerDpiX)) * screenDpiX;
    float ScaleY = float((printerH - 2*marginY) /
                   float((UsedY2-UsedY1) * printerDpiY)) * screenDpiY;

    if(schematicFrame->PageType){
        int xall, yall;
        schematicFrame->sizeOfFrame(xall, yall);
        ScaleX = ((float)(printerW - 2*marginX) /
                       (float)(xall * printerDpiX)) * screenDpiX;
        ScaleY = ((float)(printerH - 2*marginY) /
                       (float)(yall * printerDpiY)) * screenDpiY;
    }

    if(ScaleX > ScaleY)
      PrintScale = ScaleY;
    else
      PrintScale = ScaleX;
  }


  int StartX = UsedX1;
  int StartY = UsedY1;
  if(schematicFrame->PageType) {
    if(UsedX1 > 0)  StartX = 0;
    if(UsedY1 > 0)  StartY = 0;
  }

  QFont oldFont = Painter->font();

#ifdef __MINGW32__
  float PrintRatio = printerDpiX / screenDpiX;
  QFont printFont = Painter->font();
  printFont.setPointSizeF(printFont.pointSizeF()/PrintRatio);
  Painter->setFont(printFont);
#endif

  TODO("Fix Schematic::print");
  /// \todo print
  /*
  //bool selected;
  ViewPainter p;
  p.init(Painter, PrintScale * PrintRatio,
         -StartX, -StartY, -marginX, -marginY, PrintScale, PrintRatio);

  TODO("Hangle frame paint");
  ///if(!symbolMode)
  ///  paintFrame(&p);

  paintSchToViewpainter(&p,printAll,false,screenDpiX,printerDpiX);
 */
  /// \todo capture scene content onto printer
  /// Disentangle this mess:
  /// - print
  /// - paintInit
  /// - paintSchToViewpainter
  // http://doc.qt.io/qt-4.8/qgraphicsscene.html#render
  // the following simply dump the scene to the painting device
  //
  // http://doc.qt.io/qt-4.8/qgraphicsview.html#render
  // it might be better to create a new view, adjust as needed and send that
  // to the painting device.
  scene->render(Painter);

  Painter->setFont(oldFont);
}

/// \todo eliminate Schematic::paintInit ported from ViewPainter::init
void SchematicView::paintInit(QPainter *p,
			  float Scale,
			  int DX_, int DY_,
			  int dx_, int dy_,
			  float FontScale, float PrintScale)
{
  // unused DX = float(DX_) * Scale - float(dx_);
  // unused DY = float(DY_) * Scale - float(dy_);

  QFont f = p->font();
  if(FontScale == 0.0)
    FontScale = Scale;
#ifdef __MINGW32__
  FontScale = Scale;
#endif
  f.setPointSizeF( FontScale * float(f.pointSize()) );
  p->setFont(f);
  // unused int lineSpacing = p->fontMetrics().lineSpacing();
  p-> setMatrixEnabled(false);   // we use our own coordinate transformation

  QPainter::RenderHints hints = 0;
  // Ask to to antialias drawings if requested
  if (QucsSettings.GraphAntiAliasing) hints |= QPainter::Antialiasing;
  // Ask to antialias text if requested
  if (QucsSettings.TextAntiAliasing) hints |= QPainter::TextAntialiasing;
  p->setRenderHints(hints);
}

/*!
 * \brief Schematic::paintSchToViewpainter
 * \param p
 * \param printAll
 * \param toImage
 * \param screenDpiX
 * \param printerDpiX
 *
 * This deprecated function visits all items and adjust them for print in a separate scence/view (was ViewPainter).
 * It relied on the ViewPainter::init to setup things like
 *  - fontsizes, linespace, and metrics
 *  - QPainter rendering options.
 * It might print:
 *  - all
 *  - only the selected
 *  - change fontsize if the ouptut is to image or hardcopy
 *  - ...
 */
void SchematicView::paintSchToViewpainter(QPainter *p, bool printAll, bool toImage, int screenDpiX, int printerDpiX)
{
    TODO("fix print, no good to call paint()");
    /*
    bool selected;

    if (printAll) {
        // if there is a frame print?
        /// int x2,y2;
        /// \todo if (sizeOfFrame(x2,y2)) paintFrame(p);
    }

    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
      if(pc->ElemSelected || printAll) {
        selected = pc->ElemSelected;
        pc->ElemSelected = false;
        if (toImage) {
            pc->paint(p);
        } else {
            pc->print(p, (float)screenDpiX / (float)printerDpiX);
        }
        pc->ElemSelected = selected;
      }

    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next()) {
      if(pw->ElemSelected || printAll) {
        selected = pw->ElemSelected;
        pw->ElemSelected = false;
        pw->paint(p);   // paint all selected wires
        pw->ElemSelected = selected;
      }
      if(pw->Label)
        if(pw->Label->ElemSelected || printAll) {
          selected = pw->Label->ElemSelected;
          pw->Label->ElemSelected = false;
          pw->Label->paint(p);
          pw->Label->ElemSelected = selected;
        }
    }

    Element *pe;
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
      for(pe = pn->Connections.first(); pe != 0; pe = pn->Connections.next())
        if(pe->ElemSelected || printAll) {
          pn->paint(p); // paint all nodes with selected elements
          break;
        }
      if(pn->Label)
        if(pn->Label->ElemSelected || printAll) {
          selected = pn->Label->ElemSelected;
          pn->Label->ElemSelected = false;
          pn->Label->paint(p);
          pn->Label->ElemSelected = selected;
        }
    }

    for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
      if(pp->ElemSelected || printAll) {
        selected = pp->ElemSelected;
        pp->ElemSelected = false;
        pp->paint(p);   // paint all selected paintings
        pp->ElemSelected = selected;
      }

    for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
      if(pd->ElemSelected || printAll) {
        // if graph or marker is selected, deselect during printing
        foreach(Graph *pg, pd->Graphs) {
      if(pg->ElemSelected)  pg->ElemType |= 1;  // remember selection
      pg->ElemSelected = false;
      foreach(Marker *pm, pg->Markers) {
        if(pm->ElemSelected)  pm->ElemType |= 1;  // remember selection
        pm->ElemSelected = false;
      }
        }

        selected = pd->ElemSelected;
        pd->ElemSelected = false;
        pd->paintDiagram(p);  // paint all selected diagrams with graphs and markers
        pd->paintMarkers(p,printAll);
        pd->ElemSelected = selected;

        // revert selection of graphs and markers
        foreach(Graph *pg, pd->Graphs) {
      if(pg->ElemType & 1)  pg->ElemSelected = true;
      pg->ElemType &= -2;
      foreach(Marker *pm, pg->Markers) {
        if(pm->ElemType & 1)  pm->ElemSelected = true;
        pm->ElemType &= -2;
      }
        }
      }

    if(showBias > 0) {  // show DC bias points in schematic ?
      int x, y, z;
      for(Node* pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
        if(pn->Name.isEmpty()) continue;
        x = pn->cx;
        y = pn->cy + 4;
        z = pn->x1;
        if(z & 1) x -= p->Painter->fontMetrics().width(pn->Name);
        if(!(z & 2)) {
          y -= (p->LineSpacing>>1) + 4;
          if(z & 1) x -= 4;
          else x += 4;
        }
        if(z & 0x10)
          p->Painter->setPen(Qt::darkGreen);  // green for currents
        else
          p->Painter->setPen(Qt::blue);   // blue for voltages
        p->drawText(pn->Name, x, y);
      }
    }
    */
}


/*!
 *
 *
 * fit view to used area (area which has components?)
 * see Used?? and View?? variables
 * legacy:
 * Tries to fit items on the view, if any. It ignores the page frame.
 * proposal:
 * Fit whatever is large. treat frame as an item. if item ouside, fit them.
 */
void SchematicView::zoomFit()
{
  TODO("Fix zoomFit");
  // stub
  // the scroll only works inside the scene bounding rect.
  /// \todo how to do the 'infinite' scrolling canvas?
  qDebug() << "scene itemsBoundingRect:" << scene->itemsBoundingRect();
  qDebug() << "scene item count:" << scene->items().size();
  if (scene->items().size()) {
    fitInView(this->scene->itemsBoundingRect(), Qt::KeepAspectRatio);
  }

  // keep track of scale
  Scale = matrix().m11();

  /// \todo showAll
  /*
  // if no items set max possible size of scene?
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(UsedX1 == 0)
    if(UsedX2 == 0)
      if(UsedY1 == 0)
        if(UsedY2 == 0) {
    UsedX1 = UsedY1 = INT_MAX;
    UsedX2 = UsedY2 = INT_MIN;
    return;
  }

  //make view 40 units larger than scene on each side?
  float xScale = float(visibleWidth()) / float(UsedX2-UsedX1+80);
  float yScale = float(visibleHeight()) / float(UsedY2-UsedY1+80);
  if(xScale > yScale) xScale = yScale;
  xScale /= Scale;

  ViewX1 = UsedX1 - 40;
  ViewY1 = UsedY1 - 40;
  ViewX2 = UsedX2 + 40;
  ViewY2 = UsedY2 + 40;
  zoom(xScale);
  */
}

// ---------------------------------------------------
/*!
 * \brief Schematic::zoomReset
 * Reset zoom scale to 100%.
 */
void SchematicView::zoomReset()
{
  Scale = 1.0;

  resetMatrix();
  scale(1.0, 1.0);

  TODO("fix Used and View variables");
  /*
  int x1 = UsedX1;
  int y1 = UsedY1;
  int x2 = UsedX2;
  int y2 = UsedY2;

  if(x1 > x2) {  // happens e.g. if untitled without changes
    x1 = 0;
    x2 = 800;
  }
  if(y1 > y2) {
    y1 = 0;
    y2 = 800;
  }
  if(x2==0) if(y2==0) if(x1==0) if(y1==0) x2 = y2 = 800;

  ViewX1 = x1-40;
  ViewY1 = y1-40;
  ViewX2 = x2+40;
  ViewY2 = y2+40;
  TODO("Fix resizeContents");
  ///\todo resizeContents(x2-x1+80, y2-y1+80);
  viewport()->update();
  App->view->drawn = false;
  */
}

/*!
 * \brief Schematic::zoomOut
 * Decrease zoom 50% steps
 * Limit zoom out to  10%
 */
void SchematicView::zoomOut()
{
  qDebug() << matrix();
  if (matrix().m11() > 0.1 && matrix().m22() > 0.1) {
    scale(0.5, 0.5);
  }
  qDebug() << matrix();

  // keep track of scale
  Scale = matrix().m11();
}

/*!
 * \brief Schematic::zoomIn
 * Increase zoom 50% steps
 * Limit zoom out to  500%
 */
void SchematicView::zoomIn()
{
  qDebug() << matrix();
  if (matrix().m11() < 5 && matrix().m22() < 5) {
    scale(1.5, 1.5);
  }
  qDebug() << matrix();

  // keep track of scale
  Scale = matrix().m11();
}

// -----------------------------------------------------------
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceed the
// visible area.
void SchematicView::enlargeView(int x1, int y1, int x2, int y2)
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

  TODO("Fix resizeContents");
  /// \todo resizeContents(int(Scale*float(ViewX2 - ViewX1)),
  /// 		int(Scale*float(ViewY2 - ViewY1)));
  ///scrollBy(dx,dy);
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
void SchematicView::setOnGrid(int& x, int& y)
{
  if(x<0) x -= (GridX >> 1) - 1;
  else x += GridX >> 1;
  x -= x % GridX;

  if(y<0) y -= (GridY >> 1) - 1;
  else y += GridY >> 1;
  y -= y % GridY;
}

// ---------------------------------------------------
// Correction factor for unproportional font scaling.
float SchematicView::textCorr()
{
  QFont Font = QucsSettings.font;
  Font.setPointSizeF( Scale * float(Font.pointSize()) );
  // use the screen-compatible metric
  QFontMetrics  metrics(Font, 0);
  return (Scale / float(metrics.lineSpacing()));
}

/*!
 * \brief Schematic::sizeOfAll return scene rectangle, bbox of elements
 * \param xmin
 * \param ymin
 * \param xmax
 * \param ymax
 */
void SchematicView::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax)
{
  QRect box = scene->sceneRect().toRect();
  xmin = box.x();
  ymin = box.y();
  xmax = box.x() + box.width();
  ymax = box.y() + box.height();
  return;
}

// ---------------------------------------------------
// Rotates all selected components around their midpoint.
bool SchematicView::rotateElements()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1=INT_MAX, y1=INT_MAX;
  int x2=INT_MIN, y2=INT_MIN;
  QList<Element *> ElementCache;
  copyLabels(x1, y1, x2, y2, &ElementCache);   // must be first of all !
  copyComponents(x1, y1, x2, y2, &ElementCache);
  copyWires(x1, y1, x2, y2, &ElementCache);
  copyPaintings(x1, y1, x2, y2, &ElementCache);
  if(y1 == INT_MAX) return false;   // no element selected

  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // center for rotation
  y1 = (y1+y2) >> 1;
  //setOnGrid(x1, y1);


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->ElemType) {
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
          if(pl->ElemType == isHWireLabel)
            pl->ElemType = isVWireLabel;
          else pl->ElemType = isHWireLabel;
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
        //qDebug("pp->getCenter(x2, y2): (%i,%i)\n", x2, y2);
        //qDebug("(x1,y1) (x2,y2): (%i,%i) (%i,%i)\n", x1,y1,x2,y2);
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
bool SchematicView::mirrorXComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QList<Element *> ElementCache;
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
  foreach(Element *pe, ElementCache)
    switch(pe->ElemType) {
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
bool SchematicView::mirrorYComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QList<Element *> ElementCache;
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
  foreach(Element *pe, ElementCache)
    switch(pe->ElemType) {
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
void SchematicView::reloadGraphs()
{
  QFileInfo Info(DocName);
  for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    pd->loadGraphData(Info.path()+QDir::separator()+DataSet);
}

// Copy function, 
void SchematicView::copy()
{
  QString s = createClipboardFile();
  QClipboard *cb = QApplication::clipboard();  // get system clipboard
  if (!s.isEmpty()) {
    cb->setText(s, QClipboard::Clipboard);
  }
}

// ---------------------------------------------------
// Cut function, copy followed by deletion
void SchematicView::cut()
{
  copy();
  deleteElements(); //delete selected elements
  viewport()->update();
}

// ---------------------------------------------------
// Performs paste function from clipboard
bool SchematicView::paste(QTextStream *stream, Q3PtrList<Element> *pe)
{
  return pasteFromClipboard(stream, pe);
}

// ---------------------------------------------------
// Loads this Qucs document.
bool SchematicView::load()
{
  DocComps.clear();
  DocWires.clear();
  DocNodes.clear();
  DocDiags.clear();
  DocPaints.clear();
  SymbolPaints.clear();

  if(!loadDocument()) return false;
  lastSaved = QDateTime::currentDateTime();
 
  // have to call this to avoid crash at sizeOfAll
  becomeCurrent(false);

  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(ViewX1 > UsedX1)  ViewX1 = UsedX1;
  if(ViewY1 > UsedY1)  ViewY1 = UsedY1;
  if(ViewX2 < UsedX2)  ViewX2 = UsedX2;
  if(ViewY2 < UsedY2)  ViewY2 = UsedY2;
  zoomReset();
  TODO("Fix setContentsPos");
  /// \todo setContentsPos(tmpViewX1, tmpViewY1);
  tmpViewX1 = tmpViewY1 = -200;   // was used as temporary cache
  return true;
}

// ---------------------------------------------------
// Saves this Qucs document. Returns the number of subcircuit ports.
int SchematicView::save()
{
  int result = adjustPortNumbers();// same port number for schematic and symbol
  if(saveDocument() < 0)
     return -1;

  QFileInfo Info(DocName);
  lastSaved = Info.lastModified();

  // update the subcircuit file lookup hashes
  QucsMain->updateSchNameHash();
  QucsMain->updateSpiceNameHash();

  return result;
}

// ---------------------------------------------------
// If the port number of the schematic and of the symbol are not
// equal add or remove some in the symbol.
int SchematicView::adjustPortNumbers()
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

  QFileInfo Info (DataDisplay);
  QString Suffix = Info.suffix();

  // handle VHDL file symbol
  if (Suffix == "vhd" || Suffix == "vhdl") {
    QStringList::iterator it;
    QStringList Names, GNames, GTypes, GDefs;
    int Number;

    // get ports from VHDL file
    QFileInfo Info(DocName);
    QString Name = Info.path() + QDir::separator() + DataDisplay;

    // obtain VHDL information either from open text document or the
    // file directly
    VHDL_File_Info VInfo;
    TextDoc * d = (TextDoc*)App->findDoc (Name);
    if (d)
      VInfo = VHDL_File_Info (d->document()->toPlainText());
    else
      VInfo = VHDL_File_Info (Name, true);

    if (!VInfo.PortNames.isEmpty())
      Names = VInfo.PortNames.split(",", QString::SkipEmptyParts);

    for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
      if(pp->Name == ".ID ") {
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.EntityName.toUpper();
	id->Parameter.clear();
	if (!VInfo.GenNames.isEmpty())
	  GNames = VInfo.GenNames.split(",", QString::SkipEmptyParts);
	if (!VInfo.GenTypes.isEmpty())
	  GTypes = VInfo.GenTypes.split(",", QString::SkipEmptyParts);
	if (!VInfo.GenDefs.isEmpty())
	  GDefs = VInfo.GenDefs.split(",", QString::SkipEmptyParts);;
	for(Number = 1, it = GNames.begin(); it != GNames.end(); ++it) {
	  id->Parameter.append(new SubParameter(
 	    true,
	    *it+"="+GDefs[Number-1],
	    tr("generic")+" "+QString::number(Number),
	    GTypes[Number-1]));
	  Number++;
	}
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) {
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else {
	SymbolPaints.append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }
  // handle Verilog-HDL file symbol
  else if (Suffix == "v") {

    QStringList::iterator it;
    QStringList Names;
    int Number;

    // get ports from Verilog-HDL file
    QFileInfo Info (DocName);
    QString Name = Info.path() + QDir::separator() + DataDisplay;

    // obtain Verilog-HDL information either from open text document or the
    // file directly
    Verilog_File_Info VInfo;
    TextDoc * d = (TextDoc*)App->findDoc (Name);
    if (d)
      VInfo = Verilog_File_Info (d->document()->toPlainText());
    else
      VInfo = Verilog_File_Info (Name, true);
    if (!VInfo.PortNames.isEmpty())
      Names = VInfo.PortNames.split(",", QString::SkipEmptyParts);

    for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
      if(pp->Name == ".ID ") {
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.ModuleName.toUpper();
	id->Parameter.clear();
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) {
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else {
	SymbolPaints.append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }
  // handle Verilog-A file symbol
  else if (Suffix == "va") {

    QStringList::iterator it;
    QStringList Names;
    int Number;

    // get ports from Verilog-A file
    QFileInfo Info (DocName);
    QString Name = Info.path() + QDir::separator() + DataDisplay;

    // obtain Verilog-A information either from open text document or the
    // file directly
    VerilogA_File_Info VInfo;
    TextDoc * d = (TextDoc*)App->findDoc (Name);
    if (d)
      VInfo = VerilogA_File_Info (d->toPlainText());
    else
      VInfo = VerilogA_File_Info (Name, true);

    if (!VInfo.PortNames.isEmpty())
      Names = VInfo.PortNames.split(",", QString::SkipEmptyParts);

    for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
      if(pp->Name == ".ID ") {
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.ModuleName.toUpper();
	id->Parameter.clear();
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) {
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else {
	SymbolPaints.append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }
  // handle schematic symbol
  else
  {
      // go through all components in a schematic
      for(Component *pc = DocComps.first(); pc!=0; pc = DocComps.next())
      {
         if(pc->obsolete_model_hack() == "Port") { // BUG. move to device.
             countPort++;

             Str = pc->Props.getFirst()->Value;
             // search for matching port symbol
             for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
             {
                 if(pp->Name == ".PortSym ")
                 {
                   if(((PortSymbol*)pp)->numberStr == Str) break;
                 }
             }

             if(pp)
             {
                 ((PortSymbol*)pp)->nameStr = pc->name();
             }
             else
             {
                 SymbolPaints.append(new PortSymbol(x1, y2, Str, pc->name()));
                 y2 += 40;
             }
          }
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
bool SchematicView::undo()
{
  undoStack->undo();
  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
bool SchematicView::redo()
{
  undoStack->redo();
  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
// Sets selected elements on grid.
/// \bug pass list of selected instead of searching for them.
bool SchematicView::elementsOnGrid()
{
  int x, y, No;
  bool count = false;
  WireLabel *pl, *pLabel;
  Q3PtrList<WireLabel> LabelCache;

  // test all components
  Components->setAutoDelete(false);
  for(Component *pc = Components->last(); pc != 0; pc = Components->prev())
    if(pc->ElemSelected) {

      // rescue non-selected node labels
      foreach(Port *pp, pc->Ports)
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
      pc->ElemSelected = false;
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

    if(pw->ElemSelected) {
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
      pw->ElemSelected = false;
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
      if(pl->ElemSelected) {
        setOnGrid(pl->x1, pl->y1);
        pl->ElemSelected = false;
        count = true;
      }
    }
  }
  Wires->setAutoDelete(true);

  // test all node labels
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label)
      if(pn->Label->ElemSelected) {
        setOnGrid(pn->Label->x1, pn->Label->y1);
        pn->Label->ElemSelected = false;
        count = true;
      }

  // test all diagrams
  for(Diagram *pd = Diagrams->last(); pd != 0; pd = Diagrams->prev()) {
    if(pd->ElemSelected) {
      setOnGrid(pd->cx, pd->cy);
      pd->ElemSelected = false;
      count = true;
    }

    foreach(Graph *pg,pd->Graphs)
      // test markers of diagram
      foreach(Marker *pm, pg->Markers)
        if(pm->ElemSelected) {
	  x = pm->x1 + pd->cx;
	  y = pm->y1 + pd->cy;
	  setOnGrid(x, y);
	  pm->x1 = x - pd->cx;
	  pm->y1 = y - pd->cy;
	  pm->ElemSelected = false;
	  count = true;
        }
  }

  // test all paintings
  for(Painting *pa = Paintings->last(); pa != 0; pa = Paintings->prev())
    if(pa->ElemSelected) {
      setOnGrid(pa->cx, pa->cy);
      pa->ElemSelected = false;
      count = true;
    }

  if(count) setChanged(true, true);
  return count;
}

// ---------------------------------------------------
void SchematicView::switchPaintMode()
{
  symbolMode = !symbolMode;  // change mode

  TODO("Fix contentsY");
  /** \todo switch paint mode
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
  */
}


// *********************************************************************
// **********                                                 **********
// **********      Function for serving mouse wheel moving    **********
// **********                                                 **********
// *********************************************************************
void SchematicView::contentsWheelEvent(QWheelEvent *Event)
{
  App->editText->setHidden(true);  // disable edit of component property
  // use smaller steps; typically the returned delta() is a multiple of 120

  TODO("Fix wheelEvent");
  /** \todo
  int delta = Event->delta() >> 1;

  // ...................................................................
  if((Event->modifiers() & Qt::ShiftModifier) ||
     (Event->orientation() == Qt::Horizontal)) { // scroll horizontally ?
      if(delta > 0) { if(scrollLeft(delta)) scrollBy(-delta, 0); }
      else { if(scrollRight(delta)) scrollBy(-delta, 0); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }
  // ...................................................................
  else if(Event->modifiers() & Qt::ControlModifier) {  // use mouse wheel to zoom ?
      // zoom factor scaled according to the wheel delta, to accomodate
      //  values different from 60 (slower or faster zoom)
      float Scaling = pow(1.1, delta/60.0);
      zoom(Scaling);
      Scaling -= 1.0;
      scrollBy( int(Scaling * float(Event->pos().x())),
                int(Scaling * float(Event->pos().y())) );
  }
  // ...................................................................
  else {     // scroll vertically !
      if(delta > 0) { if(scrollUp(delta)) scrollBy(0, -delta); }
      else { if(scrollDown(delta)) scrollBy(0, -delta); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }

  Event->accept();   // QScrollView must not handle this event
  */
}

// -----------------------------------------------------------
// Scrolls the visible area upwards and enlarges or reduces the view
// area accordingly.
bool SchematicView::scrollUp(int step)
{
  TODO("Fix scrollUp");
  /**
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
  */
  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area downwards and enlarges or reduces the view
// area accordingly. ("step" must be negative!)
bool SchematicView::scrollDown(int step)
{
  TODO("Fix scrollDown");
  /**
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
  */
  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the left and enlarges or reduces the view
// area accordingly.
bool SchematicView::scrollLeft(int step)
{
  TODO("Fix scrollLeft");
  /**
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
  */
  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the right and enlarges or reduces the
// view area accordingly. ("step" must be negative!)
bool SchematicView::scrollRight(int step)
{

  TODO("Fix scrollRight");
  /**
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
  */
  return true;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicView::slotScrollUp()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollUp(verticalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicView::slotScrollDown()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollDown(-verticalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicView::slotScrollLeft()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollLeft(horizontalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicView::slotScrollRight()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollRight(-horizontalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}


// *********************************************************************
// **********                                                 **********
// **********        Function for serving drag'n drop         **********
// **********                                                 **********
// *********************************************************************

// Is called if an object is dropped (after drag'n drop).
void SchematicView::contentsDropEvent(QDropEvent *Event)
{
  if(dragIsOkay) {
    QList<QUrl> urls = Event->mimeData()->urls();
    if (urls.isEmpty()) {
      return;
    }

    // do not close untitled document to avoid segfault
    QucsDoc *d = QucsMain->getDoc(0);
    bool changed = d->DocChanged;
    d->DocChanged = true;

    // URI:  file:/home/linuxuser/Desktop/example.sch
    foreach(QUrl url, urls) {
      App->gotoPage(QDir::toNativeSeparators(url.toLocalFile()));
    }

    d->DocChanged = changed;
    return;
  }


  QMouseEvent e(QEvent::MouseButtonPress, Event->pos(),
                Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

  App->view->MPressElement(this, &e);

  if(App->view->selElem) delete App->view->selElem;
  App->view->selElem = 0;  // no component selected

  if(formerAction)
    formerAction->setChecked(true);  // restore old action
}

// ---------------------------------------------------
void SchematicView::contentsDragEnterEvent(QDragEnterEvent *Event)
{
  //FIXME: the function of drag library component seems not working?
  formerAction = 0;
  dragIsOkay = false;

  // file dragged in ?
  if(Event->mimeData()->hasUrls()) {
    dragIsOkay = true;
    Event->accept();
    return;
  }

  // drag library component
  if(Event->mimeData()->hasText()) {
    QString s = Event->mimeData()->text();
    if(s.left(15) == "QucsComponent:<") {
      s = s.mid(14);
      App->view->selElem = getComponentFromName(s);
      if(App->view->selElem) {
        Event->accept();
        return;
      }
    }
    Event->ignore();
    return;
  }


//   if(Event->format(1) == 0) {  // only one MIME type ?

    // drag component from listview
//     if(Event->provides("application/x-qabstractitemmodeldatalist")) {
    if(Event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
      QListWidgetItem *Item = App->CompComps->currentItem();
      if(Item) {
        formerAction = App->activeAction;
        App->slotSelectComponent(Item);  // also sets drawn=false
        App->MouseMoveAction = 0;
        App->MousePressAction = 0;

        Event->accept();
        return;
      }
    }
//   }

  Event->ignore();
}

// ---------------------------------------------------
void SchematicView::contentsDragLeaveEvent(QDragLeaveEvent*)
{
  if(App->view->selElem)
    if(App->view->selElem->ElemType & isComponent)
      if(App->view->drawn) {

        QPainter painter(viewport());
        //App->view->setPainter(this);
        TODO("fix paintscheme")
        //((Component*)App->view->selElem)->paintScheme(this);
        App->view->drawn = false;
      }

  if(formerAction)
    formerAction->setChecked(true);  // restore old action
}

// ---------------------------------------------------
void SchematicView::contentsDragMoveEvent(QDragMoveEvent *Event)
{
  if(!dragIsOkay) {
    if(App->view->selElem == 0) {
      Event->ignore();
      return;
    }

    QMouseEvent e(QEvent::MouseMove, Event->pos(), Qt::NoButton, 
		  Qt::NoButton, Qt::NoModifier);
    App->view->MMoveElement(this, &e);
  }

  Event->accept();
}

