/***************************************************************************
                               qf_cauer.h
                             --------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef  _QF_CAUER_H
#define  _QF_CAUER_H

class qf_cauer: public qf_filter {

 protected:
 // General variables

  unsigned	      m;
  qf_double_t	      Ws;
  qf_double_t	      rho;
  Rvector *	      zr{nullptr};

		      qf_cauer	    (qf_spec*);

  bool		      order	    (void);
  bool		      iorder	    (void);

  void		      scpt_odd      (void);
  void		      scpt_b	    (void);
  void		      scpt_c	   (void);

  bool		      synth_even    (void);
  bool		      synth_odd	    (void);

  bool		      do_synth	    (void);
  bool		      do_synth_even (void);
  bool		      do_synth_odd  (void);

  // Optimization functions and variables
  vector <unsigned>   pole, best;
  vector <bool>	      poleb;
  qf_double_t	      opt_best;

  qf_double_t	      check_optim   (void);
  void		      pole_idx_std  (unsigned);
  void		      pole_idx_init (unsigned);
  bool		      pole_idx_next (unsigned, unsigned, bool);
  void		      cauer_optimize(void);

 public :
  virtual	      ~qf_cauer	    (void) {delete zr;};
  bool		      synth	    (void);
  friend qf_filter*   cauer	    (qf_spec* Pspec)
				    {return new qf_cauer (Pspec);}
};

qf_filter*	      cauer	    (qf_spec*);

  # ifdef _QF_API
struct qf_filter_api qf_cauer_api =
{
	"Cauer",
	QF_CAUER,

	NULL,
	cauer,

	false,
	true,
	false,
	"\004",
	CAN_ORDER |
	CAN_ALL_ORDERS | CAN_BANDPASS | CAN_OPTIMIZE | CAN_POLE,
	CAN_STOPBAND | CAN_ATTENUATION | CAN_IMPEDANCE2 | CAN_RIPPLE,
	CAN_SUBORDER | CAN_IMPEDANCE2 | CAN_ANGLE | CAN_RIPPLE,
	CAN_RIPPLE | CAN_ANGLE,
	QF_NONETF
};

  # endif // _QF_API
# endif	  // _QF_CAUER_H
