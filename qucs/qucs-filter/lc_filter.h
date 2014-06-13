/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
 ***************************************************************************/

#ifndef LC_FILTER_H
#define LC_FILTER_H

#include "filter.h"

// ladder filter containing inductors L and capacitors C
class LC_Filter : public Filter {
public:
  LC_Filter();

  static QString* createSchematic(tFilter*, bool);
};

#endif
