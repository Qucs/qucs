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

void Schematic::set_attribute(std::string name, std::string value)
{ untested();
  if("qucs_ViewX1"==name) {
    ViewX1=std::stoi(value);
  } else if ("qucs_ViewY1"==name) {
    ViewY1=std::stoi(value);
  } else if ("qucs_ViewX2"==name) {
    ViewX2=std::stoi(value);
  } else if ("qucs_ViewY2"==name) {
    ViewY2=std::stoi(value);
  } else if ("qucs_Scale"==name) {
    Scale=std::stof(value);
  } else if ("qucs_tmpViewX1"==name) {
    tmpViewX1=std::stoi(value);
  } else if ("qucs_tmpViewY1"==name) {
    tmpViewY1=std::stoi(value);
  } else if ("qucs_GridX"==name) {
    GridX=std::stoi(value);
  } else if ("qucs_GridY"==name) {
    GridY=std::stoi(value);
  } else if ("qucs_GridOn"==name) {
    if(std::stoi(value)==0) {
      GridOn=false;
    } else if(std::stoi(value)==1) {
      GridOn=true;
    } else {
      warn(0,"Wrong flag for grid switch");
    }
  } else if ("qucs_DataSet"==name) {
    DataSet=QString::fromStdString(value);
  } else if ("qucs_DataDisplay"==name) {
    DataDisplay=QString::fromStdString(value);
  } else if ("qucs_SimOpenDpl"==name) {
    if(std::stoi(value)==0) {
      SimOpenDpl=false;
    } else if(std::stoi(value)==1) {
      SimOpenDpl=true;
    } else {
      warn(0,"Wrong flag for sim open switch");
    }
  } else if ("qucs_Script"==name) {
    Script=QString::fromStdString(value);
  } else if ("qucs_SimRunScript"==name) {
    if(std::stoi(value)==0) {
      SimRunScript=false;
    } else if(std::stoi(value)==1) {
      SimRunScript=true;
    } else {
      warn(0,"Wrong flag for run script switch");
    }
  } else if ("qucs_showFrame"==name) {
    showFrame=std::stoi(value);
  } else if ("qucs_FrameText0"==name) {
    Frame_Text0=QString::fromStdString(value);
  } else if ("qucs_FrameText1"==name) {
    Frame_Text1=QString::fromStdString(value);
  } else if ("qucs_FrameText2"==name) {
    Frame_Text2=QString::fromStdString(value);
  } else if ("qucs_FrameText3"==name) {
    Frame_Text3=QString::fromStdString(value);
  } else {
  }
}
