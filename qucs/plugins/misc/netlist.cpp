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
namespace {
/* -------------------------------------------------------------------------------- */
class Netlister : public Command{
public:
	typedef std::map<std::string, Element const*> declmap;
	Netlister(Netlister const&) = delete;
public:
	explicit Netlister() : _lang(nullptr) {}
private: // Command
	void do_it(istream_t&, SchematicModel*) override;

private: // legacy implementation
	void printDeclarations(ostream_t& d,
			declmap& declarations) const;
}c0;
static Dispatcher<Command>::INSTALL p1(&command_dispatcher, "netlist", &c0);
/* -------------------------------------------------------------------------------- */
void Netlist::do_it(istream_t& cs, SchematicModel* m)
{
	declmap declarations;
	std::string fn;
	cs >> fn;

	Get(cs, "{l}ang", &language);

	QFile NetlistFile(QString::fromStdString(fn));
	if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
		fprintf(stderr, "Failed to open %s\n", fn.c_str());
		incomplete();
		throw Exception("...");
	}else{
	}
	ostream_t Stream(&NetlistFile);

	Language const* lang = nullptr;
	if(language==""){
		Language = tQucsSettings::Language;
	}else{
		lang = language_dispatcher[language];
	}
	assert(lang);

	// global symbols are not in this model, need to find them and
	// emit separately.
	if(emit_decl){
		resolve(Stream, m, declarations);
		printDeclarations(Stream, declarations);
	}else{
	}

	Stream << '\n';
	createNetlist(Stream, m);
}
/* -------------------------------------------------------------------------------- */
} // namespace
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
