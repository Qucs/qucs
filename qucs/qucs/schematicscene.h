/*
 * schematicscene.h - define schematic scene
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
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

#ifndef SCHEMATICSCENE_H_
#define SCHEMATICSCENE_H_ value

#include <QGraphicsScene>

// why is Schematic a QGraphics thingy as well?
class SchematicScene : public QGraphicsScene
{
Q_OBJECT
public:
  SchematicScene (QObject *);
  virtual ~SchematicScene ();

private:

protected:
  void drawBackground(QPainter *painter, const QRectF& rect);
};

// moved from element.h
class GraphicsElement : public QGraphicsItem {
private:
	GraphicsElement(){}
public:
	explicit GraphicsElement(Element* e)
		: _e(e)
	{
		assert(_e);
	}
	~GraphicsElement(){ delete _e; }
private: // later: Qgraphics virtual overrides
//  void paint() { assert(_e); _e->paint(); }
//  void paintScheme(Schematic *s) { assert(_e); _e->paintScheme(s); }
  void paintScheme(QPainter *p) { assert(_e); _e->paintScheme(p); }
  void setCenter(int i, int j, bool relative=false){
	  assert(_e);
	  _e->setCenter(i, j, relative);
  }
  void getCenter(int& i, int& j){
	  assert(_e);
	  _e->getCenter(i, j);
  }
  QRectF boundingRect() const {return _e->boundingRect(); }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0){
	  assert(_e);
	  return _e->paint(painter, option, widget);
  }

  // TODO: move coordinate stuff here.
//  void setPos(int a, int b){
//     _e->setPos(a, b);
//  }
public:
  Element* operator->(){ assert(_e); return _e; }
  Element const* operator->() const{ assert(_e); return _e; }
private: // owned elements
  Element* _e;
};

#endif /* SCHEMATICSCENE_H_ */
