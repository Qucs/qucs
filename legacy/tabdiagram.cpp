/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QPolygon>
#include <QPainter>

#include "diagram.h"
#include "qucs_app.h"
#include <cmath>
#include "misc.h"
#include "qucs_globals.h"
#include "module.h"

#include "some_font_stuff.h"
#include "obsolete_stuff.h"

namespace{
using namespace qucs;

class TabDiagram : public Diagram  {
private:
	TabDiagram(TabDiagram const& d) : Diagram(d) {}
public: 
  TabDiagram(int _cx=0, int _cy=0);
 ~TabDiagram();

  Diagram* newOne() { unreachable(); return nullptr; }
  Element* clone() const { return new TabDiagram(*this); }
  static Element* info(QString&, char* &, bool getNewOne=false);
  virtual void paint(ViewPainter*);
  virtual void paintDiagram(ViewPainter *p);
  virtual int calcDiagram();
  int scroll(int);
  bool scrollTo(int, int, int);

  void createAxisLabels() {};   // no labels in this diagram

public: // it's not actually a diagram. so what?
  virtual diag_coordinate_t calcCoordinate(double const& x, double const& y) const override{
	  return diag_coordinate_t();
  }

public: // legacy cruft.
  QList<Graph *>  Graphs;
  QList<Arc *>    Arcs;
  QList<Line *>   Lines;
  QList<Text *>   Texts;

protected:
  void calcData(Graph*) {};  // no graph data
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Tab", &D);
Module::INSTALL pp("diagrams", &D);

TabDiagram::TabDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 0;    // no extension to select area
  y1 = 0;
  _width = x3 = 300;  // initial size of diagram
  _height = 200;
  Name = "Tab";
  xAxis.limit_min = 0.0;  // scroll bar position (needs to be saved in file)

  // setName("Tab");
  calcDiagram();
}

TabDiagram::~TabDiagram()
{
}

void TabDiagram::paint(ViewPainter *p)
{
    paintDiagram(p);
}

// ------------------------------------------------------------
void TabDiagram::paintDiagram(ViewPainter *p)
{
	auto cx=0; Element::cx();
	auto cy=0; Element::cy();
  // paint all lines
  foreach(Line *pl, Lines) {
    p->setPen(pl->style);
    p->drawLine(cx+pl->x1, cy-pl->y1, cx+pl->x2, cy-pl->y2);
  }

  if(x1 > 0) {  // paint scroll bar ?
    int   x, y, dx, dy;
    QPolygon Points;
    y = _height - 20;
    // draw scroll bar
    int by = cy-y + yAxis.numGraphs;
    p->fillRect(cx-14, by+1, 12, zAxis.numGraphs-1, QColor(192, 192, 192));

    // draw frame for scroll bar
    p->setPen(QPen(Qt::black,0));
    p->drawLine(cx-17, cy-_height, cx-17, cy);
    p->drawLine(cx-17, cy-_height, cx, cy-_height);
    p->drawLine(cx-17, cy, cx, cy);
    y += 2;
    p->drawLine(cx-17, cy-y, cx, cy-y);
    y -= _height;
    p->drawLine(cx-17, cy+y, cx, cy+y);

    // draw the arrows above and below the scroll bar
    p->setBrush(QColor(192, 192, 192));
    p->setPen(QColor(152, 152, 152));
    p->drawLine(cx-2, by, cx-2, by + zAxis.numGraphs);
    p->drawLine(cx-15, by + zAxis.numGraphs, cx-2, by + zAxis.numGraphs);

    p->map(cx-14, cy-_height+3, x, y);
    p->map(cx-3,  cy-_height+14, dx, dy);
    Points.setPoints(3, x, dy, (x+dx)>>1, y, dx, dy);
    p->drawConvexPolygon(Points);
    p->setPen(QColor(224, 224, 224));
    p->drawLine(x, dy, (x+dx)>>1, y);
    p->drawLine(cx-15, by, cx-2, by);
    p->drawLine(cx-15, by, cx-15, by + zAxis.numGraphs);

    p->setPen(QColor(152, 152, 152));
    dy -= y;
    p->map(cx-14,  cy-3, x, y);
    Points.setPoints(3, x, y-dy, (x+dx)>>1, y, dx, y-dy);
    p->drawConvexPolygon(Points);
    p->setPen(QColor(208, 208, 208));
    p->drawLine(x, y-dy, (x+dx)>>1, y);
    p->setPen(QColor(224, 224, 224));
    p->drawLine(x, y-dy, dx, y-dy);

    p->setBrush(QBrush(Qt::NoBrush));
  }


  p->setPen(Qt::black);
  // write whole text
  foreach(Text *pt, Texts)
    p->drawText(pt->s, cx+pt->x, cy-pt->y);


#if 0
  if(isSelected()) {
    p->setPen(QPen(Qt::darkGray,3));
    p->drawRect(cx-5, cy-_height-5, x2+10, _height+10);
    p->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy-_height);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy-_height);
    p->drawResizeRect(cx+x2, cy);
  }
