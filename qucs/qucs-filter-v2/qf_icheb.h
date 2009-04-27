#ifndef	_QF_ICHEB_H
#define	_QF_ICHEB_H
// Icheb : inverse chebychev filters

class qf_icheb : public qf_filter {
  private:
  Rvector	    zr;

		    qf_icheb  (qf_spec*);
  inline	    ~qf_icheb (void) {};

  unsigned	    order     (qf_double_t, qf_double_t);
  void		    scpt      (qf_double_t);
  bool		    synth     (void);

  friend qf_filter* icheb     (qf_spec* Pspec) {return new qf_icheb (Pspec);}
};

qf_filter*	    icheb     (qf_spec*);

  # ifdef _QF_API

struct	qf_filter_api	qf_icheb_api = {

	"Cheby 2",
	QF_ICHEB,

	NULL,
	icheb,

	false,
	false,
	false,
	NULL,
	CAN_ORDER |
	CAN_ORDER_ODD |
	CAN_STOPBAND | CAN_RIPPLE | CAN_BANDPASS,
	CAN_ATTENUATION,
	0,
	0,
	QF_NONETF | QF_ZIGZAG	
};

  # endif // _QF_API
#endif // _QF_ICHEB_H
