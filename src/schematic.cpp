/***************************************************************************
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

#include "qucs_app.h"
#include "node.h"
#include "viewpainter.h"
#include "misc.h"
#include "qucs_globals.h"
#include "io_trace.h"
#include "painting.h"

#include "qt_compat.h"

class QPrinter;

//ElementList SymbolComps;


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
      if(z & 1) x -= Painter.fontMetrics().width(pn->name());
      if(!(z & 2)) { untested();
        y -= (Painter.LineSpacing>>1) + 4;
        if(z & 1) x -= 4;
        else x += 4;
      }
      if(z & 0x10)
        Painter.setPen(Qt::darkGreen);  // green for currents
      else
        Painter.setPen(Qt::blue);   // blue for voltages
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
          Painter.setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
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
        Painter.setPen(Qt::DotLine);
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

void SchematicDoc::PostPaintEvent (PE pe, int x1, int y1, int x2, int y2, int a, int b, bool PaintOnViewport)
{ itested();
  PostedPaintEvent p = {pe, x1,y1,x2,y2,a,b,PaintOnViewport};
  PostedPaintEvents.push_back(p);
  updateViewport();
  update();
}
#endif

// -----------------------------------------------------------

#if 0
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
#endif

// -----------------------------------------------------------
// ---------------------------------------------------

// ---------------------------------------------------


// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
// BUG: fp?

// ---------------------------------------------------
#if 0
void SchematicDoc::paintGrid(ViewPainter *, int , int, int, int )
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
{itested();
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
#endif

// ---------------------------------------------------
// use boundingRect instead
#if 0
void SchematicModel::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax, float) const
{itested();
  xmin=INT_MAX;
  ymin=INT_MAX;
  xmax=INT_MIN;
  ymax=INT_MIN;
  unreachable();
//  WireLabel *pl;

  if(components().isEmpty())
    if(wires().isEmpty())
      if(diagrams().isEmpty())
        if(paintings().isEmpty()) {itested();
          xmin = xmax = 0;
          ymin = ymax = 0;
          return;
        }


  int x1, y1, x2, y2;
  // find boundings of all components

  float Corr = textCorr;
  for(auto pc : components()) {itested();
    pc->entireBounds(x1, y1, x2, y2, Corr);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
#endif

#if 0
  // find boundings of all wires
  for(auto pw : wires()) {itested();
    if(pw->x1_() < xmin) xmin = pw->x1_();
    if(pw->x2_() > xmax) xmax = pw->x2_();
    if(pw->y1_() < ymin) ymin = pw->y1_();
    if(pw->y2_() > ymax) ymax = pw->y2_();

    pl = pw->Label;
    if(pl) {     // check position of wire label
        pl->getLabelBounding(x1,y1,x2,y2);
        if(x1 < xmin) xmin = x1;
        if(x2 > xmax) xmax = x2;
        if(y1 < ymin) ymin = y1;
        if(y2 > ymax) ymax = y2;
    }
  }
#endif

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

  // find boundings of all diagrams
  for(auto pd : diagrams()) {itested();
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
#endif

// ---------------------------------------------------

// ---------------------------------------------------


// ---------------------------------------------------


#if 0
// ---------------------------------------------------
// If the port number of the schematic and of the symbol are not
// equal add or remove some in the symbol.
// BUG: this is far to complex.
// (not even sure, what it is trying to do...)
int SchematicDoc::adjustPortNumbers()
{ untested();
#if 0
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
         if(pc->typeName() == "Port") { // BUG. move to device.
             countPort++;

             // Str = pc->Props.getFirst()->Value;
	     // try
             Str = QString::fromStdString(pc->paramValue("portname"));
             // search for matching port symbol
             for(pp = symbolPaintings().first(); pp!=0; pp = symbolPaintings().next())
             { untested();
                 if(pp->name() == ".PortSym ")
                 { untested();
                   if(((PortSymbol*)pp)->numberStr == Str) break;
                 }
             }

             if(pp) { untested();
                 ((PortSymbol*)pp)->nameStr = pc->label();
             } else { untested();
	         Painting* ps=painting_dispatcher.clone("PortSymbol");
                 ps->setSomeArgsHack(x1, y2, Str, pc->label());
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
#endif
  return 0;
}
#endif

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
#if 0
bool SchematicDoc::elementsOnGrid()
{ untested();
  incomplete();
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
  return 0;
}
#endif

// ---------------------------------------------------
#if 0
void SchematicDoc::switchPaintMode()
{ untested();
  // BUG. this messes with SchematicModel functions
  // setSymbolMode(!isSymbolMode());

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
#endif


// *********************************************************************
// **********                                                 **********
// **********      Function for serving mouse wheel moving    **********
// **********                                                 **********
// *********************************************************************


#if 0
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
  assert(_app);
  _app->editText->setHidden(true);  // disable edit of component property
  scrollUp(verticalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollDown()
{ untested();
  assert(_app);
  _app->editText->setHidden(true);  // disable edit of component property
  scrollDown(-verticalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollLeft()
{ untested();
  assert(_app);
  _app->editText->setHidden(true);  // disable edit of component property
  scrollLeft(horizontalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void SchematicDoc::slotScrollRight()
{ untested();
  assert(_app);
  _app->editText->setHidden(true);  // disable edit of component property
  scrollRight(-horizontalScrollBar()->singleStep());
  updateViewport();
  // App->view->drawn = false;
}

#endif


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

// ===============================================================
#if 0
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
#endif

#if 0
Component* SchematicDoc::find_component(QString const&)
{ untested();
  assert(false);
    for(auto pc : components()){ untested();
       if(pc->label() == n){ untested();
	 incomplete();
	 return dynamic_cast<Component*>(pc);
       }
    }
    return nullptr;
}
#endif

// ----------------------------------------------------------------
#if 0
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
#endif

// ----------------------------------------------------------------

#if 0
void SchematicModel::removeNode(Node const* x)
{ untested();
#ifndef USE_SCROLLVIEW
  incomplete();
  //scene()->removeItem(x);
#else
#endif
  nodes().erase((Node*)x);
}
#endif

// vim:ts=8:sw=2:noet
