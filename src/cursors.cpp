// (c) 2020 Felix Salfelder
// GPLv3+

#include <QCursor>
#include <QBitmap>
#include <QPainter>
#include "io_trace.h"

static int CHS=32;

class cb0 : public QBitmap{
public:
	virtual ~cb0(){}
public:
	cb0() : QBitmap(CHS, CHS){
		clear();
		QPainter painter(this);
		painter.setPen(Qt::black);

		for(int i=0; i<CHS; ++i){
			if (i==CHS/2-3){
				i += 6;
			}else{
				painter.drawPoint(CHS/2,i);
				painter.drawPoint(i,CHS/2);
			}
		}
	}
};

// class CursorCrosshair : public QCursor{
// public:
// 	CursorCrosshair() : QCursor(cb0(), cb0()) {}
// };

QCursor const& crosshair()
{itested();
	static cb0 c;
	static QCursor x(c,c);
//	if(!x){
//		x = new cb0;
//	}else{
//	}
	return x;
}

// more cursor drawing. slightly newer version in mousecursor.cpp?

#if 0
void MouseActions::MMoveMirrorY(SchematicDoc *, QMouseEvent *)
{ untested();
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3-11, MAy3-4, MAx3-9, MAy3-9);
  Doc->PostPaintEvent (_Line, MAx3-11, MAy3-3, MAx3-6, MAy3-3);
  Doc->PostPaintEvent (_Line, MAx3+11, MAy3-4, MAx3+9, MAy3-9);
  Doc->PostPaintEvent (_Line, MAx3+11, MAy3-3, MAx3+6, MAy3-3);
  Doc->PostPaintEvent (_Arc, MAx3-10, MAy3-8, 21, 10, 16*20, 16*140,false);
}


/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about x axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorX(SchematicDoc *, QMouseEvent *)
{ untested();
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3-4, MAy3-11, MAx3-9, MAy3-9);
  Doc->PostPaintEvent (_Line, MAx3-3, MAy3-11, MAx3-3, MAy3-6);
  Doc->PostPaintEvent (_Line, MAx3-4, MAy3+11, MAx3-9, MAy3+9);
  Doc->PostPaintEvent (_Line, MAx3-3, MAy3+11, MAx3-3, MAy3+6);
  Doc->PostPaintEvent (_Arc, MAx3-8, MAy3-10, 10, 21, 16*110, 16*140,false);
}

/**
 * @brief MouseActions::MMoveMirrorX Paints "rotate" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveRotate(SchematicDoc *, QMouseEvent *)
{ untested();
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3-6, MAy3+8, MAx3-6, MAy3+1);
  Doc->PostPaintEvent (_Line, MAx3-7, MAy3+8, MAx3-12, MAy3+8);
  Doc->PostPaintEvent (_Arc, MAx3-10, MAy3-10, 21, 21, -16*20, 16*240,false);
}
#endif


#if 0

/**
 * @brief MouseActions::MMoveOnGrid Paints a grid beside the mouse cursor, put "on grid" mode.
 * @param Event
 * @param Doc
 */
void MouseActions::MMoveOnGrid(SchematicDoc *, QMouseEvent *)
{ untested();
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3+10, MAy3+ 3, MAx3+25, MAy3+3);
  Doc->PostPaintEvent (_Line, MAx3+10, MAy3+ 7, MAx3+25, MAy3+7);
  Doc->PostPaintEvent (_Line, MAx3+10, MAy3+11, MAx3+25, MAy3+11);
  Doc->PostPaintEvent (_Line, MAx3+13, MAy3, MAx3+13, MAy3+15);
  Doc->PostPaintEvent (_Line, MAx3+17, MAy3, MAx3+17, MAy3+15);
  Doc->PostPaintEvent (_Line, MAx3+21, MAy3, MAx3+21, MAy3+15);
}

#endif

/**
 * @brief MouseActions::MMoveMoveTextB Paints mouse symbol for "move component text" mode.
 * @param Doc
 * @param Event
 */
#if 0
void MouseActions::MMoveMoveTextB(SchematicDoc *, QMouseEvent *)
{ untested();
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3+14, MAy3   , MAx3+16, MAy3);
  Doc->PostPaintEvent (_Line, MAx3+23, MAy3   , MAx3+25, MAy3);
  Doc->PostPaintEvent (_Line, MAx3+13, MAy3   , MAx3+13, MAy3+ 3);
  Doc->PostPaintEvent (_Line, MAx3+13, MAy3+ 7, MAx3+13, MAy3+10);
  Doc->PostPaintEvent (_Line, MAx3+14, MAy3+10, MAx3+16, MAy3+10);
  Doc->PostPaintEvent (_Line, MAx3+23, MAy3+10, MAx3+25, MAy3+10);
  Doc->PostPaintEvent (_Line, MAx3+26, MAy3   , MAx3+26, MAy3+ 3);
  Doc->PostPaintEvent (_Line, MAx3+26, MAy3+ 7, MAx3+26, MAy3+10);
}


/**
 * @brief MouseActions::MMoveMoveText Paint rectangle around component text being mouse moved
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveText(SchematicDoc *, QMouseEvent *)
{ untested();
  QPointF pos=Doc->mapToScene(Event->pos());
  int newX=pos.x();
  int newY=pos.y();

  MAx1 += newX - MAx3;
  MAy1 += newY - MAy3;
  MAx3  = newX;
  MAy3  = newY;

  Doc->PostPaintEvent (_Rect, MAx1, MAy1, MAx2, MAy2);
}


/**
 * @brief MouseActions::MMoveZoomIn Paints symbol beside the mouse to show the "Zoom in" modus.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveZoomIn(SchematicDoc *, QMouseEvent *)
{ untested();
  Set3(Event, Doc);

  Doc->PostPaintEvent (_Line, MAx3+14, MAy3   , MAx3+22, MAy3);
  Doc->PostPaintEvent (_Line, MAx3+18, MAy3-4 , MAx3+18, MAy3+4);
  Doc->PostPaintEvent (_Ellipse, MAx3+12, MAy3-6, 13, 13,0,0,false);
  Doc->viewport()->update();
}
#endif
