/***************************************************************************
                              schematic.cpp
                             ---------------
    copyright            : (C) 2006 by Michael Margraf
                           2018 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <limits.h>

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
#include <QMimeData>

#include "qucs.h"
#include "schematic_doc.h"
#include "node.h"
#include "textdoc.h"
#include "viewpainter.h"
#include "mouseactions.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/vafile.h"
#include "misc.h"
#include "globals.h"
#include "trace.h"

#include "qt_compat.h"

class QPrinter;

// just dummies for empty lists
WireList      SymbolWires;
//NodeList      SymbolNodes;
DiagramList   SymbolDiags;
ComponentList SymbolComps;

//PaintingList& SchematicDoc::symbolPaintings()
//{ untested();
//  return DocModel.symbolPaintings();
//}

void SchematicDoc::printCursorPosition(int x, int y)
{ itested();
  QPoint p(x,y);
  QPointF mp=mapToScene(p);
  App->printCursorPosition(mp.x(),mp.y());
}


// ---------------------------------------------------
bool SchematicDoc::createSubcircuitSymbol()
{ untested();
  incomplete();
#if 0
  // If the number of ports is not equal, remove or add some.
  unsigned int countPort = adjustPortNumbers();

  // If a symbol does not yet exist, create one.
  if(symbolPaintings().count() != countPort)
    return false;

  int h = 30*((countPort-1)/2) + 10;

  Painting* txt=painting_dispatcher.clone("ID_Text");
  assert(txt);
  txt->setArgs2Int(-20, h+4); // fix later.
  symbolPaintings().prepend(txt);

  Painting* pp;

  pp = painting_dispatcher.clone("GraphicsLine");
  assert(pp);
  pp->setSomeStuff(-20, -h, 40,  0, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  pp = painting_dispatcher.clone("GraphicsLine");
  pp->setSomeStuff( 20, -h,  0,2*h, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  pp = painting_dispatcher.clone("GraphicsLine");
  pp->setSomeStuff(-20,  h, 40,  0, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  pp = painting_dispatcher.clone("GraphicsLine");
  pp->setSomeStuff(-20, -h,  0,2*h, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  unsigned int i=0, y = 10-h;
  while(i<countPort) { untested();
    i++;
    pp = painting_dispatcher.clone("GraphicsLine");
    pp->setSomeStuff(-30, y, 10, 0, QPen(Qt::darkBlue,2));
    symbolPaintings().append(pp);
    symbolPaintings().at(i)->setCenter(-30,  y);

    if(i == countPort)  break;
    i++;
    pp = painting_dispatcher.clone("GraphicsLine");
    pp->setSomeStuff( 20, y, 10, 0, QPen(Qt::darkBlue,2));
    symbolPaintings().append(pp);
    symbolPaintings().at(i)->setCenter(30,  y);
    y += 60;
  }
#endif
  return true;
}
// ---------------------------------------------------
// // expose tab?
// same as showEvent?
void SchematicDoc::becomeCurrent(bool update)
{ untested();
  emit signalCursorPosChanged(0, 0);

  // update appropriate menu entry
  if (isSymbolMode()) { untested();
    incomplete(); // SymbolDoc.
    if (docName().right(4) == ".sym") { untested();
      App->symEdit->setText(tr("Edit Text"));
      App->symEdit->setStatusTip(tr("Edits the Text"));
      App->symEdit->setWhatsThis(tr("Edit Text\n\nEdits the text file"));
    }else{ untested();
      App->symEdit->setText(tr("Edit Schematic"));
      App->symEdit->setStatusTip(tr("Edits the schematic"));
      App->symEdit->setWhatsThis(tr("Edit Schematic\n\nEdits the schematic"));
    }
  }else{ untested();
    App->symEdit->setText(tr("Edit Circuit Symbol"));
    App->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
    App->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  }

  if(isSymbolMode()) { untested();
    incomplete();
    assert(0);

    // if no symbol yet exists -> create one
    if(createSubcircuitSymbol()) { untested();
      symbolPaintings().sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
      setChanged(true, true);
    }

    emit signalUndoState(undoSymbolIdx != 0);
    emit signalRedoState(undoSymbolIdx != undoSymbol.size()-1);
  } else { untested();
    incomplete();
#if 0
    Nodes = &DocNodes;
    Wires = &DocWires;
    Diagrams = &DocDiags;
    Paintings = &DocPaints;
    Components = &DocComps;
#endif

    emit signalUndoState(undoActionIdx != 0);
    emit signalRedoState(undoActionIdx != undoAction.size()-1);
    if(update){ untested();
      incomplete();
      reloadGraphs();   // load recent simulation data
    }else{ untested();
    }
  }
}

// ---------------------------------------------------
void SchematicDoc::setName (const QString& Name_)
{ untested();
  setDocName(Name_);
  QFileInfo Info (docName());
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
void SchematicDoc::setChanged(bool c, bool fillStack, char Op)
{ untested();
  if((!DocChanged) && c)
    emit signalFileChanged(true);
  else if(DocChanged && (!c))
    emit signalFileChanged(false);
  DocChanged = c;

  showBias = -1;   // schematic changed => bias points may be invalid

  if(!fillStack)
    return;


  // ................................................
  if(isSymbolMode()) {  // for symbol edit mode
    while(undoSymbol.size() > undoSymbolIdx + 1) { untested();
      delete undoSymbol.last();
      undoSymbol.pop_back();
    }

    undoSymbol.append(new QString(createSymbolUndoString(Op)));
    undoSymbolIdx++;

    emit signalUndoState(true);
    emit signalRedoState(false);

    while(static_cast<unsigned int>(undoSymbol.size()) > QucsSettings.maxUndo) { // "while..." because
      delete undoSymbol.first();
      undoSymbol.pop_front();
      undoSymbolIdx--;
    }
    return;
  }

  // ................................................
  // for schematic edit mode
  while(undoAction.size() > undoActionIdx + 1) { untested();
    delete undoAction.last();
    undoAction.pop_back();
  }

  if(Op == 'm') {   // only one for move marker
    if (undoAction.at(undoActionIdx)->at(0) == Op) { untested();
      delete undoAction.last();
      undoAction.pop_back();
      undoActionIdx--;
    }
  }

  undoAction.append(new QString(createUndoString(Op)));
  undoActionIdx++;

  emit signalUndoState(true);
  emit signalRedoState(false);

  while(static_cast<unsigned int>(undoAction.size()) > QucsSettings.maxUndo) { // "while..." because
    delete undoAction.first(); // "maxUndo" could be decreased meanwhile
    undoAction.pop_front();
    undoActionIdx--;
  }
  return;
}

// -----------------------------------------------------------
//
  // Values exclude border of 1.5cm at each side.
    // DIN A5 landscape
    // DIN A5 portrait
    // DIN A4 landscape
    // DIN A4 portrait
    // DIN A3 landscape
    // DIN A3 portrait
    // letter landscape
    // letter portrait
static int frameX[10]= {0, 1020, 765, 1530, 1020, 2295, 1530, 1414, 1054 };
static int frameY[10]= {0, 765, 1020, 1020, 1530, 1530, 2295, 1054, 1414 };

bool SchematicDoc::sizeOfFrame(int& xall, int& yall)
{ untested();
  int i = showFrame();
  if(!i) { untested();
    // don't show? why does it not have a size?!
    return false;
  }else if(i<9) { untested();
    xall = frameX[i];
    yall = frameY[i];
    return true;
  }else{ untested();
    return false;
  }
}

// -----------------------------------------------------------
void SchematicDoc::paintFrame(ViewPainter *p)
{ untested();
  // dimensions:  X cm / 2.54 * 144
  int xall, yall;
  if(!sizeOfFrame(xall, yall))
    return;
  p->Painter->setPen(QPen(Qt::darkGray,1));
  //p->Painter->setPen(QPen(Qt::black,0));
  int d = p->LineSpacing + int(4.0 * p->Scale);
  int x1_, y1_, x2_, y2_;
  p->map(xall, yall, x1_, y1_);
  x2_ = int(xall * p->Scale) + 1;
  y2_ = int(yall * p->Scale) + 1;
  p->Painter->drawRect(x1_, y1_, -x2_, -y2_);
  p->Painter->drawRect(x1_-d, y1_-d, 2*d-x2_, 2*d-y2_);

  int z;
  int step = xall / ((xall+127) / 255);
  for(z=step; z<=xall-step; z+=step) { untested();
    p->map(z, 0, x2_, y2_);
    p->Painter->drawLine(x2_, y2_, x2_, y2_+d);
    p->Painter->drawLine(x2_, y1_-d, x2_, y1_);
  }
  char Letter[2] = "1";
  for(z=step/2+5; z<xall; z+=step) { untested();
    p->drawText(Letter, z, 3, 0);
    p->map(z, yall+3, x2_, y2_);
    p->Painter->drawText(x2_, y2_-d, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  step = yall / ((yall+127) / 255);
  for(z=step; z<=yall-step; z+=step) { untested();
    p->map(0, z, x2_, y2_);
    p->Painter->drawLine(x2_, y2_, x2_+d, y2_);
    p->Painter->drawLine(x1_-d, y2_, x1_, y2_);
  }
  Letter[0] = 'A';
  for(z=step/2+5; z<yall; z+=step) { untested();
    p->drawText(Letter, 5, z, 0);
    p->map(xall+5, z, x2_, y2_);
    p->Painter->drawText(x2_-d, y2_, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  // draw text box with text
  p->map(xall-340, yall-3, x1_, y1_);
  p->map(xall-3,   yall-3, x2_, y2_);
  x1_ -= d;  x2_ -= d;
  y1_ -= d;  y2_ -= d;
  d = int(6.0 * p->Scale);
  z = int(200.0 * p->Scale);
  y1_ -= p->LineSpacing + d;
  p->Painter->drawLine(x1_, y1_, x2_, y1_);
  p->Painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, FrameText[2]);
  p->Painter->drawLine(x1_+z, y1_, x1_+z, y1_ + p->LineSpacing+d);
  p->Painter->drawText(x1_+d+z, y1_+(d>>1), 0, 0, Qt::TextDontClip, FrameText[3]);
  y1_ -= p->LineSpacing + d;
  p->Painter->drawLine(x1_, y1_, x2_, y1_);
  p->Painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, FrameText[1]);
  y1_ -= (FrameText[0].count('\n')+1) * p->LineSpacing + d;
  p->Painter->drawRect(x2_, y2_, x1_-x2_-1, y1_-y2_-1);
  p->Painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, FrameText[0]);
}

// -----------------------------------------------------------
#if QT_VERSION < 0x050000
// Is called when the content (schematic or data display) has to be drawn.
void SchematicDoc::drawContents(QPainter *p, int, int, int, int)
{ untested();
  // no longer used.
  ViewPainter Painter;

  Painter.init(p, Scale, -ViewX1, -ViewY1, contentsX(), contentsY());

  paintGrid(&Painter, contentsX(), contentsY(),
            visibleWidth(), visibleHeight());

  if(!isSymbolMode())
    paintFrame(&Painter);

  for(auto pc : components()) { untested();
    Element* e=pc;
    e->paint(&Painter);
  }

  for(auto pw : wires()) { untested();
    pw->paint(&Painter);
    if(pw->Label)
      pw->Label->paint(&Painter);  // separate because of paintSelected
  }

  Node *pn;
  for(auto i : nodes()){ untested();
    pn=i;
    Element* e=pn;
    e->paint(&Painter);
    if(pn->Label)
      pn->Label->paint(&Painter);  // separate because of paintSelected
  }

  // FIXME disable here, issue with select box goes away
  // also, instead of red, line turns blue
  for(auto pd : diagrams()){ untested();
    Element const* e=pd;
    e->paint(&Painter);
  }

  for(auto pp : paintings()){ untested();
    pp->paint(&Painter);
  }

  if(showBias > 0) {  // show DC bias points in schematic ?
    int x, y, z;
    for(auto i: nodes()){ untested();
      pn=i;
      if(pn->name().isEmpty()) continue;
      x = pn->cx_();
      y = pn->cy_() + 4;
      z = pn->x1_();
      if(z & 1) x -= Painter.Painter->fontMetrics().width(pn->name());
      if(!(z & 2)) { untested();
        y -= (Painter.LineSpacing>>1) + 4;
        if(z & 1) x -= 4;
        else x += 4;
      }
      if(z & 0x10)
        Painter.Painter->setPen(Qt::darkGreen);  // green for currents
      else
        Painter.Painter->setPen(Qt::blue);   // blue for voltages
      Painter.drawText(pn->name(), x, y);
    }
  }

  /*
   * The following events used to be drawn from mouseactions.cpp, but since Qt4
   * Paint actions can only be called from within the paint event, so they
   * are put into a QList (PostedPaintEvents) and processed here
   */
  for(int i=0;i<PostedPaintEvents.size();i++)
  { untested();
    PostedPaintEvent p = PostedPaintEvents[i];
    QPainter painter2(viewport());

    switch(p.pe)
    { untested();
      case _NotRop:
        if(p.PaintOnViewport)
          painter2.setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
        else
          Painter.Painter->setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
        break;
      case _Rect:
        if(p.PaintOnViewport)
          painter2.drawRect(p.x1, p.y1, p.x2, p.y2);
        else
          Painter.drawRect(p.x1, p.y1, p.x2, p.y2);
        break;
      case _Line:
        if(p.PaintOnViewport)
          painter2.drawLine(p.x1, p.y1, p.x2, p.y2);
        else
          Painter.drawLine(p.x1, p.y1, p.x2, p.y2);
        break;
      case _Ellipse:
        if(p.PaintOnViewport)
          painter2.drawEllipse(p.x1, p.y1, p.x2, p.y2);
        else
          Painter.drawEllipse(p.x1, p.y1, p.x2, p.y2);
        break;
      case _Arc:
        if(p.PaintOnViewport)
          painter2.drawArc(p.x1, p.y1, p.x2, p.y2, p.a, p.b);
        else
          Painter.drawArc(p.x1, p.y1, p.x2, p.y2, p.a, p.b);
        break;
      case _DotLine:
        Painter.Painter->setPen(Qt::DotLine);
        break;
      case _Translate:

        painter2.translate(p.x1, p.y1);
        break;
      case _Scale:
        painter2.scale(p.x1,p.y1);
        break;
    }

  }
  PostedPaintEvents.clear();
}
#endif

