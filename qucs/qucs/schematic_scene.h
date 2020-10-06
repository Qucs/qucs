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
class SchematicDoc;

class ElementGraphics;

// TODO: merge schematic mouse actions into this.
class SchematicScene : public QGraphicsScene
{
Q_OBJECT
public:
  SchematicScene (QObject* parent);
  virtual ~SchematicScene ();

#if 0
//  void addItem(ElementGraphics*);
  void addItem(QGraphicsItem* x){
	  QGraphicsScene::addItem(x);
  }
  void removeItem(QGraphicsItem const*x){
	  QGraphicsScene::removeItem((QGraphicsItem*)x);
  }
#endif

public:
  QGraphicsItem& addElement(Element* x);

  // qt implements that.
  void selectedItemsAndBoundingBox(QList<ElementGraphics*>& ElementCache, QRectF& BB);
//  void removeItem(Element const*);
  bool itemEvent(QEvent*);

  QPoint gridSize() const;
  QPoint snapToGrid(QPointF const&) const;

  bool isNode(int, int) const;
  bool isConductor(int, int) const;

public: // wrap items
  QList<ElementGraphics*> items() const;
  QList<ElementGraphics*> items(const QPointF &pos,
                                Qt::ItemSelectionMode mode=Qt::IntersectsItemShape,
                                Qt::SortOrder order = Qt::DescendingOrder) const;

private:
  void selectAll(bool v=true);

protected:
	SchematicDoc* doc();
	SchematicDoc const* doc() const;

private:
  bool event(QEvent* e) override;
  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

protected:
  void drawBackground(QPainter *painter, const QRectF& rect);
};

#include "qt_compat.h"
#include "element.h" // TODO: move implementation to .cpp
                     // also: this relates to scene, but is this the right place?
							// (having other problems, still)

#endif /* SCHEMATICSCENE_H_ */
