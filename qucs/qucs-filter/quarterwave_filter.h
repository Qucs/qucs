#ifndef QUARTERWAVE_FILTER_H
#define QUARTERWAVE_FILTER_H


#include "tl_filter.h"

// Quarter wave transmission line filter
class QuarterWave_Filter : public TL_Filter {
public:
  QuarterWave_Filter();

  static QString* createSchematic(tFilter*, tSubstrate*, bool);
};

#endif
