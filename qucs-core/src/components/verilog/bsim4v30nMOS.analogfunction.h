/*
 * bsim4v30nMOS.analogfunction.h - analog function header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __bsim4v30nMOS_ANALOGFUNCTION_H__
#define __bsim4v30nMOS_ANALOGFUNCTION_H__


/*
 * analog function: get_nuintd
 */
double bsim4v30nMOS_get_nuintd (
  double nf,
  double minsd
  );
double bsim4v30nMOS_d_get_nuintd (
  double nf,
  double minsd,
  double d_nf,
  double d_minsd
  );

/*
 * analog function: get_nuendd
 */
double bsim4v30nMOS_get_nuendd (
  double nf,
  double minsd
  );
double bsim4v30nMOS_d_get_nuendd (
  double nf,
  double minsd,
  double d_nf,
  double d_minsd
  );

/*
 * analog function: get_nuints
 */
double bsim4v30nMOS_get_nuints (
  double nf,
  double minsd
  );
double bsim4v30nMOS_d_get_nuints (
  double nf,
  double minsd,
  double d_nf,
  double d_minsd
  );

/*
 * analog function: get_nuends
 */
double bsim4v30nMOS_get_nuends (
  double nf,
  double minsd
  );
double bsim4v30nMOS_d_get_nuends (
  double nf,
  double minsd,
  double d_nf,
  double d_minsd
  );

/*
 * analog function: get_ps
 */
double bsim4v30nMOS_get_ps (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg
  );
double bsim4v30nMOS_d_get_ps (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg,
  double d_nf,
  double d_geo,
  double d_minsd,
  double d_weffcj,
  double d_dmcg,
  double d_dmci,
  double d_dmdg
  );

/*
 * analog function: get_pd
 */
double bsim4v30nMOS_get_pd (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg
  );
double bsim4v30nMOS_d_get_pd (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg,
  double d_nf,
  double d_geo,
  double d_minsd,
  double d_weffcj,
  double d_dmcg,
  double d_dmci,
  double d_dmdg
  );

/*
 * analog function: get_as
 */
double bsim4v30nMOS_get_as (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg
  );
double bsim4v30nMOS_d_get_as (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg,
  double d_nf,
  double d_geo,
  double d_minsd,
  double d_weffcj,
  double d_dmcg,
  double d_dmci,
  double d_dmdg
  );

/*
 * analog function: get_ad
 */
double bsim4v30nMOS_get_ad (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg
  );
double bsim4v30nMOS_d_get_ad (
  double nf,
  double geo,
  double minsd,
  double weffcj,
  double dmcg,
  double dmci,
  double dmdg,
  double d_nf,
  double d_geo,
  double d_minsd,
  double d_weffcj,
  double d_dmcg,
  double d_dmci,
  double d_dmdg
  );

/*
 * analog function: get_rendi
 */
double bsim4v30nMOS_get_rendi (
  double weffcj,
  double rsh,
  double dmcg,
  double dmci,
  double dmdg,
  double nuend,
  double rgeo,
  double type
  );
double bsim4v30nMOS_d_get_rendi (
  double weffcj,
  double rsh,
  double dmcg,
  double dmci,
  double dmdg,
  double nuend,
  double rgeo,
  double type,
  double d_weffcj,
  double d_rsh,
  double d_dmcg,
  double d_dmci,
  double d_dmdg,
  double d_nuend,
  double d_rgeo,
  double d_type
  );

/*
 * analog function: get_renda
 */
double bsim4v30nMOS_get_renda (
  double weffcj,
  double rsh,
  double dmcg,
  double dmci,
  double dmdg,
  double nuend,
  double rgeo,
  double type
  );
double bsim4v30nMOS_d_get_renda (
  double weffcj,
  double rsh,
  double dmcg,
  double dmci,
  double dmdg,
  double nuend,
  double rgeo,
  double type,
  double d_weffcj,
  double d_rsh,
  double d_dmcg,
  double d_dmci,
  double d_dmdg,
  double d_nuend,
  double d_rgeo,
  double d_type
  );

/*
 * analog function: get_rtot
 */
double bsim4v30nMOS_get_rtot (
  double nf,
  double geo,
  double rgeo,
  double minsd,
  double weffcj,
  double rsh,
  double dmcg,
  double dmci,
  double dmdg,
  double type
  );
double bsim4v30nMOS_d_get_rtot (
  double nf,
  double geo,
  double rgeo,
  double minsd,
  double weffcj,
  double rsh,
  double dmcg,
  double dmci,
  double dmdg,
  double type,
  double d_nf,
  double d_geo,
  double d_rgeo,
  double d_minsd,
  double d_weffcj,
  double d_rsh,
  double d_dmcg,
  double d_dmci,
  double d_dmdg,
  double d_type
  );

/*
 * analog function: get_vjm
 */
double bsim4v30nMOS_get_vjm (
  double nvtm,
  double ijth,
  double isb,
  double xexpbv
  );
double bsim4v30nMOS_d_get_vjm (
  double nvtm,
  double ijth,
  double isb,
  double xexpbv,
  double d_nvtm,
  double d_ijth,
  double d_isb,
  double d_xexpbv
  );

#endif /* __bsim4v30nMOS_ANALOGFUNCTION_H__ */
