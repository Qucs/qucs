#ifndef QUCS_LANGUAGE_H
#define QUCS_LANGUAGE_H

#include "io.h"
#include "object.h"

class Symbol;
class Element;
class CmdElement;
class Painting;

// baseclass for schematic and net languages.
// use u_lang.h instead?
class DocumentLanguage : public Object{
protected:
        DocumentLanguage() : Object(){}
public:
        virtual ~DocumentLanguage() {}
// virtual void parse(DocumentStream& stream, SchematicModel*) const=0;
  virtual void printItem(Element const*, ostream_t&) const;

  virtual void parseItem(Element*, istream_t&) const;
  virtual std::string findType(istream_t&) const = 0;
private: //called by printItem
  virtual void printPainting(Painting const*, ostream_t&) const = 0;
  virtual void printDiagram(Symbol const*, ostream_t&) const = 0;
  virtual void printSymbol(Symbol const*, ostream_t&) const = 0;
  virtual void printCommand(CmdElement const*, ostream_t&) const = 0;
};

#endif
