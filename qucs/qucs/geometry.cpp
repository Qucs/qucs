
#include "geometry.h"
#include "io_trace.h"
#include <assert.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// rotate counterclockwise. NB: the y axis points downwards on the screen.
pos_t angle_t::apply(pos_t const& p) const
{itested();
	trace3("angle_apply", _degrees, p.first, p.second);
	assert(! (_degrees%90) ); //for now
	int a = _degrees/90;
	int s = dsin(a);
	int c = dcos(a);
	trace3("angle_apply", a, s, c);
	// @angle_apply  a=2  s=0  c=1

	int rx = c*p.first + s*p.second;
	int ry =-s*p.first + c*p.second;
	trace3("angle_apply", _degrees, rx, ry);
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
	// m1 * r1 * m2 * r2 = m1*m2 * m2*r1*m2*r2
	trace2("op*", degrees_int(), A.degrees_int());

	int new_angle = A.degrees_int() + 360;
	if(A._m){itested();
		new_angle -= int(B.degrees_int());
	}else{ untested();
		new_angle += int(B.degrees_int());
	}
	trace2("op*", degrees_int(), new_angle);
	new_angle %= 360;

	bool m1m2 = B._m && A._m;
	return rotate_after_mirror1_t(m1m2, new_angle);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
