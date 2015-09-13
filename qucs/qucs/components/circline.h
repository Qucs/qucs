#ifndef CIRCLINE_H
#define CIRCLINE_H

#include "component.h"


class CircLine : public Component  {
public:
  CircLine();
 ~CircLine();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
