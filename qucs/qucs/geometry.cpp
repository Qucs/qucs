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

#include "geometry.h"
#include "io_trace.h"
#include <assert.h>

// not here.
std::ostream& operator<<(std::ostream& o, pos_t const& p)
{
	o << "(" << getX(p) << ", " << getY(p) << ")";
	return o;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// rotate counterclockwise. NB: the y axis points downwards on the screen.
pos_t angle_t::apply(pos_t const& p) const
{itested();
	trace3("angle_apply0", _degrees, p.first, p.second);
	assert(! (_degrees%90) ); //for now
	int a = _degrees/90;
	int s = dsin(a);
	int c = dcos(a);
	trace3("angle_apply1", a, s, c);
	assert(s <=1 && s>=-1);
	assert(c <=1 && c>=-1);
	// @angle_apply  a=2  s=0  c=1

#if 1
	int rx = c*p.first + s*p.second;
	int ry =-s*p.first + c*p.second;
#else
	int rx = c*p.first - s*p.second;
	int ry = s*p.first + c*p.second;
#endif
	trace3("angle_apply2", _degrees, rx, ry);
	return pos_t(rx, ry);
}
/*--------------------------------------------------------------------------*/
rotate_after_mirror1_t rotate_after_mirror1_t::inverse() const
{
	if(_m){ untested();
		// well. (r m) * (r m) = r mrm = r*r^-1 = id ...
		return *this;
	}else{ untested();
		rotate_after_mirror1_t a(angle_t::inverse());
		assert(!(a.degrees_int() % 90));
		return a;
	}
}
/*--------------------------------------------------------------------------*/
pos_t rotate_after_mirror1_t::apply(pos_t const& p) const
{itested();
	auto tmp = p;
	if(_m){itested();
		tmp.second *= -1;
	}else{ untested();
	}
	return angle_t::apply(tmp);
}
/*--------------------------------------------------------------------------*/
rotate_after_mirror1_t rotate_after_mirror1_t::operator*(rotate_after_mirror1_t const& that)
{itested();
	auto B = *this;
	auto A = that;
	// compute B*A
	// B*A = r1 m1 r2 m2 = r1*m1*r2 m1  m1m2
	trace2("op*", degrees_int(), that.degrees_int());

	int new_angle = B.degrees_int() + 360;
	if(_m){ untested();
		new_angle -= int(A.degrees_int()) - 360;
	}else{ untested();
		new_angle += int(A.degrees_int());
	}
	trace2("op*", degrees_int(), new_angle);
	new_angle %= 360;

	bool m1m2 = B._m ^ A._m;
	return rotate_after_mirror1_t(m1m2, new_angle);
}
/*--------------------------------------------------------------------------*/
rotate_after_mirror1_t rotate_after_mirror1_t::operator*(rotate_after_mirror const& that)
{
	rotate_after_mirror1_t t(that);
	return (*this) * t;
}
/*--------------------------------------------------------------------------*/
rotate_after_mirror1_t::rotate_after_mirror1_t(rotate_after_mirror const& that)
	: angle_t(0)
{
	trace2("rotate_after_mirror1_t::rotate_after_mirror1_t", that.mh(), that.mv());
	if(!that.mh()){ untested();
		_m = that.mv();
		_degrees = that._degrees;
	}else if(that.mv()){ untested();
		_m = false;
		_degrees = (180 + that._degrees) % 360;
	}else{ untested();
		_m = true;
		_degrees = (180 + that._degrees) % 360;
	}
}
/*--------------------------------------------------------------------------*/
pos_t rotate_after_mirror::apply(pos_t const& p) const
{itested();
	auto tmp = p;
	if(_v){itested();
		tmp.second *= -1;
	}else{
	}
	if(_h){itested();
		tmp.first *= -1;
	}else{
	}
	pos_t ret = angle_t::apply(tmp);
	trace2("rot", tmp, ret);

	return ret;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include <QRectF>
#include "qt_compat.h"
rect_t::rect_t(QRectF const& p) : _tl(0,0), _br(0,0)
{
	auto tl = p.topLeft();
	_tl = pos_t(getX(tl), getY(tl));
	auto br = p.bottomRight();
	_br = pos_t(getX(br), getY(br));
}
/*--------------------------------------------------------------------------*/
QRectF rect_t::toRectF() const
{
	return QRectF(_tl.first, _tl.second, w(), h());
}
/*--------------------------------------------------------------------------*/
rect_t rect_t::operator+(pos_t const& m) const
{
	return rect_t(tl()+m, br()+m);
}
/*--------------------------------------------------------------------------*/
rect_t& rect_t::operator+=(pos_t const& m)
{
	_tl += m;
	_br += m;
	return *this;
}
/*--------------------------------------------------------------------------*/
rect_t& rect_t::operator|=(rect_t const& o)
{
	if(o.w()==0 || o.h()==0){
	}else if(w()==0 || h()==0){
		operator=(o);
	}else{
		int l = std::min(_tl.first, o._tl.first);
		int t = std::min(_tl.second, o._tl.second);
		int r = std::max(_br.first, o._br.first);
		int b = std::max(_br.second, o._br.second);

		_tl = pos_t(l,t);
		_br = pos_t(r,b);
	}
	return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
