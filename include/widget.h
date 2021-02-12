
#ifndef QUCS_WIDGET_H
#define QUCS_WIDGET_H

#include "object.h"

class Widget : public Object{
public:
	virtual Widget* clone() const = 0;
};

#endif
