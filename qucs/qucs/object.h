/***************************************************************************
    copyright            : (C) 2016, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// \brief base class for all sorts of objects

#ifndef QUCS_OBJECT_H
#define QUCS_OBJECT_H
#include "../include/io_trace.h"

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

	QString const& label() const{ return Label;}
	void setLabel(QString const& l) {Label = l;}
	void setLabel(std::string const& l) {Label = QString::fromStdString(l);}
	void setLabel(char const* l) {Label = QString(l);}

protected: // error handling
	virtual void message(QucsMsgType, const char*) const;

private:
	QString Label;
};

#endif
