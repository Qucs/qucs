/***************************************************************************
    copyright            : (C) 2015, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// print flat netlists
/* -------------------------------------------------------------------------------- */
#include "command.h"
#include "qucs_globals.h"
#include "settings.h"
#include "language.h"
#include "conductor.h"
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
class Netlister : public Command{
public:
	typedef std::map<std::string, Element const*> declmap;
	Netlister(Netlister const&) = delete;
public:
	explicit Netlister() {}
private: // Command
	void do_it(istream_t&, SchematicModel*) override;

private: // implementation
	void resolve(ostream_t&, SchematicModel*, declmap&) const{incomplete();}
	void printDeclarations(ostream_t& d,
			declmap& declarations) const{incomplete();}
	void printMain(ostream_t& stream, SchematicModel const*,
			DocumentLanguage const*) const;
}c0;
static Dispatcher<Command>::INSTALL p1(&command_dispatcher, "netlist", &c0);
/* -------------------------------------------------------------------------------- */
void Netlister::do_it(istream_t& cs, SchematicModel* m)
{
	declmap declarations;
	std::string fn, language;
	bool emit_decl = false;
	cs >> "netlist";

	size_t here = cs.cursor();

	do{
		trace1("netlist", cs.tail());
		Get(cs, "l{ang}", &language);
	} while(cs.more() && !cs.stuck(&here));
	trace2("netlist", language, cs.tail());

	cs >> fn;

	if(fn==""){
		fn="/dev/stdout";
	}else{
	}

	QFile NetlistFile(QString::fromStdString(fn));
	if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
		fprintf(stderr, "Failed to open %s\n", fn.c_str());
		incomplete();
		throw qucs::Exception("...");
	}else{
	}
	ostream_t Stream(&NetlistFile);

	DocumentLanguage const* lang = nullptr;
	if(language==""){ untested();
		lang = tQucsSettings::language;
	}else{
		lang = language_dispatcher[language];
		trace2("netlist", lang, language);
		assert(language_dispatcher[language]);
	}

	if(lang){
	}else{ untested();
		throw qucs::Exception("no lang: " + language);
	}

	// global symbols are not in this model, need to find them and
	// emit separately.
	if(emit_decl){
		resolve(Stream, m, declarations);
		printDeclarations(Stream, declarations);
	}else{
	}

	Stream << '\n';
	printMain(Stream, m, lang);
}
/* -------------------------------------------------------------------------------- */
void Netlister::printMain(ostream_t& stream,
		SchematicModel const* scope_, DocumentLanguage const* lang) const
{
	assert(scope_);
	auto s = scope_->find_("main");
	assert(s!=scope_->end());
	assert(*s);
	auto scope = (*s)->scope();

	stream << "netlisting...\n";

	QString Time;
	assert(scope);
	for(auto it_ : *scope){
		auto pc = dynamic_cast<Symbol const*>(it_);
		if(pc){
		}else{
			incomplete();
			continue;
		}

		// if dynamic_cast<Label*>
		//   ignore
		if(dynamic_cast<Conductor const*>(pc)){
			// possibly a wire.
		}else if(pc->typeName()=="GND"){
			// qucsator hack, just ignore.
		}else if(pc->typeName()=="NodeLabel"){ untested();
			// qucsator hack, just ignore.
		}else{
			lang->printItem(stream, pc);
		}
	} // components
}
/* -------------------------------------------------------------------------------- */
} // namespace
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
