
#include "schematic_scene.h"
#include "schematic.h"

// ---------------------------------------------------
// forward to graphicscene, once it is there.
ElementGraphics* Schematic::itemAt(float x, float y)
{
#if QT_VERSION >= 0x050000
	QPointF p(x, y);
	QGraphicsItem* I=scene()->itemAt(p, QTransform());
	if(ElementGraphics* G=dynamic_cast<ElementGraphics*>(I)){
		return G;
	}else{
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

#if QT_VERSION >= 0x050000
Component* component(ElementGraphics* e){
	return component(e->operator->());
}
Wire* wire(ElementGraphics* e){
	return wire(e->operator->());
}
WireLabel* wireLabel(ElementGraphics* e){
	return wireLabel(e->operator->());
}
Diagram* diagram(ElementGraphics* e){
	return diagram(e->operator->());
}
Painting* painting(ElementGraphics* e){
	return painting(e->operator->());
}
#endif
