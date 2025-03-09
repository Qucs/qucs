#define TRACE_FUNCTION_CALLS 1

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <iostream>

#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

#include "ap.h"
#include "qucs.h"
#include "node.h"
#include "schematic.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/libcomp.h"
#include "module.h"
#include "misc.h"

#if TRACE_FUNCTION_CALLS
#define trace_method_calls() qInfo()<<__FILE__ <<":"<<__func__
#else
#define trace_method_calls() {}
#endif

bool Schematic::readLegacy(QFile &file) {
  trace_method_calls();
  file.reset();
  QString Line;
  QTextStream stream(&file);
  Line = stream.readLine();
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;
    if(Line == "<Symbol>") {
      if(!loadPaintings(&stream, SymbolPaints)) {
        file.close();
        return false;
      }
    }
    else
    if(Line == "<Properties>") {
      if(!loadProperties(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Components>") {
      if(!loadComponents(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Wires>") {
      if(!loadWires(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(&stream, DocDiags)) { file.close(); return false; } }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(&stream, DocPaints)) { file.close(); return false; } }
    else {
      qDebug() << Line;
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("File Format Error:\nUnknown field!"));
      file.close();
      return false;
    }
  }
  file.close();
  return true;
}

// BUG; this is not Schematic code.
bool Schematic::readVerilog(QFile /* BUG: need CS */ &file)
{
  trace_method_calls();
  file.reset(); // BUG. why?
  QTextStream stream(&file); // BUG, remove
  CS cmd(&stream); // BUG. remove
  //std::cout << "Dummy read" << std::endl;
  //std::cout << "-------------" << std::endl;
  char c;
  while(!cmd.atEnd()) {
    cmd.read_line();
    do {
      c = cmd.ctoc();
      //std::cout << c;
    } while(c);
  }
  //std::cout << std::endl << "-------------" << std::endl;
  file.close(); // BUG. see above.
  return true;
}
