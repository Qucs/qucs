#define TRACE_FUNCTION_CALLS 1

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <iostream>

#include "ap.h"
#include "qucs.h"
#include "node.h"
#include "schematic.h"
#include "module.h"
#include "misc.h"
#include "trace.h"
#include "exception.h"
#include "components/component.h"
#include "qt_compat.h"

#if TRACE_FUNCTION_CALLS
#define trace_method_calls() qInfo()<<__FILE__ <<":"<<__func__
#else
#define trace_method_calls() {}
#endif

struct outputStream{
  explicit outputStream(QTextStream& q) : _s(q) {}
  QTextStream& _s;
  template<class T>
  outputStream& operator<<(T const& t){
    _s << t;
    return *this;
  }
  // support standard strings
  outputStream& operator<<(std::string const& t){
    _s << QString::fromStdString(t);
    return *this;
  }
  void flush(){_s.flush();}
};

void Schematic::dumpVerilogQucsPreamble(outputStream& stream) const
{
  // View
  stream <<   "qucs_ViewX1="      << ViewX1
         << ", qucs_ViewY1="      << ViewY1
         << ", qucs_ViewX2="      << ViewX2
         << ", qucs_ViewY2="      << ViewY2
         << ", qucs_Scale="       << Scale
         << ", qucs_tmpViewX1="   << tmpViewX1
         << ", qucs_tmpViewY1="   << tmpViewY1;
  // Grid
  stream << ", qucs_GridX="       << GridX
         << ", qucs_GridY="       << GridY
         << ", qucs_GridOn="      << GridOn;
  // Data
  stream << ", qucs_DataSet="     << "\"" << DataSet << "\""
         << ", qucs_DataDisplay=" << "\"" << DataDisplay << "\"";
  // DPL
  stream << ", qucs_SimOpenDpl="  << SimOpenDpl;
  // Scripts
  stream << ", qucs_Script="      << "\"" << Script << "\""
         << ", qucs_SimRunScript="<< SimRunScript;
  // Frame
  stream << ", qucs_showFrame="   << showFrame
         << ", qucs_FrameText0="  << "\"" << Frame_Text0 << "\""
         << ", qucs_FrameText1="  << "\"" << Frame_Text1 << "\""
         << ", qucs_FrameText2="  << "\"" << Frame_Text2 << "\""
         << ", qucs_FrameText3="  << "\"" << Frame_Text3 << "\"";
}


#define INACTIVE 0
static void print_args(outputStream& o, Component const* x)
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

template<class S>
void dump_identifier(outputStream& o, S const& name)
{
  bool plain = true;

  if(isalpha(name[0])){
  }else if(name[0] == '$'){ untested();
  }else if(name[0] == '_'){ untested();
  }else{
    plain = false;
  }

  for(size_t i=1; plain && i<name.size(); ++i){
    if(isalnum(name[i])){
    }else if(name[i] == '_'){
    }else{ untested();
      plain = false;
    }
  }

  if(plain){
    o << name;
  }else{
    o << '\\';
    for(size_t i=0; i<name.size(); ++i){
      if(name[i] == '\\'){
  o << '\\';
      }else{
      }
      o << name[i];
    }
    o << ' ';
  }
}

/*--------------------------------------------------------------------------*/
// get identifier and turn into internal representation
// "\1 " -> "1"         -- so it also works with spice
// "\a " -> "a"         -- identical, use simple form
// "\$ " -> "$"         -- not sure.
// "\a* " -> "a*"       -- store unprotected
// "\\\xyz " -> "\xyz"  -- remove additional escapes
// "\foo\bar"           -- incomplete
static std::string parse_identifier(CS& cmd, std::string const& term)
{
  cmd.skipbl();
  std::string id;

  if(cmd.is_digit()) {
    cmd.warn(bDANGER, "invalid identifier");
  }else{
  }

  bool esc = cmd.skip1('\\');

  while(esc && cmd.more()) {
    if(cmd.skip1('\\')){
      if(cmd.skip1('\\')){
  id += "\\";
      }else{ untested();
  cmd.warn(bDANGER, "invalid escaped char");
      }
    }else{
    }
    id += cmd.get_to(" \t\f\\");

    if(cmd.skip1(" \t\f")){
      break;
    }else{
    }
  }

  if(!esc) {
    id = cmd.ctos(term, "", "");
  }else{
  }

  trace1("identifier", id);
  return id;
}

