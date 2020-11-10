/* Copyright (C) 2020 Felix Salfelder
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef QUCS_GEOMETRY_H
#define QUCS_GEOMETRY_H
/*--------------------------------------------------------------------------*/
#include <utility> // std::pair
#include "io_trace.h"
#include "assert.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class pos_t : public std::pair<int, int>{
public:
	pos_t(pos_t const& p) : std::pair<int, int>(p) {}
	pos_t(std::pair<int, int> const& p) : std::pair<int, int>(p) {}
	pos_t(int a, int b) : std::pair<int, int>(a,b) {}
	bool operator<=(pos_t const& b) const;

	pos_t& operator=(pos_t const& o){
		std::pair<int, int>::operator=(o);
		return *this;
	}
};
/*--------------------------------------------------------------------------*/
inline bool pos_t::operator<=(pos_t const& b) const
{
	return first<=b.first && second<=b.second;
}
/*--------------------------------------------------------------------------*/
inline pos_t& operator+=(pos_t& r, pos_t const& q)
{
  r.first += q.first;
  r.second += q.second;
  return r;
}
/*--------------------------------------------------------------------------*/
inline pos_t operator+(pos_t const& p, pos_t const& q)
{
  pos_t r(p);
  r.first += q.first;
  r.second += q.second;
  return r;
}
/*--------------------------------------------------------------------------*/
inline pos_t operator-(pos_t const& p, pos_t const& q)
{
  pos_t r(p);
  r.first -= q.first;
  r.second -= q.second;
  return r;
}
/*--------------------------------------------------------------------------*/
inline pos_t operator*(pos_t const& p, double d)
{
  pos_t r(p);
  r.first *= d;
  r.second *= d;
  return r;
}
/*--------------------------------------------------------------------------*/
class QRectF;
class rect_t{
public:
	explicit rect_t(int x, int y, int w, int h)
		:_tl(x, y), _br(x+w, y+h) {}
	explicit rect_t(pos_t const& tl, pos_t const& br)
		:_tl(tl), _br(br) {}
	rect_t() : _tl(0,0), _br(0,0) {}
	rect_t(QRectF const&);
	rect_t(rect_t const&) = default;
public:
	QRectF toRectF() const;
	rect_t operator+(pos_t const& m) const;
	rect_t& operator+=(pos_t const& m);
	rect_t operator|(rect_t const&) const;
	rect_t& operator|=(rect_t const&);

	pos_t const& tl() const{return _tl;}
	pos_t const& br() const{return _br;}
	pos_t center() const{return (_tl+_br)*.5;}
	int w() const{return _br.first - _tl.first;}
	int h() const{return _br.second - _tl.second;}
private:
	pos_t _tl;
	pos_t _br;

