/***************************************************************************
    copyright            : (C) 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*--------------------------------------------------------------------------*/
// This represents a variable in a plot, similar to "Graph" in around 0.0.20.
// Qt should take care of painting...
/*--------------------------------------------------------------------------*/
#include "data.h"
#include "diagram.h"
#include "output.h"
#include "exception.h"
#include "output.h"
#include "qucs_globals.h"
#include "element_list.h"
#include "sckt_base.h"
#include "viewpainter.h"
#include "painting.h"
#include <QGraphicsItem>
#include <QPainterPath>
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Data;
using qucs::Diagram;
using qucs::Element;
using qucs::CommonData;
using qucs::ElementList;
using qucs::SubcktBase;
using qucs::SimOutputDir;
using qucs::SimOutputData;
using qucs::element_dispatcher;
/*--------------------------------------------------------------------------*/
class DiagramVariable : public Data, public QGraphicsItem {
public:
	DiagramVariable() : Data(), QGraphicsItem() {}
	DiagramVariable(DiagramVariable const& e)
	  : Data(e), QGraphicsItem(/*e*/),
	    _color(e._color),
	    _thick(e._thick),
	    _precision(e._precision),
	    _num_mode(e._num_mode),
	    _style(e._style),
	    _xaxisno(e._xaxisno)
	{itested();
		// avoid (expensive) paint calls, one way or another
		setCacheMode(ItemCoordinateCache);
		// setCacheMode(DeviceCoordinateCache);
	}

private: // QGraphicsItem
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;

public: // Element
	Element* clone() const override{itested();
		return new DiagramVariable(*this);
	}
	index_t param_count() const{return 6;}
	void set_param_by_index(index_t i, std::string const& value) override;
	void set_param_by_name(std::string const& name, std::string const& value) override{ untested();
		if(name=="color"){ untested();
			_color = value;
		}else if(name=="thick"){ untested();
			_thick = value;
		}else{ untested();
			trace2("var" , name, value);
			incomplete();
		}
	}
	std::string param_value(index_t) const override;
	std::string param_value_by_name(std::string const& name) const override{ untested();
		if(name=="color"){ untested();
			return _color;
		}else if(name=="thick"){ untested();
			return _thick;
		}else{ untested();
			trace1("var" , name);
			throw qucs::ExceptionCantFind(name, label());
		}
	}
private:
	void prepare(){itested();
		if(common()){ untested();
			// TODO: debug check
		}else{itested();
			Element* o = this;
			while(o->owner()){itested();
				o = o->owner();
				trace1("var top?", o->label());
			}
			CommonData const* d = find_in_scope(o->scope(), label());
			if(d){itested();
				trace1("var found it", label());
				attach(d);
			}else{itested();
				trace1("var miss", label());
			}
		}
	}
	CommonData const* find_in_scope(ElementList const* scope, std::string const& what)
	{itested();
		trace0("var sckt");
		if(scope) {itested();
			CommonData const* f=nullptr;
			for(auto x : *scope){itested();
				trace1("var Element", x->label());
				f = find_it(x, what);
				if(f){itested();
					break;
				}else{itested();
				}
			}
			return f;
		}else{itested();
			return nullptr;
		}
	}

