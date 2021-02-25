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

#include "qucs_globals.h"
#include "module.h"
#include "schematic_lang.h"
#include "symbol.h"
#include "schematic_model.h" // tmp, debugging
#include "io_trace.h"
#include "qt_compat.h"
#include "docfmt.h"
#include "sckt_base.h"
#include <QList> // yikes.
#include "../qucs-lib/qucslib_common.h"
#include "command.h"
#include "qio.h"

namespace{

class LIB : public Command{
public:
	LIB(){
	}
	~LIB(){
		incomplete();
#if 0
		for(auto i: stash){ untested();
			delete i->item;
			delete i;
		}
#endif
	}
private:
	void do_it(istream_t&, SchematicModel* scope) override;
	void load_single(std::string what, SchematicModel* scope);
	void stash(std::string const& t, Symbol* ssym, SchematicModel* scope) {
		auto i = new Dispatcher<Symbol>::INSTALL(&symbol_dispatcher, t, ssym);
		_stash.push_back(i);
	}

	std::vector<Dispatcher<Symbol>::INSTALL*> _stash;
	std::vector<Module::INSTALL*> _mod;
}l;
Dispatcher<Command>::INSTALL p(&commandDispatcher, "loadlegacylib", &l);

void LIB::load_single(std::string what, SchematicModel* scope)
{
	QString libPath = QString_(what);
	libPath.chop(4); // remove extension

	ComponentLibrary parsedlib;
	parsedlib.components.clear();
	int result = parseComponentLibrary(libPath, parsedlib);
	assert(result == QUCS_COMP_LIB_OK);

	// ComponentLibraryItem are not Elements, but just a concoction of QStrings.
	// turn it into Element... (do it here, for now, but that's silly).
	for(ComponentLibraryItem c : parsedlib){
		trace2("libcomp", parsedlib.name, c.name); // ->label());
		if(c.symbol==""){
			getSection("Symbol", c.definition, c.symbol); // GAAH
		}else{ untested();
		}

		if(c.symbol==""){
			c.definition += "\n<Symbol>" + parsedlib.defaultSymbol + "\n</Symbol>\n";
			trace1("attached symbol to defn", c.definition);
		}else{
		}

		auto D = languageDispatcher["legacy_lib"];
		auto L_ = dynamic_cast<SchematicLanguage const*>(D);
		auto C = commandDispatcher["leg_sch"];
		auto L = dynamic_cast<DocumentFormat const*>(C);
		assert(L);

		istream_t stream(istream_t::_STRING, c.modelString.toStdString());
		stream.read_line();
		std::string type = L_->find_type_in_string(stream); // BUG?

		if(type=="Lib"){
			// possibly a subcircuit. parse and stash
			//
			// // stuff should already be parsed in, but isn't
			// BUG: parse c.definition. but not here.
			istream_t stream(istream_t::_STRING, c.definition.toStdString());
			Symbol* sym = symbol_dispatcher.clone("LegacyLibProto");
			auto ssym = prechecked_cast<SubcktBase*>(sym);
			std::string t = "Lib:" + parsedlib.name.toStdString() + ":" + c.name.toStdString();

			sym->setLabel(c.name.toStdString());

			assert(ssym);
			try{
				// trace1("parse", c.definition);
				L->load(stream, ssym);

				trace1("Lib stashing", t);
				stash(t, ssym, scope);
				new Module::INSTALL(parsedlib.name.toStdString(), ssym);
			}catch(qucs::Exception const&){ untested();
				trace0("not stashing");
				delete ssym;
			}

			// trace3("Lib", c.modelString, type, c.definition);
			// d'uh
		}else if(type=="Sub"){
			// TODO: new__instance does this.
			Symbol* sym = symbol_dispatcher.clone("LegacyParamset");
			sym->set_param_by_name("modelstring", c.modelString.toStdString());
			std::string t = "P:" + parsedlib.name.toStdString() + ":" + c.name.toStdString();
			sym->setLabel(t);

			if(symbol_dispatcher[type]){
				sym->setTypeName(type);
				L->load(stream, sym);
				new Module::INSTALL(parsedlib.name.toStdString(), sym);
			}else{
				trace1("paramset skip", type);
				// unreachable(); eventually
				// possibly not ported yet.
			}
		}else{
		}
		// todo: memory leak.
	}
}


void LIB::do_it(istream_t& cs, SchematicModel* scope)
{
	cs >> "loadlegacylib";
	std::string what;
	cs >> what;

	if(what!=""){
		load_single(what, scope);
	}else{
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
		// here? or in QucsDoc?
		QStringList UserLibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
		UserLibCount = UserLibFiles.count();
		foreach(QString s, UserLibFiles) // build list with full path
			LibFiles.append(qMakePair(UserLibDir.absoluteFilePath(s), false));
#endif

		trace2("scanning", QucsSettings.libDir(), &QucsSettings);
		QDir SysLibDir(QString_(QucsSettings.libDir()));
		// system libraries
		QStringList SysLibFiles = SysLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
		foreach(QString s, SysLibFiles) {
			// build list with relative path
			LibFiles.append(qMakePair(s, true));
		}

		for (int i = 0; i < LibFiles.count(); ++i ) {
			// TODO: load_single
			libPath = LibFiles[i].first;
			load_single(libPath.toStdString(), scope);

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

}
