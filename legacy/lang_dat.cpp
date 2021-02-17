/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// dat "language".

#include "language.h"
#include "qucs_globals.h"
#include "data.h"
#include "output.h"
#include "qio.h"
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
class DatLang : public DocumentLanguage {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;
  std::string findType(istream_t&) const override {incomplete(); return "incomplete";}
  Element* parseItem(istream_t&, Element*) const {incomplete(); return nullptr; }

private: // Language
  void printElement(Element const*, ostream_t&) const override {incomplete();}
  void printPainting(Painting const*, ostream_t&) const override {incomplete();}
  void printDiagram(Diagram const*, ostream_t&) const override {incomplete();}
  void printSymbol(Symbol const*, ostream_t&) const override {incomplete();}
  void printSubckt(SubcktBase const*, ostream_t&) const override {incomplete();}
  void printItem(ostream_t&, Element const*) const override;

private: // local
	Data* parseData(istream_t& s, Data* x) const {incomplete(); return nullptr; }
	void printData(Data const*, ostream_t&) const;
	void printCommonData(CommonData const*, ostream_t&) const;
}d0;
static Dispatcher<DocumentLanguage>::INSTALL p(&languageDispatcher, "dat", &d0);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void DatLang::printItem(ostream_t& s, Element const* c) const
{
	s << "... dat item\n";
	if(auto d=dynamic_cast<Data const*>(c)){itested();
		printData(d, s);
	}else if(c){ untested();
		DocumentLanguage::printItem(s, c);
	}else{
		unreachable();
	}
}
/* -------------------------------------------------------------------------------- */
static void print_deps(ostream_t& s, SimOutputData const* dd)
{
	for(size_t i=0; i<dd->numDeps(); ++i){
		CommonData const* dep = dd->dep(i);
		s << " " << dep->label();
	}
}
/* -------------------------------------------------------------------------------- */
void DatLang::printCommonData(CommonData const* c, ostream_t& s) const
{
	if(auto dd=dynamic_cast<SimOutputData const*>(c)){
		if(dd->numDeps()){
			s << "<var dep " << dd->label();
			print_deps(s,dd);
			s << ">\n";
		}else{
			s << "<var indep " << dd->label() << " " << dd->size() << ">\n";
		}
		for(auto p : *dd){
//			auto x = p.first;
			auto i = p.second;
			s << "  " << i.real() << "+i*" << i.imag() << "\n";
		}
		s << "</var>\n";
	}else if(auto sod=dynamic_cast<SimOutputDir const*>(c)){
		s << "data from " << sod->label() << "\n";
		for(auto i : *sod){
			printCommonData(i, s);
		}
	}else{
		incomplete();
	}
}
/* -------------------------------------------------------------------------------- */
void DatLang::printData(Data const* c, ostream_t& s) const
{
	assert(c);
	s << " TODO " << c->label() << "\n";
	CommonData const* cc = c->common();

	if(cc){
		printCommonData(cc, s);
	}else{itested();
	}
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
}//namespace
/* -------------------------------------------------------------------------------- */