template<class T>
void dump_attributes(outputStream& stream, T const* c, QList<QPoint> ports, Schematic const* s)
{
  //assert(c);
  QStringList nets;
  int port_idx = 0;
  stream << "(* ";
  //print_attributes(o, nets);
  std::string attr;
  if(c){
    attr = c->attr_get();
  }else{
    //what?
  }
  stream << QString::fromStdString(attr);
  QString sep;
  if(attr.size()){
    sep = ", ";
  }else{
  }
  if(!ports.size()) {
    stream << sep << QString("S0_x%1=%2, S0_y%1=%3")
      .arg(++port_idx)
      .arg(c->cx)
      .arg(c->cy);
  }else{
  for (auto pp = ports.begin(); pp != ports.end(); ++pp) {
    stream << sep << QString("S0_x%1=%2, S0_y%1=%3")
        .arg(++port_idx)
        .arg(pp->x())
        .arg(pp->y());
    sep = ", ";
    nets.append(s->getWireName(&(*pp)));
  }
  }
  stream << QString(" *) ");
}

static std::string wirelabel(Wire const* w)
{
  std::string name;
  static int wire_index = 1;
  if(w->Label && !w->Label->Name.isEmpty()) {
    name = w->Label->Name.toStdString();
  } else {
    name = "net" + std::to_string(wire_index++);
  }
  return name;
}

static void dumpDeclaration(outputStream& stream, Element const* e, QList<QPoint> ports,
    Schematic const* s)
{
  auto c = dynamic_cast<Component const*>(e);
  auto w = dynamic_cast<Wire const*>(e);
  if(c){
    std::string type = c->dev_type();
    dump_identifier(stream, type);
  }else{
    stream << "net"; // BUG
  }
  print_args(stream, c);
  if(c){
    dump_identifier(stream, c->name().toStdString());
  }else if(w){
    // BUG. Wire is not a Component.
    stream << wirelabel(w);
  }else{
    unreachable();
  }
  stream << " ( ";
  std::string sep;
  for (auto pp = ports.begin(); pp != ports.end(); ++pp) {
    stream << sep << s->getWireName(&(*pp));
    sep = ", ";
  }
  stream << " );\n";
}

void Schematic::dumpVerilogComponent(outputStream& stream, Element const* e) const
{
  assert(e);
  QList<QPoint> ports;
  if(auto c = dynamic_cast<Component const*>(e)){
    for (auto pp = c->Ports.begin(); pp != c->Ports.end(); ++pp) {
      auto con = pp->getConnection();
      if(con) {
  ports.append(QPoint(con->cx,con->cy));
      }
    }
  }else if(auto w = dynamic_cast<Wire const*>(e)){
    // BUG. wire is not a component.
    ports.append(QPoint(w->x1,w->y1));
    ports.append(QPoint(w->x2,w->y2));
  }else{
    unreachable()
  }
  stream << "    ";

  dump_attributes(stream, e, ports, this);
  dumpDeclaration(stream, e, ports, this);
}

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
  QTextStream Qs(file);
  outputStream stream(Qs);
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
    dumpVerilogComponent(stream, &*it);
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

void skip_attributes(CS& cmd)
{
  while (cmd >> "(*") {
    cmd.skipto1('*') && (cmd >> "*)");
  }
}

template <class T>
void set_attribute(T* x, std::string name, std::string value)
{
  assert(x);
  if(name == "S0_x1"){
    x->set_qucs_x1(std::stoi(value));
  }
  else
  if(name == "S0_y1"){
    x->set_qucs_y1(std::stoi(value));
  }
  else {
    x->set_attribute(name, value);
  }
}