void SchematicDoc::PostPaintEvent (PE pe, int x1, int y1, int x2, int y2, int a, int b, bool PaintOnViewport)
{ itested();
  PostedPaintEvent p = {pe, x1,y1,x2,y2,a,b,PaintOnViewport};
  PostedPaintEvents.push_back(p);
  updateViewport();
  update();
}

// -----------------------------------------------------------
void SchematicDoc::print(QPrinter*, QPainter *Painter, bool printAll, bool fitToPage)
{ untested();
#ifndef USE_SCROLLVIEW
  (void) Painter;
  (void) printAll;
  (void) fitToPage;
  incomplete(); // does not work with qt5
#else
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

  if(fitToPage) { untested();

    float ScaleX = float((printerW - 2*marginX) /
                   float((UsedX2-UsedX1) * printerDpiX)) * screenDpiX;
    float ScaleY = float((printerH - 2*marginY) /
                   float((UsedY2-UsedY1) * printerDpiY)) * screenDpiY;

    if(showFrame()){ untested();
        int xall, yall;
        sizeOfFrame(xall, yall);
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


  //bool selected;
  ViewPainter p;
  int StartX = UsedX1;
  int StartY = UsedY1;
  if(showFrame()) { untested();
    if(UsedX1 > 0)  StartX = 0;
    if(UsedY1 > 0)  StartY = 0;
  }

  float PrintRatio = printerDpiX / screenDpiX;
  QFont oldFont = Painter->font();
  QFont printFont = Painter->font();
#ifdef __MINGW32__
  printFont.setPointSizeF(printFont.pointSizeF()/PrintRatio);
  Painter->setFont(printFont);
#endif
  p.init(Painter, PrintScale * PrintRatio,
         -StartX, -StartY, -marginX, -marginY, PrintScale, PrintRatio);

  if(!isSymbolMode())
    paintFrame(&p);

  paintSchToViewpainter(&p,printAll,false,screenDpiX,printerDpiX);

  Painter->setFont(oldFont);
#endif
}


void SchematicDoc::paintSchToViewpainter(ViewPainter *p, bool printAll, bool toImage, int screenDpiX, int printerDpiX)
{ untested();
  (void) toImage;
  (void) screenDpiX;
  (void) printerDpiX;

    if (printAll) { untested();
        int x2,y2;
        if (sizeOfFrame(x2,y2)) paintFrame(p);
    }

#if 0
    bool selected;
    // this is a mess
    for(auto pc : components()){ untested();
      if(pc->isSelected() || printAll) { untested();
        selected = pc->isSelected();
        pc->setSelected(false);
        if (toImage) { untested();
            ((Element*)pc)->paint(p);
        } else { untested();
            pc->print(p, (float)screenDpiX / (float)printerDpiX);
        }
        pc->setSelected(selected);
      }
    }

    for(auto pw : wires()){ untested();
      if(pw->isSelected() || printAll) { untested();
        selected = pw->isSelected();
        pw->setSelected(false);
        pw->paint(p);   // paint all selected wires
        pw->setSelected(selected);
      }
      if(pw->Label)
        if(pw->Label->isSelected() || printAll) { untested();
          selected = pw->Label->isSelected();
          pw->Label->setSelected(false);
          pw->Label->paint(p);
          pw->Label->setSelected(selected);
        }
    }

    Element *pe;
    for(auto pn : nodes()) { untested();
      for( auto i : pn->connections()){ untested();
	pe=i;
        if(pe->isSelected() || printAll) { untested();
	  Element* e=pn;
	  e->paint(p); // paint all nodes with selected elements
          break;
        }else{ untested();
	}
      }
      if(pn->Label)
        if(pn->Label->isSelected() || printAll) { untested();
          selected = pn->Label->isSelected();
          pn->Label->setSelected(false);
          pn->Label->paint(p);
          pn->Label->setSelected(selected);
        }
    }

    for(auto pp : paintings()){ untested();
      if(pp->isSelected() || printAll) { untested();
        selected = pp->isSelected();
        pp->setSelected(false);
        pp->paint(p);   // paint all selected paintings
        pp->setSelected(selected);
      }
    }

    for(auto pd : diagrams()){ untested();
      if(pd->isSelected() || printAll) { untested();
        // if graph or marker is selected, deselect during printing
        foreach(Graph *pg, pd->Graphs) { untested();
      if(pg->isSelected())  pg->Type |= 1;  // remember selection
      pg->setSelected(false);
      foreach(Marker *pm, pg->Markers) { untested();
        if(pm->isSelected())  pm->Type |= 1;  // remember selection
        pm->setSelected(false);
      }
        }

        selected = pd->isSelected();
        pd->setSelected(false);
        pd->paintDiagram(p);  // paint all selected diagrams with graphs and markers
        pd->paintMarkers(p,printAll);
        pd->setSelected(selected);

        // revert selection of graphs and markers
        foreach(Graph *pg, pd->Graphs) { untested();
      if(pg->Type & 1)  pg->setSelected();
      pg->Type &= -2;
      foreach(Marker *pm, pg->Markers) { untested();
        if(pm->Type & 1)  pm->setSelected();
        pm->Type &= -2;
      }
        }
      }
    }

    if(showBias > 0) {  // show DC bias points in schematic ?
      int x, y, z;
      for(auto pn : nodes()) { untested();
        if(pn->name().isEmpty()) continue;
        x = pn->cx_();
        y = pn->cy_() + 4;
        z = pn->x1_();
        if(z & 1) x -= p->Painter->fontMetrics().width(pn->name());
        if(!(z & 2)) { untested();
          y -= (p->LineSpacing>>1) + 4;
          if(z & 1) x -= 4;
          else x += 4;
        }
        if(z & 0x10)
          p->Painter->setPen(Qt::darkGreen);  // green for currents
        else
          p->Painter->setPen(Qt::blue);   // blue for voltages
        p->drawText(pn->name(), x, y);
      }
    }
#endif
}

// -----------------------------------------------------------
// ---------------------------------------------------

// ---------------------------------------------------

// -----------------------------------------------------------
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceed the
// visible area.
//
// same as center on union(current, rectangle(xy))??
// // implemented in QGraphicsView::ensureVisible, it seems
void SchematicDoc::enlargeView(int x1, int y1, int x2, int y2)
{ untested();
  incomplete();
#if 0
  int dx=0, dy=0;
  (void) (dx+dy);
  if(x1 < UsedX1) UsedX1 = x1;
  if(y1 < UsedY1) UsedY1 = y1;
  if(x2 > UsedX2) UsedX2 = x2;
  if(y2 > UsedY2) UsedY2 = y2;

  if(x1 < ViewX1) { untested();
    dx = int(Scale * float(ViewX1-x1+40));
    ViewX1 = x1-40;
  }
  if(y1 < ViewY1) { untested();
    dy = int(Scale * float(ViewY1-y1+40));
    ViewY1 = y1-40;
  }
  if(x2 > ViewX2) ViewX2 = x2+40;
  if(y2 > ViewY2) ViewY2 = y2+40;

  TODO("Fix resizeContents");
  /// \todo resizeContents(int(Scale*float(ViewX2 - ViewX1)),
  /// 		int(Scale*float(ViewY2 - ViewY1)));
  ///scrollBy(dx,dy);
#endif
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
// BUG: fp?

// ---------------------------------------------------
void SchematicDoc::paintGrid(ViewPainter *p, int cX, int cY, int Width, int Height)
{ untested();
#if 0 // obsolete
  if(!GridOn) return;

  p->Painter->setPen(QPen(Qt::black,0));
  int dx = -int(Scale*float(ViewX1)) - cX;
  int dy = -int(Scale*float(ViewY1)) - cY;
  p->Painter->drawLine(-3+dx, dy, 4+dx, dy); // small cross at origin
  p->Painter->drawLine( dx,-3+dy, dx, 4+dy);


  int x1  = int(float(cX)/Scale) + ViewX1;
  int y1  = int(float(cY)/Scale) + ViewY1;

  /// \todo setting the center of rotation on the grid causes the center to move when doing multiple rotations when it is not already on the grid. Should not force the center but force the component alignment after rotation.
  setOnGrid(x1, y1);
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

  while(x1 < xEnd) { untested();
    Y = Y0;
    y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);
    while(y1 < yEnd) { untested();
      p->Painter->drawPoint(x1, y1);    // paint grid
      Y += DY;
      y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);
    }
    X += DX;
    x1 = X > 0.0 ? int(X + 0.5) : int(X - 0.5);
  }
#endif
}

// ---------------------------------------------------
// Correction factor for unproportional font scaling.
float SchematicDoc::textCorr()
{ untested();
  incomplete();
  return 1.;
#if 0
  QFont Font = QucsSettings.font;
  Font.setPointSizeF( Scale * float(Font.pointSize()) );
  // use the screen-compatible metric
  QFontMetrics  metrics(Font, 0);
  return (Scale / float(metrics.lineSpacing()));
#endif
}

// ---------------------------------------------------
void SchematicModel::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax, float textCorr) const
{ untested();
  xmin=INT_MAX;
  ymin=INT_MAX;
  xmax=INT_MIN;
  ymax=INT_MIN;
  WireLabel *pl;

  if(components().isEmpty())
    if(wires().isEmpty())
      if(diagrams().isEmpty())
        if(paintings().isEmpty()) { untested();
          xmin = xmax = 0;
          ymin = ymax = 0;
          return;
        }


  float Corr = textCorr;
  int x1, y1, x2, y2;
  // find boundings of all components
  for(auto pc : components()) { untested();
    pc->entireBounds(x1, y1, x2, y2, Corr);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  // find boundings of all wires
  for(auto pw : wires()) { untested();
    if(pw->x1_() < xmin) xmin = pw->x1_();
    if(pw->x2_() > xmax) xmax = pw->x2_();
    if(pw->y1_() < ymin) ymin = pw->y1_();
    if(pw->y2_() > ymax) ymax = pw->y2_();

#if 0
    pl = pw->Label;
    if(pl) {     // check position of wire label
        pl->getLabelBounding(x1,y1,x2,y2);
        if(x1 < xmin) xmin = x1;
        if(x2 > xmax) xmax = x2;
        if(y1 < ymin) ymin = y1;
        if(y2 > ymax) ymax = y2;
    }
#endif
  }

#if 0
  // find boundings of all node labels
  for(auto pn : nodes()){ untested();
    pl = pn->Label;
    if(pl) {     // check position of node label
        pl->getLabelBounding(x1,y1,x2,y2);
        if(x1 < xmin) xmin = x1;
        if(x2 > xmax) xmax = x2;
        if(y1 < ymin) ymin = y1;
        if(y2 > ymax) ymax = y2;
    }
  }
#endif

  // find boundings of all diagrams
  for(auto pd : diagrams()) { untested();
    pd->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;

    foreach(Graph *pg, pd->Graphs)
      // test all markers of diagram
      foreach(Marker *pm, pg->Markers) { untested();
        pm->Bounding(x1, y1, x2, y2);
        if(x1 < xmin) xmin = x1;
        if(x2 > xmax) xmax = x2;
        if(y1 < ymin) ymin = y1;
        if(y2 > ymax) ymax = y2;
      }
  }

  // find boundings of all Paintings
  for(auto pp : paintings()) { untested();
    pp->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
}

// ---------------------------------------------------
// Rotates all selected components around their midpoint.
bool SchematicDoc::rotateElements()
{ untested();
  // disconnect();?

  QRectF BB(1., 1., -1. , -1.);
  QList<ElementGraphics*> ElementCache;

#ifdef USE_SCROLLVIEW
  ElementCache = SchematicDoc::cropSelectedElements();
#else
  assert(scene());
  scene()->selectedItemsAndBoundingBox(ElementCache, BB);
  assert(BB.isEmpty() == ElementCache.isEmpty());
#endif

  qreal _x1, _x2, _y1, _y2;
  BB.getCoords(&_x1, &_y1, &_x2, &_y2);
  _x1 = (_x1+_x2) * .5;   // center for rotation
  _y1 = (_y1+_y2) * .5;
  QPoint gp=setOnGrid(_x1, _y1);
  int x1=gp.x();
  int y1=gp.y();

  int x2;
  int y2;

  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  for(auto g: ElementCache) { untested();
    Element* pe=element(g);
    assert(pe);
    auto pw=dynamic_cast<Wire*>(pe);
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->rotate();   //rotate component !before! rotating its center
        pc->setCenter(pc->cy_() - y1 + x1, x1 - pc->cx_() + y1);
        insertRawComponent(pc);
        break;

      case isWire:
        x2 = pw->x1_();
        pw->x1__() = pw->y1_() - y1 + x1;
        pw->y1__() = x1 - x2 + y1;
        x2 = pw->x2_();
        pw->x2__() = pw->y2_() - y1 + x1;
        pw->y2__() = x1 - x2 + y1;
#if 0
        pl = pw->Label;
        if(pl) { untested();
          x2 = pl->cx_();
          pl->cx__() = pl->cy_() - y1 + x1;
          pl->cy__() = x1 - x2 + y1;
          if(pl->Type == isHWireLabel)
            pl->Type = isVWireLabel;
          else pl->Type = isHWireLabel;
        }
#endif
        insertWire(pw);
        break;

      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	x2 = pl->x1_();
	pl->x1__() = pl->y1_() - y1 + x1;
	pl->y1__() = x1 - x2 + y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0) { untested();
	  x2 = pl->x1_();
	  pl->x1__() = pl->y1_() - y1 + x1;
	  pl->y1__() = x1 - x2 + y1;
	}
	x2 = pl->cx_();
	pl->cx__() = pl->cy_() - y1 + x1;
	pl->cy__() = x1 - x2 + y1;
	insertNodeLabel(pl);
	break;

      case isPainting:
        pp = (Painting*)pe;
        pp->rotate();   // rotate painting !before! rotating its center
        pp->getCenter(x2, y2);
        //qDebug("pp->getCenter(x2, y2): (%i,%i)\n", x2, y2);
        //qDebug("(x1,y1) (x2,y2): (%i,%i) (%i,%i)\n", x1,y1,x2,y2);
        pp->setCenter(y2-y1 + x1, x1-x2 + y1);
        paintings().append(pp);
        break;
      default: ;
    }
  }

  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components.
// First copy them to 'ElementCache', then mirror and insert again.
bool SchematicDoc::mirrorXComponents()
{ untested();
  QRectF BB(1., 1., -1. , -1.);
  QList<ElementGraphics*> ElementCache;

#ifdef USE_SCROLLVIEW
  ElementCache = SchematicDoc::cropSelectedElements();
#else
  assert(scene());
  scene()->selectedItemsAndBoundingBox(ElementCache, BB);
  assert(BB.isEmpty() == ElementCache.isEmpty());
#endif

  qreal _x1, _x2, _y1, _y2;
  BB.getCoords(&_x1, &_y1, &_x2, &_y2);
  _y1 = (_y1+_y2) * .5;

  QPoint gp=setOnGrid(_y2, _y1);
  int y2=gp.x();
  int y1=gp.y();

  y1 *= 2;

  int x2;

  Wire *pw;
  Painting  *pp;
  Component *pc; (void) pc;
  WireLabel *pl;
  // re-insert elements
  for(ElementGraphics *g : ElementCache) { untested();
    Element* pe=element(g);

    assert(pe);
    if(auto c=component(pe)){ untested();
	c->mirrorX();   // mirror component !before! mirroring its center
	c->setCenter(c->cx_(), y1 - c->cy_());
    }else{ untested();
      incomplete();
    }
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
	break;
      case isWire:
	pw = (Wire*)pe;
	pw->y1__() = y1 - pw->y1_();
	pw->y2__() = y1 - pw->y2_();
#if 0
	pl = pw->Label;
	if(pl){ untested();
          pl->cy__() = y1 - pl->cy_();
        }else{ untested();
        }
#endif
	break;
      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	pl->y1__() = y1 - pl->y1_();
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0)
	  pl->y1__() = y1 - pl->y1_();
	pl->cy__() = y1 - pl->cy_();
	break;
      case isPainting:
	pp = (Painting*)pe;
	pp->getCenter(x2, y2);
	pp->mirrorX();   // mirror painting !before! mirroring its center
	pp->setCenter(x2, y1 - y2);
	break;
      default: ;
    }
  }

  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components. First copy them to 'ElementCache', then mirror and insert again.
