(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="chargepump.dat", qucs_DataDisplay="chargepump.dpl", qucs_SimOpenDpl=1, qucs_Script="chargepump.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module chargepump();
    wire n_110_200;
    wire n_110_260;
    wire n_190_190;
    wire n_220_280;
    wire n_330_240;
    wire n_330_300;
    wire n_350_50;
    wire n_350_190;
    (* S0_x1=110, S0_y1=260, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_110_260 );
    (* S0_x1=330, S0_y1=300, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_330_300 );
    (* S0_x1=220, S0_y1=280, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_220_280 );
    (* S0_x1=190, S0_y1=280, S0_x2=250, S0_y2=280, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(10 pF),.V(),.Symbol(neutral)) C1 ( n_190_280, n_250_280 );
    (* S0_x1=320, S0_y1=190, S0_x2=380, S0_y2=190, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(10k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_320_190, n_380_190 );
    (* S0_x1=160, S0_y1=190, S0_x2=190, S0_y2=160, S0_x3=190, S0_y3=220, S0_x4=210, S0_y4=190, qucs_mirroredX=0, qucs_rotated=0 *) MOSFET #(.Type(nfet),.Vt0(0.7 V),.Kp(50e-6),.Gamma(0.96),.Phi(0.5763 V),.Lambda(0.0),.Rd(0.0 Ohm),.Rs(0.0 Ohm),.Rg(0.0 Ohm),.Is(0 A),.N(1.0),.W(3.5 um),.L(5.5 um),.Ld(0.5e-6),.Tox(50e-9),.Cgso(0.0),.Cgdo(0.0),.Cgbo(0.0),.Cbd(0.0 F),.Cbs(0.0 F),.Pb(0.7 V),.Mj(0.5),.Fc(0.5),.Cjsw(0.05e-9),.Mjsw(0.33),.Tt(0.0 ps),.Nsub(1e16),.Nss(0.0),.Tpg(1),.Uo(600.0),.Rsh(20),.Nrd(1),.Nrs(1),.Cj(1e-4),.Js(1e-8),.Ad(100p),.As(100p),.Pd(50u),.Ps(50u),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Temp(26.85),.Tnom(26.85)) T1 ( n_160_190, n_190_160, n_190_220, n_210_190 );
    (* S0_x1=110, S0_y1=170, S0_x2=110, S0_y2=230, qucs_mirroredX=0, qucs_rotated=1 *) Vac #(.U(5 V),.f(10 MHz),.Phase(0),.Theta(0)) V1 ( n_110_170, n_110_230 );
    (* S0_x1=330, S0_y1=210, S0_x2=330, S0_y2=270, qucs_mirroredX=0, qucs_rotated=1 *) Vac #(.U(-5 V),.f(10 MHz),.Phase(0),.Theta(0)) V3 ( n_330_210, n_330_270 );
    (* S0_x1=350, S0_y1=20, S0_x2=350, S0_y2=80, qucs_mirroredX=0, qucs_rotated=1 *) Vac #(.U(5 V),.f(10 MHz),.Phase(0),.Theta(0)) V2 ( n_350_20, n_350_80 );
endmodule
