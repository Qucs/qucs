#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_cauer.h"
#include "qf_tform.h"
#include "qf_zigzag.h"

#undef	_QF_OPTIM_DEBUG

static	double hugedbl = numeric_limits <qf_double_t>::max ();
// Generic utility routines

// This routine computes the error function
// This error function is either Xmin if we optimize for Xmin (X = C or L)
// or it is 1/Xmax if we optimize for Xmax
qf_double_t  qf_filter::opt_error (qf_double_t& cm, qf_double_t& cM,
				   qf_double_t& lm, qf_double_t& lM) {
	
  if (! Pspec -> optc) return 0;

  qf_double_t dist = 0;

  if (Pspec -> Copt == -1)
    dist += 1 / cm;
  else if (Pspec -> Copt == 1)
    dist += cM;

  if (Pspec -> Lopt == -1)
    dist += 1 / lm;
  else if (Pspec -> Lopt == 1)
    dist += lM;

#ifdef _QF_OPTIM_DEBUG
  std::cout << "Error function: " << dist << '\n';
#endif

  return dist;
}

void  qf_filter::get_bounds (qf_double_t& cm, qf_double_t& cM,
			     qf_double_t& lm, qf_double_t& lM) {
  
  qf_cmp* i;

  qf_cap*   pc;
  qf_ind*   pi;
  qf_cmplc* plc;
  qf_pslc*  ppslc;

  lcmp. init ();
  while ((i = lcmp. next ()) != NULL) {

    if ((pc = dynamic_cast <qf_cap*> (i)) != NULL) {
      cm = min (cm, pc -> val);
      cM = max (cM, pc -> val);
      continue;
    }

    if ((pi = dynamic_cast <qf_ind*> (i)) != NULL) {
      lm = min (lm, pi -> val);
      lM = max (lM, pi -> val);
      continue;
    }

    if ((plc = dynamic_cast <qf_cmplc*> (i)) != NULL) {
      cm = min (cm, plc -> vC);
      cM = max (cM, plc -> vC);
      lm = min (lm, plc -> vL);
      lM = max (lM, plc -> vL);
      continue;
    }

    if ((ppslc = dynamic_cast <qf_pslc*> (i)) != NULL) {
      cm = min (cm, ppslc -> vC);
      cM = max (cM, ppslc -> vC);
      lm = min (lm, ppslc -> vL);
      lM = max (lM, ppslc -> vL);
      cm = min (cm, ppslc -> vC2);
      cM = max (cM, ppslc -> vC2);
      lm = min (lm, ppslc -> vL2);
      lM = max (lM, ppslc -> vL2);
      continue;
    }
  }
# ifdef _QF_OPTIM_DEBUG
  std::cout << "cm = " << cm << " cM = " << cM << " lm = " << lm <<
	    " lM = " << lM << '\n';
# endif
  return;
}
    
// Optimization for Cauer filters
// This function orders the poles according to the standard arrangement
// found in Zverev

void qf_cauer::pole_idx_std (unsigned m) {

  unsigned j = 0;
  m--;

  for (unsigned k = 0; k <= m; k ++) {
    pole [k] = j;
    j = m - j;
    if (j < m/2) j ++;
  }
}

void qf_cauer::pole_idx_init (unsigned m) {

  for (unsigned i = 0; i < m; i++) {
    pole. push_back (i);
    poleb. push_back (true);
  }
}

bool qf_cauer::pole_idx_next (unsigned k, unsigned n, bool flag) {
  
  unsigned i = pole [k];

  while (i != n)
    if (! poleb[i]) {
	    if (! flag) poleb [pole [k]] = false;
	    poleb [pole [k] = i] = true;
	    return true;
    }
    else i++;

  if (k == 0)
    return false;

  poleb [pole [k]] = false;
  pole [k] = 0;

  if (pole_idx_next (k - 1, n, false)) 
    pole_idx_next (k, n, true);
  else
    return false;

  return true;
}

qf_double_t qf_cauer::check_optim (void) {

  if (! Pspec -> optc) return 0;

  qf_double_t cm, cM, lm, lM;

  lm = cm = hugedbl;
  lM = cM = -hugedbl;

  get_bounds (cm, cM, lm, lM);

  return opt_error (cm, cM, lm, lM);
}