bool SchematicDoc::mirrorYComponents()
{ untested();

  QRectF BB(1., 1., -1. , -1.);
  QList<ElementGraphics*> ElementCache;
#ifdef USE_SCROLLVIEW
  ElementCache = SchematicDoc::cropSelectedElements();
#else
  assert(scene());
  scene()->selectedItemsAndBoundingBox(ElementCache, BB);
  assert(BB.isEmpty() == ElementCache.isEmpty());
#endif

  qreal _x1, _x2, _y1, _y2;
  BB.getCoords(&_x1, &_y1, &_x2, &_y2);

  _x1 = (_x1+_x2) * .5;
  QPoint gp=setOnGrid(_x1, _x2);
  int x1=gp.x();
  int x2=gp.y();
  int y2;
  x1 *= 2.;

  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  for(ElementGraphics *g : ElementCache) { untested();
    Element* pe=element(g);
    assert(pe);
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->mirrorY();   // mirror component !before! mirroring its center
        pc->setCenter(x1 - pc->cx_(), pc->cy_());
        insertRawComponent(pc);
        break;
      case isWire:
        pw = (Wire*)pe;
        pw->x1__() = x1 - pw->x1_();
        pw->x2__() = x1 - pw->x2_();
#if 0
        pl = pw->Label;
        if(pl) { untested();
          pl->cx__() = x1 - pl->cx_();
        }else{ untested();
        }
#endif
        insertWire(pw);
        break;
      case isHWireLabel:
      case isVWireLabel:
        pl = (WireLabel*)pe;
        pl->x1__() = x1 - pl->x1_();
        break;
      case isNodeLabel:
        pl = (WireLabel*)pe;
        if(pl->pOwner == 0){ untested();
          pl->x1__() = x1 - pl->x1_();
        }
        pl->cx__() = x1 - pl->cx_();
        insertNodeLabel(pl);
        break;
      case isPainting:
        pp = (Painting*)pe;
        pp->getCenter(x2, y2);
        pp->mirrorY();   // mirror painting !before! mirroring its center
        pp->setCenter(x1 - x2, y2);
        paintings().append(pp);
        break;
      default: ;
    }
  }

  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Updates the graph data of all diagrams (load from data files).
