/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_ACTION_H
#define QUCS_ACTION_H

#include "object.h"

class QAction;
class QObject;

class Action : public Object{
public:
	virtual Action* clone() const = 0;
	virtual QAction* createAction(QObject*) const = 0;
};

#endif
