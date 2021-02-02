/*--------------------------------------------------------------------------*/
#include "dispatcher.h"
#include "message.h"

#define CKT_BASE Object
#define error message
#define DISPATCHER Dispatcher
/*--------------------------------------------------------------------------*/
CKT_BASE* DISPATCHER_BASE::operator[](std::string s)
{
  CKT_BASE* rv = _map[s];
  if (!rv && OPT::case_insensitive) {
    notstd::to_lower(&s);
    rv = _map[s];
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void DISPATCHER_BASE::uninstall(CKT_BASE* p)
{
  for (typename std::map<std::string, CKT_BASE*>::iterator
	 ii = _map.begin();  ii != _map.end();  ++ii) {
    if (ii->second == p) {
      ii->second = NULL;
    }else{
    }
  }
#if !defined(NDEBUG)
  for (typename std::map<std::string, CKT_BASE*>::iterator
	 ii = _map.begin();  ii != _map.end();  ++ii) {
    assert(ii->second != p);
  }
#endif
}
/*--------------------------------------------------------------------------*/
void DISPATCHER_BASE::install(const std::string& s, CKT_BASE* p)
{
  assert(s.find(',', 0) == std::string::npos);
  trace0(s.c_str());
  // loop over all keys, separated by '|'
  for (std::string::size_type			// bss: begin sub-string
	 bss = 0, ess = s.find('|', bss);	// ess: end sub-string
       bss != std::string::npos;
       bss = (ess != std::string::npos) ? ess+1 : std::string::npos,
	 ess = s.find('|', bss)) {
    std::string name = s.substr(bss, (ess != std::string::npos) ? ess-bss : std::string::npos);
    trace2(name.c_str(), bss, ess);
    if (name == "") {untested();
      // quietly ignore empty string
    }else if (_map[name]) {
      // duplicate .. stash the old one so we can get it back
      error(bWARNING, name + ": already installed, replacing\n");
      std::string save_name = name + ":0";
      for (int ii = 0; _map[save_name]; ++ii) {untested();
	save_name = name + ":" + to_string(ii);
      }
      _map[save_name] = _map[name];
      error(bWARNING, "stashing as " + save_name + "\n");
    }else{
      // it's new, just put it in
    }
    _map[name] = p;
  }
}
/*--------------------------------------------------------------------------*/