template <class T>
void parse_attributes(CS& cmd, T* x)
{
  assert(x);
  incomplete();
  while (cmd >> "(*") {
    while(cmd.ns_more() && !(cmd >> ",") && !(cmd >> "*)")) {
      std::string name, value;
      cmd >> name >> "=" >> value;
      set_attribute(x, name, value);
    }
  }
  x->apply_qucs_values();
}

// BUG. need extra function, Wire is not a Component.
void parse_type(CS& cmd, Wire* x)
{
  incomplete();
  (void)cmd;
  (void)x;
}

void parse_type(CS& cmd, Component* x)
{
  assert(x);
  //incomplete();
  std::string new_type;
  new_type = parse_identifier(cmd, ",=(){};");
  x->set_dev_type(new_type);
}

// BUG. need extra function, Wire is not a Component.
void parse_args_instance(CS& cmd, Wire* x)
{
  (void)x;
  (void)cmd;
  incomplete();
}

void parse_args_instance(CS& cmd, Component* x)
{
  assert(x);
  if (cmd >> "#(") {
    if (cmd.match1('.')) {
      // by name
      while (cmd >> '.') {
        size_t here = cmd.cursor();
        std::string name  = cmd.ctos("(", "", "");
        std::string value = cmd.ctos(",)", "(", ")");
        cmd >> ',';
        try{
          //trace2("pai", name, value);
          x->set_param_by_name(name, value);
        }catch (qucs::ExceptionNoMatch&) {untested();
          cmd.warn(0, here, x->name().toStdString() + ": bad parameter " + name + " ignored");
        }
      }
    }else{
      // by order
      int index = 1;
      while (cmd.is_alnum() || cmd.match1("+-.")) { untested();
        size_t here = cmd.cursor();
        try{ untested();
          std::string value = cmd.ctos(",)", "", "");
          //trace2("pai", index, value);
          x->set_param_by_index(x->param_count() - index++, value); // , 0/*offset*/);
        }catch (qucs::Exception_Too_Many& e) {untested();
          cmd.warn(bDANGER, here, e.message());
        }
      }
    }
    cmd >> ')';
  }else{ untested();
    // no args
  }
}

// BUG. see above
void parse_label(CS &cmd, Wire* x)
{
  (void)x;
  (void)cmd;
}

void parse_label(CS &cmd, Component* x)
{
  assert(x);
  std::string my_name;
  my_name = parse_identifier(cmd, ",=(){};");
  if (my_name!="") {
    x->set_label(my_name);
  }else{ untested();
    //x->set_label(x->id_letter() + std::string("_unnamed")); //BUG// not unique
    x->set_label(std::string("_unnamed")); //BUG// not unique
    cmd.warn(bDANGER, "label required");
  }
}

void parse_ports(CS& cmd, Wire* x, bool /*all_new*/)
{
  (void)x;
  (void)cmd;
}

