#include "components/component.h" // BUG
#include "node.h"
#include "qucs_app.h"
#include "qucsdoc.h"
#include "viewpainter.h"
#include "module.h"
#include "misc.h"
#include "io_trace.h"
#include "qucs_globals.h"

#include <QPen>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

#include <assert.h>

void GateComponent::createSymbol()
{
  int Num = Props.getFirst()->Value.toInt();
  if(Num < 2) Num = 2;
  else if(Num > 8) Num = 8;
  Props.getFirst()->Value = QString::number(Num);

  int xl, xr, y = 10*Num, z;
  x1 = -30; y1 = -y-3;
  x2 =  30; y2 =  y+3;

  tx = x1+4;
  ty = y2+4;

  z = 0;
  if(Model.at(0) == 'N')  z = 1;

  if(Props.getLast()->Value.at(0) == 'D') {  // DIN symbol
    xl = -15;
    xr =  15;
    Lines.append(new Line( 15,-y, 15, y,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15,-y, 15,-y,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15, y, 15, y,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15,-y,-15, y,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 15, 0, 30, 0,QPen(Qt::darkBlue,2)));

    if(Model.at(z) == 'O') {
      Lines.append(new Line(-11, 6-y,-6, 9-y,QPen(Qt::darkBlue,0)));
      Lines.append(new Line(-11,12-y,-6, 9-y,QPen(Qt::darkBlue,0)));
      Lines.append(new Line(-11,14-y,-6,14-y,QPen(Qt::darkBlue,0)));
      Lines.append(new Line(-11,16-y,-6,16-y,QPen(Qt::darkBlue,0)));
      Texts.append(new Text( -4, 3-y, "1", Qt::darkBlue, 15.0));
    }
    else if(Model.at(z) == 'A')
      Texts.append(new Text( -10, 3-y, "&", Qt::darkBlue, 15.0));
    else if(Model.at(0) == 'X') {
      if(Model.at(1) == 'N') {
	Ellips.append(new Area(xr,-4, 8, 8,
                  QPen(Qt::darkBlue,0), QBrush(Qt::darkBlue)));
        Texts.append(new Text( -11, 3-y, "=1", Qt::darkBlue, 15.0));
      }
      else
        Texts.append(new Text( -11, 3-y, "=1", Qt::darkBlue, 15.0));
    }
  }
  else {   // old symbol

    if(Model.at(z) == 'O')  xl = 10;
    else  xl = -10;
    xr = 10;
    Lines.append(new Line(-10,-y,-10, y,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 10, 0, 30, 0,QPen(Qt::darkBlue,2)));
    Arcs.append(new Arc(-30,-y, 40, 30, 0, 16*90,QPen(Qt::darkBlue,2)));
    Arcs.append(new Arc(-30,y-30, 40, 30, 0,-16*90,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 10,15-y, 10, y-15,QPen(Qt::darkBlue,2)));

    if(Model.at(0) == 'X') {
      Lines.append(new Line(-5, 0, 5, 0,QPen(Qt::darkBlue,1)));
      if(Model.at(1) == 'N') {
        Lines.append(new Line(-5,-3, 5,-3,QPen(Qt::darkBlue,1)));
        Lines.append(new Line(-5, 3, 5, 3,QPen(Qt::darkBlue,1)));
      }
      else {
        Arcs.append(new Arc(-5,-5, 10, 10, 0, 16*360,QPen(Qt::darkBlue,1)));
        Lines.append(new Line( 0,-5, 0, 5,QPen(Qt::darkBlue,1)));
      }
    }
  }

  if(Model.at(0) == 'N')
    Ellips.append(new Area(xr,-4, 8, 8,
                  QPen(Qt::darkBlue,0), QBrush(Qt::darkBlue)));

  Ports.append(new ComponentPort( 30,  0));
  y += 10;
  for(z=0; z<Num; z++) {
    y -= 20;
    Ports.append(new ComponentPort(-30, y));
    if(xl == 10) if((z == 0) || (z == Num-1)) {
      Lines.append(new Line(-30, y, 9, y,QPen(Qt::darkBlue,2)));
      continue;
    }
    Lines.append(new Line(-30, y, xl, y,QPen(Qt::darkBlue,2)));
  }
}
