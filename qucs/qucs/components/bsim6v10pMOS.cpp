/*
 * bsim6v10pMOS.cpp - device implementations for bsim6v10pMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "bsim6v10pMOS.h"

bsim6v10pMOS::bsim6v10pMOS()
{
  Description = QObject::tr ("bsim6v10pMOS verilog device");

  Props.append (new Property ("CVMOD", "0", false,
    QObject::tr ("0: Consistent IV-CV\n1: Different IV-CV")));
  Props.append (new Property ("GEOMOD", "0", false,
    QObject::tr ("Geo dependent parasitics model")));
  Props.append (new Property ("RGEOMOD", "0", false,
    QObject::tr ("Geometry-dependent source/drain resistance\n0: RSH-based, 1: Holistic")));
  Props.append (new Property ("COVMOD", "0", false,
    QObject::tr ("0: Use Bias-independent Overlap Capacitances\n1: Use Bias-dependent Overlap Capacitances")));
  Props.append (new Property ("RDSMOD", "0", false,
    QObject::tr ("0: Internal bias dependent and external bias independent s/d resistance model\n1: External s/d resistance model\n2: Internal s/d resistance model")));
  Props.append (new Property ("GIDLMOD", "0", false,
    QObject::tr ("0: Turn off GIDL Current\n1: Turn on GIDL Current")));
  Props.append (new Property ("RGATEMOD", "0", false,
    QObject::tr ("Gate resistance model selector")));
  Props.append (new Property ("RBODYMOD", "0", false,
    QObject::tr ("Distributed body R model")));
  Props.append (new Property ("IGCMOD", "0", false,
    QObject::tr ("0: Turn off Igc, Igs and Igd\n1: Turn on Igc, Igs and Igd")));
  Props.append (new Property ("IGBMOD", "0", false,
    QObject::tr ("0: Turn off Igb\n1: Turn on Igb")));
  Props.append (new Property ("TNOIMOD", "0", false,
    QObject::tr ("Thermal noise model selector")));
  Props.append (new Property ("SHMOD", "0", false,
    QObject::tr ("0 : Self heating model OFF\n1 : Self heating model ON")));
  Props.append (new Property ("L", "10u", false,
    QObject::tr ("units are m")));
  Props.append (new Property ("LLONG", "10u", false,
    QObject::tr ("L of extracted Long channel device - units are m")));
  Props.append (new Property ("LMLT", "1.0", false,
    QObject::tr ("Length Shrinking Parameter")));
  Props.append (new Property ("WMLT", "1.0", false,
    QObject::tr ("Width Shrinking Parameter")));
  Props.append (new Property ("XL", "0.0", false,
    QObject::tr ("L offset for channel length due to mask/etch effect")));
  Props.append (new Property ("W", "10u", false,
    QObject::tr ("Total Width including fingers (units are m)")));
  Props.append (new Property ("WWIDE", "10u", false,
    QObject::tr ("W of extracted Wide channel device - units are m")));
  Props.append (new Property ("XW", "0.0", false,
    QObject::tr ("W offset for channel width due to mask/etch effect")));
  Props.append (new Property ("LINT", "0.0", false,
    QObject::tr ("delta L for IV")));
  Props.append (new Property ("LL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WINT", "0.0", false,
    QObject::tr ("delta W for IV")));
  Props.append (new Property ("WL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DLC", "0.0", false,
    QObject::tr ("delta L for CV")));
  Props.append (new Property ("LLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWC", "0.0", false,
    QObject::tr ("delta W for CV")));
  Props.append (new Property ("WLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOXE", "3.0e-9", false,
    QObject::tr ("Effective gate dielectric thickness relative to SiO2, m")));
  Props.append (new Property ("TOXP", "3.0e-9", false,
    QObject::tr ("Physical gate dielectric thickness, m\nIf not given, TOXP is calculated from TOXE and DTOX")));
  Props.append (new Property ("DTOX", "0.0", false,
    QObject::tr ("Difference between effective dielectric thickness\nand physical thickness")));
  Props.append (new Property ("NF", "1", false,
    QObject::tr ("number of fingers")));
  Props.append (new Property ("NDEP", "1e24", false,
    QObject::tr ("Channel Doping Concentration (m^-3) for IV")));
  Props.append (new Property ("NDEPL1", "0.0", false,
    QObject::tr ("Length dependence coefficient of NDEP")));
  Props.append (new Property ("NDEPLEXP1", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of NDEP")));
  Props.append (new Property ("NDEPL2", "0.0", false,
    QObject::tr ("Length dependence of NDEP - For Short Channel Devices")));
  Props.append (new Property ("NDEPLEXP2", "2.0", false,
    QObject::tr ("Length dependence exponent coefficient of NDEP")));
  Props.append (new Property ("NDEPW", "0.0", false,
    QObject::tr ("Width dependence coefficient of NDEP")));
  Props.append (new Property ("NDEPWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of NDEP")));
  Props.append (new Property ("NDEPWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of NDEP")));
  Props.append (new Property ("NDEPWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence exponent coefficient of NDEP")));
  Props.append (new Property ("LNDEP", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNDEP", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNDEP", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NDEPCV", "1e24", false,
    QObject::tr ("Channel Doping Concentration (m^-3) for CV")));
  Props.append (new Property ("NDEPCVL1", "0.0", false,
    QObject::tr ("Length dependence coefficient of NDEPCV")));
  Props.append (new Property ("NDEPCVLEXP1", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of NDEPCV")));
  Props.append (new Property ("NDEPCVL2", "0.0", false,
    QObject::tr ("Length dependence coefficient of NDEPCV - For Short Channel Devices")));
  Props.append (new Property ("NDEPCVLEXP2", "2.0", false,
    QObject::tr ("Length dependence exponent coefficient of NDEPCV")));
  Props.append (new Property ("NDEPCVW", "0.0", false,
    QObject::tr ("Width dependence coefficient of NDEPCV")));
  Props.append (new Property ("NDEPCVWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of NDEPCV")));
  Props.append (new Property ("NDEPCVWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of NDEPCV")));
  Props.append (new Property ("NDEPCVWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence exponent coefficient of NDEPCV")));
  Props.append (new Property ("LNDEPCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNDEPCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNDEPCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NGATE", "5e25", false,
    QObject::tr ("Gate Doping Concentration (m^-3)")));
  Props.append (new Property ("LNGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EASUB", "4.05", false,
    QObject::tr ("Electron affinity of substrate, eV")));
  Props.append (new Property ("NI0SUB", "1.1e16", false,
    QObject::tr ("Intrinsic carrier concentration of the substrate at 300.15K ( m^-3)")));
  Props.append (new Property ("BG0SUB", "1.17", false,
    QObject::tr ("Band gap of substrate at 300.15K, eV")));
  Props.append (new Property ("EPSRSUB", "11.9", false,
    QObject::tr ("Relative dielectric constant of the channel material")));
  Props.append (new Property ("EPSROX", "3.9", false,
    QObject::tr ("Relative dielectric constant of the gate dielectric")));
  Props.append (new Property ("XJ", "1.5e-7", false,
    QObject::tr ("S/D junction depth")));
  Props.append (new Property ("LXJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WXJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PXJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFB", "-1.0", false,
    QObject::tr ("Flat band voltage for IV")));
  Props.append (new Property ("LVFB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVFB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVFB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFBCV", "-1.0", false,
    QObject::tr ("Flat band voltage for CV")));
  Props.append (new Property ("LVFBCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVFBCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVFBCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFBCVL", "0.0", false,
    QObject::tr ("Length dependence coefficient of VFBCV")));
  Props.append (new Property ("VFBCVLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of VFBCV")));
  Props.append (new Property ("VFBCVW", "0.0", false,
    QObject::tr ("Width dependence coefficient of VFBCV")));
  Props.append (new Property ("VFBCVWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of VFBCV")));
  Props.append (new Property ("VFBCVWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of VFBCV")));
  Props.append (new Property ("VFBCVWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence coefficient of VFBCV")));
  Props.append (new Property ("PERMOD", "1", false,
    QObject::tr ("Whether PS/PD (when given) include gate-edge perimeter")));
  Props.append (new Property ("AS", "0.0", false,
    QObject::tr ("Source to Substrate Junction Area")));
  Props.append (new Property ("AD", "0.0", false,
    QObject::tr ("Drain to Substrate Junction Area")));
  Props.append (new Property ("PS", "0.0", false,
    QObject::tr ("Source to Substrate Junction Perimeter")));
  Props.append (new Property ("PD", "0.0", false,
    QObject::tr ("Drain to Substrate Junction Perimeter")));
  Props.append (new Property ("DWJ", "0.0", false,
    QObject::tr ("delta W for S/D junctions")));
  Props.append (new Property ("VFBSDOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NRS", "1.0", false,
    QObject::tr ("Number of squares in source")));
  Props.append (new Property ("NRD", "1.0", false,
    QObject::tr ("Number of squares in drain")));
  Props.append (new Property ("MINZ", "0", false,
    QObject::tr ("Minimize either D or S")));
  Props.append (new Property ("NSD", "1e26", false,
    QObject::tr ("S/D Doping Concentration (m^-3)")));
  Props.append (new Property ("LNSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP0", "0.0", false,
    QObject::tr ("DITS")));
  Props.append (new Property ("LDVTP0", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVTP0", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVTP0", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP1", "0.0", false,
    QObject::tr ("DITS")));
  Props.append (new Property ("LDVTP1", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVTP1", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVTP1", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP2", "0.0", false,
    QObject::tr ("DITS")));
  Props.append (new Property ("LDVTP2", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVTP2", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVTP2", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP3", "0.0", false,
    QObject::tr ("DITS")));
  Props.append (new Property ("LDVTP3", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVTP3", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVTP3", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP4", "0.0", false,
    QObject::tr ("DITS")));
  Props.append (new Property ("LDVTP4", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVTP4", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVTP4", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP5", "0.0", false,
    QObject::tr ("DITS")));
  Props.append (new Property ("LDVTP5", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVTP5", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVTP5", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PHIN", "0.045", false,
    QObject::tr ("Nonuniform vertical doping effect on surface potential, V")));
  Props.append (new Property ("LPHIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPHIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPHIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ETA0", "0.08", false,
    QObject::tr ("DIBL coefficient")));
  Props.append (new Property ("LETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DSUB", "1.0", false,
    QObject::tr ("Length scaling exponent for DIBL")));
  Props.append (new Property ("ETAB", "-0.07", false,
    QObject::tr ("Body bias coefficient for subthreshold DIBL effect")));
  Props.append (new Property ("ETABEXP", "1.0", false,
    QObject::tr ("Exponent coefficient of ETAB")));
  Props.append (new Property ("LETAB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WETAB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PETAB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K2", "0.0", false,
    QObject::tr ("Vth shift due to Vertical Non-uniform doping")));
  Props.append (new Property ("K2L", "0.0", false,
    QObject::tr ("Length dependence coefficient of K2")));
  Props.append (new Property ("K2LEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of K2")));
  Props.append (new Property ("K2W", "0.0", false,
    QObject::tr ("Width dependence coefficient of K2")));
  Props.append (new Property ("K2WEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of K2")));
  Props.append (new Property ("K2WL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of K2")));
  Props.append (new Property ("K2WLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence exponent coefficient of K2")));
  Props.append (new Property ("LK2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WK2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PK2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ADOS", "0.0", false,
    QObject::tr ("Quantum mechanical effect prefactor cum switch in inversion")));
  Props.append (new Property ("BDOS", "1.0", false,
    QObject::tr ("Charge centroid parameter - slope of CV curve under QME in inversion")));
  Props.append (new Property ("QM0", "1.0e-3", false,
    QObject::tr ("Charge centroid parameter - starting point for QME in inversion")));
  Props.append (new Property ("ETAQM", "0.54", false,
    QObject::tr ("Bulk charge coefficient for charge centroid in inversion")));
  Props.append (new Property ("CIT", "0.0", false,
    QObject::tr ("parameter for interface trap")));
  Props.append (new Property ("LCIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NFACTOR", "0.0", false,
    QObject::tr ("Sub-threshold slope factor")));
  Props.append (new Property ("NFACTORL", "0.0", false,
    QObject::tr ("Length dependence coefficient of NFACTOR")));
  Props.append (new Property ("NFACTORLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of NFACTOR")));
  Props.append (new Property ("NFACTORW", "0.0", false,
    QObject::tr ("Width dependence coefficient of NFACTOR")));
  Props.append (new Property ("NFACTORWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of NFACTOR")));
  Props.append (new Property ("NFACTORWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of NFACTOR")));
  Props.append (new Property ("NFACTORWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence exponent coefficient of NFACTOR")));
  Props.append (new Property ("LNFACTOR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNFACTOR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNFACTOR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSCD", "1e-9", false,
    QObject::tr ("drain-bias sensitivity of sub-threshold slope")));
  Props.append (new Property ("CDSCDL", "0.0", false,
    QObject::tr ("Length dependence coefficient of CDSCD")));
  Props.append (new Property ("CDSCDLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of CDSCD")));
  Props.append (new Property ("LCDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSCB", "0.0", false,
    QObject::tr ("body-bias sensitivity of sub-threshold slope")));
  Props.append (new Property ("CDSCBL", "0.0", false,
    QObject::tr ("Length dependence coefficient of CDSCB")));
  Props.append (new Property ("CDSCBLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of CDSCB")));
  Props.append (new Property ("LCDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSAT", "1e5", false,
    QObject::tr ("Saturation Velocity m/s")));
  Props.append (new Property ("LVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATL", "0.0", false,
    QObject::tr ("Length dependence coefficient of of VSAT")));
  Props.append (new Property ("VSATLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of VSAT")));
  Props.append (new Property ("VSATW", "0.0", false,
    QObject::tr ("Width dependence coefficient of of VSAT")));
  Props.append (new Property ("VSATWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of of VSAT")));
  Props.append (new Property ("VSATWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of of VSAT")));
  Props.append (new Property ("VSATWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence exponent coefficient of of VSAT")));
  Props.append (new Property ("DELTA", "0.125", false,
    QObject::tr ("Smoothing function factor for Vdsat")));
  Props.append (new Property ("LDELTA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDELTA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDELTA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DELTAL", "0.0", false,
    QObject::tr ("Length dependence coefficient of DELTA")));
  Props.append (new Property ("DELTALEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of DELTA")));
  Props.append (new Property ("VSATCV", "1e5", false,
    QObject::tr ("VSAT parameter for CV")));
  Props.append (new Property ("LVSATCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVSATCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVSATCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATCVL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATCVLEXP", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATCVW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATCVWEXP", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATCVWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSATCVWLEXP", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("U0", "67.0e-3", false,
    QObject::tr ("units are m2/Vs")));
  Props.append (new Property ("U0L", "0.0", false,
    QObject::tr ("Length dependence coefficient of U0L")));
  Props.append (new Property ("U0LEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of U0L")));
  Props.append (new Property ("LU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ETAMOB", "1.0", false,
    QObject::tr ("Effective field parameter (should be kept close to 1)")));
  Props.append (new Property ("UA", "0.001", false,
    QObject::tr ("Mobility reduction coefficient")));
  Props.append (new Property ("UAL", "0.0", false,
    QObject::tr ("Length dependence coefficient of  UA")));
  Props.append (new Property ("UALEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of  UA")));
  Props.append (new Property ("UAW", "0.0", false,
    QObject::tr ("Width dependence coefficient of UA")));
  Props.append (new Property ("UAWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of UA")));
  Props.append (new Property ("UAWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of UA")));
  Props.append (new Property ("UAWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence coefficient of UA")));
  Props.append (new Property ("LUA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EU", "1.5", false,
    QObject::tr ("Mobility reduction exponent")));
  Props.append (new Property ("LEU", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WEU", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PEU", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EUL", "0.0", false,
    QObject::tr ("Length dependence coefficient of EU")));
  Props.append (new Property ("EULEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of EU")));
  Props.append (new Property ("EUW", "0.0", false,
    QObject::tr ("Width dependence coefficient of EU")));
  Props.append (new Property ("EUWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of EU")));
  Props.append (new Property ("EUWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of EU")));
  Props.append (new Property ("EUWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence coefficient of EU")));
  Props.append (new Property ("UD", "0.001", false,
    QObject::tr ("Coulombic scattering parameter")));
  Props.append (new Property ("UDL", "0.0", false,
    QObject::tr ("Length dependence coefficient of UD")));
  Props.append (new Property ("UDLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of UD")));
  Props.append (new Property ("LUD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UCS", "2.0", false,
    QObject::tr ("Coulombic scattering parameter")));
  Props.append (new Property ("LUCS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUCS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUCS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UC", "0.0", false,
    QObject::tr ("Mobility reduction with body bias")));
  Props.append (new Property ("UCL", "0.0", false,
    QObject::tr ("Length dependence coefficient of UC")));
  Props.append (new Property ("UCLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of UC")));
  Props.append (new Property ("UCW", "0.0", false,
    QObject::tr ("Width dependence coefficient of UC")));
  Props.append (new Property ("UCWEXP", "1.0", false,
    QObject::tr ("Width dependence exponent coefficient of UC")));
  Props.append (new Property ("UCWL", "0.0", false,
    QObject::tr ("Width-Length dependence coefficient of UC")));
  Props.append (new Property ("UCWLEXP", "1.0", false,
    QObject::tr ("Width-Length dependence exponent coefficient of UC")));
  Props.append (new Property ("LUC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLM", "0.0", false,
    QObject::tr ("CLM prefactor")));
  Props.append (new Property ("PCLML", "0.0", false,
    QObject::tr ("Length dependence coefficient of PCLM")));
  Props.append (new Property ("PCLMLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of PCLM")));
  Props.append (new Property ("LPCLM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPCLM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPCLM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLMG", "0.0", false,
    QObject::tr ("CLM prefactor gate voltage dependence")));
  Props.append (new Property ("PCLMCV", "0.0", false,
    QObject::tr ("CLM parameter for CV")));
  Props.append (new Property ("PCLMCVL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLMCVLEXP", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPCLMCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPCLMCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPCLMCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSCBE1", "4.24e8", false,
    QObject::tr ("Substrate current body-effect coeff")));
  Props.append (new Property ("LPSCBE1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPSCBE1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPSCBE1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSCBE2", "1.0e-8", false,
    QObject::tr ("Substrate current body-effect coeff")));
  Props.append (new Property ("LPSCBE2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPSCBE2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPSCBE2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDITS", "0.0", false,
    QObject::tr ("Coefficient for drain-induced Vth shifts")));
  Props.append (new Property ("LPDITS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDITS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDITS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDITSL", "0.0", false,
    QObject::tr ("L dep of drain-induced Vth shifts")));
  Props.append (new Property ("PDITSD", "0.0", false,
    QObject::tr ("Vds dep of drain-induced Vth shifts")));
  Props.append (new Property ("LPDITSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDITSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDITSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSH", "0.0", false,
    QObject::tr ("Source-drain sheet resistance [Ohm]")));
  Props.append (new Property ("PRWG", "1.0", false,
    QObject::tr ("gate bias dependence of S/D extension resistance, V-1")));
  Props.append (new Property ("LPRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRWB", "0.0", false,
    QObject::tr ("Body bias dependence of resistance")));
  Props.append (new Property ("LPRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRWBL", "0.0", false,
    QObject::tr ("Length dependence coefficient of PPRWB")));
  Props.append (new Property ("PRWBLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of PPRWB")));
  Props.append (new Property ("WR", "1.0", false,
    QObject::tr ("W dependence parameter of S/D extension resistance")));
  Props.append (new Property ("LWR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PWR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSWMIN", "0.0", false,
    QObject::tr ("Source Resistance per unit width at high Vgs (RDSMOD=1)")));
  Props.append (new Property ("LRSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSW", "10.0", false,
    QObject::tr ("zero bias Source Resistance (RDSMOD=1)")));
  Props.append (new Property ("LRSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSWL", "0.0", false,
    QObject::tr ("Geometrical scaling of RSW (RDSMOD=1)")));
  Props.append (new Property ("RSWLEXP", "1.0", false,
    QObject::tr ("Geometrical scaling of RSW (RDSMOD=1)")));
  Props.append (new Property ("RDWMIN", "0.0", false,
    QObject::tr ("Drain Resistance per unit width at high Vgs (RDSMOD=1)")));
  Props.append (new Property ("LRDWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRDWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRDWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDW", "10.0", false,
    QObject::tr ("zero bias Drain Resistance (RDSMOD=1)")));
  Props.append (new Property ("LRDW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRDW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRDW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDWL", "0.0", false,
    QObject::tr ("Geometrical scaling of RDW (RDSMOD=1)")));
  Props.append (new Property ("RDWLEXP", "1.0", false,
    QObject::tr ("Geometrical scaling of RDW (RDSMOD=1)")));
  Props.append (new Property ("RDSWMIN", "0.0", false,
    QObject::tr ("S/D Resistance per unit width at high Vgs (RDSMOD=0 and RDSMOD=2)")));
  Props.append (new Property ("LRDSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRDSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRDSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDSW", "20.0", false,
    QObject::tr ("zero bias Resistance (RDSMOD=0 and RDSMOD=2)")));
  Props.append (new Property ("RDSWL", "0.0", false,
    QObject::tr ("Geometrical scaling of RDSW (RDSMOD=0 and RDSMOD=2)")));
  Props.append (new Property ("RDSWLEXP", "1.0", false,
    QObject::tr ("Geometrical scaling of RDSW (RDSMOD=0 and RDSMOD=2)")));
  Props.append (new Property ("LRDSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRDSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRDSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSAT", "1.0", false,
    QObject::tr ("Gmsat variation with gate bias")));
  Props.append (new Property ("LPSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSATL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSATLEXP", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSATB", "0.0", false,
    QObject::tr ("Body bias effect on Idsat")));
  Props.append (new Property ("LPSATB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPSATB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPSATB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSATX", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PTWG", "0.0", false,
    QObject::tr ("Idsat variation with gate bias")));
  Props.append (new Property ("LPTWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPTWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPTWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PTWGL", "0.0", false,
    QObject::tr ("Length dependence coefficient of PTWG")));
  Props.append (new Property ("PTWGLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of PTWG")));
  Props.append (new Property ("PDIBLC", "0.0", false,
    QObject::tr ("parameter for DIBL effect on Rout")));
  Props.append (new Property ("PDIBLCL", "0.0", false,
    QObject::tr ("Length dependence coefficient of PDIBLC")));
  Props.append (new Property ("PDIBLCLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of PDIBLC")));
  Props.append (new Property ("LPDIBLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDIBLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDIBLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBLCB", "0.0", false,
    QObject::tr ("parameter for DIBL effect on Rout")));
  Props.append (new Property ("LPDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVAG", "1.0", false,
    QObject::tr ("Vg dependence of early voltage")));
  Props.append (new Property ("LPVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("FPROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("FPROUTL", "0.0", false,
    QObject::tr ("Length dependence coefficient of FPROUT")));
  Props.append (new Property ("FPROUTLEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of FPROUT")));
  Props.append (new Property ("LFPROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WFPROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PFPROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ALPHA0", "0.0", false,
    QObject::tr ("first parameter of Iii, m/V")));
  Props.append (new Property ("ALPHA0L", "0.0", false,
    QObject::tr ("Length dependence coefficient of ALPHA0")));
  Props.append (new Property ("ALPHA0LEXP", "1.0", false,
    QObject::tr ("Length dependence exponent coefficient of ALPHA0")));
  Props.append (new Property ("LALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BETA0", "0.0", false,
    QObject::tr ("Vds dependent parameter of Iii, 1/V")));
  Props.append (new Property ("LBETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AIGBACC", "1.36e-2", false,
    QObject::tr ("Parameter for Igb")));
  Props.append (new Property ("BIGBACC", "1.71e-3", false,
    QObject::tr ("Parameter for Igb")));
  Props.append (new Property ("CIGBACC", "0.075", false,
    QObject::tr ("Parameter for Igb")));
  Props.append (new Property ("NIGBACC", "1.0", false,
    QObject::tr ("Parameter for Igbacc slope")));
  Props.append (new Property ("AIGBINV", "1.11e-2", false,
    QObject::tr ("Parameter for Igb")));
  Props.append (new Property ("BIGBINV", "9.49e-4", false,
    QObject::tr ("Parameter for Igb")));
  Props.append (new Property ("CIGBINV", "0.006", false,
    QObject::tr ("Parameter for Igb")));
  Props.append (new Property ("EIGBINV", "1.1", false,
    QObject::tr ("Parm for the Si bandgap for Igbinv [V]")));
  Props.append (new Property ("NIGBINV", "3.0", false,
    QObject::tr ("Parameter for Igbinv slope")));
  Props.append (new Property ("AIGC", "9.8e-3", false,
    QObject::tr ("Parameter for Igc")));
  Props.append (new Property ("BIGC", "7.59e-4", false,
    QObject::tr ("Parameter for Igc")));
  Props.append (new Property ("CIGC", "0.03", false,
    QObject::tr ("Parameter for Igc")));
  Props.append (new Property ("AIGS", "9.8e-3", false,
    QObject::tr ("Parameter for Igs d")));
  Props.append (new Property ("BIGS", "7.59e-4", false,
    QObject::tr ("Parameter for Igs d")));
  Props.append (new Property ("CIGS", "0.03", false,
    QObject::tr ("Parameter for Igs d")));
  Props.append (new Property ("AIGD", "9.8e-3", false,
    QObject::tr ("Parameter for Igs d")));
  Props.append (new Property ("BIGD", "7.59e-4", false,
    QObject::tr ("Parameter for Igs d")));
  Props.append (new Property ("CIGD", "0.03", false,
    QObject::tr ("Parameter for Igs d")));
  Props.append (new Property ("DLCIG", "0.0", false,
    QObject::tr ("Delta L for Ig model [m]")));
  Props.append (new Property ("DLCIGD", "0.0", false,
    QObject::tr ("Delta L for Ig model [m]")));
  Props.append (new Property ("POXEDGE", "1.0", false,
    QObject::tr ("Factor for the gate edge Tox")));
  Props.append (new Property ("NTOX", "1.0", false,
    QObject::tr ("Exponent for Tox ratio")));
  Props.append (new Property ("TOXREF", "3.0e-9", false,
    QObject::tr ("Target tox value [m]")));
  Props.append (new Property ("PIGCD", "1.0", false,
    QObject::tr ("Igc, S/D partition parameter")));
  Props.append (new Property ("AIGCL", "0.0", false,
    QObject::tr ("Length dependence coefficient of AIGC")));
  Props.append (new Property ("AIGCW", "0.0", false,
    QObject::tr ("Width dependence coefficient of AIGC")));
  Props.append (new Property ("AIGSL", "0.0", false,
    QObject::tr ("Length dependence coefficient of AIGS")));
  Props.append (new Property ("AIGSW", "0.0", false,
    QObject::tr ("Width dependence coefficient of AIGS")));
  Props.append (new Property ("AIGDL", "0.0", false,
    QObject::tr ("Length dependence coefficient of AIGD")));
  Props.append (new Property ("AIGDW", "0.0", false,
    QObject::tr ("Width dependence coefficient of AIGD")));
  Props.append (new Property ("PIGCDL", "0.0", false,
    QObject::tr ("Length dependence coefficient of PIGCD")));
  Props.append (new Property ("LAIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LBIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LEIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WEIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PEIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNIGBINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LBIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNIGBACC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LBIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIGC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LBIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LBIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPOXEDGE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPOXEDGE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPOXEDGE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDLCIG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDLCIG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDLCIG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDLCIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDLCIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDLCIGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNTOX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNTOX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNTOX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AGIDL", "0.0", false,
    QObject::tr ("pre-exponential coeff. for GIDL in mho")));
  Props.append (new Property ("AGIDLL", "0.0", false,
    QObject::tr ("Length dependence coefficient of AGIDL")));
  Props.append (new Property ("AGIDLW", "0.0", false,
    QObject::tr ("Width dependence coefficient of AGIDL")));
  Props.append (new Property ("LAGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BGIDL", "2.3e9", false,
    QObject::tr ("exponential coeff. for GIDL in V/m")));
  Props.append (new Property ("LBGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGIDL", "0.5", false,
    QObject::tr ("exponential coeff. for GIDL in V/m")));
  Props.append (new Property ("LCGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EGIDL", "0.8", false,
    QObject::tr ("band bending parameter for GIDL in V")));
  Props.append (new Property ("LEGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WEGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PEGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AGISL", "0.0", false,
    QObject::tr ("pre-exponential coeff. for GISL in mho")));
  Props.append (new Property ("AGISLL", "0.0", false,
    QObject::tr ("Length dependence coefficient of AGISL")));
  Props.append (new Property ("AGISLW", "0.0", false,
    QObject::tr ("Width dependence coefficient of AGISL")));
  Props.append (new Property ("LAGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BGISL", "2.3e9", false,
    QObject::tr ("exponential coeff. for GISL in V/m")));
  Props.append (new Property ("LBGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGISL", "0.5", false,
    QObject::tr ("exponential coeff. for GISL in V/m")));
  Props.append (new Property ("LCGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EGISL", "0.8", false,
    QObject::tr ("band bending parameter for GISL in V")));
  Props.append (new Property ("LEGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WEGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PEGISL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CF", "0.0", false,
    QObject::tr ("Outer Fringe Cap (F)")));
  Props.append (new Property ("LCF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CFRCOEFF", "1.0", false,
    QObject::tr ("Coefficient for Outer Fringe Cap (F)")));
  Props.append (new Property ("CGSO", "0.0", false,
    QObject::tr ("Gate - Source overlap capacitance (F)")));
  Props.append (new Property ("CGDO", "0.0", false,
    QObject::tr ("Gate - Drain overlap capacitance (F)")));
  Props.append (new Property ("CGBO", "0.0", false,
    QObject::tr ("Gate - Body overlap capacitance (F)")));
  Props.append (new Property ("CGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CKAPPAS", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCKAPPAS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCKAPPAS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCKAPPAS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CKAPPAD", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCKAPPAD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCKAPPAD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCKAPPAD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DMCG", "0.0", false,
    QObject::tr ("Distance of Mid-Contact to Gate edge [m]")));
  Props.append (new Property ("DMCI", "0.0", false,
    QObject::tr ("Distance of Mid-Contact to Isolation [m]")));
  Props.append (new Property ("DMDG", "0.0", false,
    QObject::tr ("Distance of Mid-Diffusion to Gate edge [m]")));
  Props.append (new Property ("DMCGT", "0.0", false,
    QObject::tr ("Dist of Mid-Contact to Gate edge in Test [m]")));
  Props.append (new Property ("CJS", "5.0e-4", false,
    QObject::tr ("Unit area source-side junction capacitance at zero bias")));
  Props.append (new Property ("CJD", "5.0e-4", false,
    QObject::tr ("Unit area drain-side junction capacitance at zero bias")));
  Props.append (new Property ("CJSWS", "5.0e-10", false,
    QObject::tr ("Unit length source-side sidewall junction capacitance at zero bias")));
  Props.append (new Property ("CJSWD", "5.0e-10", false,
    QObject::tr ("Unit length drain-side sidewall junction capacitance at zero bias")));
  Props.append (new Property ("CJSWGS", "0.0", false,
    QObject::tr ("Unit length source-side gate sidewall junction capacitance at zero bias")));
  Props.append (new Property ("CJSWGD", "0.0", false,
    QObject::tr ("Unit length drain-side gate sidewall junction capacitance at zero bias")));
  Props.append (new Property ("PBS", "1.0", false,
    QObject::tr ("Source-side bulk junction built-in potential")));
  Props.append (new Property ("PBD", "1.0", false,
    QObject::tr ("Drain-side bulk junction built-in potential")));
  Props.append (new Property ("PBSWS", "1.0", false,
    QObject::tr ("Built-in potential for Source-side sidewall junction capacitance")));
  Props.append (new Property ("PBSWD", "1.0", false,
    QObject::tr ("Built-in potential for Drain-side sidewall junction capacitance")));
  Props.append (new Property ("PBSWGS", "1.0", false,
    QObject::tr ("Built-in potential for Source-side gate sidewall junction capacitance")));
  Props.append (new Property ("PBSWGD", "1.0", false,
    QObject::tr ("Built-in potential for Drain-side gate sidewall junction capacitance")));
  Props.append (new Property ("MJS", "0.5", false,
    QObject::tr ("Source bottom junction capacitance grading coefficient")));
  Props.append (new Property ("MJD", "0.5", false,
    QObject::tr ("Drain bottom junction capacitance grading coefficient")));
  Props.append (new Property ("MJSWS", "0.33", false,
    QObject::tr ("Source sidewall junction capacitance grading coefficient")));
  Props.append (new Property ("MJSWD", "0.33", false,
    QObject::tr ("Drain sidewall junction capacitance grading coefficient")));
  Props.append (new Property ("MJSWGS", "0.33", false,
    QObject::tr ("Source-side gate sidewall junction capacitance grading coefficient")));
  Props.append (new Property ("MJSWGD", "0.33", false,
    QObject::tr ("Drain-side gate sidewall junction capacitance grading coefficient")));
  Props.append (new Property ("JSS", "1.0e-4", false,
    QObject::tr ("Bottom source junction reverse saturation current density")));
  Props.append (new Property ("JSD", "1.0e-4", false,
    QObject::tr ("Bottom drain junction reverse saturation current density")));
  Props.append (new Property ("JSWS", "0.0", false,
    QObject::tr ("Unit length reverse saturation current for sidewall source junction")));
  Props.append (new Property ("JSWD", "0.0", false,
    QObject::tr ("Unit length reverse saturation current for sidewall drain junction")));
  Props.append (new Property ("JSWGS", "0.0", false,
    QObject::tr ("Unit length reverse saturation current for gate-edge sidewall source junction")));
  Props.append (new Property ("JSWGD", "0.0", false,
    QObject::tr ("Unit length reverse saturation current for gate-edge sidewall drain junction")));
  Props.append (new Property ("NJS", "1.0", false,
    QObject::tr ("Source junction emission coefficient")));
  Props.append (new Property ("NJD", "1.0", false,
    QObject::tr ("Drain junction emission coefficient")));
  Props.append (new Property ("IJTHSFWD", "0.1", false,
    QObject::tr ("Forward source diode breakdown limiting current")));
  Props.append (new Property ("IJTHDFWD", "0.1", false,
    QObject::tr ("Forward drain diode breakdown limiting current")));
  Props.append (new Property ("IJTHSREV", "0.1", false,
    QObject::tr ("Reverse source diode breakdown limiting current")));
  Props.append (new Property ("IJTHDREV", "0.1", false,
    QObject::tr ("Reverse drain diode breakdown limiting current")));
  Props.append (new Property ("BVS", "10.0", false,
    QObject::tr ("Source diode breakdown voltage")));
  Props.append (new Property ("BVD", "10.0", false,
    QObject::tr ("Drain diode breakdown voltage")));
  Props.append (new Property ("XJBVS", "1.0", false,
    QObject::tr ("Fitting parameter for source diode breakdown current")));
  Props.append (new Property ("XJBVD", "1.0", false,
    QObject::tr ("Fitting parameter for drain diode breakdown current")));
  Props.append (new Property ("JTSS", "0.0", false,
    QObject::tr ("Bottom source junction trap-assisted saturation current density")));
  Props.append (new Property ("JTSD", "0.0", false,
    QObject::tr ("Bottom drain junction trap-assisted saturation current density")));
  Props.append (new Property ("JTSSWS", "0.0", false,
    QObject::tr ("Unit length trap-assisted saturation current for sidewall source junction")));
  Props.append (new Property ("JTSSWD", "0.0", false,
    QObject::tr ("Unit length trap-assisted saturation current for sidewall drain junction")));
  Props.append (new Property ("JTSSWGS", "0.0", false,
    QObject::tr ("Unit length trap-assisted saturation current for gate-edge sidewall source junction")));
  Props.append (new Property ("JTSSWGD", "0.0", false,
    QObject::tr ("Unit length trap-assisted saturation current for gate-edge sidewall drain junction")));
  Props.append (new Property ("JTWEFF", "0.0", false,
    QObject::tr ("Trap assisted tunneling current width dependence")));
  Props.append (new Property ("NJTS", "20.0", false,
    QObject::tr ("Non-ideality factor for JTSS")));
  Props.append (new Property ("NJTSD", "20.0", false,
    QObject::tr ("Non-ideality factor for JTSD")));
  Props.append (new Property ("NJTSSW", "20.0", false,
    QObject::tr ("Non-ideality factor for JTSSWS")));
  Props.append (new Property ("NJTSSWD", "20.0", false,
    QObject::tr ("Non-ideality factor for JTSSWD")));
  Props.append (new Property ("NJTSSWG", "20.0", false,
    QObject::tr ("Non-ideality factor for JTSSWGS")));
  Props.append (new Property ("NJTSSWGD", "20.0", false,
    QObject::tr ("Non-ideality factor for JTSSWGD")));
  Props.append (new Property ("VTSS", "10.0", false,
    QObject::tr ("Bottom source junction trap-assisted current voltage dependent parameter")));
  Props.append (new Property ("VTSD", "10.0", false,
    QObject::tr ("Bottom drain junction trap-assisted current voltage dependent parameter")));
  Props.append (new Property ("VTSSWS", "10.0", false,
    QObject::tr ("Unit length trap-assisted current voltage dependent parameter\nfor sidewall source junction")));
  Props.append (new Property ("VTSSWD", "10.0", false,
    QObject::tr ("Unit length trap-assisted current voltage dependent parameter\nfor sidewall drain junction")));
  Props.append (new Property ("VTSSWGS", "10.0", false,
    QObject::tr ("Unit length trap-assisted current voltage dependent parameter for\ngate-edge sidewall source junction")));
  Props.append (new Property ("VTSSWGD", "10.0", false,
    QObject::tr ("Unit length trap-assisted current voltage dependent parameter for\ngate-edge sidewall drain junction")));
  Props.append (new Property ("EF", "1.0", false,
    QObject::tr ("Flicker Noise frequency exponent")));
  Props.append (new Property ("EM", "4.1e7", false,
    QObject::tr ("-")));
  Props.append (new Property ("NOIA", "6.250e+40", false,
    QObject::tr ("-")));
  Props.append (new Property ("NOIB", "3.125e+25", false,
    QObject::tr ("-")));
  Props.append (new Property ("NOIC", "8.750e+8", false,
    QObject::tr ("-")));
  Props.append (new Property ("LINTNOI", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NTNOI", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BINUNIT", "1", false,
    QObject::tr ("Unit of L and W for Binning\n1 : micro-meter, 0 : default")));
  Props.append (new Property ("DLBIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWBIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TNOM", "27.0", false,
    QObject::tr ("Temperature at which the model was extracted (degrees)")));
  Props.append (new Property ("DTEMP", "0.0", false,
    QObject::tr ("Offset of Device Temperature")));
  Props.append (new Property ("TBGASUB", "4.73e-4", false,
    QObject::tr ("Bandgap Temperature Coefficient (eV / degrees)")));
  Props.append (new Property ("TBGBSUB", "636.0", false,
    QObject::tr ("Bandgap Temperature Coefficient (degrees)")));
  Props.append (new Property ("TNFACTOR", "0.0", false,
    QObject::tr ("Temperature exponent for NFACTOR")));
  Props.append (new Property ("UTE", "-1.5", false,
    QObject::tr ("Mobility temperature exponent")));
  Props.append (new Property ("LUTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UTEL", "0.0", false,
    QObject::tr ("Length Scaling parameter for UTE")));
  Props.append (new Property ("UA1", "1.0e-3", false,
    QObject::tr ("Temperature coefficient for UA")));
  Props.append (new Property ("LUA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UA1L", "0.0", false,
    QObject::tr ("Length Scaling parameter for UA1")));
  Props.append (new Property ("UC1", "0.056e-9", false,
    QObject::tr ("Temperature coefficient for UC")));
  Props.append (new Property ("LUC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UD1", "0.0", false,
    QObject::tr ("Temperature coefficient for UD")));
  Props.append (new Property ("LUD1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUD1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUD1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UD1L", "0.0", false,
    QObject::tr ("Length Scaling parameter for UD1")));
  Props.append (new Property ("UCSTE", "-4.775e-3", false,
    QObject::tr ("Temperature coefficient for UCS")));
  Props.append (new Property ("LUCSTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUCSTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUCSTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TETA0", "0.0", false,
    QObject::tr ("Temperature coefficient for ETA0")));
  Props.append (new Property ("PRT", "0.0", false,
    QObject::tr ("Temperature coefficient for resistance")));
  Props.append (new Property ("LPRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AT", "-1.56e-3", false,
    QObject::tr ("Temperature coefficient for saturation velocity")));
  Props.append (new Property ("LAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ATL", "0.0", false,
    QObject::tr ("Length Scaling parameter for AT")));
  Props.append (new Property ("TDELTA", "0.0", false,
    QObject::tr ("Temperature coefficient for DELTA")));
  Props.append (new Property ("PTWGT", "0.0", false,
    QObject::tr ("Temperature coefficient for PTWG")));
  Props.append (new Property ("LPTWGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPTWGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPTWGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PTWGTL", "0.0", false,
    QObject::tr ("Length Scaling parameter for PTWGT")));
  Props.append (new Property ("KT1", "-0.11", false,
    QObject::tr ("Temperature coefficient for Vth")));
  Props.append (new Property ("KT1EXP", "1.0", false,
    QObject::tr ("Temperature coefficient for Vth")));
  Props.append (new Property ("KT1L", "0.0", false,
    QObject::tr ("Temperature coefficient for Vth")));
  Props.append (new Property ("LKT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT2", "0.022", false,
    QObject::tr ("Temperature coefficient for Vth")));
  Props.append (new Property ("LKT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("IIT", "0.0", false,
    QObject::tr ("Temperature coefficient for BETA0")));
  Props.append (new Property ("LIIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WIIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PIIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("IGT", "2.5", false,
    QObject::tr ("Gate Current Temperature Dependence")));
  Props.append (new Property ("LIGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WIGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PIGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TGIDL", "0.0", false,
    QObject::tr ("Temperature coefficient for GIDL/GISL")));
  Props.append (new Property ("LTGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WTGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PTGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJ", "0.0", false,
    QObject::tr ("Temperature coefficient for CJS/CJD")));
  Props.append (new Property ("TCJSW", "0.0", false,
    QObject::tr ("Temperature coefficient for CJSWS/CJSWD")));
  Props.append (new Property ("TCJSWG", "0.0", false,
    QObject::tr ("Temperature coefficient for CJSWGS/CJSWGD")));
  Props.append (new Property ("TPB", "0.0", false,
    QObject::tr ("Temperature coefficient for PBS/PBD")));
  Props.append (new Property ("TPBSW", "0.0", false,
    QObject::tr ("Temperature coefficient for PBSWS/PBSWD")));
  Props.append (new Property ("TPBSWG", "0.0", false,
    QObject::tr ("Temperature coefficient for PBSWGS/PBSWGD")));
  Props.append (new Property ("XTIS", "3.0", false,
    QObject::tr ("Source junction current temperature exponent")));
  Props.append (new Property ("XTID", "3.0", false,
    QObject::tr ("Drain junction current temperature exponent")));
  Props.append (new Property ("XTSS", "0.02", false,
    QObject::tr ("Power dependence of JTSS on temperature")));
  Props.append (new Property ("XTSD", "0.02", false,
    QObject::tr ("Power dependence of JTSD on temperature")));
  Props.append (new Property ("XTSSWS", "0.02", false,
    QObject::tr ("Power dependence of JTSSWS on temperature")));
  Props.append (new Property ("XTSSWD", "0.02", false,
    QObject::tr ("Power dependence of JTSSWD on temperature")));
  Props.append (new Property ("XTSSWGS", "0.02", false,
    QObject::tr ("Power dependence of JTSSWGS on temperature")));
  Props.append (new Property ("XTSSWGD", "0.02", false,
    QObject::tr ("Power dependence of JTSSWGD on temperature")));
  Props.append (new Property ("TNJTS", "0.0", false,
    QObject::tr ("Temperature coefficient for NJTS")));
  Props.append (new Property ("TNJTSD", "0.0", false,
    QObject::tr ("Temperature coefficient for NJTSD")));
  Props.append (new Property ("TNJTSSW", "0.0", false,
    QObject::tr ("Temperature coefficient for NJTSSW")));
  Props.append (new Property ("TNJTSSWD", "0.0", false,
    QObject::tr ("Temperature coefficient for NJTSSWD")));
  Props.append (new Property ("TNJTSSWG", "0.0", false,
    QObject::tr ("Temperature coefficient for NJTSSWG")));
  Props.append (new Property ("TNJTSSWGD", "0.0", false,
    QObject::tr ("Temperature coefficient for NJTSSWGD")));
  Props.append (new Property ("RTH0", "0.0", false,
    QObject::tr ("Thermal resistance")));
  Props.append (new Property ("CTH0", "1.0E-05", false,
    QObject::tr ("Thermal capacitance")));
  Props.append (new Property ("WTH0", "0.0", false,
    QObject::tr ("Width dependence coefficient for Rth and Cth")));
  Props.append (new Property ("SA", "0.0", false,
    QObject::tr ("Distance between OD edge from Poly from one side")));
  Props.append (new Property ("SB", "0.0", false,
    QObject::tr ("Distance between OD edge from Poly from other side")));
  Props.append (new Property ("SD", "0.0", false,
    QObject::tr ("Distance between neighboring fingers")));
  Props.append (new Property ("SAREF", "1.0e-6", false,
    QObject::tr ("Reference distance between OD edge from Poly from one side")));
  Props.append (new Property ("SBREF", "1.0e-6", false,
    QObject::tr ("Reference distance between OD edge from Poly from other side")));
  Props.append (new Property ("WLOD", "0.0", false,
    QObject::tr ("Width Parameter for Stress Effect")));
  Props.append (new Property ("KU0", "0.0", false,
    QObject::tr ("Mobility degradation/enhancement Parameter for Stress Effect")));
  Props.append (new Property ("KVSAT", "0.0", false,
    QObject::tr ("Saturation Velocity degradation/enhancement Parameter for Stress Effect")));
  Props.append (new Property ("TKU0", "0.0", false,
    QObject::tr ("Temperature Coefficient for KU0")));
  Props.append (new Property ("LKU0", "0.0", false,
    QObject::tr ("Length Dependence of KU0")));
  Props.append (new Property ("WKU0", "0.0", false,
    QObject::tr ("Width Dependence of KU0")));
  Props.append (new Property ("PKU0", "0.0", false,
    QObject::tr ("Cross Term Dependence of KU0")));
  Props.append (new Property ("LLODKU0", "0.0", false,
    QObject::tr ("Length Parameter for u0 stress effect")));
  Props.append (new Property ("WLODKU0", "0.0", false,
    QObject::tr ("Width Parameter for u0 stress effect")));
  Props.append (new Property ("KVTH0", "0.0", false,
    QObject::tr ("Threshold Shift parameter for stress effect")));
  Props.append (new Property ("LKVTH0", "0.0", false,
    QObject::tr ("Length dependence of KVTH0")));
  Props.append (new Property ("WKVTH0", "0.0", false,
    QObject::tr ("Width dependence of KVTH0")));
  Props.append (new Property ("PKVTH0", "0.0", false,
    QObject::tr ("Cross-term dependence of KVTH0")));
  Props.append (new Property ("LLODVTH", "0.0", false,
    QObject::tr ("Length Parameter for Vth stress effect")));
  Props.append (new Property ("WLODVTH", "0.0", false,
    QObject::tr ("Width Parameter for Vth stress effect")));
  Props.append (new Property ("STK2", "0.0", false,
    QObject::tr ("K2 shift factor related to Vth change")));
  Props.append (new Property ("LODK2", "0.0", false,
    QObject::tr ("K2 shift modification factor for stress effect")));
  Props.append (new Property ("STETA0", "0.0", false,
    QObject::tr ("eta0 shift related to Vth0 change")));
  Props.append (new Property ("LODETA0", "0.0", false,
    QObject::tr ("eta0 modification foator for stress effect")));
  Props.append (new Property ("SCA", "0.0", false,
    QObject::tr ("Integration of first distribution function for scattered well dopant")));
  Props.append (new Property ("SCB", "0.0", false,
    QObject::tr ("Integration of second distribution function for scattered well dopant")));
  Props.append (new Property ("SCC", "0.0", false,
    QObject::tr ("Integration of second distribution function for scattered well dopant")));
  Props.append (new Property ("SC", "0.0", false,
    QObject::tr ("Distance to a single well edge")));
  Props.append (new Property ("WEB", "0.0", false,
    QObject::tr ("Coefficient for SCB")));
  Props.append (new Property ("WEC", "0.0", false,
    QObject::tr ("Coefficient for SCC")));
  Props.append (new Property ("KVTH0WE", "0.0", false,
    QObject::tr ("Vth shift for well proximity effect")));
  Props.append (new Property ("LKVTH0WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKVTH0WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKVTH0WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K2WE", "0.0", false,
    QObject::tr ("K2 shift for well proximity effect")));
  Props.append (new Property ("LK2WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WK2WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PK2WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KU0WE", "0.0", false,
    QObject::tr ("Mobility degradation factor for well proximity effect")));
  Props.append (new Property ("LKU0WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKU0WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKU0WE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("SCREF", "1.0e-6", false,
    QObject::tr ("Reference distance to calculate SCA,SCB and SCC")));
  Props.append (new Property ("GMIN", "1e-15", false,
    QObject::tr ("-")));
  Props.append (new Property ("Temp", "27.0", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "bsim6v10pMOS";
  Name  = "bsim6_";
}

Component * bsim6v10pMOS::newOne()
{
  bsim6v10pMOS * p = new bsim6v10pMOS();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * bsim6v10pMOS::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("bsim6v10pMOS");
  BitmapFile = (char *) "bsim6v10pMOS";

  if(getNewOne) return new bsim6v10pMOS();
  return 0;
}

void bsim6v10pMOS::createSymbol()
{
  Lines.append(new Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 11,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
  
  Lines.append(new Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));
  Lines.append(new Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  5,  0,   0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  -5, 0,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -3,-10,  3,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-10, -8,-10, -6,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-10,  8,-10,  6,QPen(Qt::darkBlue,3)));


  Ports.append(new Port(  0,-30)); // drain
  Ports.append(new Port(-30,  0)); // gate
  Ports.append(new Port(  0, 30)); // source
  Ports.append(new Port( 20,  0)); // bulk

  x1 = -30; y1 = -30;
  x2 =  20; y2 =  30;

}
