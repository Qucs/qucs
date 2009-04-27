// Work on Hessenberg matrices to find roots polynom

// Ref: Gene H. Golub, Charles F. Van Loan: Matrix Computations
// Third edition, 1996
// With portions borrowed and adapted from Numerical recipes

#include <ctime>
#include "qf_common.h"
#include "qf_matrix.h"
#include "qf_qr.h"

#undef	  _QF_QR_DEBUG
#undef	  _QF_QR_DUMP

// From a vector 'd' form the (virtual) matrix D = diag (d) and
// computes DH/D

void  qf_qr::diagtrfm (Rarray& d) {
  
  for (unsigned i = 0; i < n; i++) {

// Apparently, LIBSTDC++ does not know about slice_array operator *= cste
    Rarray D (d[i], n);
    (*H)[row (i)] *= D;
    (*H)[col (i)] /= D;
  }
}

// Populate compagnon matrix
void  qf_qr::cmpgn (qf_coeff& cf) {

  unsigned s = cf.size ();
  qf_coeff frow = cf[slice (s - 1, s, -1)]; // Inverts cf

  (*H)[row (0)] = (-frow.shift (1) / frow[0])[slice (0, s - 1, 1)];
  (*H)[subdiag ()] = 1.0;
#ifdef _QF_QR_DUMP
  disp (n, n);
#endif

  balance ();
#ifdef	_QF_QR_DEBUG
  disp (n, n);
#endif
}

// Balance matrix according to the method in [1]
void  qf_qr::balance (void) {

  unsigned	    iteration = 10;
  const	qf_double_t thr = 1e-8;

  while (iteration--) {
    Rarray Rdm (n);
    srand (static_cast <unsigned long> (time (NULL)));

    // Form a vector of random +/- 1
    for (unsigned i = 0; i < n; i++)
      Rdm[i] = rand () < (RAND_MAX / 2) ? 1.0 : -1.0;

    Rarray p (((*H) * Rdm).apply (fabs));
    Rarray r ((Rdm * (*H)).apply (fabs));

    Rarray d (n);

    for (unsigned i = 0; i < n; i++)
      d[i] = ((p[i] < thr) || (r[i] < thr)) ? 
		  1 : sqrt (fabs (r[i] / p[i]));

    diagtrfm (d);
  }
}

// This function examines the matrix to find if we have found roots
bool  qf_qr::eig_f (unsigned &r, qf_roots & rt, unsigned& it) {

  const qf_double_t ngl = numeric_limits<qf_double_t>::epsilon () * 10;

  if (r == 0) return false;

#ifdef _QF_QR_DEBUG
    cout << "eig_f (" << r << ") " << endl;
#endif

    if (r == 1) {
#ifdef _QF_QR_DUMP
    cout << "Last real root: " << (*this) (1, 1) << endl;
#endif
    // We have found a single root
    rt.push_back (Cplx ((*this) (1, 1), 0));
    r = 0;
    it = 1;
    return true;
  }

  // Test if H(n, n) is an eigenvector
  qf_double_t d = (*this) (r, r);
  qf_double_t off = fabs ((*this) (r, r - 1));
  qf_double_t a = (*this) (r - 1, r - 1);

  if (((off <= fabs (a * ngl)) && (off <= fabs (d * ngl))) ||
      (r == 1)) {
  
#ifdef _QF_QR_DUMP
    cout << "Found one real root: " << d << endl;
#endif
    // We have found a single root
    rt.push_back (Cplx (d, 0));
    r --;
    it = 1;
    return true;
  }

  // Then we maybe got a 2x2 block diagonal submatrix?
  if (r > 2)
    off = fabs ((*this) (r - 1, r - 2));

  if ((r == 2) || ((off <= fabs (a * ngl)) && 
      (off <= fabs ((*this) (r - 2, r - 2) * ngl)))) {
  
    // We have found two roots
    // Calculate the eignevectors of the 2x2 block diagonal submetrix
    qf_double_t b = (*this) (r - 1, r);
    it = 1;

    if (fabs (b) <= ngl * (fabs (d) + fabs (a))
        || ((fabs (d) + fabs (a) == 0) && (fabs (b) <= ngl)))
	b = 0;

    qf_double_t	c = (*this) (r, r - 1);
		a /= 2.0;
		d /= 2.0;

    qf_double_t dlt = (a - d);
    dlt = dlt * dlt + b * c;

    if (dlt == 0) { 
      // Two identical real roots
#ifdef _QF_QR_DUMP
    cout << "Found a double real root: " << a + d << endl;
#endif
      rt.push_back (Cplx (a + d, 0));
      rt.push_back (Cplx (a + d, 0));
      r -= 2;
      return true;
    }

    // Round up real part if negligeable
    if (fabs(a + d) < ngl * fabs (dlt)) {a = 0; d = 0;}

    if (dlt > 0) {
      // Two real roots
      dlt = sqrt (dlt);
#ifdef _QF_QR_DUMP
    cout << "Found two real roots: " << a + d + dlt << ", " << 
	 a + d - dlt << endl;
#endif
      rt.push_back (Cplx (a + d + dlt, 0));
      rt.push_back (Cplx (a + d - dlt, 0));
      r -= 2;
      return true;
    }

    // Two complex conjugate roots (first imag (z) > 0, then imag (z) < 0)
    dlt = sqrt (-dlt);
    Cplx z (a + d, dlt);
    rt.push_back (z);
    rt.push_back (conj (z));
#ifdef _QF_QR_DUMP
    cout << "Found two complex roots: " << z << ", " << conj (z) << endl;
#endif
    r -= 2;
    return true;
  }

  return false;
}

