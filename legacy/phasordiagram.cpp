/*
 * phasordiagram.cpp - Implementation file for the phasor diagram
 *
 * Copyright (C) 2016, Nelson Martins, macedo.pp@gmail.com
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt	
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

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif


#include <QMessageBox>

#include "phasordiagram.h"
#include "qucs_app.h"
#include "misc.h"

// found this somewhere.
  /* PHASOR DIAGRAM RELATED
  Axis  xAxisV, yAxisV, zAxisV, xAxisI, yAxisI, zAxisI, xAxisP, yAxisP, zAxisP, xAxisZ, yAxisZ,
 zAxisZ, *xAxisA, *yAxisA, *zAxisA;*/
//draws the vectors of phasor diagram
void Graph::drawvect(int x0, int y0, ViewPainter *p) const
{
  float DX_, DY_;
  double beta, phi;
  QPolygon Points;
  auto Painter = p->Painter;
  QPen pen = Painter->pen();
  auto Scale = p->Scale;

  Painter->setPen(pen);
  QPainterPath path;
  auto pp = begin();
  if(!pp->isPt())
    pp++;

  DX_ = p->DX + float(x0)*Scale;
  DY_ = p->DY + float(y0)*Scale;
  
  float x1, y1,x2,y2,x3,y3,x4,y4;

  while(!pp->isGraphEnd())
  {
    if(!pp->isBranchEnd())//draws the main line
    {
      x1=DX_ + pp->getScrX()*Scale;
      y1=DY_ - (pp++)->getScrY()*Scale;
      x2=DX_ + pp->getScrX()*Scale;
      y2=DY_ - (pp++)->getScrY()*Scale;
      Painter->drawLine(QLineF(x1, y1, x2, y2));
    }
    else
    {
      pp++;
      continue;
    }

      phi = atan2(double(y2-y1), double(x2-x1));     
      beta = atan2(double(4), double(10));
      double alfa = beta+phi;
      double Length = sqrt(4*4+10*10);
      x3 = x2-int(Length*cos(alfa));
      y3 = y2-int(Length*sin(alfa));
      Painter->drawLine(QLineF(x3, y3, x2, y2));
      pp++;
 
      alfa = phi-beta;
      x4 = x2-int(Length*cos(alfa));
      y4 = y2-int(Length*sin(alfa));
      
      Painter->drawLine(QLineF(x4, y4, x2, y2));

  }

}
//for phasor diagram while detect with type of graph it is (voltage, current....) and save in the auxiliary axis
void PhasorDiagram::findaxisA(Graph *g) 
{ untested();
    QString var = g->Var;
    
    xAxisA = &xAxis;
    yAxisA = &yAxis;
    zAxisA = &zAxis;

    if(var.indexOf(".v",0,Qt::CaseSensitive) != -1)
    { untested();
      xAxisA = &xAxisV;
      yAxisA = &yAxisV;
      zAxisA = &zAxisV;
    }
    else if(var.indexOf(".i",0,Qt::CaseSensitive) != -1)
    { untested();
      xAxisA = &xAxisI;
      yAxisA = &yAxisI;
      zAxisA = &zAxisI;
    }
    else if(var.indexOf(".S",0,Qt::CaseSensitive) != -1)
    { untested();
      xAxisA = &xAxisP;
      yAxisA = &yAxisP;
      zAxisA = &zAxisP;
    }
    else if(var.indexOf(".Ohm",0,Qt::CaseSensitive) != -1)
    { untested();
      xAxisA = &xAxisZ;
      yAxisA = &yAxisZ;
      zAxisA = &zAxisZ;
    }
}

//will determine the value of the graph for one frequency
bool PhasorDiagram::findmatch(Graph *g , int m)
{ untested();
  double *px;
  double *pz = g->cPointsY + 2*m*g->axis(0)->count;
  int z;
  if(freq <= (double*) 0)
  { untested();
    freq=0;
    sfreq = "0 Hz";
    return false;
  } 
	px = g->axis(0)->Points;
	for(z=g->axis(0)->count; z>0; z--) {  // every point
	  if(*px == freq[nfreqa])
	  { untested();
	    g->gy = pz;//save value
	    return true;
	  }
	  ++px;
	  pz += 2;
	}
  return false;

}