#endif
}

// ------------------------------------------------------------
// calculates the text in the tabular
int TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x1 = 0;  // no scroll bar
  x3 = _width;
  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=8, y;

  if(_height < (41 + MIN_SCROLLBAR_SIZE))
    _height = 41 + MIN_SCROLLBAR_SIZE;

  if(_height < (tHeight + 8))
    _height = tHeight + 8;
  y = _height - tHeight - 6;

  // outer frame
  Lines.append(new Line(0, _height, _width, _height, QPen(Qt::black,0)));
  Lines.append(new Line(0, _height, 0, 0, QPen(Qt::black,0)));
  Lines.append(new Line(_width, _height, _width, 0, QPen(Qt::black,0)));
  Lines.append(new Line(0, 0, _width, 0, QPen(Qt::black,0)));
  Lines.append(new Line(0, y+2, _width, y+2, QPen(Qt::black,2)));

  if(xAxis.limit_min < 0.0)
    xAxis.limit_min = 0.0;

//  Graph *firstGraph;

#if 0
  QListIterator<Graph *> ig(Graphs);
  Graph *g = 0;
  if (ig.hasNext()) // point to first graph
    g = ig.next();

  if(g == 0) {  // no variables specified in diagram ?
    Str = QObject::tr("no variables");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, _height);
    if(colWidth >= 0)
      Texts.append(new Text(x-4, _height-2, Str)); // independent variable
    return 0;
  }
#endif

//  int NumAll=0;   // how many numbers per column
//  int NumLeft=0;  // how many numbers could not be written

//  double *py, *px;
//  int counting, invisibleCount=0;
//  int startWriting, lastCount = 1;

