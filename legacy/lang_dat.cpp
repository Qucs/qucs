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
#include "globals.h"
#include "data.h"
#include "output.h"
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
class DatLang : public DocumentLanguage {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;
  std::string findType(istream_t&) const override {incomplete(); return "incomplete";}

private: // Language
  void printTaskElement(TaskElement const*, ostream_t&) const override {incomplete();}
  void printPainting(Painting const*, ostream_t&) const override {incomplete();}
  void printDiagram(Symbol const*, ostream_t&) const override {incomplete();}
  void printSymbol(Symbol const*, ostream_t&) const override {incomplete();}
  void printSubckt(SubcktBase const*, ostream_t&) const override {incomplete();}
  void printItem(ostream_t&, Element const*) const override;
private: // local
  void printData(Data const*, ostream_t&) const;
}d0;
static Dispatcher<DocumentLanguage>::INSTALL p(&language_dispatcher, "dat", &d0);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void DatLang::printItem(ostream_t& s, Element const* c) const
{ untested();
	if(auto d=dynamic_cast<Data const*>(c)){ untested();
		printData(d, s);
	}else{ untested();
		DocumentLanguage::printItem(s, c);
	}
}
/* -------------------------------------------------------------------------------- */
void DatLang::printData(Data const* c, ostream_t& s) const
{ untested();
	assert(c);
	CommonData const* cc = c->common();

	if(auto sod=dynamic_cast<SimOutputData const*>(cc)){
	}else if(auto sod=dynamic_cast<SimOutputDir const*>(cc)){
		for(auto i : *sod){
			s << " TODO " << i->label() << "\n";
		}
	}else{ untested();
	}
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
}//namespace
/* -------------------------------------------------------------------------------- */
