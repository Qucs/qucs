// Bessel filters

#ifndef	_QF_BESSEL_H
#define	_QF_BESSEL_H

class qf_bessel: public qf_filter {

  private:

  qf_double_t	      s;

		      qf_bessel	  (qf_spec*);
  inline	      ~qf_bessel  (void)	  {}

  qf_poly	      poly_bessel (unsigned);
  void		      scpt	  (qf_double_t &);

  public:

  bool		      synth	  (void);
  friend qf_filter*   bessel	  (qf_spec* Pspec) 
				    {return new qf_bessel (Pspec);}
};

// Global interface
qf_filter*	      bessel	  (qf_spec*);

  # ifdef _QF_API
struct qf_filter_api qf_bessel_api = 
{
  "Bessel",
  QF_BESL,

  NULL,
  bessel,

  true,
  false,
  false,
  NULL,
  CAN_ORDER | CAN_ORDER_ONLY | CAN_ALL_ORDERS,
  0,
  0,
  0,
  // Forbids BANDPASS and BANDSTOP and ZIGZAG
  QF_NONETF | QF_BANDPASS | QF_BANDSTOP | QF_ZIGZAG	
}; 

  # endif // _QF_API
# endif	  // _QF_BESSEL_H
