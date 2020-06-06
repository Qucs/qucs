/*
 * schematic_scene.h - define schematic scene
 *
 * Copyright (C) 2014 Yodalee, lc85301@gmail.com
 *               2018, 2020 Felix Salfelder
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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

#ifndef QUCS_SCHEMATIC_SCENE_H_
#define QUCS_SCHEMATIC_SCENE_H_

#include <QGraphicsScene>
#include <QList> // BUG
#include <QGraphicsItem>
#include <assert.h>

#include "viewpainter.h"
#include "qt_compat.h"

class Element;

#ifdef USE_SCROLLVIEW
class SchematicScene {
  SchematicScene (QObject *);
  virtual ~SchematicScene ();
};
#else
class ElementGraphics;
class SchematicScene : public QGraphicsScene
{
Q_OBJECT
public:
  SchematicScene (QObject *);
  virtual ~SchematicScene ();

//  void addItem(ElementGraphics*);
  void addItem(QGraphicsItem* x){
	  QGraphicsScene::addItem(x);
  }
  void removeItem(QGraphicsItem const*x){
	  QGraphicsScene::removeItem((QGraphicsItem*)x);
  }

  void selectedItemsAndBoundingBox(QList<ElementGraphics*>& ElementCache, QRectF& BB);
  void removeItem(Element const*);

private:

protected:
  void drawBackground(QPainter *painter, const QRectF& rect);
};
#endif

#include "qt_compat.h"
#include "element.h" // TODO: move implementation to .cpp
                     // also: this relates to scene, but is this the right place?
							// (having other problems, still)

#if QT_MAJOR_VERSION < 5
// use naked pointer, as legacy qucs does.
typedef Element ElementGraphics;
#else

#endif


// a mouse action on an element.
// formerly, a mouse action was implemented by means of messing with element
// internals.

#if 0
Component const* component(ElementMouseAction const*);
Wire const* wire(ElementMouseAction const*);
WireLabel const* wireLabel(ElementMouseAction const*);
Diagram const* diagram(ElementMouseAction const*);
Painting const* painting(ElementMouseAction const*);

Component* component(ElementMouseAction*);
Wire* wire(ElementMouseAction*);
WireLabel* wireLabel(ElementMouseAction*);
Diagram* diagram(ElementMouseAction*);
Painting* painting(ElementMouseAction*);
#endif

#endif /* SCHEMATICSCENE_H_ */