// Return a Householder vector
// V = house (X)
// V is normalized such that V = (1, V[1], ... ,V[r - 1]) 
// in V[0] we store the real b such that I - b*V*Vt is orthogonal

void  qf_qr::house (Rarray& V, Rarray& X) {

  size_t      r = X.size ();
  Rarray      S (X[slice (1, r - 1, 1)]);
  qf_double_t s = (S * S).sum ();
  qf_double_t bt = 0;
  qf_double_t w;

  V = X;

#ifdef _QF_QR_DEBUG
  cout << "s = " << s << endl;
#endif

  if (s == 0)  {
    V[0] = 0;
    return ;
  }

  qf_double_t mu = sqrt (X[0] * X[0] + s);

  if (X[0] <= 0)
    w = X[0] - mu;

  else
    w = -s / (X[0] + mu);

#ifdef _QF_QR_DEBUG
  cout << "w = " << w << endl;
#endif

  bt = 2 * w * w / (s + w * w);
  V /= w;
  V[0] = bt;
  return ;
}

// Perform a Francis double QR step on H[i:j,i:j]

void  qf_qr::dstep (unsigned i, unsigned j, bool excep) {

  Rarray      V (3), X (3), Y (2), Z(2);

  qf_double_t hn1n2 = (*this) (j - 1, j - 2);
  qf_double_t hn1n1 = (*this) (j - 1, j - 1);
  qf_double_t hnn1 = (*this) (j, j - 1);
  qf_double_t hn1n = (*this) (j - 1, j);
  qf_double_t hnn = (*this) (j, j);
  qf_double_t h11 = (*this) (i, i);
  qf_double_t h21 = (*this) (i + 1, i);
  qf_double_t h12 = (*this) (i, i + 1);
  qf_double_t h22 = (*this) (i + 1, i + 1);
  qf_double_t h32 = (*this) (i + 2, i + 1);

  qf_double_t s, t;

  // See if we must use exceptional shifts
  if (excep) {
    qf_double_t r = fabs (hnn1) + fabs (hn1n2);
    s = 1.5 * (fabs (hnn1) + fabs (hn1n2));
    t = r * r;
  }
  else {
    s = hnn + hn1n1;
    t = hnn * hn1n1 - hnn1 * hn1n;
  }

  qf_double_t x = h11 * h11 + h12 * h21 - s * h11 + t;
  qf_double_t y = h21 * (h11 + h22 - s);
  qf_double_t z = h21 * h32;

  for (unsigned k = i - 1; k < j - 2; k ++) {
    X[0] = x;
    X[1] = y;
    X[2] = z;

#ifdef _QF_QR_DEBUG
    disp ('X', X);
#endif

    house (V, X);

#ifdef _QF_QR_DEBUG
    disp ('V', V);
#endif

    unsigned	q = max (i, k);
    unsigned	r = min (j, k + 4);

#ifdef	_QF_QR_DEBUG
    cout << "H1: (" << k + 1 << ":" << k + 3 << ", "
	 << q << ":" << j << ")" << endl;
#endif

    gslice	gs = submx (k + 1, k + 3, q, j);
    Rarray	H1 (bVtVH (V, gs));
    (*H)[gs] -= H1;


#ifdef _QF_QR_DEBUG
    disp (n, n);
    cout << "H2: (" << i << ":" << r << ", "
	 << k + 1 << ":" << k + 3 << ")" << endl;
#endif

    gs = submx (i, r, k + 1, k + 3);
    Rarray	H2 (bHVtV (V, gs));
    (*H)[gs] -= H2;

#ifdef _QF_QR_DEBUG
    disp (n, n);
#endif

    x = (*this) (k + 2, k + 1);
    y = (*this) (k + 3, k + 1);

    if (k < j - 3) z = (*this) (k + 4, k + 1);
  }

  Y[0] = x;
  Y[1] = y;

#ifdef _QF_QR_DEBUG
  disp ('Y', Y);
#endif

  house (Z, Y);

#ifdef _QF_QR_DEBUG
  disp ('Z', Z);
#endif


#ifdef	_QF_QR_DEBUG
    cout << "H3: (" << j - 1 << ":" << j << ", "
	 << j - 2 << ":" << j << ")" << endl;
#endif

  gslice gs = submx (j - 1, j, j - 2, j);
  Rarray H3 (bVtVH (Z, gs));
  (*H)[gs] -= H3;


#ifdef	_QF_QR_DEBUG
    disp (n, n);
    cout << "H4: (" << 1 << ":" << j << ", "
	 << j - 1 << ":" << j << ")" << endl;
#endif

  gs = submx (1, j, j - 1, j);
  Rarray      H4 (bHVtV (Z, gs));
  (*H)[gs] -= H4;

#ifdef	_QF_QR_DEBUG
    disp (n, n);
#endif
}