void parse_ports(CS& cmd, Component* x, bool all_new)
{
  assert(x);
  if (cmd >> '(') {
    if (cmd.is_alnum()) {
      // by order
      int index = 0;
      while (cmd.is_alnum()) {
        size_t here = cmd.cursor();
        try{
          std::string value;
          cmd >> value;
          x->set_port_by_index(index, value);
          if (all_new) {
            //    if (x->node_is_grounded(index)) { untested();
            //      cmd.warn(bDANGER, here, "node 0 not allowed here");
            //    }else
            //if (x->subckt() && int(x->subckt()->nodes()->size()) != index+1) { untested();
            //  cmd.warn(bDANGER, here, "duplicate port name, skipping");
            //}else{
              ++index;
            //}
          }else{
            ++index;
          }
        }catch (qucs::Exception_Too_Many& e) { untested();
          cmd.warn(bDANGER, here, e.message());
        }
      }
//			if (index < x->min_nodes()) { untested();
//				cmd.warn(bDANGER, "need " + to_string(x->min_nodes()-index) +" more nodes, grounding");
//				for (int iii = index;  iii < x->min_nodes();  ++iii) { untested();
//					x->set_port_to_ground(iii);
//				}
//			}else{ untested();
//			}
    }else{
      // by name
      while (cmd >> '.') { untested();
        size_t here = cmd.cursor();
        try{ untested();
          std::string name, value;
          cmd >> name >> '(' >> value >> ')' >> ',';
          x->set_port_by_name(name, value);
        }catch (qucs::ExceptionNoMatch const&) {untested();
          cmd.warn(bDANGER, here, "mismatch, ignored");
        }
      }
//			for (int iii = 0;  iii < x->min_nodes();  ++iii) { untested();
//				if (!(x->node_is_connected(iii))) {untested();
//					cmd.warn(bDANGER, x->port_name(iii) + ": port unconnected, grounding");
//					x->set_port_to_ground(iii);
//				}else{ untested();
//				}
//			}
    }
    cmd >> ')';
  }else{ untested();
    cmd.warn(bDANGER, "'(' required (parse ports) (grounding)");
    incomplete();
//		for (int iii = 0;  iii < x->min_nodes();  ++iii) { untested();
//			if (!(x->node_is_connected(iii))) { untested();
//				cmd.warn(bDANGER, x->port_name(iii) + ": port unconnected, grounding");
//				x->set_port_to_ground(iii);
//			}else{ untested();
//				unreachable();
//			}
//		}
  }
}

template <class T>
void parse_instance(CS& cmd, T* x)
{
  assert(x);
  cmd.reset();
  parse_attributes(cmd, x);
  parse_type(cmd, x);
  parse_args_instance(cmd, x);
  parse_label(cmd, x);
  parse_ports(cmd, x, false/*allow dups*/);
  cmd >> ';';
  cmd.check(0, "what's this?");
}

class inspect_attributes {
  std::string _type;
public:
  explicit inspect_attributes(CS& cmd) { untested();
    while (cmd >> "(*") { untested();
      while(cmd.ns_more() && !(cmd >> ",") && !(cmd >> "*)")) { untested();
	std::string name, value;
	cmd >> name;
        if(cmd >> "="){
	  cmd >> value;
	}else{
	  value = "1";
	}
	trace2("inspect", name, value);
	if(name=="qucs_type") { untested();
	  _type = value;
	}else{ untested();
	}
      }
    }
  }

  std::string type()const {return _type;}
  bool has_type()const {return _type.size();}
};

bool readVerilog(CS &cmd, Schematic*s)
{
  trace0("readVerilog0");
  assert(s);
  // todo: catch ExceptionEOF.
  while(!cmd.atEnd()) {
    cmd.read_line();
    inspect_attributes attr(cmd);
    trace1("inspected", cmd.tail());
    if(cmd>>"module") { untested();
      //ignore for now;
    }else if(cmd>>"endmodule"){
      //ignore for now;
    }else{
      std::string type;
      type = parse_identifier(cmd, ",=(){};");
      if(type=="wire") {
	 // BUG: Not a component
      }else if(type=="net") {
        Wire* w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
        if(w) {
          parse_instance(cmd, w);
          s->pushBack(w);
        }else{
		  }
      } else {
        QString qtype = QString::fromStdString(type);
	if(attr.has_type()){
	  qtype = QString::fromStdString(attr.type());
	}else{
	}
        std::shared_ptr<Component> x = Module::getComponent(qtype); // BUG. need proper dispatcher.
        if(x) {
	  trace3("readVerilog, gotComponent", qtype, x->tx(), x->ty());
          parse_instance(cmd, x.get());
          // BUG: Gives inconsisten values when generating refs
          // setting text position to 0,0 for now.
	  x->set_qucs_text_position(0, 0);
          s->pushBack(x);
        }else{
	  trace1("readVerilog, no Component", qtype);
	}
      }
    }
    trace2("readVerilog3", cmd.fullstring(), cmd.atEnd());
  }
  return true;
}
// vim:ts=8:sw=2:noet:
