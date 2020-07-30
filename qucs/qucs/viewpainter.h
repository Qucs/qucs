/***************************************************************************
                               viewpainter.h
                              ---------------
    copyright            : (C) 2004 by Michael Margraf
                               2018, 2019 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_VIEWPAINTER_H
#define QUCS_VIEWPAINTER_H

#include <QPainter> // ugly, still better than direct include..
#include "trace.h"

class QString;
class QColor;

#define TO_INT(f)   (f > 0.0 ? int(f + 0.5) : int(f - 0.5))

/* This class is neccessary because scaled fonts looks very ungly.
   Avoiding this is unfortunately not easy.
   This class scales the font point size instead of the font glyphes.
   But this has the disadvantage, that the text size does not increase
   equally with the rest of the drawing. We therefore must compute
   some thing anew each time the paint function is called. The class
   "ViewPainter" supports this. */

class ViewPainter{
	ViewPainter(const ViewPainter&);
public:
	explicit ViewPainter(QPainter* p);
public: // viewpainter.cpp
	     // don't use in new code
	void map(int xin, int yin, int& x, int& y);
public: // just forward
	void drawLine (QPoint a, QPoint b){
		Painter->drawLine(a, b);
	}
	void drawLine (int a, int b, int c, int d){
		Painter->drawLine(a, b, c, d);
	}
	void drawRect (int a, int b, int c, int d){
		Painter->drawRect(a, b, c, d);
	}
	void drawRectD (int a, int b, int c, int d){
		/// there is no drawRectD.
		Painter->drawRect(a, b, c, d);
	}
	void drawRoundRect (int a, int b, int c, int d){
		Painter->drawRoundRect(a, b, c, d);
	}
	void drawArc (int a, int b, int c, int d, int e, int f){
		Painter->drawArc(a, b, c, d, e, f);
	}
	void drawEllipse (int a, int b, int c, int d){
		Painter->drawEllipse(a, b, c, d);
	}
	int drawText(const QString& s, int a, int b, int *Height=0){
		(void)Height;
		Painter->drawText(a, b, s);
		incomplete(); // return some bounding box width?
		return 0;
	}
	int  drawTextMapped(const QString&, int, int, int *Height=0);
	void fillRect(int a, int b, int c, int d, const QColor& e){
		Painter->fillRect(a, b, c, d, e);
	}
	void eraseRect(int a, int b, int c, int d){
		Painter->eraseRect(a, b, c, d);
	}
	void drawResizeRect(int, int);

	template<class T>
	void setPen(T t){
		Painter->setPen(t);
	}
public: // obsolete?
  void init(QPainter*, float, int, int, int, int, 
	    float FontScale_=0.0, float PrintScale_=1.0)
  {
	  (void)FontScale_;
	  (void)PrintScale_;
	  incomplete();
  }
public: // BUG, is accessed directly.
        // some of them are used in random places
	QPainter *Painter;
	float Scale;
	float PrintScale;
	float FontScale;
	float DX;
	float DY;
	int LineSpacing;
};

#endif
