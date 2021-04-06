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

// EDD symbol. shared across old and new EDD

#include "qucs_globals.h"
#include "viewpainter.h"
#include "obsolete_stuff.h"
#include "painting.h"
#include "symbol.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
static const std::string prefix = ":EDD_";
/*--------------------------------------------------------------------------*/
using qucs::Painting;
using qucs::Symbol;
using qucs::Element;
/*--------------------------------------------------------------------------*/
class ComponentPort : public Port{
public:
  ComponentPort(const ComponentPort&p) : Port(), _p(p._p) {}
  explicit ComponentPort(int x, int y)
	  : Port(), _p(x, y)
  {
  }

public:
  pos_t const& position() const { return _p; }
  void setPosition(int x, int y) { _p=pos_t(x,y); }
  void setPosition(pos_t p) { _p=p; }
  int x() const{ return getX(_p); }
  int y() const{ return getY(_p); }

private:
  pos_t _p;
};
/*--------------------------------------------------------------------------*/
class DefaultSymbol : public Symbol{
public:
	explicit DefaultSymbol() : Symbol() {}
	// explicit DefaultSymbol(CommonComponent* s);
	~DefaultSymbol() {};

private:
	explicit DefaultSymbol(DefaultSymbol const&x)
	    : Symbol(x), _branches(x._branches){
	}
	std::string dev_type()const { untested();
		return prefix + std::to_string(_branches);
	}
	// 	pos_t portPosition(index_t i) const override { untested();
	// 		auto r = Component::portPosition(i);
	// 		trace3("defsym pp", i, getX(r), getY(r));
	// 		return r;
	// 	}
public:
	Symbol* clone() const override{
		return new DefaultSymbol(*this);
	}

protected:
	void init();

private: // Symbol
	// bool portExists(index_t) const override;
	// std::string const portName(index_t) const override;

	void set_param_by_name(std::string const& name, std::string const& value){
		if(name == "Branches"){
			_branches = atoi(value.c_str());
			assert(_branches);
			init();
		}else{ untested();
			unreachable();
		}
	}