PhasorDiagram::PhasorDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 2;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for text
  Name = "Phasor";
  
  calcDiagram();
}

PhasorDiagram::~PhasorDiagram()
{
}
// --------------------------------------------------------------
void PhasorDiagram::finishMarkerCoordinates(float& fCX, float& fCY) const
{
  if(!insideDiagram(fCX, fCY)) {
	  fCX = fCY = 0.0;
  }
}

// ------------------------------------------------------------
void PhasorDiagram::calcCoordinatePh(const double* yD,float *px, float *py, Axis const *pa,Axis const *pt) const
{
 double yr = yD[0];
 double yi = yD[1];
 	

  *px = float((yr - pt->low) / (pt->up - pt->low) * double(x2));
  *py = float((yi - pa->low) / (pa->up - pa->low) * double(y2));
 
  if(std::isfinite(*px))
    if(std::isfinite(*py))
      return;

  *px = *py = 0.0;
 
}
// --------------------------------------------------------------
void PhasorDiagram::calcLimits()
{
  double a, b,c;
  calcAxisScale(&xAxis, a, b, c, xAxis.step, double(x2));
  xAxis.limit_min = xAxis.low;
  xAxis.limit_max = xAxis.up;
  calcAxisScale(&yAxis, a, b, c, yAxis.step, double(y2));
  yAxis.limit_min = yAxis.low;
  yAxis.limit_max = yAxis.up;
  calcAxisScale(&zAxis, a, b, c, zAxis.step, double(y2));
  zAxis.limit_min = zAxis.low;
  zAxis.limit_max = zAxis.up;
}

// --------------------------------------------------------------
int PhasorDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

 double GridStep, zD, zDstep, GridNum;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  y1 = QucsSettings.font.pointSize() + 6;

  x1 = 10;      // position of label text
  x3 = x2 + 7;
  QString tmp;
  int  z, valid = 0;

  // =====  give "step" the right sign (if user made it wrong)  ==============
  xAxis.step = fabs(xAxis.step);
  if(xAxis.limit_min > xAxis.limit_max)
    xAxis.step *= -1.0;

  yAxis.step = fabs(yAxis.step);
  if(yAxis.limit_min > yAxis.limit_max)
    yAxis.step *= -1.0;

  zAxis.step = fabs(zAxis.step);
  if(zAxis.limit_min > zAxis.limit_max)
    zAxis.step *= -1.0;


  // ====  x grid  =======================================================

  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));   // x tick marks
    zD += zDstep;
    z = int(zD);
  }
  
  if(xAxis.up >= 0.0) if(xAxis.low <= 0.0) {  // paint origin cross ?
    z = int(double(x2) * fabs(xAxis.low / (xAxis.up-xAxis.low)) + 0.5);
    Lines.append(new Line(z, 0, z, y2, QPen(Qt::black,0)));
  }
  // ====  y grid  =======================================================
  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 2;
  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0)) {
    valid |= 1;
    if(yAxis.up >= 0.0) if(yAxis.low <= 0.0) {  // paint origin cross ?
      z = int(double(y2) * fabs(yAxis.low / (yAxis.up-yAxis.low)) + 0.5);
      Lines.append(new Line(0, z, x2, z, QPen(Qt::black,0)));
    }
  }

  calcRestAxis(&xAxisV,&yAxisV,&zAxisV);
  calcRestAxis(&xAxisI,&yAxisI,&zAxisI);
  calcRestAxis(&xAxisP,&yAxisP,&zAxisP);
  calcRestAxis(&xAxisZ,&yAxisZ,&zAxisZ);

  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(Qt::black,0)));
  return valid;
}
// ------------------------------------------------------------
/*this function will compare the global limits with the specific limit of the graph
  to represent vector in a way that is all visible */
