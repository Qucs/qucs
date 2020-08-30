// (c) 2020 Felix Salfelder
// GPLv3+

#include "globals.h"
#include "symbol.h"
#include "viewpainter.h"

namespace{

// BUG/FEATURE. Wires can't go around the corner
// constructing a ghostwire, which is then converted to a (pair of) wires
class GhostWire : public Symbol {
public:
	enum angle_mode_t{ am_H=0, am_V=1 };
public:
	GhostWire() : _mode(am_H) {
	}

public:
	Element* clone() const{
		return new GhostWire(*this);
	}

public:
	void paint(ViewPainter *p) const { itested();
		assert(p);

		QPoint pm = pmid();
		trace3("paint", _p0, _p1, pm);
		p->setPen(QPen(Qt::darkBlue,2));
		p->drawLine(_p0, pm);
		p->drawLine(pm, _p1);
	}

private:
	QPoint pmid() const{
		QPoint pm;
		switch(_mode){
		case am_H: itested();
			pm = _p0;
			pm.setX(_p1.x());
			break;
		case am_V: untested();
			pm = _p0;
			pm.setY(_p1.y());
			break;
		}
		return pm;
	}

private: // symbol
	unsigned numPorts() const{ return 2; }
	void setParameter(std::string const& n, std::string const& v);

	QRectF boundingRect() const { itested();
	  int xlo = std::min(_p0.x(), _p1.x());
	  int xhi = std::max(_p0.x(), _p1.x());
	  int ylo = std::min(_p0.y(), _p1.y());
	  int yhi = std::max(_p0.y(), _p1.y());
	  QPointF tl(xlo-5, ylo-5);
	  QPointF br(xhi+5, yhi+5);
	  auto r = QRectF(tl, br);
	  assert(!r.isEmpty());
	  return r;
	}
public:
	void set0(QPoint p){ _p0 = p; }
	void set1(QPoint p){ _p1 = p; }
	void toggleMode(){_mode = angle_mode_t(1 - _mode); }
private:
	angle_mode_t _mode;
	QPoint _p0;
	QPoint _p1;
} w;

void GhostWire::setParameter(std::string const& n, std::string const& v)
{
	trace2("gwsp", n, v);
	if(n=="x0"){
		_p0.setX(atoi(v.c_str()));
	}else if(n=="y0"){
		_p0.setY(atoi(v.c_str()));
	}else if(n=="x1"){
		_p1.setX(atoi(v.c_str()));
	}else if(n=="y1"){
		_p1.setY(atoi(v.c_str()));
	}else if(n=="mode"){
		switch(atoi(v.c_str())){
		case 0: untested();
			_mode = am_H;
			break;
		case 1: untested();
			_mode = am_V;
			break;
		}
	}else{
		Symbol::setParameter(n, v);
	}
}

Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "__ma_ghostwire", &w);

} // namespace
