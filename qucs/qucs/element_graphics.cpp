#include "element.h"
#include "schematic_scene.h"
#include <QGraphicsScene>


#ifndef USE_SCROLLVIEW
void ElementGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	assert(_e);
	assert(painter);
	ViewPainter v(painter);
	auto br=boundingRect();

	// debug.
	if(isSelected()){
		painter->fillRect(br, QColor("grey"));
	}else{
		painter->fillRect(br, QColor("white"));
	}

	_e->paint(&v);
}
#endif