void PhasorDiagram::calcRestAxis(Axis *xA, Axis *yA , Axis *zA) 
{
       
    if(xAxis.max > 3.0 * xA->max)
    {
      xA->up  = 3.0 * xA->max * xAxis.up / xAxis.max;   
      xA->low = 3.0 * xA->min * xAxis.low / xAxis.min;
      yA->up  = 3.0 * yA->max * yAxis.up / yAxis.max;   
      yA->low = 3.0 * yA->min * yAxis.low / yAxis.min;
      zA->up  = 3.0 * zA->max * zAxis.up / zAxis.max;   
      zA->low = 3.0 * zA->min * zAxis.low / zAxis.min;
    }
    else
    {
      xA->up  = xAxis.up;   
      xA->low = xAxis.low;
      yA->up  = yAxis.up;   
      yA->low = yAxis.low;
      zA->up  = zAxis.up;   
      zA->low = zAxis.low;
    }
}
// ------------------------------------------------------------

void PhasorDiagram::calcData(Graph *g)
{
  double *pz = g->cPointsY;
  if(!pz)  return;
  if(g->numAxes() < 1) return;

  int z, i;
  int Size = ((2*(g->count(0)) + 1)*nfreqt * g->countY) + 10;

  g->resizeScrPoints(Size);
  auto p = g->begin();
  auto p_end = g->begin();
  
  p_end += Size - 9;   // limit of buffer
  p->setStrokeEnd();
  ++p;
  assert(p!=g->end());

  Axis *pa,*pt;
  findaxisA(g);
  pt = xAxisA;
  if(g->yAxisNo == 0)  pa = yAxisA;
  else  pa = zAxisA;

  double yf[] = {0.0,0.0};//points of the beginning of the vector 
  float xn,yn;
  float f1,f2; 
  char a=0; 
  double *yp;

  //meaning that it only works in AC 
  z = QString::compare(g->axis(0)->Var,"acfrequency",Qt::CaseInsensitive); 
  if(z != 0 )
  {
    sfreq = "0 Hz";
    p->setGraphEnd();
    return;
  }
  for(z=0;z<nfreqt;z++)
  {
    nfreqa=z;
    for(i=0;i<g->countY; i++) {  // every branch of curves
      if(!findmatch(g,i))//will find the y value of a certain frequency and curve
      {  
        p->setGraphEnd();
	return;
      }
	 
      yp = g->gy;
      calcCoordinatePh(yf, &f1, &f2, pa, pt);//coordinates of the begining of vector(0,0)
      p->setScr(f1, f2);
      if(!insideDiagramPh(p,&xn,&yn)) a=1;  // within diagram ?
	      		
      ++p;
      calcCoordinatePh(yp, &f1, &f2, pa,pt);//coordinates of the end of vector
      p->setScr(f1, f2);
      /*if the point(0,0) of vector isn't inside diagram, it will try to find new 
        coordinates close to the old coordinates that are within the diagram*/
      if(a==1)
      {
        if(newcoordinate(p,&xn,&yn)) (p-1)->setScr(xn, yn);//saves if a new point is found
	else //it will not create a vector
	{
          --p;
	  continue;  
	}
      }
      if(insideDiagramPh(p,&xn,&yn)) p++;   // if final point of vector is within diagram
      else//it will find a new one closer to the old ones
      {
        if(newcoordinate(p,&xn,&yn))//saves if a new point is found
        {
	  (p++)->setScr(xn, yn);
          (p++)->setScrX(-1);
        }
        else --p;//it will not create a vector
      }
      (p++)->setBranchEnd();
    }
  }
  p->setGraphEnd();
  return;
}

Diagram* PhasorDiagram::newOne()
{
  return new PhasorDiagram();
}

// ------------------------------------------------------------
Element* PhasorDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("AC Phasorial diagram");
  BitmapFile = (char *) "phasor";

  if(getNewOne)  return new PhasorDiagram();
  return 0;
}