	index_t numPorts() const override{return _ports.size();}
	pos_t portPosition(index_t i) const{
		assert(i<numPorts());
		return _ports[i].position();
	}
	Port& port(index_t i){ untested();
		assert(i<numPorts());
		return _ports[i];
	}
	index_t param_count() const override{ untested();
		return _params.size();
	}

public: // painting
	void paint(ViewPainter *p) const;
	virtual rect_t bounding_rect() const{ untested();
		incomplete();
		return rect_t(0,0,30,30);
	}

private:
	int _branches{0};
	int _num{0};

//	std::map<std::string, std::string> _params; // not yet
	std::vector<std::string> _param_names; // could be common?
	std::vector<std::string> _params; // could be common

private:
	std::vector<Line> _lines;
	std::vector<ComponentPort> _ports;
	std::vector<Text> _tests;
}d0; // DefaultSymbol
/*--------------------------------------------------------------------------*/
// BUG:
// - port names? (not known here.)
// - text orientation. use graphicstext?
void DefaultSymbol::paint(ViewPainter *p) const
{ untested();
	//	auto wm = p->worldMatrix();
	// write all text
	QFont f = p->font();   // save current font
	QFont newFont = f;
	for(auto t : _tests) {itested();
		//p->setWorldMatrix(
		//		QMatrix(pt->mCos, -pt->mSin, pt->mSin, pt->mCos,
		//			p->DX + float(pt->x) * p->Scale,
		//			p->DY + float(pt->y) * p->Scale));

		newFont.setPointSizeF(p->Scale * t.Size);
		newFont.setOverline(t.over);
		newFont.setUnderline(t.under);
		p->setFont(newFont);
		p->setPen(t.Color);
		if (1) { untested();
			p->drawText(t.x, t.y, 0, 0, Qt::AlignLeft|Qt::TextDontClip, t.s);
		} else {itested();
			int w, h;
			w = p->drawTextMapped (t.s, 0, 0, &h);
			Q_UNUSED(w);
		}
	}
	for(auto  p1 : _lines) {itested();
		p->setPen(p1.style);
		p->drawLine(p1.x1, p1.y1, p1.x2, p1.y2);
	}
}
/*--------------------------------------------------------------------------*/
void DefaultSymbol::init()
{
	//  QFont Font(QucsSettings.font); // default application font
	//  // symbol text is smaller (10 pt default)
	//  //Font.setPointSizeF(Font.pointSizeF()/1.2);  // symbol text size proportional to default font size
	//  Font.setPointSize(10); // symbol text size fixed at 10 pt
	// get the small font size; use the screen-compatible metric
	// FontMetrics  smallmetrics;
	// smallmetrics.setSmall();
	//  int fHeight = smallmetrics.lineSpacing();
	int fHeight = 5;
	QString tmp;
	int i, PortDistance = 60;

	// adjust branch number

	if(_num == _branches){ untested();
		return; // BUG
	}else{
		_num = _branches;
	}

	if(_branches < 1) { untested();
		_branches = 1;
	} else if(_branches > 4) {
		PortDistance = 40;
		if(_branches > 20) _branches = 20;
	}
	trace2("EDDSymbol::createSymbol", _branches, short_label());

	// adjust actual number of properties
	int NumProps = 0;
	_params.clear();
	_param_names.clear();

	for(i = NumProps; i < _branches; i++) {
		_param_names.push_back("I"+std::to_string(i+1));
		_params.push_back("");
		// QObject::tr("current equation") + " " +QString::number(i+1)));
		_param_names.push_back("Q"+std::to_string(i+1));
		_params.push_back("");
		// QObject::tr("charge equation") + " " +QString::number(i+1)));
	}

	// draw symbol
	_lines.clear();
	int h = (PortDistance/2)*((_branches-1)) + PortDistance/2; // total component half-height
	_lines.push_back( Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2))); // top side
	_lines.push_back( Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2))); // right side
	_lines.push_back( Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2))); // bottom side
	_lines.push_back( Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2))); // left side

	int y = PortDistance/2-h;
	int yh; // y is the actual vertical center
	for(i=0; i<2*_branches; ++i) {
		i++;
		// left connection with port
		_lines.push_back( Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
		_ports.push_back( ComponentPort(-30, y));
		// small black arrow inside the box
		_lines.push_back( Line( 7,y-3, 10, y,QPen(Qt::black,1)));
		_lines.push_back( Line( 7,y+3, 10, y,QPen(Qt::black,1)));
		_lines.push_back( Line(-10, y, 10, y,QPen(Qt::black,1)));

		if (i > 1) {
			yh = y-PortDistance/2; // bottom of the branch box
			// draw horizontal separation between boxes
			_lines.push_back( Line(-15, yh, 15, yh, QPen(Qt::darkBlue,2)));
		}else{
		}
		// right connection with port
		_lines.push_back( Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
		_ports.push_back( ComponentPort( 30, y));
		// add branch number near the right connection port
		_tests.push_back( Text(25,y-fHeight-2,QString::number(i))); // left-aligned
		// move the vertical center down for the next branch
		y += PortDistance;
	}

	//x1 = -30;
	//y1 = -h-2;
	//x2 =  30;
	//y2 =  h+2;
	//// compute component name text position - normal size font
	//// FontMetrics  metrics;
	//tx = x1+4;
	//// ty = y1 - 2*metrics.lineSpacing() - 4;
}
/*--------------------------------------------------------------------------*/
class DefaultSymbols{
	typedef Dispatcher<Symbol>::INSTALL disp_t;
public:
	DefaultSymbols(){
		for(index_t i=1; i<10; i++){
			auto s = d0.clone();
			s->set_param_by_name("Branches", std::to_string(i));
			s->set_label(prefix + std::to_string(i));
			trace3("default symbol",  s->label(), i, s->numPorts());
			assert(s->numPorts() == i*2);
			auto d = new disp_t(&qucs::symbol_dispatcher, s->label(), s);
			_sstash.push_back(s);
			_stash.push_back(d);
		}
	}
	~DefaultSymbols(){ itested();
		for(auto& i : _stash){ itested();
			delete i;
			i = nullptr;
		}
		for(auto i : _sstash){ itested();
			delete i;
			i = nullptr;
		}
	}
private:
	std::vector<Element*> _sstash;
	std::vector<disp_t*> _stash;
}p;
/*--------------------------------------------------------------------------*/
}//namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
