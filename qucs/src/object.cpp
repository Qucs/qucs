
#include "object.h"

// base error handler. this is for errors related to qucs
//
// has nothing to do with http://qt-project.org/doc/qt-4.8/qtglobal.html#qInstallMsgHandler
// which is meant for Qt debugging.
void Object::message(QucsMsgType t, const char* m) const
{
	incomplete();
	std::cerr << label() << ":" << t << " " << m << "\n";
	// if(gui)
	//   messagebox
	// else
	//   printf
	//
	// if logfile
	//   printf to logfile
}
