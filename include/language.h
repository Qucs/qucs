#ifndef QUCS_LANGUAGE_H
#define QUCS_LANGUAGE_H

//#include "qio.h"
#include "object.h"

#define DEV_DOT Dot
// include u_lang.h // TODO
//class Command;
class DEV_DOT;
class Element;
class istream_t;
class Painting;
class SchematicModel;
class SubcktBase;
class Symbol;
class TaskElement;
class ostream_t;

/* -------------------------------------------------------------------------------- */
// baseclass for schematic and net languages.
// use u_lang.h instead?
class DocumentLanguage : public Object{
protected:
        DocumentLanguage() : Object(){}
public:
        virtual ~DocumentLanguage() {}
// virtual void parse(DocumentStream& stream, SchematicModel*) const=0;
  virtual void printItem(ostream_t&, Element const*) const;

  virtual void		parse_top_item(istream_t&, SchematicModel*) const {incomplete();}
  virtual Element* parseItem(istream_t&, Element*) const = 0;
  virtual DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const{
	  untested(); return nullptr;
  }
  virtual std::string findType(istream_t&) const = 0;
  void new__instance(istream_t& cmd, Element* owner,
                     SchematicModel* Scope) const;
  const Element* find_proto(const std::string&, const Element*) const;
private: //called by printItem
  virtual void printPainting(Painting const*, ostream_t&) const = 0;
  virtual void printDiagram(Symbol const*, ostream_t&) const = 0;
  virtual void printSymbol(Symbol const*, ostream_t&) const = 0;
  virtual void printSubckt(SubcktBase const*, ostream_t&) const = 0;
  virtual void printTaskElement(TaskElement const*, ostream_t&) const = 0;
//  virtual void print_comment(ostream_t&, const DEV_COMMENT const*){incomplete();}
  virtual void print_command(ostream_t&, const DEV_DOT*) const{incomplete();}
};

// group netlist languages. needed? obsolete?
class NetLang : public DocumentLanguage {
public:
  virtual ~NetLang(){}
};
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#endif
