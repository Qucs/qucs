
#include "schematic_scene.h"
#include "schematic_doc.h"
#include "qt_compat.h"

#include <QFileInfo>

// ---------------------------------------------------
// forward to graphicscene, once it is there.
// BUG: what if there are multiple items?
ElementGraphics* SchematicDoc::itemAt(float x, float y)
{
	QPointF p(x, y);
	QGraphicsItem* I=scene()->itemAt(p, QTransform());
	if(ElementGraphics* G=dynamic_cast<ElementGraphics*>(I)){ untested();
		qDebug() << "got something" << element(G)->name();
		return G;
	}else{ untested();
		qDebug() << "miss";
		return nullptr;
	}
}

#ifndef USE_SCROLLVIEW
// scene()->selectedItems gives QGraphicsItems
Element* element(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return e->operator->();
}
Component* component(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return component(e->operator->());
}
Wire* wire(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return wire(e->operator->());
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
	return diagram(e->operator->());
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
Node* node(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return node(e->operator->());
}
Graph* graph(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return graph(e->operator->());
}

#endif

SchematicScene::SchematicScene(QObject *parent)
  : QGraphicsScene(parent)
{
}

SchematicDoc* SchematicScene::doc()
{
	assert(parent());
	return dynamic_cast<SchematicDoc*>(parent());

}

SchematicScene::~SchematicScene()
{
}

void SchematicScene::drawBackground(QPainter *painter, const QRectF &rect)
{
	QGraphicsScene::drawBackground(painter, rect);

	// Draw origin when visible
	if(rect.contains(QPointF(0, 0))) {
		painter->drawLine(QLine(-3.0, 0.0, 3.0, 0.0));
		painter->drawLine(QLine(0.0, -3.0, 0.0, 3.0));
	}else{
	}
	return;
	/// \todo getter and setter
	int GridX = 10;
	int GridY = 10;

	// Get associated view, assume single view
	SchematicDoc *v = static_cast<SchematicDoc *>(views().at(0));

	// When scaling, adjust visible grid spacing
	float scale = v->Scale;
	if(scale < 1) {
		if(scale > 0.5) {
			GridX *= 4;
			GridY *= 4;
		} else {
			GridX *= 16;
			GridY *= 16;
		}
	}
}

#ifndef USE_SCROLLVIEW
void ElementGraphics::paintScheme(SchematicDoc *p)
{ untested();
  	assert(_e);
	_e->paintScheme(p);
}

// scene::display(SchematicModel&)?
// 'l' is a bit of a hack. let's see
void SchematicModel::toScene(QGraphicsScene& s, QList<ElementGraphics*>* l) const
{
	incomplete(); // this is too strange.
  for(auto i : components()){ untested();
    auto x=new ElementGraphics(i);
	 if(l){
		 l->append(x);
	 }
    s.addItem(x);
  }
  for(auto i : wires()){ untested();
    auto x=new ElementGraphics(i);
	 if(l){
		 l->append(x);
	 }
    s.addItem(x);
  }

  incomplete(); // do the others...
  qDebug() << "wires" << s.items().size();
  s.update();
}

void SchematicScene::removeItem(Element const* xx)
{ unreachable();
}

// FIXME: is the weird order really necessary?
void SchematicScene::selectedItemsAndBoundingBox(QList<ElementGraphics*>& ElementCache, QRectF& BB)
{
	for(auto elt : selectedItems()){
		if(BB.isEmpty()){
			// BUG
			BB = elt->boundingRect();
		}else{
			BB = BB.united(elt->boundingRect());
		}
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		qDebug() << "selected" << element(eg)->name() << element(eg)->boundingRect();
		qDebug() << "unite" << BB;
		assert(eg);
		if(auto l=wireLabel(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
	for(auto elt : selectedItems()){
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		assert(eg);
		if(auto c=component(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
	for(auto elt : selectedItems()){
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		assert(eg);
		if(auto w=wire(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
	for(auto elt : selectedItems()){
		ElementGraphics* eg=prechecked_cast<ElementGraphics*>(elt);
		assert(eg);
		if(auto p=painting(elt)){
			ElementCache.append(eg);
		}else{
		}
	}
}
#endif

void SchematicScene::dropEvent(QGraphicsSceneDragDropEvent* e)
{
	incomplete();
}

bool SchematicScene::itemEvent(QEvent* e)
{ untested();
	trace1("scene::itemEvent", e->type());
	return doc()->handleMouseActions(e);
}

// https://stackoverflow.com/questions/14631098/qt-properly-integrating-undo-framework-with-qgraphicsscene

// should return true if the event e was "recognized and processed."
// (whatever that means)
//
// this is called before SchematicDoc::event
bool SchematicScene::event(QEvent* e)
{ untested();
	trace2("SchematicScene::event", e->isAccepted(), e->type());
	bool r = QGraphicsScene::event(e);
	trace2("SchematicScene::event post", e->isAccepted(), e->type());
	if(e->isAccepted()){ itested();
		// move objects is here.
		//
		// // but also release.
		// signal/slot instead? (why/how?)
		// doc()->handleMouseActions(e);
//		e->ignore(); // pass on to view??
	}else{ untested();
		// rectangle draw is here
		// recrangle release also here.
		// done in the "View".
		//
//		doc()->handleMouseActions(e);
	}
	assert(doc());



	return r;
}
