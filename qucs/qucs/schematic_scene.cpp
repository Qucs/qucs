
#include "schematic_scene.h"
#include "schematic.h"

#include <QFileInfo>


// ---------------------------------------------------
//
#if QT_VERSION >= 0x050000
ElementGraphics::ElementGraphics() : QGraphicsItem()
{
	unreachable();
}

ElementGraphics::ElementGraphics(Element* e)
	: QGraphicsItem(), _e(e)
{
	setFlags(ItemIsSelectable|ItemIsMovable);
	setAcceptHoverEvents(true);
	assert(_e);
}

QRectF ElementGraphics::boundingRect() const
{ itested();
	assert(_e);
	return _e->boundingRect();
}

void ElementGraphics::setSelected(bool s)
{
	qDebug() << "setSeletected" << s << this;
	QGraphicsItem::setSelected(s);
	assert(QGraphicsItem::isSelected()==s);
	assert(_e);
	_e->setSelected(s);
}

// ?!
void ElementGraphics::setPos(int a, int b)
{
	assert(_e);
	qDebug() << "EG::setPos" << a << _e->cx_();
	QGraphicsItem::setPos(QPointF(a, b));
	qDebug() << "EG::setPos" << boundingRect();
}
#endif
// ---------------------------------------------------
// forward to graphicscene, once it is there.
ElementGraphics* Schematic::itemAt(float x, float y)
{
	qDebug() << "Schematic::itemAt" << x << y;
#if QT_VERSION >= 0x050000
	QPoint p(x, y);
	QGraphicsItem* I=scene()->itemAt(p, QTransform());
	if(ElementGraphics* G=dynamic_cast<ElementGraphics*>(I)){ untested();
		qDebug() << "got something";
		return G;
	}else{ untested();
		qDebug() << "miss";
		return nullptr;
	}
#else
	for(Component *pc = Components->first(); pc != 0; pc = Components->next())
		if(pc->getSelected(x, y))
			return (ElementGraphics*)(pc);

	float Corr = 5.0 / Scale; // size of line select

    for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
        if(pp->getSelected(x, y, Corr))
            return (ElementGraphics*)(pp);

	incomplete(); // also select the other stuff.
	return nullptr;
#endif
}

#ifndef USE_SCROLLVIEW
Element* element(ElementGraphics* e){
	if(!e) return nullptr;
	return e->operator->();
}
Component* component(ElementGraphics* e){
	if(!e) return nullptr;
	return component(e->operator->());
}
Wire* wire(ElementGraphics* e){
	if(!e) return nullptr;
	return wire(e->operator->());
}
WireLabel* wireLabel(ElementGraphics* e){
	if(!e) return nullptr;
	return wireLabel(e->operator->());
}
Diagram* diagram(ElementGraphics* e){
	if(!e) return nullptr;
	return diagram(e->operator->());
}
Painting* painting(ElementGraphics* e){
	if(!e) return nullptr;
	return painting(e->operator->());
}
Marker* marker(ElementGraphics* e){
	if(!e) return nullptr;
	return marker(e->operator->());
}
Node* node(ElementGraphics* e){
	if(!e) return nullptr;
	return node(e->operator->());
}
Graph* graph(ElementGraphics* e){
	if(!e) return nullptr;
	return graph(e->operator->());
}

SchematicScene::SchematicScene(QObject *parent) :
  QGraphicsScene(parent)
{
}

SchematicScene::~SchematicScene()
{
}

void SchematicScene::drawBackground(QPainter *painter, const QRectF &rect)
{
	QGraphicsScene::drawBackground(painter, rect);
	return;

	// Draw origin when visible
	if(rect.contains(QPointF(0, 0))) {
		painter->drawLine(QLine(-3.0, 0.0, 3.0, 0.0));
		painter->drawLine(QLine(0.0, -3.0, 0.0, 3.0));
	}else{
	}
	/// \todo getter and setter
	int GridX = 10;
	int GridY = 10;

	// Get associated view, assume single view
	Schematic *v = static_cast<Schematic *>(views().at(0));

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

void ElementGraphics::paintScheme(Schematic *p)
{
  	assert(_e);
	_e->paintScheme(p);
}

void SchematicModel::toScene(QGraphicsScene& s) const
{
  for(auto i : components()){ itested();
    s.addItem(new ElementGraphics(i));
  }
  for(auto i : wires()){ itested();
    s.addItem(new ElementGraphics(i));
  }
  qDebug() << "wires" << s.items().size();
  s.update();
}


#endif
