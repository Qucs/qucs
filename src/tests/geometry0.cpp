

#include "geometry.h"
#include "io_trace.h"
// not here.
std::ostream& operator<<(std::ostream& o, pos_t const& p)
{
	o << "(" << getX(p) << ", " << getY(p) << ")";
	return o;
}

int main()
{
	auto flipH = transformFlipHorizontally;
	auto flipV = transformFlipVertically;

	{
		rotate_after_mirror1_t current(0, false);
		rotate_after_mirror1_t new_mr = flipH * current;

		trace2("test", flipH.det(), new_mr.det());
		trace2("test", flipH.degrees_int(), new_mr.degrees_int());
		assert(new_mr == flipH);
	}


	{
		rotate_after_mirror1_t new_mr = flipH * flipV;
		assert(new_mr.det()==1);
		assert(new_mr.degrees_int() == 180);
	}

	{
		rotate_after_mirror1_t current(90, false);
		rotate_after_mirror1_t new_mr = flipV * current;

		trace2("test", new_mr.det(), new_mr.degrees_int());
		assert(new_mr.det()==-1);
		assert(new_mr.degrees_int() == 270);
	}

	{
		rotate_after_mirror1_t current(90, false);
		rotate_after_mirror1_t new_mr = flipH * current;

		trace2("test", new_mr.det(), new_mr.degrees_int());
		assert(new_mr.det()==-1);
		assert(new_mr.degrees_int() == 90);
	}

  assert(dsin(0) == 0);
  assert(dsin(1) == 1);
  assert(dsin(2) == 0);
  assert(dsin(3) == -1);
  assert(dcos(0) == 1);
  assert(dcos(1) == 0);
  assert(dcos(2) == -1);
  assert(dcos(3) == 0);

	//assert(!(new_mr.degrees_int()%90));
	{
		rotate_after_mirror1_t a(90, false);
		pos_t p(1,0);
		auto q = a.apply(p);
		assert(q == pos_t(0,-1));
	}
	{
		rotate_after_mirror a(90, false, false);
		pos_t p(1,0);
		auto q = a.apply(p);
		assert(q == pos_t(0,-1));
	}
	{
		rotate_after_mirror a(0, false, true);
		pos_t p(1,0);
		auto q = a.apply(p);
		trace2("dbg", p, q);
		assert(q == pos_t(1,0));
	}
	{
		rotate_after_mirror a(0, true, false);
		pos_t p(1,0);
		auto q = a.apply(p);
		assert(q == pos_t(-1,0));
	}
	{
		rotate_after_mirror a(90, true, false);
		pos_t p(1,0);
		auto q = a.apply(p);
		assert(q == pos_t(0,1));
	}
	{
		rotate_after_mirror a(90, true, true);
		pos_t p(1,0);
		auto q = a.apply(p);
		assert(q == pos_t(0,1));
	}
	{
		rotate_after_mirror a(0, false, true);
		rotate_after_mirror1_t b(90, true);

		pos_t p(1,2);
		auto q = a.apply(p);
		assert(q == pos_t(1,-2));
		q = b.apply(q);
		assert(q == pos_t(2,-1));

		q = (b*a).apply(p);
		assert(q == pos_t(2,-1));
	}

	for(int h = 0; h<2; ++h)
	for(int v = 0; v<2; ++v)
	for(unsigned x = 0; x<10; ++x)
	{
		trace3("======", h,v,x);
		rotate_after_mirror a(x*90, bool(h), bool(v));
		rotate_after_mirror1_t b(a);
		assert(a.det() == b.det());
		trace2("...", a.degrees_int(), b.degrees_int());

		pos_t p(1, 2);
		auto ap = a.apply(p);
		auto bp = b.apply(p);
		trace4("...", a.det(), p, ap, bp);
		assert(ap == bp);
	}

	for(int i = 0; i<2; ++i)
	for(int j = 0; j<2; ++j)
	for(int k = 0; k<2; ++k)
	for(unsigned x = 0; x<10; ++x)
	for(unsigned y = 0; y<10; ++y)
	{
		rotate_after_mirror a(x*90, bool(i), bool(j));
		rotate_after_mirror1_t b(y*90, bool(k));

		pos_t p(1,2);

		auto q0 = b.apply(a.apply(p));
		auto ba = b*a;
		auto q1 = ba.apply(p);
		trace5("..", i,j,k,x,y);
		trace2("..", q0, q1);
		trace2("..", ba.det(), ba.degrees_int());
		assert(q0 == q1);
	}
}
