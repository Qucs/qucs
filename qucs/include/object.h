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

#define CreateType(firstDerivative, secondDerivative, thirdDerivative) \
	(unsigned long long)thirdDerivative << 7*8 | secondDerivative << 3*8 << firstDerivative

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

	/*!
		* \brief The ObjectType enum
		* Defines which type of object it is.
		* It is used to not have to use dynamic_cast, but only
		* the type can be requested
		* The type consists of four bytes.
		* The lowest 3 bytes iare the base class which builds directly
		* on Object class (maximum 24). The next 4 bytes are the objects which build
		* on top of these first object and so on (maximum 32). And the highest byte is for a third inheritance.
		* So a maximum inheritance of 3 is possible, but I think this should be enough
		*/
	typedef enum class Type_T: unsigned long long {
		 QucsDoc = CreateType(1, 0, 0),
		    TextDoc = CreateType(QucsDoc, 1, 0),
		    SchematicDoc = CreateType(QucsDoc, 2, 0),
		 Element = CreateType(2, 0, 0),
		     Marker = CreateType(Element, 1, 0),
		 QucsData = CreateType(4, 0, 0),
		     SimOutputDir = CreateType(QucsData, 1, 0),
		         SimOutputRoot = CreateType(QucsData, SimOutputDir, 1),

		 Command = CreateType(8, 0, 0),
		     Plugins = CreateType(Command, 1, 0),
		 Net					= CreateType(0x10, 0, 0),
		 Node = CreateType(0x11, 0, 0),
		 CommonComponent = CreateType(0x12, 0, 0),
		 Language = CreateType(0x14, 0, 0),

	} Type;

protected:
	explicit Object(){}
	explicit Object(Object const&){}

public:
	virtual ~Object(){}
	virtual Object::Type type() const = 0;

	std::string const& label() const{ return _label;}
//	void setLabel(QString const& l) {_label = l.toStdString();}
	void setLabel(std::string const& l) {_label = l;}
	void setLabel(char const* l) {_label = l;}

protected: // error handling
	void message(QucsMsgType, const char*) const;
	virtual void message(QucsMsgType, std::string const&) const;

private:
	std::string _label;
};

#endif
