/***************************************************************************
                          main.h  -  description
                             -------------------
    begin                : Mon May 24  2004
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <qfont.h>
#include <qcolor.h>


struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  QColor BGColor;      // background color of view area
};

#endif

