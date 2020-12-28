// Bessel filters
// Bessel filters have the best group propagation characteristics
// Therefore, they are those which deform less linear signals like pulses
// But they have the worst cutoff rate


#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_bessel.h"

#undef 	_QF_BESSEL_DEBUG

// Computes the Bessel polynomial Bn of nth order
// which is equal to E(p)
// Bn(p) = (2n - 1) Bn-1 + p^2 Bn-2

qf_poly	qf_bessel::poly_bessel (unsigned n) {

  qf_poly B1 (0.0, 1.0, 1.0); // B0 = p + 1
  if (n == 1) return B1;

  qf_poly B2 (1, 3, 3); // B1 = p^2 + 3p + 3
  if (n == 2) return B2;

  for (unsigned i = 3; ; i ++) {
    qf_poly Bi (B2 * (2 * i - 1));
    Bi += (B1 >> 2);

    if (i == n) {
      Bi. nrm (0);
      qf_double_t s = pow (Bi. pk (), 
			    1.0 / static_cast <double> (Bi. deg ()));
      Bi. scale (1.0/s); // Scales 
      return Bi;
    }

    B1 = B2;
    B2 = Bi;
  }
}

// Calculate nth order Bessel filter
qf_bessel::qf_bessel (qf_spec*	Pspec): qf_filter (Pspec) {

  qf_double_t s;      // A scale factor 

  scpt (s);
}

bool  qf_bessel::synth (void) {

  // Computes the frequency at which the gain equals 3 dB

  qf_poly G (E. hsq ());
  G. cabs ();
  G -= 2.0;
  G. rroot (s);

#ifdef	_QF_BESSEL_DEBUG
  std::cout << "Normalization ws: " << s << endl;
#endif

  // Implicitly P = 1
  scptX3 (E, P, BN, BD);

#ifdef _QF_BESSEL_DEBUG
  BN. disp ("BN");
  BD. disp ("BD");
#endif

  return synth_ladder (1/s);
}

// Computes transfer function

void  qf_bessel::scpt (qf_double_t& s) {

  // Computes E
  E = poly_bessel (Pspec -> ord);

#ifdef _QF_BESSEL_DEBUG
  E. disp ("E");
#endif
  return;
}
