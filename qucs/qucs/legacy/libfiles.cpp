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
#include "schematic_lang.h"
#include "symbol.h"
#include "io_trace.h"
#include "qt_compat.h"
#include <QList>
#include "../qucs-lib/qucslib_common.h"

namespace{

class LIB{
public:
	LIB(){
		loadLibFiles();
	}
private:
	void loadLibFiles();

	std::vector<Symbol*> _sym;
	std::vector<Module::INSTALL*> _mod;
}l;

void LIB::loadLibFiles()
{
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
    foreach(QString s, SysLibFiles) {
		 // build list with relative path
      LibFiles.append(qMakePair(s, true));
	 }

    for (int i = 0; i < LibFiles.count(); ++i ) {
      libPath = LibFiles[i].first;
      libPath.chop(4); // remove extension
		parsedlib.components.clear();
      int result = parseComponentLibrary(libPath, parsedlib);

		trace3("got lib", parsedlib.name, libPath, parsedlib.components.size());
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
      default:
        break;
      }

		// TODO: ComponentLibraryItem are not Elements, but just a concoction of QStrings.
		// turn it into Element...
		for(ComponentLibraryItem c : parsedlib){
//			trace2("libcomp", parsedlib.name, c.name); // ->label());
			trace0("=================");
			if(c.symbol==""){
				c.symbol = parsedlib.defaultSymbol;
			}else{
			}
			trace4("libcomp", c.name, c.modelString, c.symbol, c.definition);
			trace0("=================");

			auto D = doclang_dispatcher["leg_sch"];
			auto L = dynamic_cast<SchematicLanguage const*>(D);
			assert(L);

			istream_t stream(&c.modelString);
			stream.readLine();
			auto type = L->findType(stream);
			trace2("DBG FT", c.modelString, type);

			if(type=="Lib"){
				// d'uh
			}else if(c.modelString.count('\n') < 2){
				trace1("paramset?", c.modelString);
				Symbol* sym = symbol_dispatcher.clone("LegacyParamset");
				sym->setParameter("modelstring", c.modelString.toStdString());
				trace2("libfiles", c.modelString, type);
#if 1
				std::string t = "P:" + parsedlib.name.toStdString() + ":" + c.name.toStdString();
				sym->setLabel(t);
#else
				auto line = stream.fullString();
				L->parseItem(sym, line);
#endif

				if(symbol_dispatcher[type]){
					sym->setTypeName(type);
					L->parseItem(sym, stream);
					new Module::INSTALL(parsedlib.name.toStdString(), sym);
					trace3("paramset done", type, parsedlib.name, sym->label());
				}else{
					trace1("paramset skip", type);
					// unreachable(); eventually
					// possibly not ported yet.
				}
			}else if (0){
				Symbol* sym = symbol_dispatcher.clone("Lib");

				DocumentStream stream(&c.symbol);

//				while(!stream.atEnd()){
//					incomplete();
//					auto type = L->find_type(stream);
//				}

				// e->setTypeName(parsedlib.name.toStdString() + ":" + c.name.toStdString());

				std::string t = parsedlib.name.toStdString() + ":" + c.name.toStdString();
				sym->setLabel(t);
				new Dispatcher<Symbol>::INSTALL(&symbol_dispatcher, t, sym);

				sym->setParameter("section", parsedlib.name.toStdString());
				sym->setParameter("component", c.name.toStdString());

				new Module::INSTALL(parsedlib.name.toStdString(), sym);
			}else{
				trace1("no whitespace", c.modelString);
				assert(false);
			}
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
    }else{
	 }
    // slotSelectLibrary(0);
}

}