	// unsigned _w;
	// unsigned _h;
};
/*--------------------------------------------------------------------------*/
inline int getX(std::pair<int, int> const& p)
{itested();
	return p.first;
}
/*--------------------------------------------------------------------------*/
inline int getY(std::pair<int, int> const& p)
{itested();
	return p.second;
}
/*--------------------------------------------------------------------------*/
inline int getX(pos_t const& p)
{itested();
	return p.first;
}
/*--------------------------------------------------------------------------*/
inline int getY(pos_t const& p)
{itested();
	return p.second;
}
/*--------------------------------------------------------------------------*/
inline int dsin(int angle)
{itested();
	int d = angle%2;
	d *= 1-2*(angle/2);
	return d;
}
/*--------------------------------------------------------------------------*/
inline int dcos(int angle)
{itested();
  return dsin(angle+1);
}
/*--------------------------------------------------------------------------*/
// class transform_t // maybe.
class angle_t{
public:
	angle_t(int d);
	angle_t(angle_t const&) = default;

public:
	bool operator==(angle_t const& o){
		return _degrees == o._degrees;
	}
	angle_t operator-() const{
		return angle_t(-_degrees);
	}
public:
	int degrees_int() const{return _degrees;}
	pos_t apply(pos_t const&) const;
	angle_t inverse() const{
		int a = _degrees;
		a *= -1;
		a += 360;
		a %= 360;
		return angle_t(a);
	}

protected:
	int _degrees;
};
static const angle_t ninety_degree(90);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// elements of O(2) decomposed as rotation * mirror.
// rotation is counterclockwise, and mirror is
// ( 1 0      )
// ( 0 (-1)^m )
// with m \in {0,1}
// NB: qucs schematics use this representation. there was a mirroredX flag in
// Comonent which indicates whether the y axis is flipped. this is widely known
// as "flip vertically".
class rotate_after_mirror;
class rotate_after_mirror1_t : public angle_t{
public:
	explicit rotate_after_mirror1_t(int d, bool m) : angle_t(d), _m(m){ }
	explicit rotate_after_mirror1_t(bool m, int d) : angle_t(d), _m(m){ }
	explicit rotate_after_mirror1_t(angle_t const& d) : angle_t(d), _m(false){ }
	explicit rotate_after_mirror1_t(rotate_after_mirror const& d);
	rotate_after_mirror1_t(rotate_after_mirror1_t const& d) = default;

public:
	pos_t apply(pos_t const&) const;
	rotate_after_mirror1_t operator*(rotate_after_mirror1_t const&);
	rotate_after_mirror1_t operator*(rotate_after_mirror const&);
	bool operator==(rotate_after_mirror1_t const& o){
		return _m == o._m && angle_t::operator==(o);
	}

public:
 	bool mirror() const{return _m;}
 	int det() const{ untested(); return -_m*2 + 1;}
	rotate_after_mirror1_t inverse() const;
private:
	bool _m;
};
static const rotate_after_mirror1_t transformFlipVertically(0, true); // former "mirror Y axis"
static const rotate_after_mirror1_t transformFlipHorizontally(180, true);
/*--------------------------------------------------------------------------*/
// elements of O(2) decomposed as rotation * mirror.
// rotation is counterclockwise, and mirror is
// ( (-1)^m 0 )
// ( 0      1 )
// with m \in {0,1}
// NB: geda schematics use this representation
class rotate_after_mirror0_t : public angle_t{
public:
	rotate_after_mirror0_t(int d, bool m) : angle_t(d), _m(m){ }
	rotate_after_mirror0_t(angle_t const& d) : angle_t(d), _m(false){ }

public:
	pos_t apply(pos_t const&) const;
	rotate_after_mirror0_t operator*(rotate_after_mirror0_t const&);

	rotate_after_mirror0_t inverse() const{
		if(_m){
			// well. (r m) * (r m) = r mrm = r*r^-1 = id ...
			return *this;
		}else{
			return rotate_after_mirror0_t(angle_t::inverse());
		}
	}

public:
 	bool mirror() const{return _m;}

private:
	bool _m;
};
/*--------------------------------------------------------------------------*/
class rotate_after_mirror : public angle_t{
public:
	rotate_after_mirror(int d, bool hflip=false, bool vflip=false)
	    : angle_t(d), _h(hflip), _v(vflip){ }
	rotate_after_mirror(angle_t const& d) : angle_t(d), _h(false), _v(false){ }
	rotate_after_mirror(rotate_after_mirror const& d) : angle_t(d), _h(d._h), _v(d._v){ }

public:
	pos_t apply(pos_t const&) const;
	rotate_after_mirror operator*(rotate_after_mirror const&);
 	int det() const{ untested(); return (1-_h*2) * (1-_v*2);}

	rotate_after_mirror0_t inverse() const{
		if(_h && _v){ untested();
			return rotate_after_mirror(angle_t(degrees_int()+180));
		}else if(_h || _v){ untested();
			return *this;
		}else{ untested();
			return rotate_after_mirror(angle_t::inverse());
		}
	}

protected:
	friend class rotate_after_mirror1_t;
	bool mv() const{return _v;}
	bool mh() const{return _h;}

private:
	bool _h;
	bool _v;
};
/*--------------------------------------------------------------------------*/
inline angle_t::angle_t(int d) : _degrees(d)
{
	trace1("...", d);
	assert(!(d%90)); // for now.
	while(_degrees<0){ untested();
		_degrees += 360;
	}

	_degrees %= 360;
}
/*--------------------------------------------------------------------------*/

typedef rotate_after_mirror1_t qucsSymbolTransform;
typedef rotate_after_mirror0_t gedaSymbolTransform;
#endif
