/***************************************************************************
                                wirelabel.h
                               -------------
    begin                : Sun Feb 29 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef WIRELABEL_H
#define WIRELABEL_H

#include "element.h"
#include "conductor.h"

#ifndef UNTANGLE_QT
# include "viewpainter.h"
# include <QString>
#endif

class QPainter;
class Wire;
class Node;


// seems to be a ConductorLabel? or just a Symbol?!
class WireLabel : public Element {
public:
  WireLabel(const QString& _Name=0, int _cx=0, int _cy=0,
            int _x1=0, int _y1=0, int _Type=isNodeLabel);
  ~WireLabel();

  void setCenter(int x, int y, bool relative=false);
  bool getSelected(int, int);
  void setName(const QString& Name_);
  void setHighlighted (bool newval) { isHighlighted = newval; };

  // this is likely wrong.
  void moveTo(int x, int y){ _cx=x; _cy=y; }
  void moveCenter(int x, int y){ _cx+=x; _cy+=y; }
  void moveLeft(int x){ x1-=x; }

  Component* newOne(){
	  incomplete();
	  return nullptr;
  }
  Element* clone()const{
	  incomplete();
	  return nullptr;
  }

  Conductor *pOwner;  // Wire or Node where label belongs to
  QString initValue;

  void    paint(ViewPainter*) const;
  void    rotate();
  QString save();
  bool    load(const QString& s);
  bool    isHorizontal();
  void    getLabelBounding(int& _xmin, int& _ymin, int& _xmax, int& _ymax);

public: // FIXME
	int & cx__() {incomplete(); return _cx; }
	int & cy__() {incomplete(); return _cy; }
	int & x1__() {incomplete(); return x1; }
	int & y1__() {incomplete(); return y1; }
	int & x2__() {incomplete(); return x2; }
	int & y2__() {incomplete(); return y2; }

private:
  bool isHighlighted;

};

#endif
