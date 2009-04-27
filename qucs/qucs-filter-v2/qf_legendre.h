// Headers for legendre filters
#ifndef _QF_LGNDR_H
#define	_QF_LGNDR_H

class qf_lgndr : public qf_filter {
  private:

    qf_double_t	    ws;			// Normalisation factor

		    qf_lgndr  (qf_spec*);

    qf_poly	    legendre  (void);	// Computes Legendre's polynom
    bool	    synth     (void);

  friend qf_filter* lgndr     (qf_spec* Pspec) {return new qf_lgndr (Pspec);}
};

qf_filter*	    lgndr     (qf_spec*);

  # ifdef _QF_API
struct	qf_filter_api	    qf_lgndr_api = {

	"Legendre",
	QF_LGNDR,

	NULL,
	lgndr,

	true,
	false,
	false,
	NULL,

	CAN_ORDER | CAN_ORDER_ONLY |
	CAN_ALL_ORDERS | CAN_BANDPASS,
	0,
	0,
	0,
	QF_NONETF | QF_BANDSTOP | QF_ZIGZAG
};

  # endif // _QF_API
# endif	  // _QF_LGNDR_H
