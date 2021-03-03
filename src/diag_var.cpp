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

#include "data.h"
#include "exception.h"
#include "output.h"
#include "qucs_globals.h"
#include "element_list.h"
#include "sckt_base.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Data;
using qucs::CommonData;
using qucs::ViewPainter;
using qucs::ElementList;
using qucs::SubcktBase;
using qucs::SimOutputDir;
using qucs::data_dispatcher;
/*--------------------------------------------------------------------------*/
class DiagramVariable : public Data {
public:
	DiagramVariable() : Data() {}
	DiagramVariable(DiagramVariable const& e)
	  : Data(e), _color(e._color), _thick(e._thick) {}

public: // Element
	Element* clone() const override{itested();
		return new DiagramVariable(*this);
	}
	void paint(ViewPainter*) const{}
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
	std::string get_param_by_name(std::string const& name) const override{ untested();
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
			}else{ untested();
				trace1("var miss", label());
			}
		}
	}
	CommonData const* find_in_scope(ElementList const* scope, std::string const& what)
	{
		trace0("var sckt");
		assert(scope);
		if(scope) {
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
		}else{ untested();
			return nullptr;
		}
	}

	// don't know where the data is. do DFS.
	// could use Properties/data_set_name?
	CommonData const* find_it(Object const* where, std::string const& what)
	{
		if(!where){ untested();
			return nullptr;
		}else if(auto d = dynamic_cast<SubcktBase const*>(where)) {
#if 0
			trace1("var sckt", d->label());
			if(d->scope()) {
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
	std::string _color;
	std::string _thick;
	std::string _precision;
	std::string _num_mode;
	std::string _style;
	std::string _xaxisno;
}v0;
Dispatcher<Data>::INSTALL p0(&data_dispatcher, "diagramvariable", &v0);
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
{
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
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