	// don't know where the data is. do DFS.
	// could use Properties/data_set_name?
	CommonData const* find_it(Object const* where, std::string const& what)
	{itested();
		if(!where){ untested();
			return nullptr;
		}else if(dynamic_cast<SubcktBase const*>(where)) {itested();
		//}else if(auto d = dynamic_cast<SubcktBase const*>(where)) { untested();
#if 0
			trace1("var sckt", d->label());
			if(d->scope()) { untested();
				CommonData const* f=nullptr;
				for(auto x : *d->scope()){ untested();
					f = find_it(x, what);
					if(f){ untested();
						break;
					}else{ untested();
					}
				}
				return f;
			}else{ untested();
				return nullptr;
			}
#endif
			return nullptr;
		}else if(auto d = dynamic_cast<SimOutputDir const*>(where)){itested();
			trace1("found dir", d->short_label());
			CommonData const* f=nullptr;
			for(auto x : *d){itested();
				f = find_it(x, what);
				if(f){itested();
					break;
				}else{itested();
				}
			}
			return f;
		}else if(auto d = dynamic_cast<Data const*>(where)){itested();
			d->refresh(); // prepare?
			trace2("var found data", d->label(), d->common());
			if(d->common()){itested();
				return find_it(d->common(), what);
			}else{ untested();
				return nullptr;
			}
		}else if(where->short_label() != what){itested();
			return nullptr;
		}else if(auto x=dynamic_cast<CommonData const*>(where)){itested();
			return x;
		}else{ untested();
			return nullptr;
		}
	}
private:
	void paint_graph_data(CommonData const* cd, QPainter* p) const;

private:
	std::string _color;
	std::string _thick;
	std::string _precision;
	std::string _num_mode;
	std::string _style;
	std::string _xaxisno;
}; // DiagramVariable
/*--------------------------------------------------------------------------*/
class RDV_factory : public qucs::Element{
	Element* clone() const override{
		return new RDV_factory(*this);
	}
	Element* clone_instance() const override{ untested();
		return new DiagramVariable();
	}
}v0;
Dispatcher<Element>::INSTALL p0(&element_dispatcher, "rectdiagramvariable", &v0);
/*--------------------------------------------------------------------------*/
void DiagramVariable::set_param_by_index(index_t i, std::string const& value)
{itested();
	switch(i){
	case 0:
		_color = value;
		break;
	case 1:
		_thick = value;
		break;
	case 2:
		_precision = value;
		break;
	case 3:
		_num_mode = value;
		break;
	case 4:
		_style = value;
		break;
	case 5:
		_xaxisno = value;
		break;
	default:
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
std::string DiagramVariable::param_value(index_t i) const
{ untested();
	switch(i){
	case 0:
		return _color;
	case 1:
		return _thick;
	case 2:
		return _precision;
	case 3:
		return _num_mode;
	case 4:
		return _style;
	case 5:
		return _xaxisno;
	default:
		incomplete();
		return "..";
	}
}
/*--------------------------------------------------------------------------*/
QRectF DiagramVariable::boundingRect() const
{ itested();
	if(parentItem()){
		return parentItem()->boundingRect();
	}else{
		unreachable(); //???
		return QRectF();
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DiagramVariable::paint(QPainter *painter, const QStyleOptionGraphicsItem*,
		QWidget*)
{ untested();
	if(common()){ untested();
		paint_graph_data(common(), painter);
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
// QPainterPath DiagramVariable::shape() const
// { untested();
//     QPainterPath path;
//     path.addEllipse(boundingRect());
//     return path;
// }
/*--------------------------------------------------------------------------*/
// under construction
void DiagramVariable::paint_graph_data(CommonData const* cd, QPainter* p) const
{ untested();
	auto pp = prechecked_cast<SimOutputData const*>(cd);
	assert(pp);

	Element const* o = owner();
	auto diag = dynamic_cast<Diagram const*>(o);
	if(!diag){ untested();
		incomplete();
	}else if(pp->numDeps()==1){ itested();
		auto dd = dynamic_cast<SimOutputData const*>(pp->dep(0));
		assert(dd);
		int num = pp->size();
		QVector<double> x(num), y(num);
		int ii = 0;

		QPainterPath path;
		for (auto xx : *pp){ itested();
			++ii;
			auto x = xx.first;
			auto y = xx.second.real();

			auto pp = diag->calcCoordinate(x, y);
			trace4("var::paint", x,y,pp.first, pp.second);

			// if first? path.moveTo(x,y);
			path.lineTo(getX(pp), getY(pp));
		}

		p->drawPath(path); // yikes.
		assert(ii==num);

	}else{ untested();
		incomplete();
	}

}
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
