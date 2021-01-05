/***************************************************************************
    copyright            : (C) 2018, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "schematic_scene.h"
#include "schematic_doc.h"
#include "qt_compat.h"
#include "globals.h"
#include "place.h"

#include <QFileInfo>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsProxyWidget>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
QPoint SchematicScene::gridSize() const
{
	assert(doc());
	return doc()->gridSize();
}
/*--------------------------------------------------------------------------*/
QGraphicsItem& SchematicScene::addElement(Element* x)
{
	QGraphicsItem* i=nullptr;
///	if(auto w=x->newWidget()){ untested();
///		i = addWidget(w);
///		assert(i);
///		i->setFlags(QGraphicsItem::ItemIsSelectable
///		           |QGraphicsItem::ItemIsMovable
///		           //|QGraphicsItem::ItemSendsGeometryChanges
///					  );
///	}else
	{itested();
		i = new ElementGraphics(x);
		addItem(i);
		i->show();
	}
	return *i;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* remove. later. */
// scene()->selectedItems gives QGraphicsItems
Element* element(ElementGraphics* g)
{
//	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!g) return nullptr;
	return g->operator->();
}
#if 0
Component* component(QGraphicsItem* g)
{
	incomplete();
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return component(e->operator->());
}
WireLabel* wireLabel(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return wireLabel(e->operator->());
}
Diagram* diagram(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return dynamic_cast<Diagram*>(e->operator->());
}
Painting* painting(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return painting(e->operator->());
}
Marker* marker(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return marker(e->operator->());
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SchematicScene::SchematicScene(QObject *parent)
  : QGraphicsScene(parent)
{
}
/*--------------------------------------------------------------------------*/
SchematicDoc* SchematicScene::doc()
{
	assert(parent());
	return dynamic_cast<SchematicDoc*>(parent());
}
/*--------------------------------------------------------------------------*/
SchematicDoc const* SchematicScene::doc() const
{
	assert(parent());
	return dynamic_cast<SchematicDoc const*>(parent());
}
/*--------------------------------------------------------------------------*/
SchematicScene::~SchematicScene()
{
}
/*--------------------------------------------------------------------------*/
QPoint SchematicScene::snapToGrid(QPointF const& p) const
{
	assert(doc());
	return doc()->setOnGrid(getX(p), getY(p));
}
/*--------------------------------------------------------------------------*/
void SchematicScene::drawBackground(QPainter *painter, const QRectF &r)
{itested(); // for now.
	//	QGraphicsScene::drawBackground(painter, r);

	double gridSize = 20; // TODO

	QPen pen;
	painter->setPen(pen);

	// not sure if this is good.
	double left = r.left() - fmod(r.left(), gridSize);
	double top = r.top() - fmod(r.top(), gridSize);
	for (double x=left; x<r.right(); x+=gridSize){
		for (double y=top; y<r.bottom(); y+=gridSize){
			painter->drawPoint(QPointF(x,y));
		}
	}
#if 0
	/// \todo getter and setter
	int GridX = 10;
	int GridY = 10;

	// Get associated view, assume single view
	// SchematicDoc *v = static_cast<SchematicDoc *>(views().at(0));

	// When scaling, adjust visible grid spacing
	float scale = 1;// v->Scale;
	if(scale < 1) {
		if(scale > 0.5) {
			GridX *= 4;
			GridY *= 4;
		} else {
			GridX *= 16;
			GridY *= 16;
		}
	}
#endif
}

#ifndef USE_SCROLLVIEW
// void ElementGraphics::paintScheme(SchematicDoc *p)
// { untested();
//   	assert(_e);
// 	_e->paintScheme(p);
// }

// scene::display(SchematicModel&)?
// 'l' is a bit of a hack. let's see
void SchematicModel::toScene(QGraphicsScene&, QList<ElementGraphics*>*) const
{
	incomplete(); // this is too strange.
	assert(false); // obsolete
}

#if 0
void SchematicScene::removeItem(Element const* xx)
{ unreachable();
}
#endif

#endif

#if 0

// FIXME: is the weird order really necessary?
void SchematicScene::selectedItemsAndBoundingBox(QList<ElementGraphics*>& ElementCache, QRectF& BB)
{
	assert(false); // obsolete
	for(auto elt : selectedItems()){
		if(BB.isEmpty()){
			// BUG
			BB = elt->boundingRect();
		}else{
			BB = BB.united(elt->boundingRect());
		}
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		qDebug() << "unite" << BB;
		assert(eg);
		if(wireLabel(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
	for(auto elt : selectedItems()){
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		assert(eg);
		if(component(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
	for(auto elt : selectedItems()){
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		assert(eg);
		if(wire(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
	for(auto elt : selectedItems()){
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		assert(eg);
		if(painting(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
}
#endif

void SchematicScene::dropEvent(QGraphicsSceneDragDropEvent*)
{
	// getting here when dropping stuff from the component menu
	incomplete();
}

bool SchematicScene::itemEvent(QEvent* e)
{ itested();
	if(!e){ untested();
		unreachable();
		return false;
	}else{
		trace1("scene::itemEvent", e->type());
		return doc()->handleMouseActions(e);
	}
}

//
#include "component_widget.h"
#include <QMimeData>
// https://stackoverflow.com/questions/14631098/qt-properly-integrating-undo-framework-with-qgraphicsscene

// should return true if the event e was "recognized and processed."
// (whatever that means)
//
// this is called before SchematicDoc::event
// (bound to some condition??)
bool SchematicScene::event(QEvent* e)
{ itested();
	// 155 for move.
	trace2("SchematicScene::event", e->isAccepted(), e->type());

	if(dynamic_cast<QGraphicsSceneEvent*>(e)){ itested();
	}else{ itested();
	}

	doc()->handleMouseActions(e);
	bool r = false;
	if(e->isAccepted()){
	}else{itested();
		trace0("fwd");
		r = QGraphicsScene::event(e);
	}

	trace3("SchematicScene::event post", e->isAccepted(), e->type(), r);

	if(e->isAccepted()){ itested();
		if(r){ itested();
			// move objects is here.
		}else{itested();
		}

		// doc()->handleMouseActions(e);
//		e->ignore(); // pass on to view??
	}else{itested();
		if(r){itested();
			// "new ground".
			// "rectangle draw"
		}else{itested();
		}
		// recrangle release also here.
		// done in the "View".
		//
//		doc()->handleMouseActions(e);
	}
	assert(doc());

	// TODO: this is just a stub, untangle!
	if(dynamic_cast<QDragEnterEvent*>(e)){ untested();
		trace1("scene leave", e->isAccepted());
	}else if(dynamic_cast<QMouseEvent*>(e)){ untested();
		trace1("MOUSE", e->isAccepted());
	}else if(dynamic_cast<QDragLeaveEvent*>(e)){ untested();
		trace1("scene enter", e->isAccepted());
	}else if(auto de=dynamic_cast<QGraphicsSceneDragDropEvent*>(e)){
		// 164 == QEvent::GraphicsSceneDragEnter
		// 165 == QEvent::GraphicsSceneDragMove
		// 166 == QEvent::GraphicsSceneDragLeave
		trace2("scene dragdrop", e->type(), e->isAccepted());
		auto a = ComponentWidget::itemMimeType();

		if(e->type()==QEvent::GraphicsSceneDragEnter){
			selectAll(false);
			// create element and switch to "move" mode?
			 if (de->mimeData()->hasFormat(a)){ untested();
				trace2("got payload", e->type(), e->isAccepted());

				QByteArray eltInfo = de->mimeData()->data(a);
				QDataStream dataStream(&eltInfo, QIODevice::ReadOnly);
				ComponentListWidgetItem a;
				dataStream >> a;

				Element* elt = a.cloneElement();
				assert(elt);

				trace1("setting pos", de->scenePos());
				auto pos = de->scenePos();
				elt->setPosition(pos_t(pos.x(), pos.y()));
				auto gfx = new ElementGraphics(elt);
				{ untested();
					addItem(gfx);
				}

				gfx->setSelected(true);

				if(0){
					QGraphicsSceneMouseEvent fake(QEvent::GraphicsSceneMousePress);
//					gfx->sceneEvent(&fake);
				}
			 }else{
			 }
		}else if(e->type()==165){
			e->accept(); // this sets the plus
			             // only do if mime type match...
		}else{
		}
	}else if(dynamic_cast<QGraphicsSceneMoveEvent*>(e)){
		trace1("GSM", e->type());
	}else if(dynamic_cast<QGraphicsSceneEvent*>(e)){
		trace1("GSE", e->type());
		if(e->type()==155){ itested();
			// getting here when moving around elements.
		}else{
		}
	}else{
		// 11 = leave?
		trace1("scene unknown?", e->type());
	}

	return r;
}
/*--------------------------------------------------------------------------*/
// Node const* SchematicScene::nodeAt(pos_t p) const
// {
// 	return doc()->nodeAt(p);
// }
/*--------------------------------------------------------------------------*/
bool SchematicScene::isConductor(pos_t p) const
{itested();
	QPointF pos(p.first, p.second);
	auto it = items(pos);
	for(auto i : it){itested();
		if(auto c = dynamic_cast<Conductor const*>(element(i))){
			return c->isNet(p);
		}else{itested();
		}
	}
	return false;
}
/*--------------------------------------------------------------------------*/
void SchematicScene::selectAll(bool v)
{
	for(auto i : items()){ untested();
		i->setSelected(v);
	}
}
/*--------------------------------------------------------------------------*/
static QList<ElementGraphics*> filter(QList<QGraphicsItem*> L)
{
	trace1("items raw mutable", L.size());
	for(auto l = L.begin(); l!=L.end(); ){ untested();
		if(dynamic_cast<ElementGraphics*>(*l)){ untested();
			++l;
		}else{ untested();
			// incomplete(); // actually
			auto prev = l;
			l = L.erase(prev);
		}
	}
	auto EL = reinterpret_cast<QList<ElementGraphics*>* >(&L);
	return *EL;
}
/*--------------------------------------------------------------------------*/
static QList<ElementGraphics const*> filter_const(QList<QGraphicsItem*> L)
{
	auto fL = filter(L);
	auto EL = reinterpret_cast<QList<ElementGraphics const*>* >(&fL);
	return *EL;
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicScene::selectedItems() const
{ untested();
	auto L = QGraphicsScene::selectedItems();
	return filter(L);
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicScene::items()
{
	auto L = QGraphicsScene::items();
	return filter(L);
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics const*> SchematicScene::items() const
{ untested();
	auto L = QGraphicsScene::items();
	trace1("items raw", L.size());
	for(auto l = L.begin(); l!=L.end(); ){ untested();
		if(dynamic_cast<ElementGraphics const*>(*l)){ untested();
			++l;
		}else{ untested();
			// incomplete(); // actually
			auto prev = l;
			l = L.erase(prev);
		}
	}
	auto EL = reinterpret_cast<QList<ElementGraphics const*>* >(&L);
	return *EL;
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics const*> SchematicScene::items(QRectF const& r) const
{itested();
	auto L = QGraphicsScene::items(r);
	auto b = filter(L);
	return *reinterpret_cast<QList<ElementGraphics const*>* >(&b);
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics const*> SchematicScene::items(int x, int y) const
{itested();
	QPointF r(x, y);
	auto L = QGraphicsScene::items(r);
	auto b = filter_const(L);
	return b;
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicScene::items(int x, int y)
{itested();
	QPointF r(x, y);
	auto L = QGraphicsScene::items(r);
	auto b = filter(L);
	return b;
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicScene::items(QRectF const& r)
{itested();
	auto L = QGraphicsScene::items(r);
	auto b = filter(L);
	return b;
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> SchematicScene::items(
		const QPointF &pos, Qt::ItemSelectionMode mode,
		Qt::SortOrder order) const
{itested();
	auto L = QGraphicsScene::items(pos, mode, order);
	for(auto l = L.begin(); l!=L.end(); ){itested();
		if(prechecked_cast<ElementGraphics*>(*l)){itested();
			++l;
		}else{ untested();
			// incomplete(); // actually
			auto prev = l;
			l = L.erase(prev);
		}
	}
	auto EL = reinterpret_cast<QList<ElementGraphics*>* >(&L);
	return *EL;
}
/*--------------------------------------------------------------------------*/
SchematicModel* SchematicScene::scope()
{
	assert(doc());
	return doc()->model(); // yikes.
}
/*--------------------------------------------------------------------------*/
Element* SchematicScene::detachFromModel(Element* e)
{
#if 1
	e->scope()->detach(e);
	Element const* ce = e;
	assert(ce->owner()==nullptr);
	return e;
#else
	auto c = e->clone();
	e->scope()->erase(e);
	return c;
#endif
}
/*--------------------------------------------------------------------------*/
// new_place?
Place const* SchematicScene::new_place(pos_t const& p)
{
	auto list = items(getX(p), getY(p));
	Place const* ret=nullptr;
	for(auto g : list){
		auto c = dynamic_cast<Place const*>(element(g));
		if(!c){ untested();
		}else if(p == c->position()){ untested();
			assert(getX(g->pos()) == getX(c->position()));
			assert(getY(g->pos()) == getY(c->position()));
			Element const* e = element(g);
			ret = prechecked_cast<Place const*>(e);
			break;
		}
	}
	
	if(ret){
	}else{
		auto cl = symbol_dispatcher.clone("place");
		assert(cl);
		std::string place_name = "_net_" + std::to_string(getX(p))
		                           + "_" + std::to_string(getY(p));

		{ // something_like_new_instance
			assert(!cl->mutable_owner());
			doc()->addElement(cl);
//			cl->setOwner(root);
//			scope()->push_back(cl);
			assert(cl->mutable_owner());
			cl->setPosition(p);
			cl->setLabel(place_name);
			cl->set_port_by_index(0, place_name);
//
			addElement(cl);
		}

		QGraphicsItem* gg = new ElementGraphics(cl);
		addItem(gg);
		gg->show(); // really?
		ret = prechecked_cast<Place*>(cl);
		assert(ret);
	}
	return ret;
}
/*--------------------------------------------------------------------------*/
Place const* SchematicScene::is_place(pos_t const& p) const
{
	auto list = items(getX(p), getY(p));
	Place const* ret=nullptr;
	for(auto g : list){
		auto c = dynamic_cast<Place const*>(element(g));
		if(!c){ untested();
		}else if(p == c->position()){ untested();
			assert(getX(g->pos()) == getX(c->position()));
			assert(getY(g->pos()) == getY(c->position()));
			Element const* e = element(g);
			ret = prechecked_cast<Place const*>(e);
			break;
		}
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
void SchematicScene::connectPorts(Symbol* c)
{
	assert(c->mutable_owner());

	for(unsigned i=0; c->portExists(i); ++i){
//		assert(portValue()==""); // ?
		try{
			pos_t p = c->nodePosition(i);
			Symbol const* q = new_place(p);
			std::string n = q->port_value(0);
			trace3("connectPorts", c->label(), i, n);

			assert(c->mutable_owner());
			c->set_port_by_index(i, n);
		}catch(Exception const&){ untested();
			// pass.
		}
	}
}
/*--------------------------------------------------------------------------*/
void SchematicScene::attachToModel(Element* e)
{
	Element const* ce = e;
	assert(!ce->owner());
	
	doc()->addElement(e);
	assert(ce->owner());
	if(auto s=dynamic_cast<Symbol*>(e)){
		connectPorts(s);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
void SchematicScene::possiblyRename(Element* e) const
{
	std::string label = e->label();

   int z = label.size();
	if(!z){
	}else if(std::isdigit(label[z-1])){
	}else{
		unsigned i = scope()->nextIdx(label);
		e->setLabel(label + std::to_string(i));
		// gfx->update();
	}
}
/*--------------------------------------------------------------------------*/
bool SchematicScene::isNode(pos_t x) const
{
	return is_place(x); // check if connected?
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
