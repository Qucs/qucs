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
/*--------------------------------------------------------------------------*/
#include "qucs_globals.h"
#include "symbol.h"
#include "viewpainter.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using qucs::Symbol;
/*--------------------------------------------------------------------------*/
class NodeLabel : public Symbol {
public:
	explicit NodeLabel() : Symbol(), _port() {}
	~NodeLabel(){}
private:
	NodeLabel(NodeLabel const& p)
     : Symbol(p), _port(),
		 _highlight(p._highlight) {}
	Element* clone()const override{
		return new NodeLabel(*this);
	}

	void paint(ViewPainter*) const override;
	std::string param_value_by_name(std::string const& n) const override;
	void set_param_by_name(std::string const& n, std::string const& v) override;
	virtual unsigned numPorts() const {return 1;}
	rect_t bounding_rect() const override;
	Port& port(unsigned i);
	pos_t portPosition(unsigned) const override{ return pos_t(0,0);}
private:
	Port _port;
	bool _highlight;
}d;
static Dispatcher<Symbol>::INSTALL p(&qucs::symbol_dispatcher, "NodeLabel", &d);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
std::string NodeLabel::param_value_by_name(std::string const& n) const
{
	return Symbol::param_value_by_name(n);
}
/*--------------------------------------------------------------------------*/
void NodeLabel::set_param_by_name(std::string const& n, std::string const& v)
{
	if(n == "netname"){
		incomplete();
	}else{
		Symbol::set_param_by_name(n, v);
	}
}
/*--------------------------------------------------------------------------*/
Port& NodeLabel::port(unsigned n)
{
	assert(n==0);
	return _port;
}
/*--------------------------------------------------------------------------*/
rect_t NodeLabel::bounding_rect() const
{
    return rect_t(0, 0, 15, 15);
}
/*--------------------------------------------------------------------------*/
void NodeLabel::paint(ViewPainter* p) const
{
	int w=5;

	int px = 10;
	int py = 10;

	p->drawLine(0, 0, px, py);
	p->drawLine(px, py, px+w, py);
	p->drawLine(px, py, px, py+w);
}
/*--------------------------------------------------------------------------*/

} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
