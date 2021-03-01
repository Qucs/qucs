/***************************************************************************
    copyright            : (C) 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_WIDGET_H
#define QUCS_WIDGET_H

#include "object.h"

namespace qucs {

class Widget : public Object{
public:
	virtual Widget* clone() const = 0;
};

} // qucs

#endif
