#define TRACE_FUNCTION_CALLS 0

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <iostream>

#include <QtCore>
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

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

/*

We wanna print out something in the following form.
It's not yet Verilog AMS neither is it Verilog-A, but
our own little Verilog schematics hacking.

(* qucs_symbol="capacitor.svg" *)
module capacitor(p, n);
(* x=0, y=0 *) inout p;
(* x=10, y=0 *) inout n;
[..]
endmodule

*/

QString Schematic::getWireName(const QPoint *p) const
{
  QString net = QString("n_%1_%2")
      .arg(p->x())
      .arg(p->y());
  net.replace("-","m");
  return net;
}

static void dumpIdentifier(QTextStream& stream, QString const& name)
{
  if (!name.size()){
	  //incomplete();
 // }else if name contains special character
    // stream << '\\' << name << ' ';
  }else if(isalpha(name[0].toLatin1()) || name[0] == '_') {
    stream << name;
  } else {
    stream << '\\' << name << ' ';
  }
}

/*static void print_attributes(QTextStream& o, QStringList attributes)
{
  o << "(* " << attributes.join(", ") << " *) ";
}*/

#define INACTIVE 0
static void print_args(QTextStream& o, Component const* x)
{
  // assert(x);
  o << " #(";
  if(x) {
    QString sep = "";
    //for (int ii = x->param_count() - 1; ii >= 0; --ii) {
    for (int ii = 0; ii < x->param_count(); ++ii) {
      if (x->param_is_printable(ii)) {
        o << sep;
        o << '.' << x->param_name(ii) << '(' << x->param_value(ii) << ')';
        sep = ',';
      }else{
      }
    }
  }else{
    // BUG
  }
  o << ") ";
}

// BUG. what does it do?
// BUG: wrong compilation unit
void Schematic::dumpDeclaration(QTextStream& stream, Component const* c, QString model, QString name, QList<QPoint> ports) const
{
  // assert(c); ??
  QStringList nets;
  int port_idx = 0;
  stream << "    (* ";
  QString sep;
  for (auto pp = ports.begin(); pp != ports.end(); ++pp) {
    stream << sep << QString("S0_x%1=%2, S0_y%1=%3")
        .arg(++port_idx)
        .arg(pp->x())
        .arg(pp->y());
    sep = ", ";
    nets.append(getWireName(&(*pp)));
  }
  //print_attributes(o, nets);
  std::string attr;
  if(c){
    attr = c->attr_get();
  }else{
    //what?
  }
  if(attr.size()){
    stream << sep;
  }else{
  }
  stream << QString::fromStdString(attr) << QString(" *) ");
  dumpIdentifier(stream, model);
  print_args(stream, c);
  dumpIdentifier(stream, name);
  stream << " ( ";
  stream << nets.join(", ");
  stream << " );\n";
}

// BUG: wrong compilation unit
void Schematic::dumpVerilogComponent(QTextStream& stream, Component const* c) const
{
  assert(c);
  QList<QPoint> ports;
  QString model = c->obsolete_model_hack();
  QString name = c->name();
  for (auto pp = c->Ports.begin(); pp != c->Ports.end(); ++pp) {
    auto con = pp->getConnection();
    if(con) {
      ports.append(QPoint(con->cx,con->cy));
    }
  }
  dumpDeclaration(stream, c, model, name, ports);
}

/*
 * <280 100 460 100 "" 0 0 0> == net w1(n_280_100, n_460_100);
 */
// BUG: wrong compilation unit
void Schematic::dumpVerilogWire(QTextStream& stream, Wire const* w) const
{
	assert(w);
  QList<QPoint> ports;
  QString name;
  static int wire_index = 1;
  if(w->Label && !w->Label->Name.isEmpty()) {
    name = w->Label->Name;
  } else {
    name = QString("net%1").arg(wire_index);
    wire_index++;
  }
  ports.append(QPoint(w->x1,w->y1));
  ports.append(QPoint(w->x2,w->y2));
  dumpDeclaration(stream, NULL, "net", name, ports);
}

