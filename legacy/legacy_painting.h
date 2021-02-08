/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEGACY_PAINTING_H
#define LEGACY_PAINTING_H

#include "element.h"
#include "painting.h"
#include <QString> // BUG: Name
#include <QPen> // BUG: SetSomeStuff

class QString;
class QPen;

class LegacyPainting : public Element, public Painting {
public:
  LegacyPainting();
  LegacyPainting(int cx, int cy) : Element(cx, cy) {}
 ~LegacyPainting() {}

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
  virtual QString save() /*const*/ =0;
  virtual void MouseMoving(SchematicDoc*, int, int, int, int,
                           SchematicDoc*, int, int, bool) {}
  virtual bool MousePressing() { return false; }
  virtual void Bounding(int&, int&, int&, int&);
  virtual bool resizeTouched(float, float, float) { return false; }
  virtual void MouseResizeMoving(int, int, SchematicDoc*) {}

  virtual void rotate() {}
  virtual void mirrorX() {}
  virtual void mirrorY() {}
  virtual bool Dialog() { return false; }

  QString toPenString (int);
  QString toBrushString (int);
  int  State;   // state for different mouse operations. yikes

  QString name() const{return Name;}

private: // Element?
  void paint(ViewPainter* p) const override {
	  auto c = const_cast<LegacyPainting*>(this);
	  c->paint(p);
  }
  virtual void paint(ViewPainter*){}
  rect_t bounding_rect() const override {itested();
	  int x1_, y1_, x2_, y2_;

	  auto p = const_cast<LegacyPainting*>(this);
	  p->Bounding(x1_, y1_, x2_, y2_);
	  return rect_t(x1, y1, x2-x1, y2-y1);
  }


protected:
  QString Name; // the type
  int x2, y2; // yikes
};

#endif
