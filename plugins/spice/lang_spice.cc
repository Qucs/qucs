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
#include "model.h"
#include "qio.h"
#include "qucs_globals.h"
#include "symbol.h"
namespace{
using namespace qucs;
class LangSpice : public NetLang {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;
  std::string find_type_in_string(istream_t&) const override {incomplete(); return "incomplete";}
  Element* parseItem(istream_t&, Element*) const override {incomplete(); return nullptr;}

private: // local
  void printElement(Element const*, ostream_t&) const override { untested(); }
  void print_instance(ostream_t&, Component const*) const override;
  void print_paramset(ostream_t&, Model const*) const override;
  void print_module(ostream_t&, SubcktBase const*) const {untested(); }
  void printPainting(Painting const*, ostream_t&) const override { untested(); }
  void print_diagram(ostream_t&, Diagram const*) const override { untested();}
}d0;
static Dispatcher<Language>::INSTALL p(&language_dispatcher, "spice", &d0);
/*--------------------------------------------------------------------------*/
void LangSpice::print_paramset(ostream_t& o, Model const* s) const
{
	o << s->short_label() << " paramset ... stub\n";
}
/*--------------------------------------------------------------------------*/
void LangSpice::print_instance(ostream_t& o, Component const* s) const
{
	o << s->short_label() << " ... stub\n";
}
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
