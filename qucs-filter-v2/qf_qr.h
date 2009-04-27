// A header file for QR algorithm on Hessenberg matrices
// A Hessenberg matrix is a matrix whose elements are all 0, except those
// located above the subdiagonal included.

#ifndef	_QF_QR_H
#define	_QF_QR_H


class qf_qr : public qf_matrix {

  private:
  // Private function used to solve polynomials
    bool	  eig_f	      (unsigned&, qf_roots&, unsigned&);
    void	  house	      (Rarray&, Rarray&);
    void	  dstep	      (unsigned, unsigned, bool);

    void	  cmpgn	      (qf_coeff&);
    void	  balance     (void);
    void	  diagtrfm    (Rarray&);

  public:
		  qf_qr	      (unsigned d) : qf_matrix (d) {}
		  ~qf_qr      (void) {}

  bool		  qr	      (qf_coeff&, qf_roots&);

};
# endif
