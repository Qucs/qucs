/***************************************************************************
                          smithdiagram.cpp  -  description
                             -------------------
    begin                : Sat Oct 18 2003
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

#include "smithdiagram.h"

#include <math.h>

#include <qmessagebox.h>


SmithDiagram::SmithDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  dx = 200;
  dy = 200;

  GridOn = true;
  GridX = 4;    // number of arcs with im(z)=const
  GridY = 4;    // number of arcs with re(z)=const

  Name = "Smith";
}

SmithDiagram::~SmithDiagram()
{
}

// --------------------------------------------------------------
void SmithDiagram::paint(QPainter *p)
{
  int dx2 = dx>>1;
  p->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
  p->drawLine(cx, cy-dx2, cx+dx, cy-dx2);   // horizontal line im(r)=0


  if(GridOn) {

    // **********************************************************************************
    // draw arcs with im(z)=const
    double im, x, y, n_cos, n_sin;
    int    theta, beta, m;
    
    for(m=1; m<GridX; m++) {
      n_cos = cos(M_PI*double(m)/GridX);
      n_sin = sin(M_PI*double(m)/GridX);
      im = (1-n_cos)/n_sin; // * pow(y2,0.7);
      x  = 1-fabs(im);
      y  = -im - fabs(im);

//      if(y2 == 1) {       // Smith-Chart mit |r|=1
        beta  = int(16*180*atan2(n_sin-im,n_cos-1)/M_PI); //angle(r - Mitte);
        if(beta<0) beta += 16*360;
        theta = 16*270-beta;
//      }
//      else {         // Smith-Chart mit |r|>1
/*            Reell  = (rMAXq+1)/(rMAXq-1);
            Wurzel =  Reell*Reell - Im*Im-1;
            if Wurzel<0     % liegt Kreis vollkommen in Smith-Chart ?
                beta = 0;       % ja, ...
                theta = 2*pi;   % ... dann Vollkreis zeichnen
            else
                Reell1 =  sqrt(Wurzel)-Reell;   % beide Schnittpunkte mit Außenkreis berechnen
                Reell2 = -sqrt(Wurzel)-Reell;

                r = (Reell1+j*Im-1) / (Reell1+j*Im+1);
                beta = angle(r - Mitte);        % Anfangswinkel
                r = (Reell2+j*Im-1) / (Reell2+j*Im+1);
                theta = angle(r - Mitte);       % Endwinkel
            end*/
//      }

/*        if beta<0
            beta = 2*pi + beta;   % Wertebereich 0...2*pi
        end
        if (theta<0) | (theta<beta)
            theta = 2*pi + theta;   % Wertebereich anpassen (evtl. >2*pi)
        end
*/
      p->drawArc(cx+dx2+int(x*dx2), cy-dx2+int(y*dx2), int(fabs(im)*dx), int(fabs(im)*dx), beta, theta);
      p->drawArc(cx+dx2+int(x*dx2), cy-dx2,
                 int(fabs(im)*dx), int(fabs(im)*dx), beta+theta-16*180, theta);
    }

    // **********************************************************************************
    // draw  arcs with Re(z)=const
    #define rMAX 1.0
    
    for(m=1; m<GridY; m++) {
      x = m*(rMAX+1)/GridY - rMAX;   // distribute circles uniformly
      y = (1-x)/2;    // radius

      p->drawArc(cx+dx2+int(x*dx2), cy-dx2-int(y*dx2), int(y*dx), int(y*dx), 0, 16*360);
/*        if abs(abs(r)-1) > 0.4      % sieht unschön aus (zu nah an |r|=1) ?
            linecount=linecount+1;
            ChartLinehandles(linecount,1)=plot(Kreis, 'color',txtC,'linewidth',1,...
                                                'HandleVisibility', 'off');     % linke Kreisbögen zeichnen
        end*/

/*        if rMAX > 1     % rechte Kreisbögen zeichnen ?
            r     = Mitte + 3*Radius;
            Reell = (1+r)/(1-r);
            Im    = - Reell*Reell - 2*Reell*(rMAXq+1)/(rMAXq-1) - 1;
            if Im <= 0      % liegt Kreis vollständig im Smith-Chart ?
                Kreis = 2 - Kreis;    % ja, dann Kreis an r=1 spiegeln
            else
                r = (Reell+j*sqrt(Im)-1) / (Reell+j*sqrt(Im)+1);
                beta = angle(r - 2+Mitte);        % Anfangswinkel
                Kreis = 2-Mitte + Radius*exp(j*linspace(beta,2*pi-beta,nn));
            end
            linecount=linecount+1;
            ChartLinehandles(linecount,1)=plot(Kreis, 'color',txtC,'linewidth',1,...
                                                'HandleVisibility', 'off');     % Kreis zeichnen
        end*/
    }
  } // of if(GridOn)
  
  p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
  p->drawArc(cx, cy-dx, dx, dx, 0, 16*360);
//  p->drawText(cx+dy, cy+15, QString::number(y2));

  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    pg->paint(p);
}

// ------------------------------------------------------------
void SmithDiagram::calcData(Graph *g)
{
  int *p = g->Points;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = cx+(dx>>1)+int(cp->yr/y2*double(dx>>1));
    *(p++) = cy-(dy>>1)-int(cp->yi/y2*double(dy>>1));
  }
}

// ------------------------------------------------------------
SmithDiagram* SmithDiagram::newOne()
{
  return new SmithDiagram();
}