void SchematicDoc::reloadGraphs()
{ untested();
  QFileInfo Info(docName());
  for(auto pd : diagrams()){ untested();
    pd->loadGraphData(Info.path()+QDir::separator()+DataSet);
  }
}

// Copy function, 
void SchematicDoc::copy()
{ untested();
  QString s = _model->createClipboardFile();
  QClipboard *cb = QApplication::clipboard();  // get system clipboard
  if (!s.isEmpty()) { untested();
    cb->setText(s, QClipboard::Clipboard);
  }
}

// ---------------------------------------------------
// Cut function, copy followed by deletion
void SchematicDoc::cut()
{ untested();
  copy();
  deleteElements(); //delete selected elements
  viewport()->update();
}


// ---------------------------------------------------
// Loads this Qucs document.
bool SchematicDoc::load()
{ untested();
  assert(_model);
  _model->clear();

  if(!loadDocument()){ untested();
    return false;
  }else{ untested();
    // Keep reference to source file (the schematic file)
    // setFileInfo(DocName);
  }
  lastSaved = QDateTime::currentDateTime();

  while(!undoAction.isEmpty()) { untested();
    delete undoAction.last();
    undoAction.pop_back();
  }
  undoActionIdx = 0;
  while(!undoSymbol.isEmpty()) { untested();
    delete undoSymbol.last();
    undoSymbol.pop_back();
  }
  setSymbolMode(true);
  setChanged(false, true); // "not changed" state, but put on undo stack
  undoSymbolIdx = 0;
  undoSymbol.at(undoSymbolIdx)->replace(1, 1, 'i');
  setSymbolMode(false);
  setChanged(false, true); // "not changed" state, but put on undo stack
  undoActionIdx = 0;
  undoAction.at(undoActionIdx)->replace(1, 1, 'i');

  // The undo stack of the circuit symbol is initialized when first
  // entering its edit mode.
  
  // have to call this to avoid crash at sizeOfAll
  becomeCurrent(false);

  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(ViewX1 > UsedX1)  ViewX1 = UsedX1;
  if(ViewY1 > UsedY1)  ViewY1 = UsedY1;
  if(ViewX2 < UsedX2)  ViewX2 = UsedX2;
  if(ViewY2 < UsedY2)  ViewY2 = UsedY2;
  zoom(1.0f);
  TODO("Fix setContentsPos");
  /// \todo setContentsPos(tmpViewX1, tmpViewY1);
  tmpViewX1 = tmpViewY1 = -200;   // was used as temporary cache
  return true;
}

