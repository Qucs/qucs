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

#include <assert.h>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QList> // BUG?

#include "geometry.h"
#include "qt_compat.h"
#include "viewpainter.h"


namespace qucs{
	class Element;
	class ElementGraphics;
	class Node;
	class Place;
	class Doc;
	class ElementList;
	class Symbol;
}

using qucs::Symbol;
using qucs::ElementGraphics;

// TODO: move to gui?

class SchematicScene : public QGraphicsScene {
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

  void attachToModel(qucs::Element*);
  qucs::Element* detachFromModel(qucs::Element*);
  void connectPorts(Symbol* c);

public: //places
  bool is_place(pos_t const& p) const;
  ElementGraphics* find_place(pos_t const& p);
  ElementGraphics const* find_place(pos_t const& p) const;
  ElementGraphics* new_place(pos_t const& p);

private:
public: // hack?
  qucs::ElementList* scope();

public:
  QGraphicsItem& addElement(qucs::Element* x);
  qucs::ElementList const* scope() const{
	  return const_cast<SchematicScene*>(this)->scope();
  }

  // qt implements that.
  void selectedItemsAndBoundingBox(QList<ElementGraphics*>& ElementCache, QRectF& BB);
//  void removeItem(Element const*);
  bool itemEvent(QEvent*);

  QPoint gridSize() const;
  QPoint snapToGrid(QPointF const&) const;

  bool isNode(pos_t) const;
//  Node const* nodeAt(pos_t) const;
  bool isConductor(pos_t) const;

  void possiblyRename(qucs::Element* e) const;

public: // wrap items
  QList<ElementGraphics*> selectedItems() const;
  QList<ElementGraphics*> items();
  QList<ElementGraphics*> items(int x, int y);
  QList<ElementGraphics*> items(QRectF const&);
  QList<ElementGraphics*> items(QPointF const& pos,
                                Qt::ItemSelectionMode mode=Qt::IntersectsItemShape,
                                Qt::SortOrder order = Qt::DescendingOrder) const;
  QList<ElementGraphics const*> items() const;
  QList<ElementGraphics const*> items(QRectF const&) const;
  QList<ElementGraphics const*> items(int x, int y) const;

private:
  void selectAll(bool v=true);

protected:
	qucs::Doc* doc();
	qucs::Doc const* doc() const;

private:
  bool event(QEvent* e) override;
  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

protected:
  void drawBackground(QPainter *painter, const QRectF& rect);
};

#endif /* SCHEMATICSCENE_H_ */
