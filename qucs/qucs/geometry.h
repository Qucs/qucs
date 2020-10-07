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

#include <utility> // std::pair
#include "io_trace.h"

/*--------------------------------------------------------------------------*/
typedef std::pair<int, int> pos_t;
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
// elements of O(2) decomposed as rotation * mirror.
// rotation is counterclockwise, and mirror is
// ( 1 0      )
// ( 0 (-1)^m )
// with m \in {0,1}
// NB: qucs schematics use this representation
class rotate_after_mirror1_t : public angle_t{
public:
	explicit rotate_after_mirror1_t(int d, bool m) : angle_t(d), _m(m){ }
	explicit rotate_after_mirror1_t(bool m, int d) : angle_t(d), _m(m){ }
	explicit rotate_after_mirror1_t(angle_t const& d) : angle_t(d), _m(false){ }
	rotate_after_mirror1_t(rotate_after_mirror1_t const& d) = default;

public:
	pos_t apply(pos_t const&) const;
	rotate_after_mirror1_t operator*(rotate_after_mirror1_t const&);

public:
 	bool mirror() const{return _m;}
 	int det() const{ untested(); return -_m*2 + 1;}
	rotate_after_mirror1_t inverse() const;
private:
	bool _m;
};
static const rotate_after_mirror1_t mirrorYaxis(0, true);
static const rotate_after_mirror1_t mirrorXaxis(180, true);
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

typedef rotate_after_mirror1_t qucsSymbolTransform;
typedef rotate_after_mirror0_t gedaSymbolTransform;
#endif
