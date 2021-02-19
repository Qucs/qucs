
#include "qucs_app.h"
#include <QToolBar>

// obsolete.
void QucsApp::clearWorkToolbar()
{ untested();
	assert(_docToolBar);
	_docToolBar->clear();
}

// obsolete.
void QucsApp::addWorkToolbarAction(QAction* a)
{ untested();
	assert(_docToolBar);
	_docToolBar->addAction(a);
}
