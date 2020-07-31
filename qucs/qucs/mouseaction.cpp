/* (c) 2020 Felix Salfelder
 * GPLv3+
 */
#include "mouseactions.h" // BUG
#include "io_trace.h"
#include "schematic_doc.h"
#include <QAction>
#include <QGraphicsSceneEvent> // probably bug
#include "component_widget.h" // bug
#include <QStringLiteral> //bug
#include <QMimeData> //bug
QUndoCommand* MouseAction::handle(QEvent* e)
{ itested();

  {
    // pass to other places unless accepted somewhere else.
    // assert(is_ignored) instead!
    e->ignore();
  }
  assert(e);
  auto* m = prechecked_cast<QMouseEvent*>(e);
  auto* s = prechecked_cast<QGraphicsSceneEvent*>(e);
  auto a = ComponentWidget::itemMimeType();

  if(auto de=dynamic_cast<QDragLeaveEvent*>(e)){ untested();
    unreachable();
    return leave(m);
  }else if(auto de = dynamic_cast<QDragEnterEvent*>(e)){ untested();
    trace1("dragenter", de->mimeData()->formats()[0]);
    unreachable();

    if (de->mimeData()->hasFormat(a)){ untested();
      // got a pointer. possibly from ComponentWidget
      e->accept();
    // }else if (de->mimeData()->hasFormat(someThingElse)){ untested();
    //   unreachable();
    //   e->accept();
    } else{ untested();
      e->ignore();
    }

    return enter(m);

    return nullptr;
  }else if(auto de = dynamic_cast<QDragMoveEvent*>(e)){ untested();
    trace1("dragmove", de->mimeData()->formats()[0]);
  }else if(auto de = dynamic_cast<QDropEvent*>(e)){ untested();
    trace1("dragdrop", de->mimeData()->formats()[0]);
  }else{
  }

  // why??
  switch(e->type()){
//  case QEvent::MouseLeave:
//    assert(m);
//    return leave(m);
  case QEvent::Leave:
    return leave(e);
  case QEvent::Enter:
    return enter(e);
  case QEvent::GraphicsSceneMouseDoubleClick:
	 untested();
	 return dblclk(e);
  case QEvent::GraphicsSceneMouseMove:
    // getting here when moving elements.
  case QEvent::MouseMove:
    return move(e);
  case QEvent::GraphicsSceneMouseRelease: untested();
    return nullptr;
    //fallthrough
  case QEvent::MouseButtonRelease: untested();
    assert(m);
    return release(m);
  case QEvent::GrabMouse: untested();
    return nullptr;
//    return grab(s);
  case QEvent::DragEnter: untested();
    unreachable(); // proper type check above
    return nullptr;
  case QEvent::MouseButtonPress: untested();
  case QEvent::GraphicsSceneMousePress: untested();
    return press(e);
  default:
    trace1("mouseaction miss", e->type());
    e->ignore();
    return nullptr;
  }
}
// SchematicMouseAction::doc?
SchematicDoc const& MouseAction::doc() const
{ untested();
  auto cc = const_cast<MouseAction*>(this);
  return cc->doc();
}

SchematicDoc& MouseAction::doc()
{ untested();
  QucsDoc* c=&_ctx.doc();
  auto cc = dynamic_cast<SchematicDoc*>(c);
  assert(cc);
  return *cc;
}

QPointF MouseAction::mapToScene(QPoint const& p) const
{
  return doc().mapToScene(p);
}

void MouseAction::updateViewport()
{ untested();
  ctx().updateViewport(); // use a signal?
}
void MouseActions::updateViewport()
{ untested();

  SchematicDoc* s = dynamic_cast<SchematicDoc*>(&doc());
  if(s){
    s->updateViewport(); // use a signal?
  }else{ untested();
  }
}

QUndoCommand* MouseAction::activate(QAction* sender)
{ untested();
  _sender = sender;
  return nullptr;
}

QUndoCommand* MouseAction::deactivate()
{ untested();
  return nullptr;
}

void MouseAction::uncheck()
{
  if(_sender){ untested();
    _sender->blockSignals(true); // do not call toggle slot
    _sender->setChecked(false);       // set last toolbar button off
    _sender->blockSignals(false);
  }else{ untested();
  }

  deactivate();
}

bool MouseAction::isNode(int fX, int fY) const
{
  return doc().isNode(fX, fY);
}