void Schematic::dumpVerilogQucsPreamble(QTextStream& stream) const
{
  QString preamble_string("qucs_%1=%2");
  QString preamble_text("qucs_%1=\"%2\"");
  // View
  stream << "qucs_ViewX1="    << ViewX1
       << ", qucs_ViewY1="    << ViewY1
       << ", qucs_ViewX2="    << ViewX2
       << ", qucs_ViewY2="    << ViewY2
       << ", qucs_Scale="     << Scale
       << ", qucs_tmpViewX1=" << tmpViewX1
       << ", qucs_tmpViewY1=" << tmpViewY1;
  // Grid
  stream << ", " << preamble_string.arg("GridX").arg(GridX);
  stream << ", " << preamble_string.arg("GridY").arg(GridY);
  stream << ", " << preamble_string.arg("GridOn").arg(GridOn);
  // Data
  stream << ", " << preamble_text.arg("DataSet", DataSet);
  stream << ", " << preamble_text.arg("DataDisplay", DataDisplay);
  // DPL
  stream << ", " << preamble_string.arg("SimOpenDpl").arg(SimOpenDpl);
  // Scripts
  stream << ", " << preamble_text.arg("Script", Script);
  stream << ", " << preamble_string.arg("SimRunScript").arg(SimRunScript);
  // Frame
  stream << ", " << preamble_string.arg("showFrame").arg(showFrame);
  stream << ", " << preamble_text.arg("FrameText0", Frame_Text0);
  stream << ", " << preamble_text.arg("FrameText1", Frame_Text1);
  stream << ", " << preamble_text.arg("FrameText2", Frame_Text2);
  stream << ", " << preamble_text.arg("FrameText3", Frame_Text3);
}

// BUG: wrong compilation unit
int Schematic::saveVerilogDocument(QFile *file)
{
  trace_method_calls();
  QList<QPoint> ioPortNets;
  QList<QPoint> wireList;
  QStringList ioPorts;

  for (auto it = DocComps.begin(); it != DocComps.end(); ++it) {
    QPoint p;
    if(it->obsolete_model_hack() == "Port") {
      p = QPoint(it->cx,it->cy);
      ioPorts.append(QString(".%1(%2)").arg(it->name()).arg(getWireName(&p)));
      ioPortNets.append(p);
    } else {
      p = QPoint(it->cx,it->cy);
      if(!wireList.contains(p))
        wireList.append(p);
    }
  }

  for (auto it = DocWires.begin(); it != DocWires.end(); ++it) {
    QPoint p;
    p = QPoint(it->x1,it->y1);
    if(!wireList.contains(p))
      wireList.append(p);
    p = QPoint(it->x2,it->y2);
    if(!wireList.contains(p))
      wireList.append(p);
  }

  std::sort(wireList.begin(), wireList.end(),
    [&](const QPoint& p1, const QPoint& p2){
      if( p1.x() < p2.x() )
         return true;
      if( (p1.x() == p2.x()) && (p1.y() < p2.y()) )
         return true;
      return false;
    }
  );

  // Writing stuff out
  QTextStream stream(file);
  QString module_name = QFileInfo(DocName).baseName();
  if(DocName.contains(".prj_")) {
    module_name = DocName.split(".prj_").at(1);
    module_name.replace(".sch","");
    module_name.replace(".vs","");
  }

  stream << "(* ";
  dumpVerilogQucsPreamble(stream);
  // stream << ", ";
  // stream << attr_get();
  stream << " *) ";

  stream << "module " << module_name << "(" << ioPorts.join(", ") << ");\n";

  // io defines
  for (auto it = ioPortNets.begin(); it != ioPortNets.end(); ++it) {
    stream << "    ";
    stream << "inout " << getWireName(&*it) << ";\n";
  }

  // The wires (subnets)
  for (auto it = wireList.begin(); it != wireList.end(); ++it) {
    if(!ioPortNets.contains(*it)) {
      stream << "    ";
      stream << "wire " << getWireName(&*it) << ";\n";
    }
  }

  // sub components
  for (auto it = DocComps.begin(); it != DocComps.end(); ++it) {
    dumpVerilogComponent(stream, &*it);
  }

  // net connections (connecting the nodes)
  for (auto it = DocWires.begin(); it != DocWires.end(); ++it) {
	  // BUG: Wire is not a Component. (why?)
    dumpVerilogWire(stream, &*it);
  }

  // done
  stream << "endmodule\n";
  stream.flush();
  file->flush();
  file->close();
  delete file;
  file = NULL;
  return 0;
}