// Main routine

bool  qf_qr::qr (qf_coeff& cf, qf_roots& roots) {

  unsigned	    r = n;
  unsigned	    it = 1, iter = 1;
  unsigned	    i;
  // Tolerance of zero
  const qf_double_t ngl = numeric_limits<qf_double_t>::epsilon () * 10.0;

  // Build compagnon matrix
  cmpgn (cf);

  while (r) {

#ifdef _QF_QR_DUMP
    cout << endl << "---- Iteration #" << iter << endl;
#endif

    // Find the largest unreduced Hessenberg block
    for (i = r; i >= 2; i --)
      if ((*this) (i, i - 1) == 0) break;

    if ((i == 2) && ((*this) (1, 1) != 0)) i = 1;

    // Do a double step QR

#ifdef	_QF_QR_DUMP
    cout << "dstep (" << i << ", " << r << ")" << endl;
#endif

    if (it == 30) {
      cout << "qf_qr::qr () convergence failed." << endl;
      return false;
    }

    // Do we need to apply an exceptional shift?
    if (it % 10)
      dstep (i, r, false);
    else 
      dstep (i, r, true);

    // Zeroes the "negligeable" subdiagonal elements
    for (i = 2; i <= r; i ++) {
      qf_double_t hii = fabs ((*this) (i, i));
      qf_double_t hiim1 = fabs ((*this) (i, i - 1));
      qf_double_t him1im1 = fabs ((*this) (i - 1, i - 1));

      if ((hii + him1im1 == 0) && (hiim1 <= ngl))
	(*this) (i, i - 1) = 0;

      else
	if (hiim1 <= ngl * (hii + him1im1))
	  (*this) (i, i - 1) = 0;
    }

#ifdef	_QF_QR_DUMP
    disp (r, r);
#endif

    // Increase iteration number
    it ++; iter ++;

    // Deflates and store eigenvalues, exits at end
    while (eig_f (r, roots, it)) ;
  }

  // Okay, fine
  return true;
}

