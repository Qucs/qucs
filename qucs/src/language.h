#ifndef QUCS_LANGUAGE_H
#define QUCS_LANGUAGE_H

#include "io.h"
#include "object.h"

class Command;
class DEV_DOT;
class Element;
class istream_t;
class Painting;
class SchematicModel;
class SubcktBase;
class Symbol;
class TaskElement;

// baseclass for schematic and net languages.
// use u_lang.h instead?
class DocumentLanguage : public Object{
protected:
        DocumentLanguage() : Object(){}
public:
        virtual ~DocumentLanguage() {}
// virtual void parse(DocumentStream& stream, SchematicModel*) const=0;
  virtual void printItem(Element const*, ostream_t&) const;

  virtual Element* parseItem(istream_t&, Element*) const;
  virtual DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const{
	  untested(); return nullptr;
  }
  virtual std::string findType(istream_t&) const = 0;
  void new__instance(istream_t& cmd, Symbol* /*sckt?*/ owner,
                     SchematicModel* Scope) const;
  const Element* find_proto(const std::string&, const Element*) const;
private: //called by printItem
  virtual void printPainting(Painting const*, ostream_t&) const = 0;
  virtual void printDiagram(Symbol const*, ostream_t&) const = 0;
  virtual void printSymbol(Symbol const*, ostream_t&) const = 0;
  virtual void printSubckt(SubcktBase const*, ostream_t&) const = 0;
  virtual void printtaskElement(TaskElement const*, ostream_t&) const = 0;
};

#endif
