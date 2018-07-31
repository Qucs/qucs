/***************************************************************************
                               viewpainter.h
                              ---------------
    begin                : Tue Oct 05 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VIEWPAINTER_H
#define VIEWPAINTER_H

#include <QPainter>

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

#if QT_VERSION >= 0x050000
class ViewPainter{
	ViewPainter(const ViewPainter&);
public:
	explicit ViewPainter(QPainter* p)
		: Painter(p),
		  Scale(1.f),
		  PrintScale(1.f),
		  FontScale(1.f),
		  DX(0.), DY(0.),
		 LineSpacing(p->fontMetrics().lineSpacing())
	{
	}
public: // viewpainter.cpp
	     // don't use in new code
	void map(int xin, int yin, int& x, int& y);
public: // just forward
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
	int  drawTextMapped(const QString&, int, int, int *Height=0){
		(void)Height;
		incomplete();
		return 0;
	}
	void fillRect(int a, int b, int c, int d, const QColor& e){
		Painter->fillRect(a, b, c, d, e);
	}
	void eraseRect(int a, int b, int c, int d){
		Painter->eraseRect(a, b, c, d);
	}
	void drawResizeRect(int, int);
public: // obsolete?
  void init(QPainter*, float, int, int, int, int, 
	    float FontScale_=0.0, float PrintScale_=1.0)
  {
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
#else
class ViewPainter {
public:
  ViewPainter(QPainter *p=0);
 ~ViewPainter();

  void init(QPainter*, float, int, int, int, int, 
	    float FontScale_=0.0, float PrintScale_=1.0);
  void map(int, int, int&, int&);
  void drawPoint(int, int);
  void drawLine (int, int, int, int);
  void drawRect (int, int, int, int);
  void drawRectD(int, int, int, int);
  void drawRoundRect(int, int, int, int);
  void drawEllipse(int, int, int, int);
  void drawArc(int, int, int, int, int, int);
  int  drawText(const QString&, int, int, int *Height=0);
  int  drawTextMapped(const QString&, int, int, int *Height=0);
  void fillRect(int, int, int, int, const QColor&);
  void eraseRect(int, int, int, int);
  void drawResizeRect(int, int);

  QPainter *Painter;
  float Scale, FontScale, PrintScale, DX , DY;
  int LineSpacing;   // updated by init(), just for info
};
#endif

#endif
