/***************************************************************************
                                painting.h
                               ------------
    begin                : Sat Nov 22 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PAINTING_H
#define PAINTING_H

#include "element.h"
#include "viewpainter.h"


class Painting : public Element  {
public:
  Painting();
 ~Painting() {}

  virtual void getCenter(int&, int &) {}
  virtual bool getSelected(float, float, float) { return false; }

public: // legacy anonymous stuff.
  virtual void setArgs2Int(int, int) {
	  unreachable();
  }
  virtual void setSomeArgsHack(int, int, const QString&,  const QString&){
	  unreachable();
  }
  virtual void setSomeStuff( int, int, int, int, QPen){
	  unreachable();
  }

public:
  virtual bool load(const QString&) { return true; };
  virtual QString save();
  virtual QString saveCpp();
  virtual QString saveJSON();
  virtual void paint(ViewPainter*) {}
  virtual void MouseMoving(Schematic*, int, int, int, int,
                           Schematic*, int, int, bool) {}
  virtual bool MousePressing() { return false; }
  virtual void Bounding(int&, int&, int&, int&);
  virtual bool resizeTouched(float, float, float) { return false; }
  virtual void MouseResizeMoving(int, int, Schematic*) {}

  virtual void rotate() {}
  virtual void mirrorX() {}
  virtual void mirrorY() {}
  virtual bool Dialog() { return false; }

  QString toPenString (int);
  QString toBrushString (int);
  int  State;   // state for different mouse operations. yikes
};

#endif