#if 0 // later
//scales use in phasor and waveac this function only reset the value of the limits every scale
void Diagram::phasorscale() 
{ untested();
  xAxisV.min = xAxisI.min = xAxisP.min = xAxisZ.min = DBL_MAX;
  xAxisV.max = xAxisI.max = xAxisP.max = xAxisZ.max = -DBL_MAX;
  yAxisV.min = yAxisI.min = yAxisP.min = yAxisZ.min = DBL_MAX;
  yAxisV.max = yAxisI.max = yAxisP.max = yAxisZ.max = -DBL_MAX;
  zAxisV.min = zAxisI.min = zAxisP.min = zAxisZ.min = DBL_MAX;
  zAxisV.max = zAxisI.max = zAxisP.max = zAxisZ.max = -DBL_MAX;
}
/* RELATED TO PHASOR AND WAVEAC DIAGRAMS
// if the checkbox 'V' change stated
void DiagramDialog::PhasorvalV(int state)
{
  if(state == 2) {//if check add graph of type ".v" if exist
    addvar(".v");
  }
  else {//if uncheck remove graph of type ".v"
    remvar(".v");
  }
}

// if the checkbox 'I' change stated
void DiagramDialog::PhasorvalI(int state)
{
  if(state == 2) {//if check add graph of type ".i" if exist
    addvar(".i");
  }
  else {//if uncheck remove graph of type ".i"
    remvar(".i");
  }
}

//if the checkbox 'P' change stated
void DiagramDialog::PhasorvalP(int state)
{
  if(state == 2) {//if check add graph of type ".S" if exist
    addvar(".S");
  }
  else {//if uncheck remove graph of type ".S"
    remvar(".S");
  }
}
//if the checkbox 'Z' change stated
void DiagramDialog::PhasorvalZ(int state)
{
  if(state == 2) {//if check add graph of type ".Ohm" if exist
    addvar(".Ohm");
  }
  else {
    remvar(".Ohm");//if uncheck remove graph of type ".Ohm"
  }
}
//this function will find graph of a certain type and place on screen
void DiagramDialog::addvar(QString a)
{
  QFileInfo Info(defaultDataSet);
  QString DocName = ChooseData->currentText()+".dat";

  QFile file(Info.path() + QDir::separator() + DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return;
  }

  QString Line, tmp, Var;
  Var2 = "";
  //int varNumber = 0;
  // reading the file as a whole improves speed very much, also using
  // a QByteArray rather than a QString
  QByteArray FileString = file.readAll();
  file.close();

  
  int i=0, j=0, l=0;
  QList<QListWidgetItem *> m;

  for(i = GraphList->count()-1; i>=0; i--)
  {
    
      Var = GraphList->item(i)->text();

      if(Var.indexOf(a,0,Qt::CaseSensitive) != -1) return;
  }

  i = FileString.indexOf('<')+1;

  if(i > 0)
  do {
    j = FileString.indexOf('>', i);
    for(int k=0;k<j-i;k++) Line[k]=FileString[k+i];
    Line.truncate(j-i);
    i = FileString.indexOf('<', j)+1;

    Var2 = Line.section(' ', 1, 1).remove('>');
    if(Var2.length()>0)
      if(Var2.at(0) == '_')  continue;   

    m = GraphList->findItems(Var2, Qt::MatchExactly);
    l = Var2.indexOf(a,0,Qt::CaseSensitive);


    if( l != -1 && Var2.size() == (l + a.size()) && !(m.size()>0))//Var2.size == (l + a.size()) in case of voltage (.v) to don't let pass a variable like (name.var)
    {
      slotTakeVar(NULL);//In the case of the phasor diagram, the table ChooseVars is not used. Instead of that, the graph is put in the list bu using Var2.
    }

  } while(i > 0);
  
}

//will locate if exist a graph on screen that match the type and removes
void DiagramDialog::remvar(QString a)
{
    loc = -1;
    QString Var;
    int i;
    Var2 = ".a";    

    for(i = GraphList->count()-1; i>=0; i--)
    {
    
      Var = GraphList->item(i)->text();

      if(Var.indexOf(a,0,Qt::CaseSensitive) != -1)
      {
	loc = i;
	slotDeleteGraph();
      }
    }
    Var2="";

}

//checks if a type of graph is on screen
bool DiagramDialog::testvar (QString a)
{
  QString Var;

  foreach(Graph *pg, Diag->Graphs) {
  
    Var = pg->Var;
    if(Var.indexOf(a,0,Qt::CaseSensitive) != -1)
    {
      return true;
    }
  }
    return false;
}
#endif

/* PHASOR AND WAVEAC RELATED CODE
//only for phasor diagram detect if the points are in the diagram,
//  if not tell with are the limits that the point has passed
bool Diagram::insideDiagramPh(Graph::iterator const& p ,float* xn, float* yn) const 
{ untested();
  float f1 = p->getScrX();
  float f2 = p->getScrY();
  float xa,ya;

  xa = f1;
  ya = f2;

  if(f1 < 0.0)
    xa = 0.0;
  if(f1 > float(x2))
    xa = float(x2);
  if(f2 < 0.0)
    ya = 0.0;
  if(f2 > float(y2))
    ya = float(y2);
  
  *xn = xa;
  *yn = ya;

  return ((xa == f1)&&(ya == f2));
}
//for phasor if the original point isn't in diagram with the limits calculated in insideDiagramPh
//  will create a point inside the diagram if possible
bool Diagram::newcoordinate(Graph::iterator const& p,float* xn, float* yn) const
{ untested();
  float f1 = (p-1)->getScrX();
  float f2 = (p-1)->getScrY();
  float f3 = p->getScrX();
  float f4 = p->getScrY();
  float xc = *xn;
  float yc = *yn;
  float xt,yt;
  float d;
  float b;
  
  if(((f1 > f3 - 3) && (f1 < f3 + 3)) || ((f2 > f4 - 3) && (f2 < f4 + 3)))
  { untested();
    d = 0.0;
    b = 0.0;
  }
  else
  { untested();
    d = (f4 - f2) / (f3 - f1);
    b = f2 - d * f1;
  }


  if((f1 > f3 - 3) && (f1 < f3 + 3) && (f2 != f4))
  { untested();
    xt = f1;
    yt = yc;
  }
  else
  { untested();
    if((f2 > f4 - 3) && (f2 < f4 + 3) && (f1 != f3))
    { untested();
      xt = xc;
      yt = f2;
    }
    else
    { untested();
      yt = d*xc + b;
      xt = (yc - b) / d;
    }
  }
  if((yt >= 0.0) && (yt <= float(y2)))
  { untested();
      *yn = yt;
      return true;
  }
  else
  { untested();
    if((xt >= 0.0) && (xt <= float(x2)))
    { untested();
	*xn = xt;
	return true;
    }
    else
	return false;
	
  }  
}

//  will read the values receive and find if is one the values determined by AC and remove repeated number.
//  if there isn't any value that match will find the closest number and replace
void Diagram::findfreq(Graph *g)
{ untested();
  if(freq!=nullptr) delete[] freq;
  freq= nullptr;
  int z = QString::compare(g->axis(0)->Var,"acfrequency",Qt::CaseInsensitive);//meaning that only work in AC 
  if(z != 0)
  { untested();
    nfreqt=1;
    freq = new double;
    freq[0] = 0;
    sfreq = "0 Hz;";
    return;
  }
  double scale = 1.0;
  QString num;
  bool ok;
  double freqnum;
  int n=0; 
  int m=0;
  int a;
  QString value;
  int s;
  n=sfreq.count(';')+1;
  freq= new double[n];

  do{ untested();
    n = sfreq.indexOf(";",m,Qt::CaseInsensitive);
    if(n==-1 || Name == "Waveac") n = sfreq.size()-1;
    value=sfreq.mid(m,n+1-m);
    a=value.size();

    if(value.indexOf("ghz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1e9;
      a = value.indexOf("ghz",0,Qt::CaseInsensitive);    
    }
    else if(value.indexOf("mhz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1e6;
      a = value.indexOf("mhz",0,Qt::CaseInsensitive);     
    }
    else if(value.indexOf("khz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1e3;
      a = value.indexOf("khz",0,Qt::CaseInsensitive);
    }
    else if(value.indexOf("hz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1.0;
      a = value.indexOf("hz",0,Qt::CaseInsensitive);
    }

    double *px,f=0;
    int i,z;
    double d,dmin=DBL_MAX;
    num = value.mid(0,a);
    freqnum = num.toDouble(&ok) * scale;
    if(!ok)
    { untested();
      scale = 1.0;
      for(s=a;s>0;s--)
      { untested();
	num = value.mid(0,s);
	freqnum = num.toDouble(&ok) * scale;
	if(ok)
	{ untested();
	  value.resize(s);
	  break;
	}
      }
      if(s==0)
	goto end;
    }

    for(i=g->countY; i>0; i--) {  // every branch of curves
      px = g->axis(0)->Points;
      for(z=g->axis(0)->count; z>0; z--) {  // every point
	if(*px > 0)
	{ untested();
	  d=fabs(freqnum - *px);
	  if(d<dmin) 
	  { untested();
	    dmin=d;
	    f= *px; 
	  }
	}
	++px;
      }
    }
    freqnum = f;
    for(s=0;s<nfreqt;s++)
    { untested();
      if(freq[s]==freqnum)
      { untested();
	freqnum = 0;
	break;
      }
      if(freq[s]>freqnum)
      { untested();
	f=freq[s];
	freq[s]=freqnum;
	freqnum=f;
      }
    }  
    if(freqnum == 0) 
    { untested();
      value.clear();
      goto end;
    }
    nfreqt++;
    freq[nfreqt-1]=freqnum;
end:
    m=n+1;
    
  }while(n!=sfreq.size()-1);

  if(freqnum==0 &&nfreqt==0)
  { untested();
    nfreqt=1;
    freq[0] = 0;
    sfreq = "0 Hz;";
    return;   
  }
  nfreqa=0;
  sfreq.clear();
  while(nfreqa<nfreqt)
  { untested();
    freqnum=freq[nfreqa];

    if(freqnum >= 1e9)
    { untested();
      freqnum/= 1e9;
      value.setNum(freqnum);
      value+= " GHz;";
    }
    else if(freqnum >= 1e6)
    { untested();
      freqnum/= 1e6;
      value.setNum(freqnum);
      value+= " MHz;";
    }
    else if(freqnum >= 1e3)
    { untested();
      freqnum/= 1e3;
      value.setNum(freqnum);
      value+= " KHz;";
    }
    else
    { untested();
      value.setNum(freqnum);
      value+= " Hz;";
    }

    sfreq.append(value);
    nfreqa++;
  }

  

  
}

// for phasor will find the biggest absolute value of all max limits and replace the others
void Diagram::setlimitsphasor(Axis *x ,Axis *y)
{ untested();
  double yrx,yrn,yix,yin;

    yrn = x->min;
    yrx = x->max;
    yin = y->min;
    yix = y->max;

    if(fabs(yrn) > yrx)
      yrx = fabs(yrn);
    else
      yrn = (-1.0) * yrx;

    if(fabs(yin) > yix)
      yix = fabs(yin);
    else
      yin = (-1.0) * yix;
    
    if(yrx < yix)
      yrx = yix;

    x->max = y->max = yrx ;
    x->min = y->min = (-1.0) * yrx;

}

//for marker of waveac to find the value of x
double Diagram::wavevalX(int i) const
{ untested();
    return i*xAxis.up/(sc*50); 
}
*/
