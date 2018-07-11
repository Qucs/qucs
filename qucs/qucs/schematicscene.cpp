/*
 * schematicscene.cpp - implement schematic scene
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 * Copyright (C) 2017, Guilherme Brondani Torri, guitorri@gmail.com
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


#include "schematicscene.h"

#include <QPainter>

SchematicScene::SchematicScene(QObject *parent) :
  QGraphicsScene(parent)
{
}

SchematicScene::~SchematicScene()
{
}

/*!
 * \brief SchematicScene::drawBackground of scene
 * \param painter
 * \param rect
 */
void SchematicScene::drawBackground(QPainter *painter, const QRectF &rect)
{

  // Draw origin when visible
  if(rect.contains(QPointF(0, 0))) {
    painter->drawLine(QLine(-3.0, 0.0, 3.0, 0.0));
    painter->drawLine(QLine(0.0, -3.0, 0.0, 3.0));
  }
  /// \todo getter and setter
  int GridX = 10;
  int GridY = 10;

  // Extrema grid points
  qreal left = int(rect.left()) - (int(rect.left()) % GridX);
  qreal top = int(rect.top()) - (int(rect.top()) % GridY);
  qreal right = int(rect.right()) - (int(rect.right()) % GridX);
  qreal bottom = int(rect.bottom()) - (int(rect.bottom()) % GridY);

  // Draw grid
  painter->setBrush(Qt::NoBrush);
  for (int x=left; x<=right; x+=GridX)
      for (int y=top; y<=bottom; y+=GridY)
          painter->drawPoint(x, y);
}

