/* Copyright (C) 2020 Felix Salfelder
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

// spice language implementation. (stub)
//
#include "language.h"
#include "globals.h"
namespace{
class LangSpice : public NetLang {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;
  std::string findType(istream_t&) const override {incomplete(); return "incomplete";}

private: // local
  void printTaskElement(TaskElement const*, ostream_t&) const { untested(); }
  void printSymbol(Symbol const*, ostream_t&) const override;
  void printSubckt(SubcktBase const*, ostream_t&) const {untested(); }
  void printPainting(Painting const*, ostream_t&) const override { untested(); }
  void printDiagram(Symbol const*, ostream_t&) const override { untested();}
}d0;
static Dispatcher<DocumentLanguage>::INSTALL p(&language_dispatcher, "spice", &d0);
/*--------------------------------------------------------------------------*/
void LangSpice::printSymbol(Symbol const* s, ostream_t& o) const
{ untested();
	o << s->label() << " ... stub\n";
}
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
