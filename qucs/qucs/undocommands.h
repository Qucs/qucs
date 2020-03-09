/*
 * undocommand.h - define commands for undo/redo
 *
 * Copyright (C) 2020, Guilherme Brondani Torri, guitorri@gmail.com
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

#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QPointF>

class SchematicScene;

// used to keep track of items and positions
typedef QPair<QGraphicsItem*, QPointF> pairItemPos;


class AddItemCommand: public QUndoCommand
{
public:
    AddItemCommand(SchematicScene *scene,
                   QGraphicsItem *item,
                   QPointF pos,
                   QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;
private:
    QGraphicsItem *m_item;
    SchematicScene *m_scene;
    QPointF m_pos;
};

class RemoveItemsCommand: public QUndoCommand
{
public:
    RemoveItemsCommand(SchematicScene *scene,
                       QList<QGraphicsItem *> items,
                       QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;
private:
    SchematicScene *m_scene;
    QList<pairItemPos> m_ItemPosList;
};
#endif // UNDOCOMMANDS_H
