#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_blinch.h"

/* BLINCHIKOFF BANDPASS FILTERS
   These filters are the equivalent of Bessel filters but for bandpass
   response: they have a nearly flat delay in the passband (in the sense
   of least squares minimization).

   Ref: H. Blinchikoff, M. Savetman: Least-Squares Approximation to
   Wide-Band Constant Delay. IEEE ...
*/

// Some restrictions apply to Blinchikoff : order = 2 or 4 and fractional
// bandwidth is limited by the published tables

bool  blval (qf_spec *Pspec) {

  Pspec -> th = Pspec -> bw / Pspec -> fc;  // Compute fractional bdwth
  switch (Pspec -> ord) {

    case 2:
      if ((Pspec -> th < 0.1) || (Pspec -> th > 0.9)) return false;
      return true;

    case 4:
      if ((Pspec -> th < 0.3) || (Pspec -> th > 0.7)) return false;
      return true;
  }
  // Avoids a warning
  return false;
}

// Linear interpolation functions to extrapolate coeffs.

inline	qf_double_t qf_blinch::interp2 (qf_double_t x, unsigned i) {

  if (x == 0) return blc_o2 [i];
  else return blc_o2 [i] + 10 * x * (blc_o2 [i + 5] - blc_o2 [i]);
}

inline	qf_double_t qf_blinch::interp4 (qf_double_t x, unsigned i) {

  if (x == 0) return blc_o4 [i];
  else return blc_o4 [i] + 10 * x * (blc_o4 [i + 9] - blc_o4 [i]);
}

// Direct synthesis out of the published tables

  qf_blinch::qf_blinch	(qf_spec *P) : qf_filter (P) {

  unsigned    idx;
  qf_double_t val;
  Pspec -> th = Pspec -> bw / Pspec -> fc;  // Compute fractional bdwth
  qf_double_t offset = (Pspec -> th) - floor (Pspec -> th * 10) / 10;
  qf_double_t dnrml = Pspec -> r1 / Pspec -> fc;
  qf_double_t dnrmc = 1 / (Pspec -> r1 * Pspec -> fc);

  switch (Pspec -> ord) {
    
    case 2: // Filter of order 2. Minimum inductor type
      idx = 5 * static_cast <unsigned> (floor ((Pspec -> th - 0.1) * 10));

      // Serial L
      val = interp2 (offset, idx);
      lcmp. insert (new qf_ind (val * dnrml, false, false));
      idx ++;

      // Serial C
      val = interp2 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, false, false));
      idx ++;

      // Parallel C
      val = interp2 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, true, true));
      idx ++;

      // Serial C
      val = interp2 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, false, false));
      idx ++;

      // Serial L
      val = interp2 (offset, idx);
      lcmp. insert (new qf_ind (val * dnrml, false, false));
      idx ++;
      
      lcmp. insert (new qf_end ());
      return ;
    
    case 4:
      idx = 9 * static_cast <unsigned> (floor ((Pspec -> th - 0.3) * 10));

      // Serial L
      val = interp4 (offset, idx);
      lcmp. insert (new qf_ind (val * dnrml, false, false));
      idx ++;

      // Serial C
      val = interp4 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, false, false));
      idx ++;

      // Parallel C
      val = interp4 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, true, true));
      idx ++;

      // Serial L
      val = interp4 (offset, idx);
      lcmp. insert (new qf_ind (val * dnrml, false, false));
      idx++ ;

      // Serial C
      val = interp4 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, false, false));
      idx ++;

      // Parallel L
      val = interp4 (offset, idx);
      lcmp. insert (new qf_ind (val * dnrml, true, true));
      idx ++;

      // Parallel C
      val = interp4 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, true, true));
      idx ++;

      // Serial L
      val = interp4 (offset, idx);
      lcmp. insert (new qf_ind (val * dnrml, false, false));
      idx++ ;

      // Parallel C
      val = interp4 (offset, idx);
      lcmp. insert (new qf_cap (val * dnrmc, true, true));
      idx ++;

      lcmp. insert (new qf_end ());
      return;
  }
}