#if 0 // interface change. possibly refactored somewhere.
  // any graph with data ?
  while(g->isEmpty()) {
    if (!ig.hasNext()) break; // no more graphs
    g = ig.next(); // point to next graph
  }

  if(!g->isEmpty()) { // did we find a graph with data ?
    // ................................................
    counting = g->axis(0)->count * g->countY;  // number of values
    NumAll = counting;
    
    invisibleCount = counting - y/tHeight;
    if(invisibleCount <= 0)  xAxis.limit_min = 0.0;// height bigger than needed
    else {
      NumLeft = invisibleCount - int(xAxis.limit_min + 0.5);
      if(invisibleCount < int(xAxis.limit_min + 0.5))
	xAxis.limit_min = double(invisibleCount); // adjust limit of scroll bar
    }
    
    for(int h = g->numAxes(); h>0;){
		DataX const *pD = g->axis(--h); // BUG
      colWidth = 0;
      Str = pD->Var;
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, _height);
      if(colWidth < 0)  goto funcEnd;
      startWriting = int(xAxis.limit_min + 0.5);  // when to reach visible area
      
      Texts.append(new Text(x-4, _height-2, Str)); // independent variable
      if(pD->count != 0) {
	y = _height-tHeight-5;
	counting /= pD->count;   // how many rows to be skipped
	for(int z1=0; z1<lastCount; z1++) {
	  px = pD->Points;
	  for(int z=pD->count; z>0; z--) {
	    if(startWriting <= 0) { // reached visible area ?
	      y += tHeight*startWriting;
	      startWriting = 0;
	      if(y < tHeight) break;  // no room for more rows ?
	      Str = misc::StringNum(*px, 'g', g->Precision);
	      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
	      if(colWidth < 0)  goto funcEnd;
	      
	      Texts.append(new Text( x, y, Str));
	      y -= tHeight*counting;
	    }
	    else startWriting -= counting;
	    px++;
	  }
	  if(pD == g->axis(0))   // only paint one time
	    if(y >= tHeight) if(y < _height-tHeight-5)
	      Lines.append(new Line(0, y+1, x2, y+1, QPen(Qt::black,0)));
	}
	lastCount *= pD->count;
      }
      x += colWidth+15;
      Lines.append(new Line(x-8, _height, x-8, 0, QPen(Qt::black,0)));
    }
    Lines.last()->style = QPen(Qt::black,2);
  }  // of "if no data in graphs"

  
  firstGraph = g;
  // ................................................
  // all dependent variables
  foreach(Graph *g, Graphs) {
    y = _height-tHeight-5;
    colWidth = 0;

    Str = g->Var;
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, _height);
    if(colWidth < 0)  goto funcEnd;
    Texts.append(new Text(x, _height-2, Str));  // dependent variable


    startWriting = int(xAxis.limit_min + 0.5); // when to reach visible area
    py = g->cPointsY - 2;
    if(g->axis(0)) {

      if (!g->cPointsY) {   // no data points
	Str = QObject::tr("invalid");
	colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
	if(colWidth < 0)  goto funcEnd;
	Texts.append(new Text(x, y, Str));
      }
      else if(sameDependencies(g, firstGraph)) {
        int z=g->axis(0)->count * g->countY;
        if(z > NumAll)  NumAll = z;

        if(g->Var.right(2) != ".X")
          for(; z>0; z--) {
            py += 2;
            if(startWriting-- > 0) continue; // reached visible area ?
            if(y < tHeight) break;           // no room for more rows ?
            switch(g->numMode) {
              case 0: Str = misc::complexRect(*py, *(py+1), g->Precision); break;
              case 1: Str = misc::complexDeg (*py, *(py+1), g->Precision); break;
              case 2: Str = misc::complexRad (*py, *(py+1), g->Precision); break;
            }

            colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
            if(colWidth < 0)  goto funcEnd;

            Texts.append(new Text(x, y, Str));
            y -= tHeight;
          }

        else {  // digital data
          char *pcy = (char*)g->cPointsY;
          for(; z>0; z--) {
            if(startWriting-- > 0) {  // reached visible area ?
              pcy += strlen(pcy) + 1;
              continue;
            }
            if(y < tHeight) break;           // no room for more rows ?
            Str = QString(pcy);

            colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
            if(colWidth < 0)  goto funcEnd;

            Texts.append(new Text(x, y, Str));
            pcy += strlen(pcy) + 1;
            y -= tHeight;
          }
        }

        if(z > NumLeft)  NumLeft = z;
      }  // of "if(sameDeps)"
      else {
        Str = QObject::tr("wrong dependency");
        colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
        if(colWidth < 0)  goto funcEnd;
        Texts.append(new Text(x, y, Str));
      }
    }
    else {   // no data in graph
      Str = QObject::tr("no data");
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
      if(colWidth < 0)  goto funcEnd;
      Texts.append(new Text(x, y, Str));
    }
    x += colWidth+15;
    if(g != Graphs.last())   // do not paint last line
      Lines.append(new Line(x-8, _height, x-8, 0, QPen(Qt::black,0)));
  }

funcEnd:
  if(invisibleCount > 0) {  // could all numbers be written ?
    x1 = 18;   // extend the select area to the left

    zAxis.limit_max = double(NumAll);  // number of data (rows) 

    // calculate data for painting scroll bar
    y = int(xAxis.limit_min + 0.5);
    NumLeft = NumAll - NumLeft - y;

    // position of scroll bar in pixel
    yAxis.numGraphs = (_height - 39) * y / NumAll;

    // height of scroll bar
    zAxis.numGraphs = (_height - 39) * NumLeft / NumAll;
    if(zAxis.numGraphs < MIN_SCROLLBAR_SIZE) {
      yAxis.numGraphs -= (MIN_SCROLLBAR_SIZE - zAxis.numGraphs + 1)
                         * y / NumAll;
      zAxis.numGraphs = MIN_SCROLLBAR_SIZE;
    }

    xAxis.numGraphs = NumLeft;  // number of lines in the diagram
  }

  return 1;
#endif
  return 0;
}

