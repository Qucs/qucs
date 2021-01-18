/***************************************************************************
    copyright            : (C) 2018-2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QApplication>
#include <QClipboard>
#include "docfmt.h"
#include "sckt_base.h"

static const std::string cnp_lang = "leg_sch";
/*--------------------------------------------------------------------------*/
// TODO: "NewElementCommand", deduplicate
class PasteCommand : public SchematicEdit {
public:
	PasteCommand(SchematicDoc& ctx, ElementGraphics* gfx)
	: SchematicEdit(*ctx.sceneHACK()) { untested();
		setText("Paste $n elements"); // tr?

		auto pos = gfx->pos();

		for(auto i : gfx->childItems()){ untested();
			if(auto j=dynamic_cast<ElementGraphics*>(i)){ untested();
				auto c = j->clone();

				auto p = i->pos();
				c->setPos((p + pos).toPoint());

				{ // move to qInsert?
					ctx.sceneAddItem(c);
					c->setVisible(false);
					// ctx.takeOwnership(element(c)); // BUG?
				}

				{ // dropTrailingDigits(c)
					std::string label = element(c)->label();
					int z = label.size() - 1;
					while(z && std::isdigit(label[z])){
						--z;
					}
					element(c)->setLabel(label.substr(0, z+1));
				}

				qInsert(c);
			}else{ untested();
			}
		}
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
	cmd* release(QEvent*) override;

private:
	cmd* makeNew(QEvent*);
	cmd* rotate(QEvent*);

private:
	ElementGraphics* _gfx;
	Element const* _proto;
};
/*--------------------------------------------------------------------------*/
class pastebuffer : public SubcktBase{
public:
	explicit pastebuffer(){ untested();
		new_subckt();
		auto const* lang = language_dispatcher[cnp_lang];
		assert(lang);

		QClipboard *cb = QApplication::clipboard();
		QString s = cb->text(QClipboard::Clipboard);
		istream_t cs(istream_t::_STRING, s.toStdString());

		// fmt->load(stream, this);
		while(!cs.is_end()){ untested();
			trace1("paste", cs.fullstring());
			lang->new__instance(cs, this, subckt());
			cs.read_line();
		}

		trace1("got paste", subckt()->size());
	}

public:
	rect_t bounding_rect() const override{ untested();
		rect_t r;
		for (auto i : *subckt()){ itested();
			auto c = i->center();
			r |= i->bounding_rect() + c;
		}
		return r;
	}
private:
	Port& port(unsigned) override { throw "unreachable"; }
};
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::activate(QObject* sender)
{ untested();
	SubcktBase* buf = nullptr;
	try{ untested();
		buf = new pastebuffer();
	}catch(...){ untested();
		incomplete();
		uncheck();
		return nullptr;
	}

  auto br = buf->bounding_rect();
  auto center = br.center();
  QPoint gc = doc().snapToGrid(QPoint(getX(center), getY(center)));
  center = pos_t(getX(gc), getY(gc));


  for(auto i : *buf->subckt()){ untested();
	  trace2("paste: centering", i->label(), br.center());
	  auto p = i->center();
	  i->setPosition(p - center);
  }

#if 0
  // BUG
  for(auto i : buf->wires()){ untested();
	  auto p = i->center();
	  i->setPosition(p - center);
  }
#endif

  _gfx = new ElementGraphics(buf);
  assert(_gfx);

  trace3("paste::activate", _gfx->boundingRect().topLeft(),
		               _gfx->boundingRect().bottomRight(), _gfx->childItems().count());


  doc().sceneAddItem(_gfx);

  if(1){
	  QPoint p = doc().mapFromGlobal(QCursor::pos());
	  auto sp = mapToScene(p);
	  _gfx->setPos(sp.x(), sp.y());
	  //_gfx->show();
  }else{
  }

  return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::release(QEvent* ev)
{ untested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QGraphicsSceneMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){ untested();
		cmd = makeNew(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::makeNew(QEvent* ev)
{ untested();
	// assert(ev->widget=doc->scene()) // or so.
	trace1("RELEASE", ev->type());
	if(ev->type() == QEvent::MouseButtonRelease){ itested();
	}else{ untested();
		unreachable();
	}

	if(!_gfx){ untested();
		unreachable();
		return nullptr;
	}else if(!element(_gfx)){ untested();
		unreachable();
		return nullptr;
	}else{ untested();
	}
	auto elt = element(_gfx);
	assert(elt);

	if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		QPointF pos = se->scenePos();
		QPoint xx = doc().snapToGrid(pos);
		_gfx->setPos(xx);
	}else{ untested();
	}

	cmd* c = new PasteCommand(doc(), _gfx);
	_gfx->hide();
	delete _gfx;
	_gfx = nullptr;

	uncheck();

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
//	uncheck();
//	doc().setActiveAction(nullptr);
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
		sp = doc().snapToGrid(sp);
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
	if(!_gfx){
		unreachable();
		return nullptr;
	}else{
	}
	trace1("paste enter", ev->type());
	auto ee = prechecked_cast<QEnterEvent*>(ev);
	assert(ee);
	
	auto wp = ee->localPos();

	SchematicDoc* d = &doc();
	auto sp = d->mapToScene(wp.toPoint());

	assert(_gfx);

	trace1("enter", sp);
	_gfx->setPos(sp.x(), sp.y());
	
	doc().sceneAddItem(_gfx);

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionPaste::leave(QEvent* ev)
{ untested();
	if(!_gfx){
		unreachable();
		return nullptr;
	}else{
	}
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
