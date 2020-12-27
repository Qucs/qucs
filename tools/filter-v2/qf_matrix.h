#ifndef _QF_MATRIX_H
  #define _QF_MATRIX_H

#ifdef	_QF_MATRIX_DEBUG
  #define IL
#else
  #define IL  inline
#endif

class qf_matrix {

  protected:

    unsigned  n;
    Rarray*   H;

    // Private functions used by operator *

    IL slice row	      (unsigned r) {return slice (r * n, n, 1);}
    IL slice col	      (unsigned c) {return slice (c, n, n);}
    IL slice subdiag	      (void) {return slice (n, n - 1, n + 1);}

    // There is no way to copy a matrix

    qf_matrix&	   operator = (const qf_matrix&) {return (*this);}
    qf_matrix		      (const qf_matrix& M) : n (M.n), H (M.H) {}

  public:

    // Please note that we follow the traditional mathematical convention,
    // so that the first element, on the uppermost left slot is index (1, 1)
    // and not (0, 0) as C/C++ goes.

		  qf_matrix   (unsigned d): n(d), H (NULL) 
			      {H = new Rarray (0.0, n * n);}
    virtual	  ~qf_matrix  ()  {delete H;}


    // Access operator
IL  qf_double_t&  operator () (unsigned r, unsigned c) 
				     {r --; c--; return (*H)[r * n + c];}

    qf_matrix&	  operator =  (qf_matrix& M) {n = M.n; H = new Rarray (n * n);
					      H = M.H; return (*this);}

    friend Rarray operator *  (qf_matrix&, Rarray&);
    friend Rarray operator *  (Rarray&, qf_matrix&);
    gslice	  submx	      (unsigned, unsigned, unsigned, unsigned);

    Rarray	  bVtVH	      (Rarray&, gslice&);
    Rarray	  bHVtV	      (Rarray&, gslice&);

    void	  disp	      (unsigned, unsigned);
    void	  disp	      (char, Rarray&);
};

#endif
