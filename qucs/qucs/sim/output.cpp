/***************************************************************************
                              output.cpp
                              ----------
    begin                : yes
    copyright            : (C) 2015 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "output.h"
#include "data.h"

SimOutputData::SimOutputData()
    : SimOutputBase(), Min(INFINITY), Max(-INFINITY) {}
SimOutputData::~SimOutputData() {}

void SimOutputData::attach(SimOutputData* what, SimOutputData** where)
{
  assert(!*where); // for now.
  *where = what;
  ++(what->attach_count);
}

void SimOutputData::detach(SimOutputData** what)
{
  assert(*what);
  --((*what)->attach_count);
  if(!(*what)->attach_count){
	  delete(*what);
  }
  *what = 0;
}
