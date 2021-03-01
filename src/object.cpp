/***************************************************************************
    copyright            : (C) 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "element.h"

namespace qucs {

// base error handler. this is for errors related to qucs
//
// has nothing to do with http://qt-project.org/doc/qt-4.8/qtglobal.html#qInstallMsgHandler
// which is meant for Qt debugging.
void Element::message(MsgType t, const char* m) const
{
	incomplete();
	std::cerr << short_label() << ":" << t << " " << m << "\n";
	// if(gui)
	//   messagebox
	// else
	//   printf
	//
	// if logfile
	//   printf to logfile
}

void Element::message(MsgType t, std::string const& m) const
{
	std::cerr << short_label() << ":" << t << " " << m << "\n";
}

}
