/*
 * schematicscene.h - define schematic scene
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 * Copyright (C) 2019-2020, Guilherme Brondani Torri, guitorri@gmail.com
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

#include "undocommands.h"
#include "schematicscene.h"
#include "graphicitem.h"

AddItemCommand::AddItemCommand(SchematicScene *scene,
                               QGraphicsItem *item,
                               QPointF pos,
                               QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_item = item;
    m_pos = pos;
    m_scene = scene;
    setText("Add Item");
}

void AddItemCommand::undo()
{
    m_scene->removeItem(m_item);
    m_scene->update();
}

void AddItemCommand::redo()
{
    m_scene->addItem(m_item);
    m_item->setPos(m_pos.x(), m_pos.y());
    m_scene->clearSelection();
    m_scene->update();
}


RemoveItemsCommand::RemoveItemsCommand(SchematicScene *scene,
                                       QList<QGraphicsItem *> items,
                                       QUndoCommand *parent)
    : QUndoCommand(parent),
      m_scene(scene)
{
    foreach(QGraphicsItem *item, items) {
      m_ItemPosList.append(pairItemPos(item, item->pos()));
    }
    setText("Remove Items");
}

void RemoveItemsCommand::undo()
{
    foreach(pairItemPos pair, m_ItemPosList) {
        m_scene->addItem(pair.first);
        pair.first->setPos(pair.second);
    }
}

void RemoveItemsCommand::redo()
{
    foreach(pairItemPos pair, m_ItemPosList) {
        m_scene->removeItem(pair.first);
    }
}