// ------------------------------------------------------------
int TabDiagram::scroll(int clickPos)
{
//	auto cx=Element::cx();
	auto cy=Element::cy();
  if(x1 <= 0) return 0;   // no scroll bar ?
  int tmp = int(xAxis.limit_min + 0.5);

  int y = cy;
  if(clickPos > (cy-20)) {  // scroll one line down ?
    xAxis.limit_min++;
  }
  else {
    y -= _height - 20;
    if(clickPos < y) {  // scroll bar one line up ?
      if(xAxis.limit_min <= 0.0)  return 0;
      xAxis.limit_min--;
    }
    else {
      y += yAxis.numGraphs;
      if(clickPos < y)   // scroll bar one page up ?
        xAxis.limit_min -= double(xAxis.numGraphs);
      else {
        y += zAxis.numGraphs;
        if(clickPos > y)   // a page down?
          xAxis.limit_min += double(xAxis.numGraphs);
        else
          return 2;  // click on position bar
      }
    }
  }

  calcDiagram();
  if(tmp == int(xAxis.limit_min + 0.5))
    return 0;   // did anything change ?

  return 1;
}

// ------------------------------------------------------------
bool TabDiagram::scrollTo(int initial, int, int dy)
{
  int tmp = int(xAxis.limit_min + 0.5);
  xAxis.limit_min  = double(initial);
  xAxis.limit_min += double(dy) / double(_height - 39) * zAxis.limit_max;
  xAxis.limit_min  = floor(xAxis.limit_min + 0.5);

  calcDiagram();
  if(tmp == int(xAxis.limit_min + 0.5))
    return false;   // did anything change ?

  return true;
}


// ------------------------------------------------------------
#if 0
Element* TabDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Tabular");
  BitmapFile = (char *) "tabular";

  if(getNewOne)  return new TabDiagram();
  return 0;
}

// "Apply" button pressed
void DiagramDialog::slotButtApply()
{
  if(Diag->name().at(0) == 'T') {
	  // tabular or timing
  }else{
    if(Diag->xAxis.Label.isEmpty())
      Diag->xAxis.Label = ""; // can be not 0 and empty!
    if(xLabel->text().isEmpty()) xLabel->setText("");
    if(Diag->xAxis.Label != xLabel->text()) {
      Diag->xAxis.Label = xLabel->text();
      changed = true;
    }
    if(Diag->yAxis.Label.isEmpty())
      Diag->yAxis.Label = ""; // can be not 0 and empty!
    if(ylLabel->text().isEmpty()) ylLabel->setText("");
    if(Diag->yAxis.Label != ylLabel->text()) {
      Diag->yAxis.Label = ylLabel->text();
      changed = true;
    }
    
    if(GridOn) if(Diag->xAxis.GridOn != GridOn->isChecked()) {
      Diag->xAxis.GridOn = GridOn->isChecked();
      Diag->yAxis.GridOn = GridOn->isChecked();
      changed = true;
    }
    if(GridColorButt) {
      QColor gridColBut = misc::getWidgetBackgroundColor(GridColorButt);
      if(Diag->GridPen.color() != gridColBut) {
        Diag->GridPen.setColor(gridColBut);
        changed = true;
      }
    }
    if(GridStyleBox)
      if(Diag->GridPen.style()!=(Qt::PenStyle)(GridStyleBox->currentIndex()+1)) {
      Diag->GridPen.setStyle((Qt::PenStyle)(GridStyleBox->currentIndex()+1));
      changed = true;
    }
    if((Diag->name() != "Smith") && (Diag->name() != "Polar")) {
      if(Diag->zAxis.Label.isEmpty())
        Diag->zAxis.Label = ""; // can be not 0 and empty!
      if(yrLabel->text().isEmpty()) yrLabel->setText("");
      if(Diag->zAxis.Label != yrLabel->text()) {
        Diag->zAxis.Label = yrLabel->text();
        changed = true;
      }
    }

    if(Diag->name().left(4) == "Rect") {
      if(Diag->xAxis.log != GridLogX->isChecked()) {
        Diag->xAxis.log = GridLogX->isChecked();
        changed = true;
      }
      if(Diag->yAxis.log != GridLogY->isChecked()) {
        Diag->yAxis.log = GridLogY->isChecked();
        changed = true;
      }
      if(Diag->zAxis.log != GridLogZ->isChecked()) {
        Diag->zAxis.log = GridLogZ->isChecked();
        changed = true;
      }
    }

    if((Diag->name() == "Smith") || (Diag->name() == "ySmith") ||
				  (Diag->name() == "PS"))
      if(stopY->text().toDouble() < 1.0)
        stopY->setText("1");

    if(Diag->name() == "SP")
      if(stopZ->text().toDouble() < 1.0)
        stopZ->setText("1");

    if(Diag->xAxis.autoScale == manualX->isChecked()) {
      Diag->xAxis.autoScale = !(manualX->isChecked());
      changed = true;
    }

    // Use string compares for all floating point numbers, in
    // order to avoid rounding problems.
    if(QString::number(Diag->xAxis.limit_min) != startX->text()) {
      Diag->xAxis.limit_min = startX->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->xAxis.step) != stepX->text()) {
      Diag->xAxis.step = stepX->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->xAxis.limit_max) != stopX->text()) {
      Diag->xAxis.limit_max = stopX->text().toDouble();
      changed = true;
    }
    if(Diag->yAxis.autoScale == manualY->isChecked()) {
      Diag->yAxis.autoScale = !(manualY->isChecked());
      changed = true;
    }
    if(QString::number(Diag->yAxis.limit_min) != startY->text()) {
      Diag->yAxis.limit_min = startY->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->yAxis.step) != stepY->text()) {
      Diag->yAxis.step = stepY->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->yAxis.limit_max) != stopY->text()) {
      Diag->yAxis.limit_max = stopY->text().toDouble();
      changed = true;
    }
    if(Diag->zAxis.autoScale == manualZ->isChecked()) {
      Diag->zAxis.autoScale = !(manualZ->isChecked());
      changed = true;
    }
    if(QString::number(Diag->zAxis.limit_min) != startZ->text()) {
      Diag->zAxis.limit_min = startZ->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->zAxis.step) != stepZ->text()) {
      Diag->zAxis.step = stepZ->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->zAxis.limit_max) != stopZ->text()) {
      Diag->zAxis.limit_max = stopZ->text().toDouble();
      changed = true;
    }

