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

#include "qucs_globals.h"
#include "data.h"
#include "exception.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class DiagramVariable : public Data {
public:
	DiagramVariable() : Data() {}
	DiagramVariable(DiagramVariable const& e)
	  : Data(e), _color(e._color), _thick(e._thick) {}

public: // Element
	Element* clone() const override{
		return new DiagramVariable(*this);
	}
	void paint(ViewPainter*) const{}
	index_t param_count() const{return 6;}
	void set_param_by_index(index_t i, std::string const& value) override;
	void set_param_by_name(std::string const& name, std::string const& value) override{
		if(name=="color"){
			_color = value;
		}else if(name=="thick"){
			_thick = value;
		}else{
			trace2("var" , name, value);
			incomplete();
		}
	}
	std::string param_value(index_t) const override;
	std::string get_param_by_name(std::string const& name) const override{
		if(name=="color"){
			return _color;
		}else if(name=="thick"){
			return _thick;
		}else{
			trace1("var" , name);
			throw qucs::ExceptionCantFind(name, label());
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
Dispatcher<Data>::INSTALL p0(&dataDispatcher, "diagramvariable", &v0);
/*--------------------------------------------------------------------------*/
void DiagramVariable::set_param_by_index(index_t i, std::string const& value)
{
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
