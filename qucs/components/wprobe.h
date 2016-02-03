#ifndef WPROBE_H
#define WPROBE_H

#include "component.h"


class wProbe : public Component  {
public:
  wProbe();
  ~wProbe();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
