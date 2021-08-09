// Minimal matrix implementation to carry out Hessenberg QR factorization

#undef	_QF_MATRIX_DEBUG

#include "qf_common.h"
#include "qf_matrix.h"

// Matrix by vector (BLAS Level 2) multiplications

Rarray   operator * (qf_matrix& M, Rarray& V) {

  assert (V.size () == M.n); 
  Rarray W (M.n);

  for (unsigned i = 0; i < M.n; i++) {
    Rarray row = (* M.H)[slice (M.n * i, M.n, 1)];
    W[i] = (row * V).sum ();
  }

  return W;
}

// Transpose form
Rarray	operator * (Rarray& V, qf_matrix& M) {

  assert (V.size () == M.n);
  Rarray W (M.n);

  for (unsigned i = 0; i < M.n; i ++) {
    Rarray col = (* M.H)[slice (i, M.n, M.n)];
    W[i] = (col * V).sum ();
  }

  return W;
}

// This function generates a generalized slice corresponding to a the
// submatrix H[r:k, c:l] (that is of size r-k+1 x c-l+1)

gslice	qf_matrix::submx (unsigned r, unsigned k, unsigned c, unsigned l) {

  valarray <size_t>  len (2), stride (2);

  r --; c--;
  k -= r;
  l -= c;

  size_t offset = r * n + c;

  len[0] = k;
  len[1] = l;
  stride[0] = n;
  stride[1] = 1;

  return gslice (offset, len, stride);
}

// Forms the complex product K = b * H [i:j,k:l] * V * Vt

Rarray qf_matrix::bHVtV (Rarray& V, gslice& gs) {

  unsigned  r = (gs.size ())[0];
  unsigned  s = (gs.size ())[1];

  assert (V.size () == s);

  // Shortcut if b == 0
  if (V[0] == 0)
    return Rarray (0.0, r * s);

  // Form submatrix into independent matrix
  Rarray M (r * s), R (r * s);
  M = (*H)[gs];

  // Multiply H[i:j,k:l] by V: W = HV

  Rarray W (r);
  for (unsigned i = 0; i < r; i ++) {
    Rarray VR (V[slice (1, s - 1, 1)]);
    W[i] = M[i * s];			// Since V(0) = 1
    VR *= M[slice (i * s + 1, s - 1, 1)];
    W[i] += VR.sum ();
  }

  // And now form the inner product WVt

  R[slice (0, r, s)] = W * V[0];	// Since V(0) = 1 and V[0] = b
  for (unsigned i = 1; i < s; i ++)
    R[slice (i, r, s)] = W * (V[i] * V[0]);

  return R;
}

// Transposed form. Computes b * V * Vt * H[i:j,k:l]

Rarray qf_matrix::bVtVH (Rarray& V, gslice& gs) {

  unsigned  r = (gs. size ()) [0];
  unsigned  s = (gs. size ()) [1];

  assert (V. size () == r);

  // Shortcut if b == 0
  if (V[0] == 0)
    return Rarray (0.0, r * s);

  // Form submatrix into independent matrix
  Rarray M (r * s), R (r * s);
  M = (*H)[gs];

  // Multiply Vt by H[i:j,k:l] (tW = Vt * H[i:j,k:l])

  Rarray W (s);
  for (unsigned i = 0; i < s; i ++) {
    Rarray VR (V[slice (1, r - 1, 1)]);
    W[i] = M[i];			// Since V(0) = 1
    VR *= M[slice (i + s, r - 1, s)];
    W[i] += VR.sum ();
  }
  
  // Inner product V* Wt

  R[slice (0, s, 1)] = W * V[0];	// Since V(0) = 1 and V[0] = b
  for (unsigned i = 1; i < r; i ++)
    R[slice (s * i, s, 1)] = W * (V[i] * V[0]);

  return R;
}

// Basic display
void	      qf_matrix::disp (unsigned m, unsigned n) {

  cout << setprecision (4);

  cout << endl;
  for (unsigned i = 1; i <= m; i ++) {
    for (unsigned j = 1; j <= n; j ++)
      cout << setw (12) << left << (*this) (i, j);
    cout << endl;
  }
  cout << endl;
}

// Vector display with name
void	    qf_matrix::disp (char id, Rarray& V) {

  cout << setprecision (4) << id << " = [";

  for (unsigned i = 0; i < V.size () - 1; i ++)
    cout << setw (12) << left << V[i];

  cout << V[V. size () - 1] << "]" << endl;
}
