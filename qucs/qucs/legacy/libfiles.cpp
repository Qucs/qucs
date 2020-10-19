/**************************************************************************
    copyright            : 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// read components from legacy "lib" files.
// pieced together from various places.

#include "globals.h"
#include "module.h"
#include "symbol.h"
#include "io_trace.h"
#include "qt_compat.h"
#include <QList>
#include "../qucs-lib/qucslib_common.h"

namespace{

class LIB{
public:
	LIB(){ untested();
		loadLibFiles();
	}
private:
	void loadLibFiles();

	std::vector<Symbol*> _sym;
	std::vector<Module::INSTALL*> _mod;
}l;

void LIB::loadLibFiles()
{ untested();
    QList<QPair<QString, bool> > LibFiles;
    ComponentLibrary parsedlib;
    QString libPath;
//    libInfoStruct lineLibInfo;
//    QVariant v;

//    QPixmap userLibPixmap = QPixmap(":/bitmaps/home.png");
//    QPixmap sysLibPixmap = QPixmap(":/bitmaps/big.qucs.xpm");

    int UserLibCount = 0;

#if 0
    // user libraries
    QStringList UserLibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    UserLibCount = UserLibFiles.count();
    foreach(QString s, UserLibFiles) // build list with full path
      LibFiles.append(qMakePair(UserLibDir.absoluteFilePath(s), false));
#endif

	 trace2("scanning", QucsSettings.libDir(), &QucsSettings);
	 QDir SysLibDir(QucsSettings.libDir());
    // system libraries
    QStringList SysLibFiles = SysLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    foreach(QString s, SysLibFiles) { untested();
		 // build list with relative path
      LibFiles.append(qMakePair(s, true));
	 }

    for (int i = 0; i < LibFiles.count(); ++i ) { untested();
      libPath = LibFiles[i].first;
      libPath.chop(4); // remove extension
      int result = parseComponentLibrary(libPath, parsedlib);

		trace1("got lib", parsedlib.name);
      switch (result) {
      case QUCS_COMP_LIB_IO_ERROR:
      { untested();
        QString filename = getLibAbsPath(LibFiles[i].first);
        error(3, "Cannot open \"%1\"." + filename.toStdString());
        return;
      }
      case QUCS_COMP_LIB_CORRUPT: untested();
        error(3, "Library is corrupt.");
        return;
      default: untested();
        break;
      }

		// TODO: ComponentLibraryItem are not Elements, but just a concoction of QStrings.
		// turn it into Element...
		for(ComponentLibraryItem c : parsedlib){
//			trace2("libcomp", parsedlib.name, c.name); // ->label());
			// trace4("libcomp", c.name, c.modelString, c.symbol, c.definition);
			Symbol* e = symbol_dispatcher.clone("Lib");
			// e->setTypeName(parsedlib.name.toStdString() + ":" + c.name.toStdString());

			e->setLabel(parsedlib.name.toStdString() + ":" + c.name.toStdString());

			e->setParameter("section", parsedlib.name.toStdString());
		   e->setParameter("component", c.name.toStdString());

			new Module::INSTALL(parsedlib.name.toStdString(), e);
			// todo: memory leak.
		}

#if 0
      lineLibInfo = libInfoStruct{libPath};
      v.setValue(lineLibInfo);
      if (LibFiles[i].second) { // it's a system library ?
        Library->addItem(sysLibPixmap, parsedlib.name, v);
      } else { untested();
        Library->addItem(userLibPixmap, parsedlib.name, v);
      }
#endif
    }
    if (UserLibCount > 0) { untested();
      // add a separator to distinguish between user libraries and system libs
      // Library->insertSeparator(UserLibCount);
    }else{ untested();
	 }
    // slotSelectLibrary(0);
}

}
