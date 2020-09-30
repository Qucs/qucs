/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "diagram.h"
#include "misc.h"
#include "globals.h"
#include "module.h"
#include <QLineEdit>

namespace {

class DiagramWidget : public QWidget{
public:
	DiagramWidget(Diagram* d)
	: _diag(d){ untested();
		assert(d);
		QRectF br(d->boundingRect());
		trace2("DiagramWidget", br.topLeft(), br.bottomRight());
		setGeometry(br.toRect()); // this only sets the SIZE.
		                          // origin is in topleft corner.
	}
// 	QSize sizeHint() const override{
// 		return QSize(30,30);
// 	}

private:
	Diagram* _diag;
};

class RectDiagramWidget : public DiagramWidget{
public:
	explicit RectDiagramWidget(Diagram* d) : DiagramWidget(d){
	}
};

// must be a factory, because cant instanciate QtObject statically
class RectDiagram : public Diagram  {
	RectDiagram(RectDiagram const& c) : Diagram(c) {}
public:
	explicit RectDiagram(){}
	~RectDiagram(){}

private:
	Element* clone() const{
		return new RectDiagram(*this);
	}
private:
	// is this needed?
	std::pair<int, int> center() const override{
		return Element::center();
	}

	QWidget* newWidget(){ untested();
		QWidget* w=new RectDiagramWidget(this);
		//w->move(_cx, _cy-y2); // gaah. the origin must be in the top left corner.
		w->move(0, -y2); // gaah. the origin must be in the top left corner.
		return w;
	}
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Rect", &D);
Module::INSTALL pp("diagrams", &D);

}
