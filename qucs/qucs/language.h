#ifndef QUCS_LANGUAGE_H
#define QUCS_LANGUAGE_H

#include "io.h"
#include "object.h"

class Symbol;
class Element;
class CmdElement;
class Painting;

// baseclass for schematic and net languages.
class DocumentLanguage : public Object{
protected:
        DocumentLanguage() : Object(){}
public:
        virtual ~DocumentLanguage() {}
// virtual void parse(DocumentStream& stream, SchematicModel*) const=0;
  virtual void printItem(Element const*, stream_t&) const;
private: //called by printItem
  virtual void printPainting(Painting const*, stream_t&) const {incomplete();}
  virtual void printDiagram(Symbol const*, stream_t&) const {incomplete();}
  virtual void printSymbol(Symbol const*, stream_t&) const {incomplete();}
  virtual void printCommand(CmdElement const*, stream_t&) const {incomplete();}
};

#endif
