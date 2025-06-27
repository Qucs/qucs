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
#include "component.h"
#include "qt_compat.h"

#if TRACE_FUNCTION_CALLS
#define trace_method_calls() qInfo()<<__FILE__ <<":"<<__func__
#else
#define trace_method_calls() { untested();}
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
    //for (int ii = x->param_count() - 1; ii >= 0; --ii) { untested();
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
      if(name[i] == '\\'){ untested();
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
{ untested();
  cmd.skipbl();
  std::string id;

  if(cmd.is_digit()) { untested();
    cmd.warn(bDANGER, "invalid identifier");
  }else{ untested();
  }

  bool esc = cmd.skip1('\\');

  while(esc && cmd.more()) { untested();
    if(cmd.skip1('\\')){ untested();
      if(cmd.skip1('\\')){ untested();
  id += "\\";
      }else{ untested();
  cmd.warn(bDANGER, "invalid escaped char");
      }
    }else{ untested();
    }
    id += cmd.get_to(" \t\f\\");

    if(cmd.skip1(" \t\f")){ untested();
      break;
    }else{ untested();
    }
  }

  if(!esc) { untested();
    id = cmd.ctos(term, "", "");
  }else{ untested();
  }

  trace1("identifier", id);
  return id;
}

template<class T>
void dump_attributes(outputStream& stream, T const* x)
{
  assert(x);
  auto c = dynamic_cast<Component const*>(x);
  auto w = dynamic_cast<Wire const*>(x);
  stream << "(* ";
  std::string attr;
  attr = x->attr_get();
  stream << QString::fromStdString(attr);
  QString sep;
  if(attr.size()){
    stream << ", ";
  }else{
  }
  if(c) {
    {
      stream << "qucs_visible=\"";
      for (int ii = 0; ii < c->param_count(); ++ii) {
        stream << c->param_is_visible(ii);
      }
      stream << "\", ";
    }
    if(c->net_nodes()) {
      for (auto i=0; i<c->net_nodes();++i) {
	int port_idx = i+1;
	auto& pp = c->port(i);
	if(pp.getConnection()) {
	  stream << sep
	    << "S0_x" << port_idx
	    << "=" << pp.getConnection()->cx()
	    << ", "
	    << "S0_y" << port_idx
	    << "="<< pp.getConnection()->cy();
	  sep = ", ";
	}else{ untested();
	}
      }
    }else{
      stream << "S0_x=" << c->cx() << ", S0_y=" << c->cy();
    }
  } else if(w) {
    stream << QString("S0_x1=%1, S0_y1=%2, S0_x2=%3, S0_y2=%4")
      .arg(w->ports(0)->cx())
      .arg(w->ports(0)->cy())
      .arg(w->ports(1)->cx())
      .arg(w->ports(1)->cy());
    if(w->Label) {
      stream << ", "
             << "qucs_label_cx="
             << w->Label->cx()
             << ", "
             << "qucs_label_cy="
             << w->Label->cy()
             << ", "
             << "qucs_label_x1="
             << w->Label->x1()
             << ", "
             << "qucs_label_y1="
             << w->Label->y1();
    } else {  untested();
    }
  } else { untested();
    // what?
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

static void dumpComponent(outputStream& stream, Element const* e)
{
  assert(e);
  stream << "    ";
  dump_attributes(stream, e);
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
  }else{ untested();
    unreachable();
  }
  stream << " ( ";
  std::string sep;
  if(c) {
    for (int i=0;i<c->net_nodes();i++) {
      stream << sep << c->port(i).getConnection()->label();
      sep = ", ";
    }
  } else if(w) {
    // BUG. Wire is not a Component.
    sep = ", ";
    stream << w->ports(0)->label() << sep << w->ports(1)->label();
    for (int i=2;i<w->net_nodes();i++) {
      // TODO stream << sep << c->port(i).getConnection()->label();
      stream << sep;
      dump_identifier(stream, w->name());
    }
  } else{ untested();
    unreachable();
  }
  stream << " );\n";
}

static void dumpPainting(outputStream& stream, Element const* p)
{ untested();
  static int text_counter=1;
  { untested();
  static int graphics_counter=1;
  stream << "    "
         << "(* "
         << "S0_x="
         << p->cx()
         << ", "
         << "S0_y="
         << p->cy()
         << ", "
         << "qucs_type=\""+p->dev_type()+"\"";
  if(p->attr_get()!="") { untested();
    stream << ", " << p->attr_get();
  } else { untested();}
  stream << " *)";
  if(p->dev_type()=="Text") { untested();
    stream << " S__text #()"
           << " S0_text"
           << text_counter;
    text_counter++;
  } else { untested();
    stream << " S__graphics #()"
           << " S0_graphics"
           << graphics_counter;
    graphics_counter++;
  }
  stream << "();\n";
  }
}

int Schematic::saveVerilogDocument(QFile *file)
{
  trace_method_calls();
  std::list<std::string> ioPortNets;
  std::list<std::string> ioPorts;

  for (auto it = DocComps.begin(); it != DocComps.end(); ++it) {
    QPoint p;
    if(it->obsolete_model_hack() == "Port") { untested();
      ioPorts.push_back("."+it->name().toStdString()+"("+it->port(0).getConnection()->label()+")");
      ioPortNets.push_back(it->port(0).getConnection()->label());
    }
  }

  // Writing stuff out
  QTextStream Qs(file);
  outputStream stream(Qs);
  QString module_name = QFileInfo(DocName).baseName();
  if(DocName.contains(".prj_")) { untested();
    module_name = DocName.split(".prj_").at(1);
    module_name.replace(".sch","");
    module_name.replace(".vs","");
  }

  stream << "(* ";
  dumpVerilogQucsPreamble(stream);
  // stream << ", ";
  // stream << attr_get();
  stream << " *) ";

  stream << "module " << module_name << "(";
  std::string sep;
  for(auto ip=ioPorts.begin();ip!=ioPorts.end();ip++) { untested();
    stream << sep << *ip;
    sep=", ";
  }
  stream << ");\n";

  // io defines
  for (auto it = ioPortNets.begin(); it != ioPortNets.end(); ++it) { untested();
    stream << "    ";
    stream << "inout " << *it << ";\n";
  }

  // The wires (subnets)
  for (auto it = DocNodes.begin(); it != DocNodes.end(); ++it) {
    if((std::find(ioPortNets.begin(), ioPortNets.end(), it->label())==ioPortNets.end())) {
      stream << "    ";
      stream << "wire " << it->label() << ";\n";
    }
  }

  // sub components
  for (auto it = DocComps.begin(); it != DocComps.end(); ++it) {
    dumpComponent(stream, &*it);
  }

  // net connections (connecting the nodes)
  for (auto it = DocWires.begin(); it != DocWires.end(); ++it) {
    // BUG: Wire is not a Component. (why?)
    dumpComponent(stream, &*it);
  }

  for (auto pt = DocPaints.begin(); pt != DocPaints.end(); ++pt) { untested();
    dumpPainting(stream, &*pt);
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
{ untested();
  while (cmd >> "(*") { untested();
    cmd.skipto1('*') && (cmd >> "*)");
  }
}

template <class T>
void set_attribute(T* x, std::string name, std::string value)
{ untested();
  assert(x);
  if(name == "S0_x1"){ untested();
    x->set_qucs_x1(std::stoi(value));
  }
  else
  if(name == "S0_y1"){ untested();
    x->set_qucs_y1(std::stoi(value));
  }
  else { untested();
    x->set_attribute(name, value);
  }
}

void set_attribute(Painting* x, std::string name, std::string value)
{ untested();
  x->set_attribute(name, value);
}

void set_attribute(Schematic* x, std::string name, std::string value)
{ untested();
  x->set_attribute(name, value);
}

// BUG. need extra function, Wire is not a Component.
void parse_args_instance(CS& cmd, Wire* x)
{ untested();
  (void)cmd;
  incomplete();
  { untested();
    cmd >> "#(";
    cmd >> ')';
  }
}

void parse_args_instance(CS& cmd, Component* x)
{ untested();
  assert(x);
  if (cmd >> "#(") { untested();
    if (cmd.match1('.')) { untested();
      // by name
      while (cmd >> '.') { untested();
        size_t here = cmd.cursor();
        std::string name  = cmd.ctos("(", "", "");
        std::string value = cmd.ctos(",)", "(", ")");
        cmd >> ',';
        try{ untested();
          //trace2("pai", name, value);
          x->set_param_by_name(name, value);
        }catch (qucs::ExceptionNoMatch&) {untested();
          cmd.warn(0, here, x->name().toStdString() + ": bad parameter " + name + " ignored");
        }
      }
    }else{ untested();
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
{  untested();
  assert(x);
  std::string my_name;
  my_name = parse_identifier(cmd, ",=(){};");
  if (my_name!="") { untested();
    qDebug() << "Getting label" << QString::fromStdString(my_name);
    x->set_label(my_name);
  }else{ untested();
  }
}

void parse_label(CS &cmd, Component* x)
{ untested();
  assert(x);
  std::string my_name;
  my_name = parse_identifier(cmd, ",=(){};");
  if (my_name!="") { untested();
    x->set_label(my_name);
  }else{ untested();
    //x->set_label(x->id_letter() + std::string("_unnamed")); //BUG// not unique
    x->set_label(std::string("_unnamed")); //BUG// not unique
    cmd.warn(bDANGER, "label required");
  }
}

void parse_ports(CS& cmd, Wire* x, bool /*all_new*/)
{ untested();
  (void)x;
  (void)cmd;
}

void parse_ports(CS& cmd, Component* x, bool all_new)
{ untested();
  assert(x);
  if (cmd >> '(') { untested();
    if (cmd.is_alnum()) { untested();
      // by order
      int index = 0;
      while (cmd.is_alnum()) { untested();
        size_t here = cmd.cursor();
        try{ untested();
          std::string value;
          cmd >> value;
          x->set_port_by_index(index, value);
          if (all_new) { untested();
            //    if (x->node_is_grounded(index)) { untested();
            //      cmd.warn(bDANGER, here, "node 0 not allowed here");
            //    }else
            //if (x->subckt() && int(x->subckt()->nodes()->size()) != index+1) { untested();
            //  cmd.warn(bDANGER, here, "duplicate port name, skipping");
            //}else{ untested();
              ++index;
            //}
          }else{ untested();
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
    }else{ untested();
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
{ untested();
  assert(x);
  parse_args_instance(cmd, x);
  parse_label(cmd, x);
  parse_ports(cmd, x, false/*allow dups*/);
  cmd >> ';';
  cmd.check(0, "what's this?");
}

class inspect_attributes {
  std::string _type;
  std::vector<std::pair<std::string,std::string>> _attr;
public:
  explicit inspect_attributes(CS& cmd) { untested();
    while (cmd >> "(*") { untested();
      while(cmd.ns_more() && !(cmd >> ",") && !(cmd >> "*)")) { untested();
	std::string name, value;
	cmd >> name;
        if(cmd >> "="){ untested();
	  cmd >> value;
	}else{ untested();
	  value = "1";
	}
  _attr.push_back(std::make_pair(name,value));
	trace2("inspect", name, value);
	if(name=="qucs_type") { untested();
	  _type = value;
	}else{ untested();
	}
      }
    }
  }
  template<class T>
  void export_attrs(T* x)const { untested();
    for(auto p: _attr) { untested();
      set_attribute(x, p.first, p.second);
    }
  }
  std::string type()const { untested();return _type;}
  bool has_type()const { untested();return _type.size();}
};

std::shared_ptr<Element> clone_instance(std::string const& type)
{ untested();
  QString qtype = QString::fromStdString(type);
  std::shared_ptr<Component> x = Module::getComponent(qtype); // BUG. need proper dispatcher.
  { untested();
  if(x) { untested();
    return x;
  } else { untested();
    std::shared_ptr<Painting> p = Module::getPainting(qtype);
    return p;
  }
  }
}

bool readVerilog(CS &cmd, Schematic*s)
{ untested();
  trace0("readVerilog0");
  assert(s);
  // todo: catch ExceptionEOF.
  while(!cmd.atEnd()) { untested();
    cmd.read_line();
    inspect_attributes attr(cmd);
    trace1("inspected", cmd.tail());
    if(cmd>>"module") { untested();
      attr.export_attrs(s);
      cmd >> ';';
    }else if(cmd>>"endmodule"){ untested();
      //ignore for now;
    }else{ untested();
      std::string type;
      type = parse_identifier(cmd, ",=(){};");
      std::shared_ptr<Element> inst;
      if(attr.has_type()){ untested();
        inst = clone_instance(attr.type());
      }else{ untested();
        inst = clone_instance(type);
      }

      if(type=="wire") { untested();
	// incomplete. net decl.
      }else if(type=="net") { untested();
	// BUG: Not a component
        Wire* w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
        if(w) { untested();
          attr.export_attrs(w);
          parse_instance(cmd, w);
          s->pushBack(w);
        }else{ untested();
		  }
      }else if(auto x = dynamic_cast<Component*>(inst.get())) { untested();
	trace3("readVerilog, gotComponent", type, x->tx(), x->ty());
	attr.export_attrs(x);
	x->apply_qucs_values();
	x->set_dev_type(type);
	/*x = */ parse_instance(cmd, x);
	// BUG: Gives inconsisten values when generating refs
	// setting text position to 0,0 for now.
	x->set_qucs_text_position(0, 0);
	s->pushBack(std::dynamic_pointer_cast<Component>(inst)); // (yikes)
      }else if(dynamic_cast<Painting*>(inst.get())) { untested();
        auto pe = std::dynamic_pointer_cast<Painting>(inst);
        attr.export_attrs(pe.get());
        cmd >> ';';
        s->pushBack(pe);
      }else{ untested();
	incomplete();
      }
    }
    trace2("readVerilog3", cmd.fullstring(), cmd.atEnd());
  }
  if(!s->_dry_run) { untested();
    for(auto cp=s->DocComps.begin(); cp!=s->DocComps.end(); cp++) { untested();
      cp->check_node_positions(s);
    }
  }
  return true;
}
// vim:ts=8:sw=2:noet:
