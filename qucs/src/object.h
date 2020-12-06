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
#include "io_trace.h"

// base object for qucs.
class Object{
public:
	enum QucsMsgType{
		QucsMsgTrace=0,
		QucsMsgLog=1,
		QucsMsgDebug=2,
		QucsMsgWarning=3,
		QucsMsgCritical=4,
		QucsMsgFatal=5
	};
protected:
	explicit Object(){}
	explicit Object(Object const&){}

public:
	virtual ~Object(){}

	std::string const& label() const{ return _label;}
//	void setLabel(QString const& l) {_label = l.toStdString();}
	void setLabel(std::string const& l) {_label = l;}
	void setLabel(char const* l) {_label = l;}

protected: // error handling
	virtual void message(QucsMsgType, const char*) const;

private:
	std::string _label;
};

#endif