#if 0 //BUG
    if(hideInvisible)
      if(((Rect3DDiagram*)Diag)->hideLines != hideInvisible->isChecked()) {
        ((Rect3DDiagram*)Diag)->hideLines = hideInvisible->isChecked();
        changed = true;
      }

    if(rotationX)
      if(((Rect3DDiagram*)Diag)->rotX != rotationX->text().toInt()) {
        ((Rect3DDiagram*)Diag)->rotX = rotationX->text().toInt();
        changed = true;
      }

    if(rotationY)
      if(((Rect3DDiagram*)Diag)->rotY != rotationY->text().toInt()) {
        ((Rect3DDiagram*)Diag)->rotY = rotationY->text().toInt();
        changed = true;
      }

    if(rotationZ)
      if(((Rect3DDiagram*)Diag)->rotZ != rotationZ->text().toInt()) {
        ((Rect3DDiagram*)Diag)->rotZ = rotationZ->text().toInt();
        changed = true;
      }
#endif

  }   // of "if(Diag->Name != "Tab")"

  Diag->Graphs.clear();   // delete the graphs
  Graphs.setAutoDelete(false);
  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    Diag->Graphs.append(pg);  // transfer the new graphs to diagram
  Graphs.clear();
  Graphs.setAutoDelete(true);

  Diag->loadGraphData(defaultDataSet);
  ((SchematicDoc*)parent())->viewport()->repaint();
  copyDiagramGraphs();
  if(changed) transfer = true;   // changes have been applied ?
}
#endif

#if 0 // also used in 2 others?
int Diagram::checkColumnWidth(const QString& Str,
		const FontMetrics& metrics, int colWidth, int x, int y)
{
  //qDebug("%i", metrics.charWidth(Str,0));
  int w = metrics.boundingRect(Str).width();  // width of text
  if(w > colWidth) {
    colWidth = w;
    if((x+colWidth) >= x2) {    // enough space for text ?
      // mark lack of space with a small arrow
      Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(Qt::red,2)));
      Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(Qt::red,2)));
      Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(Qt::red,2)));
      return -1;
    }
  }
  return colWidth;
}
#endif

}