// ---------------------------------------------------
// Saves this Qucs document. Returns the number of subcircuit ports.
int SchematicDoc::save()
{ untested();
  int result = adjustPortNumbers();// same port number for schematic and symbol
  saveDocument();

  QFileInfo Info(docName());
  lastSaved = Info.lastModified();

  if(result >= 0) { untested();
    setChanged(false);

    QVector<QString *>::iterator it;
    for (it = undoAction.begin(); it != undoAction.end(); it++) { untested();
      (*it)->replace(1, 1, ' '); //at(1) = ' '; state of being changed
    }
    //(1) = 'i';   // state of being unchanged
    undoAction.at(undoActionIdx)->replace(1, 1, 'i');

    for (it = undoSymbol.begin(); it != undoSymbol.end(); it++) { untested();
      (*it)->replace(1, 1, ' '); //at(1) = ' '; state of being changed
    }
    //at(1) = 'i';   // state of being unchanged
    undoSymbol.at(undoSymbolIdx)->replace(1, 1, 'i');
  }
  // update the subcircuit file lookup hashes
  QucsMain->updateSchNameHash();
  QucsMain->updateSpiceNameHash();

  return result;
}

// ---------------------------------------------------
// If the port number of the schematic and of the symbol are not
// equal add or remove some in the symbol.
// BUG: this is far to complex.
// (not even sure, what it is trying to do...)
int SchematicDoc::adjustPortNumbers()
{ untested();
  int x1, x2, y1, y2;
  // get size of whole symbol to know where to place new ports
  if(isSymbolMode()) { untested();
    sizeOfAll(x1, y1, x2, y2);
    incomplete();
  } else { untested();
//    Components = &SymbolComps;
//    Wires      = &SymbolWires;
//    // Nodes      = &SymbolNodes;
//    Diagrams   = &SymbolDiags;
//    Paintings  = &SymbolPaints;
    sizeOfAll(x1, y1, x2, y2);
//    Components = &DocComps;
//    Wires      = &DocWires;
//    // Nodes      = &DocNodes;
//    Diagrams   = &DocDiags;
//    Paintings  = &DocPaints;
  }
  x1 += 40;
  y2 += 20;
  setOnGrid(x1, y2);


  Painting *pp;
  // delete all port names in symbol
  for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
    if(pp->name() == ".PortSym ")
      ((PortSymbol*)pp)->nameStr = "";

  QString Str;
  int countPort = 0;

  QFileInfo Info (DataDisplay);
  QString Suffix = Info.suffix();

  // handle VHDL file symbol
#if 0
  if (Suffix == "vhd" || Suffix == "vhdl") { untested();
    QStringList::iterator it;
    QStringList Names, GNames, GTypes, GDefs;
    int Number;

    // get ports from VHDL file
    QFileInfo Info(docName());
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

    for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
      if(pp->Name == ".ID ") { untested();
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.EntityName.toUpper();
	id->Parameter.clear();
	if (!VInfo.GenNames.isEmpty())
	  GNames = VInfo.GenNames.split(",", QString::SkipEmptyParts);
	if (!VInfo.GenTypes.isEmpty())
	  GTypes = VInfo.GenTypes.split(",", QString::SkipEmptyParts);
	if (!VInfo.GenDefs.isEmpty())
	  GDefs = VInfo.GenDefs.split(",", QString::SkipEmptyParts);;
	for(Number = 1, it = GNames.begin(); it != GNames.end(); ++it) { untested();
	  id->Parameter.append(new SubParameter(
 	    true,
	    *it+"="+GDefs[Number-1],
	    tr("generic")+" "+QString::number(Number),
	    GTypes[Number-1]));
	  Number++;
	}
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) { untested();
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else { untested();
	symbolPaintings().append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }else
#endif
  // handle Verilog-HDL file symbol
  if (Suffix == "v") { untested();
	  incomplete();
#if 0

    QStringList::iterator it;
    QStringList Names;
    int Number;

    // get ports from Verilog-HDL file
    QFileInfo Info (docName());
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

    for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
      if(pp->Name == ".ID ") { untested();
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.ModuleName.toUpper();
	id->Parameter.clear();
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) { untested();
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else { untested();
	symbolPaintings().append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
#endif
  }else if (Suffix == "va") { untested();
	  incomplete();
#if 0

    QStringList::iterator it;
    QStringList Names;
    int Number;

    // get ports from Verilog-A file
    QFileInfo Info (docName());
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

    for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
      if(pp->Name == ".ID ") { untested();
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.ModuleName.toUpper();
	id->Parameter.clear();
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) { untested();
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else { untested();
	symbolPaintings().append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
#endif
  }else{ untested();
      // go through all components in a schematic
      for(auto pc : components()){ untested();
         if(pc->obsolete_model_hack() == "Port") { // BUG. move to device.
             countPort++;

             Str = pc->Props.getFirst()->Value;
             // search for matching port symbol
             for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
             { untested();
                 if(pp->name() == ".PortSym ")
                 { untested();
                   if(((PortSymbol*)pp)->numberStr == Str) break;
                 }
             }

             if(pp) { untested();
                 ((PortSymbol*)pp)->nameStr = pc->name();
             } else { untested();
	         Painting* ps=painting_dispatcher.clone("PortSymbol");
                 ps->setSomeArgsHack(x1, y2, Str, pc->name());
                 symbolPaintings().append(ps);
                 y2 += 40;
             }
          }
      }
  }

  // delete not accounted port symbols
  for(pp = SymbolPaints.first(); pp!=0; ) { untested();
    if(pp->name() == ".PortSym ")
      if(((PortSymbol*)pp)->nameStr.isEmpty()) { untested();
        symbolPaintings().remove();
        pp = symbolPaintings().current();
        continue;
      }
    pp = symbolPaintings().next();
  }

  return countPort;
}

// ---------------------------------------------------
#if 0 // does not work
bool SchematicDoc::undo()
{ untested();
  if(isSymbolMode()) { untested();
    if (undoSymbolIdx == 0) { return false; }

    rebuildSymbol(undoSymbol.at(--undoSymbolIdx));
    adjustPortNumbers();  // set port names

    emit signalUndoState(undoSymbolIdx != 0);
    emit signalRedoState(undoSymbolIdx != undoSymbol.size()-1);

    if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i' && 
        undoAction.at(undoActionIdx)->at(1) == 'i') { untested();
      setChanged(false, false);
      return true;
    }

    setChanged(true, false);
    return true;
  }


  // ...... for schematic edit mode .......
  if (undoActionIdx == 0) { return false; }

  rebuild(undoAction.at(--undoActionIdx));
  reloadGraphs();  // load recent simulation data

  emit signalUndoState(undoActionIdx != 0);
  emit signalRedoState(undoActionIdx != undoAction.size()-1);

  if(undoAction.at(undoActionIdx)->at(1) == 'i') { untested();
    if(undoSymbol.isEmpty()) { untested();
      setChanged(false, false);
      return true;
    }
    else if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i') { untested();
      setChanged(false, false);
      return true;
    }
  }

  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
bool SchematicDoc::redo()
{ untested();
  if(isSymbolMode()) { untested();
    if (undoSymbolIdx == undoSymbol.size() - 1) { return false; }

    rebuildSymbol(undoSymbol.at(++undoSymbolIdx));
    adjustPortNumbers();  // set port names

    emit signalUndoState(undoSymbolIdx != 0);
    emit signalRedoState(undoSymbolIdx != undoSymbol.size()-1);

    if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i'
        && undoAction.at(undoActionIdx)->at(1) == 'i') { untested();
      setChanged(false, false);
      return true;
    }

    setChanged(true, false);
    return true;
  }


  //
  // ...... for schematic edit mode .......
  if (undoActionIdx == undoAction.size()-1) { return false; }

  rebuild(undoAction.at(++undoActionIdx));
  reloadGraphs();  // load recent simulation data

  emit signalUndoState(undoActionIdx != 0);
  emit signalRedoState(undoActionIdx != undoAction.size()-1);

  if (undoAction.at(undoActionIdx)->at(1) == 'i') { untested();
    if(undoSymbol.isEmpty()) { untested();
      setChanged(false, false);
      return true;
    }
    else if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i') { untested();
      setChanged(false, false);
      return true;
    }
    
  }

  setChanged(true, false);
  return true;
}
#endif

// ---------------------------------------------------
// Sets selected elements on grid.
bool SchematicDoc::elementsOnGrid()
{ untested();
  int x, y, No;
  bool count = false;
  WireLabel *pl, *pLabel;
  Q3PtrList<WireLabel> LabelCache;

  // test all components
  for(auto elt : selectedItems()){ untested();
    if(auto pc=component(elt)){ untested();

      // rescue non-selected node labels
#if 0
      foreach(Port *pp, pc->Ports){ untested();
        if(pp->Connection->Label){ untested();
          if(pp->Connection->connectionsCount() < 2) { untested();
            LabelCache.append(pp->Connection->Label);
            pp->Connection->Label->pOwner = 0;
            pp->Connection->Label = 0;
          }
	}
      }
#endif

      x = pc->cx_();
      y = pc->cy_();
     // No = components().at();
     // deleteComp(pc); // TODO
      pc->snapToGrid(*this); // setOnGrid(pc->cx__(), pc->cy__());
     // insertRawComponent(pc); // TODO
     // components().at(No);   // restore current list position
      // pc->setSelected(false);
      count = true;

      x -= pc->cx_();
      y -= pc->cy_();    // re-insert node labels and correct position
      for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next()) { untested();
        pl->cx__() -= x;
        pl->cy__() -= y;
        insertNodeLabel(pl);
      }
      LabelCache.clear();
    }
  }

#ifdef USE_SCROLLVIEW
  // test all wires and wire labels
  for(Wire *pw = wires().last(); pw != 0; pw = wires().prev()) { untested();
    pl = pw->Label;
    pw->Label = 0;

    if(pw->isSelected()) { untested();
      // rescue non-selected node label
      pLabel = 0;
      if(pw->Port1->Label) { untested();
        if(pw->Port1->connectionsCount() < 2) { untested();
            pLabel = pw->Port1->Label;
            pw->Port1->Label = 0;
        }
      }
      else if(pw->Port2->Label) { untested();
        if(pw->Port2->connectionsCount() < 2) { untested();
            pLabel = pw->Port2->Label;
            pw->Port2->Label = 0;
        }
      }

      No = wires().at();
      deleteWire(pw);
      setOnGrid(pw->x1__(), pw->y1__());
      setOnGrid(pw->x2__(), pw->y2__());
      insertWire(pw);
      wires().at(No);   // restore current list position
      pw->setSelected(false);
      count = true;
      if(pl){ untested();
        setOnGrid(pl->cx__(), pl->cy__());
      }

      if(pLabel) { untested();
        setOnGrid(pLabel->cx__(), pLabel->cy__());
        insertNodeLabel(pLabel);
      }
    }

    if(pl) { untested();
      pw->Label = pl;
      if(pl->isSelected()) { untested();
        setOnGrid(pl->x1__(), pl->y1__());
        pl->setSelected(false);
        count = true;
      }
    }
  }
  wires().setAutoDelete(true);

  for(auto pn : nodes()) { untested();
    if(pn->Label){ untested();
      if(pn->Label->isSelected()) { untested();
        setOnGrid(pn->Label->x1__(), pn->Label->y1__());
        pn->Label->setSelected(false);
        count = true;
      }else{ untested();
      }
    }else{ untested();
    }
  }

  for(Diagram *pd = diagrams().last(); pd != 0; pd = diagrams().prev()) { untested();
    if(pd->isSelected()) { untested();
      setOnGrid(pd->cx__(), pd->cy__());
      pd->setSelected(false);
      count = true;
    }

    foreach(Graph *pg,pd->Graphs)
      // test markers of diagram
      foreach(Marker *pm, pg->Markers)
        if(pm->isSelected()) { untested();
	  x = pm->x1_() + pd->cx_();
	  y = pm->y1_() + pd->cy_();
	  setOnGrid(x, y);
	  pm->x1__() = x - pd->cx_();
	  pm->y1__() = y - pd->cy_();
	  pm->setSelected(false);
	  count = true;
        }
  }

  // test all paintings
  for(Painting *pa = paintings().last(); pa != 0; pa = paintings().prev())
    if(pa->isSelected()) { untested();
      pa->snapToGrid(*this);
      pa->setSelected(false);
      count = true;
    }

#endif
  if(count) setChanged(true, true);
  return count;
}

// ---------------------------------------------------
void SchematicDoc::switchPaintMode()
{ untested();
  // BUG. this messes with SchematicModel functions
  setSymbolMode(!isSymbolMode());

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


// -----------------------------------------------------------
// Scrolls the visible area upwards and enlarges or reduces the view
// area accordingly.
bool SchematicDoc::scrollUp(int step)
{ untested();
#ifndef USE_SCROLLVIEW
  incomplete();
  (void) step;
#else
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
#endif
  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area downwards and enlarges or reduces the view
// area accordingly. ("step" must be negative!)
bool SchematicDoc::scrollDown(int step)
{ untested();
#ifndef USE_SCROLLVIEW
  incomplete();
  (void) step;
#else
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
#endif
  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the left and enlarges or reduces the view
// area accordingly.
bool SchematicDoc::scrollLeft(int step)
{ untested();
#ifndef SCROLLVIEW
  incomplete();
  (void) step;
#else
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
#endif
  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the right and enlarges or reduces the
// view area accordingly. ("step" must be negative!)
bool SchematicDoc::scrollRight(int step)
{ untested();
#ifndef SCROLLVIEW
  incomplete();
  (void) step;
#else
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
#endif
  return true;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollUp()
{ untested();
  App->editText->setHidden(true);  // disable edit of component property
  scrollUp(verticalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollDown()
{ untested();
  App->editText->setHidden(true);  // disable edit of component property
  scrollDown(-verticalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollLeft()
{ untested();
  App->editText->setHidden(true);  // disable edit of component property
  scrollLeft(horizontalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollRight()
{ untested();
  App->editText->setHidden(true);  // disable edit of component property
  scrollRight(-horizontalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}


// *********************************************************************
// **********                                                 **********
// **********        Function for serving drag'n drop         **********
// **********                                                 **********
// *********************************************************************

// Is called if an object is dropped (after drag'n drop).

// ---------------------------------------------------

// ---------------------------------------------------

#if 0
void SchematicDoc::getSelAreaWidthAndHeight(int &wsel, int &hsel, int& xmin_sel_, int& ymin_sel_)
{ untested();
    int xmin= INT_MAX,
        ymin= INT_MAX,
        xmax= INT_MIN,
        ymax= INT_MIN;

     for(Component *pc = components().first(); pc != 0; pc = components().next()) { untested();
         if (pc->isSelected) { untested();
           int x1,y1,x2,y2,d1,d2,d3,d4;
           pc->entireBounds(x1,y1,x2,y2,this->textCorr());
           d1 = std::min(x1,x2);
           if (d1<xmin) xmin = d1;
           d2 = std::max(x2,x1);
           if (d2>xmax) xmax = d2;
           d3 = std::min(y1,y2);
           if (d3<ymin) ymin = d3;
           d4 = std::max(y2,y1);
           if (d4>ymax) ymax = d4;
         }
    }

    for(Wire *pw = wires().first(); pw != 0; pw = wires().next()) { untested();

        if (pw->isSelected) { untested();
            int xc,yc;
            pw->getCenter(xc,yc);

            if (xc<xmin) xmin = xc;
            if (xc>xmax) xmax = xc;
            if (yc<ymin) ymin = yc;
            if (yc>ymax) ymax = yc;
        }
    }

    for(Diagram *pd = diagrams().first(); pd != 0; pd = diagrams().next()) { untested();



        if (pd->isSelected) { untested();
            int x1,y1,x2,y2,d1,d2,d3,d4;
            pd->Bounding(x1,y1,x2,y2);

            d1 = std::min(x1,x2);
            if (d1<xmin) xmin = d1;
            d2 = std::max(x2,x1);
            if (d2>xmax) xmax = d2;
            d3 = std::min(y1,y2);
            if (d3<ymin) ymin = d3;
            d4 = std::max(y2,y1);
            if (d4>ymax) ymax = d4;
        }
    }

    for(Painting *pp = paintings().first(); pp != 0; pp = paintings().next()) { untested();

       if (pp->isSelected) { untested();
           int x1,y1,x2,y2,d1,d2,d3,d4;
           pp->Bounding(x1,y1,x2,y2);
           d1 = std::min(x1,x2);
           if (d1<xmin) xmin = d1;
           d2 = std::max(x2,x1);
           if (d2>xmax) xmax = d2;
           d3 = std::min(y1,y2);
           if (d3<ymin) ymin = d3;
           d4 = std::max(y2,y1);
           if (d4>ymax) ymax = d4;
       }
    }

    wsel = abs(xmax - xmin);
    hsel = abs(ymax - ymin);
    xmin_sel_ = xmin;
    ymin_sel_ = ymin;
}
#endif

// insert an Element into the schematic.
void SchematicDoc::simpleInsertElement(Element* e)
{ untested();
  assert(false); // use PushBack
	if(Wire* c=dynamic_cast<Wire*>(e)){ untested();
		simpleInsertWire(c);
	}else if(Component* c=dynamic_cast<Component*>(e)){ untested();
		simpleInsertComponent(c);
	}else if(Command* c=dynamic_cast<Command*>(e)){ untested();
		simpleInsertCommand(c);
	}else{ untested();
		incomplete();
	}

}
// ===============================================================
void PaintingList::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax) const
{ untested();
  xmin=INT_MAX;
  ymin=INT_MAX;
  xmax=INT_MIN;
  ymax=INT_MIN;
  int x1, y1, x2, y2;

  // BUG, where is this used? //
  if(isEmpty()) { untested();
    xmin = xmax = 0;
    ymin = ymax = 0;
    return;
  }

  for(auto pp : *this) { untested();
    pp->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
}

Component* SchematicDoc::find_component(QString const& n)
{ untested();
    for(auto pc : components()){ untested();
       if(pc->name() == n){ untested();
	 return pc;
       }
    }
    return nullptr;
}

// ---------------------------------------------------

#if 0
namespace{
class ins : public SchematicSymbol{
public:
  ins(SchematicDoc* m) : _m(m) { untested();
    assert(m);
  }
private:
  void pushBack(Element* e){ untested();
    _m->pushBack(e);
  }
  void setParameter(std::string const&, std::string){ untested();
    incomplete();
  }

  PaintingList& symbolPaintings() { untested();
    return _m->symbolPaints();
  }

private: // SchematicSymbol
  SchematicModel const& schematicModel() const{ untested();
    assert(_m);
    return _m->DocModel;
  }
  SchematicModel* schematicModel() { untested();
    assert(_m);
    return &_m->DocModel;
  }
  std::string getParameter(std::string const&) const{ untested();
    return "ins, incomplete";
  }
private:
  SchematicDoc* _m;
};
}
#endif


#if 0 // transition. obsolete
void SchematicDoc::parse(DocumentStream& s, SchematicLanguage const* L)
{ untested();
  if(!L){ untested();
    auto D=doclang_dispatcher["leg_sch"];
    L = dynamic_cast<SchematicLanguage const*>(D);
  }else{ untested();
  }
  assert(L);
  ins i(this);
  while(!s.atEnd()){ untested();
    qDebug() << "entering parse";
    L->parse(s, i);
    assert(s.atEnd()); // happens with legacy lang
  }
}
#endif

// pre qt5 kludge
void SchematicDoc::pushBack(Element* what)
{ untested();
  qDebug() << "SchematicDoc::pushBack" << what;
#ifndef USE_SCROLLVIEW
  incomplete();
  assert(_model);
  _model->pushBack(what);
#else
  if(auto c=command(what)){ untested();
    incomplete();
    simpleInsertCommand(c);
  }else if(auto c=component(what)){ untested();
    qDebug() << "sic" << c->label();
    simpleInsertComponent(c);
  }else if(auto d=diagram(what)){ untested();
    diagrams().append(d);
  }else if(auto w=wire(what)){ untested();
    qDebug() << "got wire";
    simpleInsertWire(w);
  }else if(auto s=dynamic_cast<SchematicSymbol*>(what)){ untested();
    unreachable();
   //  delete _symbol;
    // _symbol = s;
  }else{ untested();
    incomplete();
  }
#endif
}

// ----------------------------------------------------------------
QString SchematicDoc::getParameter(std::string const& x) const
{ untested();
  if(x=="DocName"){ untested();
    return docName();
  }else if(x=="ViewX1"){ untested();
    return QString(ViewX1);
  }else{ untested();
    throw "Exception_cantfind";
  }
}

// ----------------------------------------------------------------
void SchematicDoc::removeWire(Wire const* x)
{ untested();
#ifndef USE_SCROLLVIEW
incomplete();
//  scene()->removeItem(x);
#else
  wires().removeRef((Wire*)x);
#endif
}

void SchematicModel::removeNode(Node const* x)
{ untested();
#ifndef USE_SCROLLVIEW
  incomplete();
  //scene()->removeItem(x);
#else
#endif
  nodes().erase((Node*)x);
}

QList<ElementGraphics*> SchematicDoc::selectedItems()
{ itested();
  assert(scene());
  // TODO/BUG: proper iterator.
  auto L = scene()->selectedItems();
  auto EL = reinterpret_cast<QList<ElementGraphics*>* >(&L);
  return *EL;
}

// vim:ts=8:sw=2:noet
