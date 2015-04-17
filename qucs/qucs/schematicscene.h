/*
 * schematicscene.h - definition scene of schematic
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

class SchematicScene : public QGraphicsScene
{
public:
  SchematicScene (QObject *);
  virtual ~SchematicScene ();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);

private:
};

#endif /* SCHEMATICSCENE_H_ */
