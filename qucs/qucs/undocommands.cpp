#include <QGraphicsScene>

#include "undocommands.h"

AddItemCommand::AddItemCommand(GraphicItem *item,
                               QPointF pos,
                               QGraphicsScene *scene,
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
