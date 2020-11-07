#include <QApplication>
#include <QClipboard>
#include "docfmt.h"

static const std::string cnp_lang = "leg_sch";
/*--------------------------------------------------------------------------*/
class PasteCommand : public SchematicEdit {
public:
	PasteCommand(SchematicDoc& ctx, ElementGraphics* gfx)
	: SchematicEdit(*ctx.sceneHACK()) { untested();
		setText("Paste $n elements"); // tr?

	// 	for(auto i : gfx.children()){
	// 	i->setParent(nullptr);
	// 	i->hide();
	// 	ctx.takeOwnership(element(i)); // BUG?
	// 		qInsert(i);
	// 	}
	}
	~PasteCommand(){ untested();
		// _gfx owns element(_gfx)
		// ctx owns _gfx
	}
}; // PasteCommand
/*--------------------------------------------------------------------------*/
class MouseActionPaste : public MouseAction{
public:
	explicit MouseActionPaste(MouseActions& ctx, Element const* proto=nullptr)
		: MouseAction(ctx), _gfx(nullptr), _proto(proto)
  	{}
private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* enter(QEvent*) override;
	cmd* leave(QEvent*) override;
	cmd* release(QMouseEvent*) override;

private:
	cmd* makeNew(QMouseEvent*);
	cmd* rotate(QEvent*);

private:
	ElementGraphics* _gfx;
	Element const* _proto;
};
/*--------------------------------------------------------------------------*/
class pastebuffer : public SchematicSymbol{
public:
	explicit pastebuffer(){
		new_subckt();
	}
private:
	rect_t bounding_rect() const override{ untested();
		rect_t r;
		for (auto i : components()){ untested();
			auto c = i->center();
			r |= i->bounding_rect() + c;
		}
		for (auto i : wires()){ untested();
			auto c = i->center();
			r |= i->bounding_rect() + c;
		}

		return r;
	}
};
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::activate(QObject* sender)
{
  QClipboard *cb = QApplication::clipboard();   // get system clipboard
  QString s = cb->text(QClipboard::Clipboard);
  istream_t stream(&s);

  Command const* cmd = command_dispatcher[cnp_lang];
  assert(cmd);
  auto fmt=prechecked_cast<DocumentFormat const*>(cmd);

  auto buf = new pastebuffer();
  try{
	  fmt->load(stream, *buf);
  }catch(...){
	  incomplete();
	  deactivate();
  }

  _gfx = new ElementGraphics(buf);
  trace3("paste::activate", _gfx->boundingRect().topLeft(),
		               _gfx->boundingRect().bottomRight(), _gfx->childItems().count());
  auto br = _gfx->boundingRect();

  for(auto i : buf->components()){
	  trace2("centering", i->label(), br.center());
	  i->setCenter(pos_t(0,0));
  }

  doc().sceneAddItem(_gfx);
  _gfx->show();

  return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::release(QMouseEvent* ev)
{ untested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){ untested();
		cmd = makeNew(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::makeNew(QMouseEvent* ev)
{ untested();
	// assert(ev->widget=doc->scene()) // or so.
	trace1("RELEASE", ev->type());
	if(ev->type() == QEvent::MouseButtonRelease){ itested();
	}else{ untested();
		unreachable();
	}

	if(!_gfx){
		unreachable();
		return nullptr;
	}else if(!element(_gfx)){
		unreachable();
		return nullptr;
	}else{
	}
	auto elt = element(_gfx);
	assert(elt);

	if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		QPointF pos = se->scenePos();
		QPoint xx = doc().snapToGrid(pos);
		_gfx->setPos(xx);
	}else{
	}

	cmd* c = new PasteCommand(doc(), _gfx);
	_gfx = nullptr; // leak?

	deactivate();

	ev->accept();
	return c;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::deactivate()
{ untested();
	// assert(!attached);
	doc().sceneRemoveItem(_gfx);
	delete _gfx; // CHECK: who owns _elt?
	_gfx = nullptr;
	incomplete();
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::move(QEvent* ev)
{ untested();
	QPointF sp;
	trace1("move", ev->type());
	if(auto ee=dynamic_cast<QMouseEvent*>(ev)){ untested();
		unreachable();
		QPointF wp;
		wp = ee->localPos(); // use oldPos?
		sp = mapToScene(wp.toPoint());
	}else if(auto ee=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		sp = ee->scenePos();

		QPoint xx = doc().snapToGrid(sp);
		sp = xx;
	}else{ untested();
		unreachable();
	}

	if(_gfx){ untested();
		_gfx->setPos(sp.x(), sp.y());
	}else{ untested();
		unreachable();
	}

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::enter(QEvent* ev)
{ untested();
	trace1("paste enter", ev->type());
	auto ee = prechecked_cast<QEnterEvent*>(ev);
	assert(ee);
	
	auto wp = ee->localPos();

	SchematicDoc* d = &doc();
	auto sp = d->mapToScene(wp.toPoint());

	Element* elt;
	assert(_gfx);
	_gfx->setPos(sp.x(), sp.y());
	
	doc().sceneAddItem(_gfx);

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::leave(QEvent* ev)
{ untested();
	sceneRemoveItem(_gfx);
	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::rotate(QEvent*)
{ untested();
	if(!_gfx){ untested();
		unreachable();
	}else if(dynamic_cast<Symbol*>(element(_gfx))){ untested();
		// always do this?
		_gfx->transform(ninety_degree_transform);
	}else{ untested();
		unreachable();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::press(QEvent* ev)
{ untested();
	auto a = dynamic_cast<QMouseEvent*>(ev);
	auto m = dynamic_cast<QGraphicsSceneMouseEvent*>(ev);
	QUndoCommand* cmd = nullptr;
	if(a){ untested();
		unreachable();
		// somehow it is a scene event??
	}else if(!m){ untested();
		trace1("MouseActionPaste::press", ev->type());
		unreachable();
	}else if(m->button() == Qt::LeftButton){ untested();
	}else if(m->button() == Qt::RightButton){ untested();
		cmd = MouseActionPaste::rotate(ev);
		ev->accept(); // really?
	}else{ untested();
		unreachable();
	}
	return cmd;
}
