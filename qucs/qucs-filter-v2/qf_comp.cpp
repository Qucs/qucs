#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qf_common.h"
#include "qf_comp.h"

// DUMP functions and utilities

static QString valstr (qf_double_t num) {

  char	      c = 0;

  num = fabs (num);

  if ((num > 1e-18) && (num < 1e6)) {
    qf_double_t cal = floor (log10 (num) / 3);
    int		expo = static_cast <int> (cal);

    switch (expo) {
      case -6: c = 'a'; break;
      case -5: c = 'f'; break;
      case -4: c = 'p'; break;
      case -3: c = 'n'; break;
      case -2: c = 'u'; break;
      case -1: c = 'm'; break;
      case  0: break;
      case  1: c = 'k'; break;
      }

    if (c != 0) num /= pow (10.0, 3 * cal);
  }

  QString buffer;
  QTextStream str (& buffer, IO_WriteOnly);

  str. precision (3);

  str << num << " " ;
  if (c) 
    str << c;

  return buffer;
}

void qf_cmp1::dump (unsigned n1, unsigned n2, QTextStream& s) {

  s << name << "\t" << n1 << "\t";
  if (n2 == 0)
    s << "GND\t";
  else
    s << n2 << "\t";

  s << valstr (val) << unit << "\n";
}

void qf_cmplc::dump (unsigned n1, unsigned n2, QTextStream& s) {

  s << name << "\t" << n1 << "\t";
  if (n2 == 0)
    s << "GND\t";
  else
    s << n2 << "\t";

  s << valstr (vC) << "F,\t" << valstr (vL) << "H\n";
}

void  qf_pslc::dump (unsigned n1, unsigned n2, QTextStream& s) {
  qf_slc (vC, vL, shnt, gnd). dump (n1, n2, s);
  qf_slc (vC2, vL2, shnt, gnd). dump (n1, n2, s);
}

// Dumps the whole filter structure
int qf_lcmp::dump_node (int& p, list <qf_cmp*>::iterator& i,
			  QTextStream& out) {

  int	    n = p + 1;

  if ((*i) -> name == "END") return -n;		  // End of structure
  while (i != lst. end ()) {

    if ((*i) == NULL) return 0;
    if ((*i) -> name == "END") return -n;	  // End of structure

    if ((*i) -> gnd) {
      (*i) -> dump (p, 0, out);
      i ++;
      continue;
    }

    (*i) -> dump (p, n, out);
    if ((*i) -> shnt) {
      i ++;
      n = dump_node (n, i, out);
    }
    else {
      i ++;
      return n;
    }
  } 
  return -n;
}

void qf_lcmp::dump_all (QTextStream& out) {

  if (lst. size () == 0) {
    out << "ERR" << endl;
    return ;
  }

  int	node = 1;

  for (list <qf_cmp*>::iterator i = lst. begin (); i != lst. end ();) 
    if ((node = dump_node (node, i, out)) < 0) break;

  out << "OUT\t" << - (node + 1) << '\n';
}

// Access to the list of components

void	qf_lcmp::init (void) {

  iter = lst. begin ();
}

qf_cmp*	qf_lcmp::next (void) {

  qf_cmp* cmp = * iter;

  if ((cmp == NULL) || (typeid (* cmp) == typeid (qf_end))) return NULL;
  iter ++;
  return cmp;
}
