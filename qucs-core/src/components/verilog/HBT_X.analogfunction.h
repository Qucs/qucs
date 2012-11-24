/*
 * HBT_X.analogfunction.h - analog function header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __HBT_X_ANALOGFUNCTION_H__
#define __HBT_X_ANALOGFUNCTION_H__


/*
 * analog function: exp_soft
 */
double HBT_X_exp_soft (
  double x
  );
double HBT_X_d_exp_soft (
  double x,
  double d_x
  );

/*
 * analog function: Vt
 */
double HBT_X_Vt (
  double U,
  double Ud
  );
double HBT_X_d_Vt (
  double U,
  double Ud,
  double d_U,
  double d_Ud
  );

/*
 * analog function: diode
 */
double HBT_X_diode (
  double U,
  double Is,
  double Ug,
  double N,
  double AREA,
  double TJ,
  double TNOM
  );
double HBT_X_d_diode (
  double U,
  double Is,
  double Ug,
  double N,
  double AREA,
  double TJ,
  double TNOM,
  double d_U,
  double d_Is,
  double d_Ug,
  double d_N,
  double d_AREA,
  double d_TJ,
  double d_TNOM
  );

/*
 * analog function: MM
 */
double HBT_X_MM (
  double VBCI,
  double VCBO,
  double MC,
  double VCBLIN,
  double BF,
  double KC
  );
double HBT_X_d_MM (
  double VBCI,
  double VCBO,
  double MC,
  double VCBLIN,
  double BF,
  double KC,
  double d_VBCI,
  double d_VCBO,
  double d_MC,
  double d_VCBLIN,
  double d_BF,
  double d_KC
  );

/*
 * analog function: charge
 */
double HBT_X_charge (
  double U,
  double C0,
  double Ud,
  double m,
  double Area
  );
double HBT_X_d_charge (
  double U,
  double C0,
  double Ud,
  double m,
  double Area,
  double d_U,
  double d_C0,
  double d_Ud,
  double d_m,
  double d_Area
  );

/*
 * analog function: Vceff
 */
double HBT_X_Vceff (
  double U,
  double VCES
  );
double HBT_X_d_Vceff (
  double U,
  double VCES,
  double d_U,
  double d_VCES
  );

/*
 * analog function: ICK
 */
double HBT_X_ICK (
  double U,
  double RCI0,
  double VLIM,
  double InvVPT,
  double VCES
  );
double HBT_X_d_ICK (
  double U,
  double RCI0,
  double VLIM,
  double InvVPT,
  double VCES,
  double d_U,
  double d_RCI0,
  double d_VLIM,
  double d_InvVPT,
  double d_VCES
  );

#endif /* __HBT_X_ANALOGFUNCTION_H__ */
