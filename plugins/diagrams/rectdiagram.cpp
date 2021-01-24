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
#include "qucs_globals.h"
#include "module.h"
#include <QPlainTextEdit>
#include <QLineEdit>

namespace {

	// todo: share?
class DiagramWidget : public QWidget{
public:
	DiagramWidget(Diagram* d)
	: _diag(d){ untested();
		assert(d);
		Element const*e = d;
		auto br(e->bounding_rect());
		trace2("DiagramWidget", br.tl(), br.br());
		setGeometry(br.toRectF().toRect()); // this only sets the SIZE.
		                          // origin is in topleft corner.
										  //
//		setPlainText("RECTDIAGRAM");
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
private:
	RectDiagram(RectDiagram const& c) : Diagram(c), _widget(c._widget) {}
public:
	explicit RectDiagram() : Diagram(), _widget(nullptr){}
	~RectDiagram(){}

private:
	Element* clone() const{
		return new RectDiagram(*this);
	}
private:
	// is this needed?
	pos_t center() const override{
		return Element::center();
	}

	QWidget* newWidget(){ itested();
		trace2("rect newWidget", label(), y2);
		if(_widget){ untested();
		}else{ untested();
			_widget = new RectDiagramWidget(this);
			_widget->move(0, -y2); // gaah. the origin must be in the top left corner.
		}
		//w->move(_cx, _cy-y2); // gaah. the origin must be in the top left corner.
		return _widget;
	}

private:
	QWidget* _widget;
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Rect", &D);
Module::INSTALL pp("diagrams", &D);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
