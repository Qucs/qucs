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
{itested();

    if (e->isAccepted())
    {
        trace0("MouseAction::handle: event was already accepted.");
    }
//  {
//    // pass to other places unless accepted somewhere else.
//    // assert(is_ignored) instead!
//    e->ignore();
//  }
  assert(e);
  auto* m = prechecked_cast<QMouseEvent*>(e);
//  auto* s = prechecked_cast<QGraphicsSceneEvent*>(e);
  auto a = ComponentWidget::itemMimeType();

  if(dynamic_cast<QDragLeaveEvent*>(e)){ untested();
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
  case QEvent::GraphicsSceneMouseRelease:itested();
    return nullptr;
    //fallthrough
  case QEvent::MouseButtonRelease:itested();
    assert(m);
    return release(m);
  case QEvent::GrabMouse:itested();
    return nullptr;
//    return grab(s);
  case QEvent::DragEnter: untested();
    unreachable(); // proper type check above
    return nullptr;
  case QEvent::MouseButtonPress: untested();
    // fallthrough
  case QEvent::GraphicsSceneMousePress:itested();
    return press(e);
  default:
    // It is not a mouse action, so ignore it.
    return nullptr;
  }
}
// SchematicMouseAction::doc?
SchematicDoc const& MouseAction::doc() const
{itested();
  auto cc = const_cast<MouseAction*>(this);
  return cc->doc();
}

SchematicDoc& MouseAction::doc()
{itested();
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
{itested();
  ctx().updateViewport(); // use a signal?
}
void MouseActions::updateViewport()
{itested();

  SchematicDoc* s = dynamic_cast<SchematicDoc*>(&doc());
  if(s){
    s->updateViewport(); // use a signal?
  }else{ untested();
  }
}

QUndoCommand* MouseAction::activate(QObject* sender)
{itested();
  _sender = dynamic_cast<QAction*>(sender);
  return nullptr;
}

QUndoCommand* MouseAction::deactivate()
{itested();
  return nullptr;
}

void MouseAction::uncheck()
{
  if(_sender){itested();
    _sender->blockSignals(true); // do not call toggle slot
    _sender->setChecked(false);       // set last toolbar button off
    _sender->blockSignals(false);
  }else{itested();
  }

  deactivate(); // BUG?
}
/*--------------------------------------------------------------------------*/
SchematicScene const* MouseAction::scene() const
{
	return doc().scene();
}
/*--------------------------------------------------------------------------*/
bool MouseAction::isNode(pos_t const& p) const
{
	// return scene()->isNode(fX, fY);?
	return doc().isNode(p);
}
/*--------------------------------------------------------------------------*/
bool MouseAction::isConductor(pos_t const&p) const
{itested();
	assert(scene());
	return scene()->isConductor(p);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
