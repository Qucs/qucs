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
{ untested();
	static cb0 c;
	static QCursor x(c,c);
//	if(!x){
//		x = new cb0;
//	}else{
//	}
	return x;
}