// -------------------------------------------------------------
// Returns the number of subcircuit ports.
int Schematic::saveSchematicDocument(QFile *file)
{
  trace_method_calls();

  QTextStream stream(file);

  stream << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  stream << "<Properties>\n";
  if(symbolMode) {
    stream << "  <View=" << tmpViewX1<<","<<tmpViewY1<<","
       << tmpViewX2<<","<<tmpViewY2<< ",";
    stream <<tmpScale<<","<<tmpPosX<<","<<tmpPosY << ">\n";
  }
  else {
    stream << "  <View=" << ViewX1<<","<<ViewY1<<","
       << ViewX2<<","<<ViewY2<< ",";
    stream << Scale <<","<<contentsX()<<","<<contentsY() << ">\n";
  }
  stream << "  <Grid=" << GridX<<","<<GridY<<","
      << GridOn << ">\n";
  stream << "  <DataSet=" << DataSet << ">\n";
  stream << "  <DataDisplay=" << DataDisplay << ">\n";
  stream << "  <OpenDisplay=" << SimOpenDpl << ">\n";
  stream << "  <Script=" << Script << ">\n";
  stream << "  <RunScript=" << SimRunScript << ">\n";
  stream << "  <showFrame=" << showFrame << ">\n";

  QString t;
  misc::convert2ASCII(t = Frame_Text0);
  stream << "  <FrameText0=" << t << ">\n";
  misc::convert2ASCII(t = Frame_Text1);
  stream << "  <FrameText1=" << t << ">\n";
  misc::convert2ASCII(t = Frame_Text2);
  stream << "  <FrameText2=" << t << ">\n";
  misc::convert2ASCII(t = Frame_Text3);
  stream << "  <FrameText3=" << t << ">\n";
  stream << "</Properties>\n";

  stream << "<Symbol>\n";     // save all paintings for symbol
  for(auto pp = SymbolPaints.begin(); pp != SymbolPaints.end(); ++pp)
    stream << "  <" << pp->save() << ">\n";
  stream << "</Symbol>\n";

  stream << "<Components>\n";    // save all components
  for(auto pc = DocComps.begin(); pc != DocComps.end(); ++pc) {
    stream << "  "; // BUG language specific.
    saveComponent(stream, pc.operator->());
    stream << "\n"; // BUG?
  }
  stream << "</Components>\n";

  stream << "<Wires>\n";    // save all wires
  for(auto pw = DocWires.begin(); pw != DocWires.end(); ++pw) {
    stream << "  " << pw->save() << "\n";
  }

  // save all labeled nodes as wires
  for(auto pn = DocNodes.begin(); pn != DocNodes.end(); ++pn) {
    if(pn->Label) stream << "  " << pn->Label->save() << "\n";
  }
  stream << "</Wires>\n";

  stream << "<Diagrams>\n";    // save all diagrams
  for(auto pd = DocDiags.begin(); pd != DocDiags.end(); ++pd) {
    stream << "  " << pd->save() << "\n";
  }
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";     // save all paintings
  for(auto pp = DocPaints.begin(); pp != DocPaints.end(); ++pp) {
    stream << "  <" << pp->save() << ">\n";
  }
  stream << "</Paintings>\n";

  file->flush();
  file->close();
  delete file;
  file = NULL;

  // additionally save symbol C++ code if in a symbol drawing and the
  // associated file is a Verilog-A file
  if (fileSuffix () == "sym") {
    if (fileSuffix (DataDisplay) == "va") {
      saveSymbolCpp ();
      saveSymbolJSON ();

      // TODO slit this into another method, or merge into saveSymbolJSON
      // handle errors in separate
      qDebug() << "  -> Run adms for symbol";

      QString vaFile;

//      QDir prefix = QDir(QucsSettings.BinDir);

      QDir include = QDir(QucsSettings.BinDir+"../include/qucs-core");

      //pick admsXml from settings
      QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#ifdef __MINGW32__
      admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
#else
      admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
#endif
      // BUG: duplicated from qucs_actions.cpp
      char const* var = getenv("QUCS_USE_PATH");
      if(var != NULL) {
  // just use PATH. this is currently bound to a contition, to maintain
  // backwards compatibility with QUCSDIR
  qDebug() << "QUCS_USE_PATH";
  admsXml = "admsXml";
      }else{
      }

      QString workDir = QucsSettings.QucsWorkDir.absolutePath();

      qDebug() << "App path : " << qApp->applicationDirPath();
      qDebug() << "workdir"  << workDir;
      qDebug() << "homedir"  << QucsSettings.QucsHomeDir.absolutePath();
      qDebug() << "projsdir"  << QucsSettings.projsDir.absolutePath();

      vaFile = QucsSettings.QucsWorkDir.filePath(fileBase()+".va");

      QStringList Arguments;
      Arguments << QDir::toNativeSeparators(vaFile)
                << "-I" << QDir::toNativeSeparators(include.absolutePath())
                << "-e" << QDir::toNativeSeparators(include.absoluteFilePath("qucsMODULEguiJSONsymbol.xml"))
                << "-A" << "dyload";

//      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

      QFile file(admsXml);
      if(var) {
  // don't do this. it will always report an error.
      }else if ( !file.exists() ){
        QMessageBox::critical(this, tr("Error"),
                              tr("Program admsXml not found: %1\n\n"
                                  "Set the admsXml location on the application settings.").arg(admsXml));
        return -1;
      }

      qDebug() << "Command: " << admsXml << Arguments.join(" ");

      // need to cd into project to run admsXml?
      QDir::setCurrent(workDir);

      QProcess builder;
      builder.setProcessChannelMode(QProcess::MergedChannels);

      builder.start(admsXml, Arguments);


      // how to capture [warning]? need to modify admsXml?
      // TODO put stdout, stderr into a dock window, not messagebox
      if (!builder.waitForFinished()) {
        QString cmdString = QString("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
        cmdString = cmdString + builder.errorString();
        QMessageBox::critical(this, tr("Error"), cmdString);
      }
      else {
        QString cmdString = QString("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
        cmdString = cmdString + builder.readAll();
        QMessageBox::information(this, tr("Status"), cmdString);
      }

      // Append _sym.json into _props.json, save into _symbol.json
      QFile f1(QucsSettings.QucsWorkDir.filePath(fileBase()+"_props.json"));
      QFile f2(QucsSettings.QucsWorkDir.filePath(fileBase()+"_sym.json"));
      f1.open(QIODevice::ReadOnly | QIODevice::Text);
      f2.open(QIODevice::ReadOnly | QIODevice::Text);

      QString dat1 = QString(f1.readAll());
      QString dat2 = QString(f2.readAll());
      QString finalJSON = dat1.append(dat2);

      // remove joining point
      finalJSON = finalJSON.replace("}{", "");

      QFile f3(QucsSettings.QucsWorkDir.filePath(fileBase()+"_symbol.json"));
      f3.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&f3);
      out << finalJSON;

      f1.close();
      f2.close();
      f3.close();

      // TODO choose icon, default to something or provided png

    } // if DataDisplay va
  } // if suffix .sym

  return 0;
}

// Decide whether we wanna generate Verilog or a Schematic file
int Schematic::saveDocument(QString OutputFileName, QString OutputTypeName)
{
  trace_method_calls();
  if(OutputFileName.isEmpty())
    OutputFileName = DocName;

  QFile *file = new QFile(OutputFileName);
  if(!file->open(QIODevice::WriteOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
        QObject::tr("Cannot save document!"));
    delete file;
    return -1;
  }
  if(QFileInfo(OutputFileName).suffix()=="sch" || OutputTypeName=="sch") {
      return saveSchematicDocument(file);
  }
  if(QFileInfo(OutputFileName).suffix()=="vs" || OutputTypeName=="vs") {
      return saveVerilogDocument(file);
  }
  QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Cannot save document: File format not supported!"));
  return -1;
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
