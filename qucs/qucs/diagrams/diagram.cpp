/***************************************************************************
                          diagram.cpp  -  description
                             -------------------
    begin                : Thu Oct 2 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "diagram.h"

#include <math.h>
#include <float.h>

#include <qtextstream.h>
#include <qmessagebox.h>
#include <qregexp.h>



Diagram::Diagram(int _cx, int _cy)
{
  cx = _cx;  cy = _cy;

  ymin = xmin = xlow = ylow = 0.0;
  ymax = xmax = xup  = yup  = 1.0;

  Type = isDiagram;
  isSelected = false;
  
  Graphs.setAutoDelete(true);
  Markers.setAutoDelete(true);
  Arcs.setAutoDelete(true);
  Lines.setAutoDelete(true);
  Texts.setAutoDelete(true);
}

Diagram::~Diagram()
{
}

// ------------------------------------------------------------
void Diagram::paint(QPainter *p)
{
  // paint all lines
  for(Line *pl = Lines.first(); pl != 0; pl = Lines.next()) {
    p->setPen(pl->style);
    p->drawLine(cx+pl->x1, cy-pl->y1, cx+pl->x2, cy-pl->y2);
  }

  // paint all arcs
  for(Arc *pa = Arcs.first(); pa != 0; pa = Arcs.next()) {
    p->setPen(pa->style);
    p->drawArc(cx+pa->x, cy-pa->y, pa->w, pa->h, pa->angle, pa->arclen);
  }

  if(Name[0] != 'T') {   // no graph within tabulars
    Graph *pg;
    QRect r;
    int delta=y1;

    if(xLabel.isEmpty())
      // draw all graphs
      for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
        pg->paint(p, cx, cy);
	// get width of text
        r = p->boundingRect(0,0,0,0,Qt::AlignAuto,pg->IndepVar);
        p->drawText(cx+((x2-r.width())>>1), cy+delta, pg->IndepVar);
        delta += r.height();
      }
    else {
      // draw all graphs
      for(pg = Graphs.first(); pg != 0; pg = Graphs.next())
        pg->paint(p, cx, cy);
      // get width of text
      r = p->boundingRect(0,0,0,0,Qt::AlignAuto,xLabel);
      p->setPen(QColor(0,0,0));
      p->drawText(cx+((x2-r.width())>>1), cy+delta, xLabel);
    }

    delta = x1;
    p->save();
    p->rotate(270);
    if(yLabel.isEmpty()) {
      // draw y-label for all graphs
      for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
        p->setPen(pg->Color);
	// get width of text
        r = p->boundingRect(0,0,0,0,Qt::AlignAuto,pg->Line);
        p->drawText(-cy+((y2-r.width())>>1), cx-delta, pg->Line);
        delta += r.height();
      }
    }
    else {
        // get width of text
        r = p->boundingRect(0,0,0,0,Qt::AlignAuto,yLabel);
        p->setPen(QColor(0,0,0));
        p->drawText(-cy+((y2-r.width())>>1), cx-delta, yLabel);
    }
    p->restore();

    // draw markers
    for(Marker *pm = Markers.first(); pm != 0; pm = Markers.next())
      pm->paint(p, cx, cy);
  }

  p->setPen(QPen(QPen::black,1));
  // write whole text
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next())
    p->drawText(cx+pt->x, cy-pt->y, pt->s);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRect(cx-5, cy-y2-5, x2+10, y2+10);
    p->setPen(QPen(QPen::darkRed,2));
    p->drawRect(cx-5, cy-y2-5, 10, 10);  // markers for changing the size
    p->drawRect(cx-5, cy-5, 10, 10);
    p->drawRect(cx+x2-5, cy-y2-5, 10, 10);
    p->drawRect(cx+x2-5, cy-5, 10, 10);
  }
}

// ------------------------------------------------------------
void Diagram::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy-y2, x2, y2);
}

// ------------------------------------------------------------
void Diagram::calcDiagram()
{
}

// ------------------------------------------------------------
void Diagram::calcData(Graph *)
{
}

// -------------------------------------------------------
void Diagram::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx-x1;
  _y1 = cy-y2;
  _x2 = cx+x2;
  _y2 = cy+y1;
}

// -------------------------------------------------------
bool Diagram::getSelected(int x_, int y_)
{
  if(x_ >= cx-x1) if(x_ <= cx+x2+5) if(y_ >= cy-y2-5) if(y_ <= cy+y1)
    return true;

  return false;
}

// ------------------------------------------------------------
// Checks if the resize area was clicked. If so return "true" and sets x1/y1 and x2/y2
// to the border coordinates to draw a rectangle.
bool Diagram::ResizeTouched(int& MAx1, int& MAy1, int& MAx2, int& MAy2)
{
  int _x1, _y1, _x2, _y2;

  if(MAx1 < cx+5) {
    _x2 = -x2;
    _x1 = cx-_x2;
  }
  else {
    if(MAx1 <= cx+x2-5) return false;
    _x2 = x2;
    _x1 = cx;
  }
  if(MAy1 > cy-5) {
    _y2 = y2;
    _y1 = cy-_y2;
  }
  else {
    if(MAy1 >= cy-y2+5) return false;
    _y2 = -y2;
    _y1 = cy;
  }

  MAx1 = _x1;  MAy1 = _y1;
  MAx2 = _x2;  MAy2 = _y2;
  return true;
}

// --------------------------------------------------------------------------
void Diagram::loadGraphData(const QString& defaultDataSet)
{
  ymin = xmin = DBL_MAX;
  ymax = xmax = -DBL_MAX;

  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    loadVarData(defaultDataSet);  // load data, determine max/min values

  if((ymin > ymax) || (xmin > xmax)) {
    ymin = xmin = 0.0;
    ymax = xmax = 1.0;
  }

  updateGraphData();
}

// --------------------------------------------------------------------------
void Diagram::updateGraphData()
{
  calcDiagram();

  Graph *pg;
  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    calcData(pg);   // calculate graph coordinates

  int n, *pi;
  double *pp;
  for(Marker *pm = Markers.first(); pm != 0; pm = Markers.next()) {
    pg = Graphs.at(pm->GraphNum);
    pp = pg->cPointsX;
    if(pp == 0) continue;
    for(n=0; n<pg->count; n++) {
      if(pm->xpos <= *pp) break;
      pp++;
    }
    if(n == pg->count) pp--;
    pm->xpos = *pp;
    pm->yr = *((pg->cPointsY)+2*n);  pm->yi = *((pg->cPointsY)+2*n+1);
    pm->createText();
    pi = pg->Points + 2*n;
    pm->cx = *pi;
    pm->cy = *(pi+1);
  }
}

// --------------------------------------------------------------------------
bool Diagram::loadVarData(const QString& fileName)
{
  Graph *g = Graphs.current();
  g->count = 0;
  if(g->Points != 0) { delete[] g->Points;  g->Points = 0; }
  if(g->cPointsX != 0) { delete[] g->cPointsX;  g->cPointsX = 0; }
  if(g->cPointsY != 0) { delete[] g->cPointsY;  g->cPointsY = 0; }
  if(g->Line.isEmpty()) return false;

  QFile file(fileName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load dataset: ")+fileName);
    return false;
  }

  // *****************************************************************
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();
  file.close();


  QString Line, tmp;
  // *****************************************************************
  // look for variable name in data file  ****************************
  int i=0, j=0, k=0;
  i = FileString.find('<')+1;
  if(i > 0)
  do {
    j = FileString.find('>', i);
    Line = FileString.mid(i, j-i);
    i = FileString.find('<', j)+1;
    if(Line.left(3) == "dep") {
      tmp = Line.section(' ', 1, 1);
      if(g->Line != tmp) continue;  // found variable with name sought for ?
      tmp = Line.section(' ', 2, 2);   // name of independent variable
      break;
    }
    if(Line.left(5) == "indep") {
      tmp = Line.section(' ', 1, 1);
      if(g->Line != tmp) continue;  // found variable with name sought for ?
      tmp = "";        // no independent variable
      break;
    }
  } while(i > 0);

  if(i <= 0)  return false;   // return if data name was not found
  g->IndepVar = tmp;    // name of independet variable (could be empty!)

  // *****************************************************************
  // get independent variable ****************************************
  bool ok=true, ok2=true;
  double *p;
  int counting;
  if(g->IndepVar.isEmpty()) {    // create independent variable by myself ?
    g->IndepVar = "number";

    tmp = Line.section(' ', 2, 2);  // get number of points
    counting = tmp.toInt(&ok);
    if(!ok) {
      QMessageBox::critical(0, QObject::tr("Error"),
                   QObject::tr("Cannot get size of independent data \"")+
		   g->Line+"\"");
      return false;
    }

    p = new double[counting];  // memory of new independent variable
    g->cPointsX = p;
    for(int z=1; z<=counting; z++)  *(p++) = double(z);
    xmin = 1.0;
    xmax = double(counting);
  }
  else {
    // get independent variable
    counting = loadIndepVarData(g->IndepVar, FileString);
    if(counting <= 0) {     // failed to load independent variable ?
      if(g->cPointsX != 0) { delete[] g->cPointsX;  g->cPointsX = 0; }
      return false;
    }
  }

  g->Points = new int[2*counting];   // create memory for points
  g->count  = counting;

  // *****************************************************************
  // get dependent variable ******************************************
  p = new double[2*counting];  // memory of new independent variable
  g->cPointsY = p;
  double x, y;
  QRegExp WhiteSpace("\\s");
  QRegExp noWhiteSpace("\\S");
  i = FileString.find(noWhiteSpace, j+1);
  j = FileString.find(WhiteSpace, i);
  Line = FileString.mid(i, j-i);
  for(int z=0; z<counting; z++) {
    k = Line.find('j');
    if(k < 0) {
      x = Line.toDouble(&ok);
      y = 0.0;
    }
    else {
      tmp = Line.mid(k);  // imaginary part
      tmp.at(0) = Line.at(k-1);   // copy sign over "j"
      y = tmp.toDouble(&ok);
      Line = Line.left(k-1);  // real part
      x = Line.toDouble(&ok2);
    }
    if((!ok) || (!ok2)) {
      QMessageBox::critical(0, QObject::tr("Error"),
                   QObject::tr("Too few dependent data \"")+
		   g->Line+"\"");
      if(g->cPointsX != 0) { delete[] g->cPointsX;  g->cPointsX = 0; }
      if(g->cPointsY != 0) { delete[] g->cPointsY;  g->cPointsY = 0; }
      g->count = 0;
      return false;
    }
    *(p++) = x;
    *(p++) = y;
    if(fabs(y) >= 1e-250) x = sqrt(x*x+y*y);
    if(x > ymax) ymax = x;
    if(x < ymin) ymin = x;

    i = FileString.find(noWhiteSpace, j);
    j = FileString.find(WhiteSpace, i);
    Line = FileString.mid(i, j-i);
  }

  return true;
}

// --------------------------------------------------------------------------
// Reads the data of an independent variable. Returns the number of points.
int Diagram::loadIndepVarData(const QString& var, const QString& FileString)
{
  QString Line, tmp;

  int i=0, j=0;
  i = FileString.find('<')+1;
  if(i > 0)
  do {    // look for variable name in data file
    j = FileString.find('>', i);
    Line = FileString.mid(i, j-i);
    i = FileString.find('<', j)+1;
    if(Line.left(5) == "indep") {
      tmp = Line.section(' ', 1, 1);
      if(var == tmp) break;     // found variable with name sought for ?
    }
  } while(i > 0);

  if(i <= 0) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Independent data \"")+var+
		 QObject::tr("\" not found"));
    return -1;
  }

  bool ok;
  tmp = Line.section(' ', 2, 2);  // get number of points
  int n = tmp.toInt(&ok);
  if(!ok) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot get size of independent data \"")+
		 var+"\"");
    return -1;
  }

  double *p = new double[n];     // memory for new independent variable
  Graphs.current()->cPointsX = p;

  double x;
  QRegExp WhiteSpace("\\s");
  QRegExp noWhiteSpace("\\S");
  i = FileString.find(noWhiteSpace, j+1);
  j = FileString.find(WhiteSpace, i);
  Line = FileString.mid(i, j-i);
  for(int z=0; z<n; z++) {
    x = Line.toDouble(&ok);  // get number
    if(!ok) {
      QMessageBox::critical(0, QObject::tr("Error"),
                   QObject::tr("Too few independent data\"")+
		   var+"\"");
      return -1;
    }
    *(p++) = x;
    if(x > xmax) xmax = x;
    if(x < xmin) xmin = x;

    i = FileString.find(noWhiteSpace, j);
    j = FileString.find(WhiteSpace, i);
    Line = FileString.mid(i, j-i);
  }

  return n;   // return number of independent data
}

// ------------------------------------------------------------
void Diagram::setCenter(int x, int y, bool relative)
{
  if(relative) {
    cx += x;  cy += y;
  }
  else {
    cx = x;  cy = y;
  }
}

// ------------------------------------------------------------
Diagram* Diagram::newOne()
{
  return new Diagram();
}

// ------------------------------------------------------------
QString Diagram::save()
{
  QString s = "   <"+Name+" "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  if(GridOn) s+= "1 ";
  else s += "0 ";
  s += "\""+xLabel+"\" \""+yLabel+"\">\n";

  for(Graph *pg=Graphs.first(); pg != 0; pg=Graphs.next())
    s += pg->save()+"\n";

  for(Marker *pm=Markers.first(); pm != 0; pm=Markers.next())
    s += pm->save()+"\n";

  s += "   </"+Name+">";
  return s;
}

// ------------------------------------------------------------
bool Diagram::load(const QString& Line, QTextStream *stream)
{
  bool ok;
  QString s = Line;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // GridOn
  if(n.toInt(&ok) == 1) GridOn = true;
  else GridOn = false;
  if(!ok) return false;

  xLabel = s.section('"',1,1);    // xLabel
  yLabel = s.section('"',3,3);    // yLabel

  // .......................................................
  // load graphs of the diagram
  while(!stream->atEnd()) {
    s = stream->readLine();
    s = s.stripWhiteSpace();
    if(s == ("</"+Name+">")) return true;  // found end tag ?
    if(s.section(' ', 0,0) == "<Mkr") break;
    Graph *pg = new Graph();
    if(!pg->load(s)) {
      delete pg;
      return false;
    }
    Graphs.append(pg);
  }

  // .......................................................
  // load markers of the diagram
  while(!stream->atEnd()) {
    Marker *pm = new Marker(this);
    if(!pm->load(s)) {
      delete pm;
      return false;
    }
    Markers.append(pm);
    s = stream->readLine();
    s = s.stripWhiteSpace();
    if(s == ("</"+Name+">")) return true;  // found end tag ?
  }

  return false;   // end tag missing
}
