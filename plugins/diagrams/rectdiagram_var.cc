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
// Ideally, some painting will be shared across diagrams. Not yet sure how to
// do excactly.
//
// This represents a variable in a plot, similar to "Graph" in around 0.0.20.
// Not sure if it should do any painting, or better leave it to the Diagram.
//
// The diagram could clip, but perhaps Qt should take care of it anyway.
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
#include <QPainterPath> // BUG.
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Data;
using qucs::Diagram;
using qucs::Painting;
using qucs::CommonData;
using qucs::ViewPainter;
using qucs::ElementList;
using qucs::SubcktBase;
using qucs::SimOutputDir;
using qucs::data_dispatcher;
using qucs::SimOutputData;
/*--------------------------------------------------------------------------*/
class DiagramVariable : public Data, public Painting {
public:
	DiagramVariable() : Data() {}
	DiagramVariable(DiagramVariable const& e)
	  : Data(e), _color(e._color), _thick(e._thick) {untested();}

public: // Painting
	virtual rect_t bounding_rect() const;
	void paint(ViewPainter*) const override;

public: // Element
	Element* clone() const override{untested();
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
	void prepare(){untested();
		if(common()){ untested();
			// TODO: debug check
		}else{untested();
			Element* o = this;
			while(o->owner()){untested();
				o = o->owner();
				trace1("var top?", o->label());
			}
			CommonData const* d = find_in_scope(o->scope(), label());
			if(d){untested();
				trace1("var found it", label());
				attach(d);
			}else{untested();
				trace1("var miss", label());
			}
		}
	}
	CommonData const* find_in_scope(ElementList const* scope, std::string const& what)
	{ untested();
		trace0("var sckt");
		if(scope) { untested();
			CommonData const* f=nullptr;
			for(auto x : *scope){untested();
				trace1("var Element", x->label());
				f = find_it(x, what);
				if(f){untested();
					break;
				}else{untested();
				}
			}
			return f;
		}else{untested();
			return nullptr;
		}
	}

	// don't know where the data is. do DFS.
	// could use Properties/data_set_name?
	CommonData const* find_it(Object const* where, std::string const& what)
	{ untested();
		if(!where){ untested();
			return nullptr;
		}else if(dynamic_cast<SubcktBase const*>(where)) { untested();
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
		}else if(auto d = dynamic_cast<SimOutputDir const*>(where)){untested();
			trace1("found dir", d->short_label());
			CommonData const* f=nullptr;
			for(auto x : *d){untested();
				f = find_it(x, what);
				if(f){untested();
					break;
				}else{untested();
				}
			}
			return f;
		}else if(auto d = dynamic_cast<Data const*>(where)){untested();
			d->refresh(); // prepare?
			trace2("var found data", d->label(), d->common());
			if(d->common()){untested();
				return find_it(d->common(), what);
			}else{ untested();
				return nullptr;
			}
		}else if(where->short_label() != what){untested();
			return nullptr;
		}else if(auto x=dynamic_cast<CommonData const*>(where)){untested();
			return x;
		}else{ untested();
			return nullptr;
		}
	}
private:
	void paint_graph_data(CommonData const* cd, ViewPainter* p) const;

private:
	std::string _color;
	std::string _thick;
	std::string _precision;
	std::string _num_mode;
	std::string _style;
	std::string _xaxisno;
}v0;
Dispatcher<Data>::INSTALL p0(&data_dispatcher, "rectdiagramvariable", &v0);
/*--------------------------------------------------------------------------*/
void DiagramVariable::set_param_by_index(index_t i, std::string const& value)
{untested();
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
rect_t DiagramVariable::bounding_rect() const
{ untested();
	if(auto p=dynamic_cast<Painting const*>(owner())){ untested();
		return p->bounding_rect();
	}else{ untested();
		unreachable(); //?
		return rect_t();
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DiagramVariable::paint(ViewPainter* v) const
{ untested();
	// is this a bug? can paint and prepare run concurrently?

	// Diagram::paint??
	if(common()){ untested();
		paint_graph_data(common(), v);
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
void DiagramVariable::paint_graph_data(CommonData const* cd, ViewPainter* p) const
{ untested();
	auto pp = prechecked_cast<SimOutputData const*>(cd);
	assert(pp);

	Element const* o = owner();
	auto diag = dynamic_cast<Diagram const*>(o);
	if(!diag){ untested();
		incomplete();
	}else if(pp->numDeps()==1){ untested();
		auto dd = dynamic_cast<SimOutputData const*>(pp->dep(0));
		assert(dd);
		int num = pp->size();
		QVector<double> x(num), y(num);
		int ii = 0;

		QPainterPath path;
		for (auto xx : *pp){ untested();
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
