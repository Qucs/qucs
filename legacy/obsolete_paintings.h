// ripped off element.h

#ifndef OBSOLETE_PAINTINGS_H
#define OBSOLETE_PAINTINGS_H

# include <QPen> // BUG

#if 1 // BUG: used in component? and in Subcircuit?
struct Line {
	Line(Line const&) = default;
  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  int   x1, y1, x2, y2;
  QPen  style;
};

struct Arc {
	Arc(Arc const&) = default;
  Arc(int _x, int _y, int _w, int _h, int _angle, int _arclen, QPen _style)
      : x(_x), y(_y), w(_w), h(_h), angle(_angle),
	arclen(_arclen), style(_style) {};
  int   x, y, w, h, angle, arclen;
  QPen  style;
};

struct Area {
  Area(int _x, int _y, int _w, int _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  int    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
};

// does not work. move to legacy stuff.
struct Text {
  Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
	float _Size = 10.0, float _mCos=1.0, float _mSin=0.0)
	: x(_x), y(_y), s(_s), Color(_Color), Size(_Size),
	  mSin(_mSin), mCos(_mCos) { over = under = false; };
  int	  x, y;
  QString s;
  QColor  Color;
  float	  Size, mSin, mCos; // font size and rotation coefficients
  bool	  over, under;      // text attributes
};
#endif

#endif // OBSOLETE_PAINTINGS_H
