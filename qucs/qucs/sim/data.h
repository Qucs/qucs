#ifndef QUCS_SIM_H
#define QUCS_SIM_H

#include <assert.h>

class SimOutputData{
public:
  SimOutputData() : attach_count(0){}
  virtual ~SimOutputData(){}
  virtual SimOutputData const* refresh(){return this;}
  static void attach(SimOutputData*, SimOutputData**);
  static void detach(SimOutputData**);
private:
  unsigned attach_count;
};

inline void SimOutputData::attach(SimOutputData* what, SimOutputData** where)
{
  assert(!*where); // for now.
  *where = what;
  ++(what->attach_count);
}

#endif
