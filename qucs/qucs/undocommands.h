#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include <QUndoCommand>

#include "element.h"

class QGraphicsScene;

class AddItemCommand: public QUndoCommand
{
public:
    AddItemCommand(Element *item,
                   QPointF pos,
                   QGraphicsScene *scene,
                   QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;
private:
    Element *m_item;
    QGraphicsScene *m_scene;
    QPointF m_pos;
};

#endif // UNDOCOMMANDS_H
