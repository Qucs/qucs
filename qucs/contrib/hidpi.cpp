// switch on hidpi mode
// pointed out by Friedrich
//
// enable with qucs -a hidpi

#include "io_trace.h"
#include <QCoreApplication>
namespace{

struct hidpi{
	hidpi(){ itested();
	  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	}
}h;

}
