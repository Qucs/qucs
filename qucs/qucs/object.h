/***************************************************************************
                                 object.h
                                -----------
    begin                : 2016
    copyright            : (C) 2016 Felix Salfelder
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

/** \file object.h
  * \brief base class for all sorts of objects
  *
  */

#ifndef QUCS_OBJECT_H
#define QUCS_OBJECT_H
#include "io_trace.h"

#include <QString> // yikes

enum QucsMsgType{
	QucsTraceMsg=0,
	QucsLogMsg=1,
	QucsDebugMsg=2,
	QucsWarningMsg=3,
   QucsCriticalMsg=4,
   QucsFatalMsg=5
};

// base object for qucs.
class Object{
protected:
	explicit Object(){}
	explicit Object(Object const&){}

public:
	virtual ~Object(){}

	QString const& label() const{untested(); return Label;}
	void setLabel(QString const& l) {Label = l;}

protected: // error handling
	virtual void message(QucsMsgType, const char*) const;

private:
	QString Label;
};

#endif
