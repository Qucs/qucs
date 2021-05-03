/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
	                            2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "legacy_painting.h"

LegacyPainting::LegacyPainting() : Element(), Painting()
{
  State = 0;
  set_label(Name.toStdString()); // legacy hack
}

void LegacyPainting::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
	// incomplete(); // fix later.
	_x1 = _x2 = _y1 = _y2 = 0;
//  _x1 = _cx;     _y1 = _cy;
//  _x2 = _cx+x2;  _y2 = _cy+y2;
}

QString LegacyPainting::toPenString (int pen) {
  switch (pen) {
  case Qt::NoPen: return "Qt::NoPen";
  case Qt::SolidLine: return "Qt::SolidLine";
  case Qt::DashLine: return "Qt::DashLine";
  case Qt::DotLine: return "Qt::DotLine";
  case Qt::DashDotLine: return "Qt::DashDotLine";
  case Qt::DashDotDotLine: return "Qt::DashDotDotLine";
  case Qt::MPenStyle: return "Qt::MPenStyle";
  }
  return "Qt::NoPen";
}

QString LegacyPainting::toBrushString (int brush) {
  switch (brush) {
  case Qt::NoBrush: return "Qt::NoBrush";
  case Qt::SolidPattern: return "Qt::SolidPattern";
  case Qt::Dense1Pattern: return "Qt::Dense1Pattern";
  case Qt::Dense2Pattern: return "Qt::Dense2Pattern";
  case Qt::Dense3Pattern: return "Qt::Dense3Pattern";
  case Qt::Dense4Pattern: return "Qt::Dense4Pattern";
  case Qt::Dense5Pattern: return "Qt::Dense5Pattern";
  case Qt::Dense6Pattern: return "Qt::Dense6Pattern";
  case Qt::Dense7Pattern: return "Qt::Dense7Pattern";
  case Qt::HorPattern: return "Qt::HorPattern";
  case Qt::VerPattern: return "Qt::VerPattern";
  case Qt::CrossPattern: return "Qt::CrossPattern";
  case Qt::BDiagPattern: return "Qt::BDiagPattern";
  case Qt::FDiagPattern: return "Qt::FDiagPattern";
  case Qt::DiagCrossPattern: return "Qt::DiagCrossPattern";
  case Qt::TexturePattern : return "Qt::CustomPattern ";
  }
  return "Qt::NoBrush";
}
