
#include "paramlist.h"

namespace qucs{

int CommonParamlist::_count = -1;
std::string COMMON_PARAMLIST::param_name(index_t i)const
{
  //assert(i < COMMON_PARAMLIST::param_count());
  if(i < COMMON_PARAMLIST::param_count()){
	  unreachable();
	  return "wrong_cpn";
  }else if (i >= COMMON_COMPONENT::param_count()) {
    return _params.name(COMMON_PARAMLIST::param_count() - 1 - i);
  }else{untested();
    return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_PARAMLIST::param_is_printable(index_t i)const
{
  if(i < COMMON_PARAMLIST::param_count()){
	  unreachable();
	  return false;
  }else if (i >= COMMON_COMPONENT::param_count()) {
    return _params.is_printable(COMMON_PARAMLIST::param_count() - 1 - i);
  }else{
    return COMMON_COMPONENT::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string CommonParamlist::param_value_by_name(std::string const& n)const
{
	trace1("CommonParamlist::param_value_by_name", n);
	try{
		return _params.value_by_name(n);
	}catch(qucs::ExceptionCantFind const&){
		trace1("fallback", n);
		return COMMON_COMPONENT::param_value_by_name(n);
	}
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_value(index_t i)const
{
	trace2("CommonParamlist::pv", i, COMMON_PARAMLIST::param_count());
	assert(i < COMMON_PARAMLIST::param_count());
	if (i >= COMMON_COMPONENT::param_count()) {
		return _params.value(COMMON_PARAMLIST::param_count() - 1 - i);
	}else{untested();
		return COMMON_COMPONENT::param_value(i);
	}
}
/*--------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& o, PARAM_LIST const& l)
{
	for(auto i: l){
		o << i.first << "=" << i.second.string() << "\n";
	}
	return o;
}
/*--------------------------------------------------------------------------*/
bool COMMON_PARAMLIST::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_PARAMLIST* p = dynamic_cast<const COMMON_PARAMLIST*>(&x);
  bool rv = p
    && _params == p->_params;

  trace3("CPL::op==", rv, p->_params.size(), _params.size());
  trace2("CPL::op==", p->_params, _params);
  rv &= COMMON_COMPONENT::operator==(x);
  return rv;
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
