#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_filter.h"

// Transforms a Tee type network of reactances into an equivalent Pi type

bool qf_filter::tee2pi (qf_lcmp& topo, qf_lcmp::iterator i) {

  qf_val	    v;
  qf_double_t	    xa, xb, xc, xs;
  Rarray	    nx (3);
  qf_cmp_type	    type;
  qf_val	    val;

  (*i) -> get (val);
  type = v.type;
  // Tee and Pi transforms are only available for CAP, IND et RES
  if ((type != CAP) && (type != IND) && (type != RES)) return false;

  xa = v.val.front (); 
  delete (*i);
  i = topo.erase (i);

  (*i) -> get (val);
  // Verifies coherency
  if (v.type != type) return false;
  // Verifies topology;
  if ((! v. shnt) || (! val.to_gnd)) return false;
  xb = v.val.front (); 
  delete (*i);
  i = topo.erase (i);

  (*i) -> get (val);
  // Verifies coherency
  if (v.type != type) return false;
  xc = v.val.front (); 
  delete (*i);

  // Calculate and substitute elements value
  if (type == CAP) {
    xs = xa + xb + xc;
    nx[0] = xa * xb / xs;
    nx[1] = xa * xc / xs;
    nx[2] = xb * xc / xs;

    qf_cap*  cap = new qf_cap (nx[0], true, true);

    topo. insert (i, cap);
    cap = new qf_cap (nx[1], false, false);
    topo. insert (i, cap);
    cap = new qf_cap (nx[2], true, true);
    topo. insert (i, cap);
    return true;
  }
    
  xs = xa * xb + xa * xc + xb * xc;
  nx[0] = xs / xa;
  nx[1] = xs / xb;
  nx[2] = xs / xc;

  if (type == IND) {
    qf_ind*  ind = new qf_ind (nx[0], true, true);

    topo. insert (i, ind);
    ind = new qf_ind (nx[1], false, false);
    topo. insert (i, ind);
    ind = new qf_ind (nx[2], true, true);
    topo. insert (i, ind);
    return true;
  }

  qf_res*  res = new qf_res (nx[0], true, true);

  topo. insert (i, res);
  res = new qf_res (nx[1], false, false);
  topo. insert (i, res);
  res = new qf_res (nx[2], true, true);
  topo. insert (i, res);
  return true;
}
