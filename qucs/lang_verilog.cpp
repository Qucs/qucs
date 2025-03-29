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
}

void parse_type(CS& cmd, Component* x)
{
  assert(x);
  //incomplete();
  std::string new_type;
  cmd >> new_type;
  x->set_dev_type(new_type);
}

// BUG. need extra function, Wire is not a Component.
void parse_args_instance(CS& cmd, Wire* x)
{}

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
}

void parse_label(CS &cmd, Component* x)
{
  assert(x);
  std::string my_name;
  if (cmd >> my_name) {
    x->set_label(my_name);
  }else{ untested();
    //x->set_label(x->id_letter() + std::string("_unnamed")); //BUG// not unique
    x->set_label(std::string("_unnamed")); //BUG// not unique
    cmd.warn(bDANGER, "label required");
  }
}

void parse_ports(CS& cmd, Wire* x, bool all_new)
{
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
      cmd >> type;
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
