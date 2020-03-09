#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include <QUndoCommand>

#include "graphicitem.h"

class QGraphicsScene;

class AddItemCommand: public QUndoCommand
{
public:
    AddItemCommand(GraphicItem *item,
                   QPointF pos,
                   QGraphicsScene *scene,
                   QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;
private:
    GraphicItem *m_item;
    QGraphicsScene *m_scene;
    QPointF m_pos;
};

#endif // UNDOCOMMANDS_H
