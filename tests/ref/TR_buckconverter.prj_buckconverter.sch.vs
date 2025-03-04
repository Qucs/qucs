(* qucs_ViewX1=0 *)
(* qucs_ViewY1=10 *)
(* qucs_ViewX2=1366 *)
(* qucs_ViewY2=861 *)
(* qucs_Scale=1 *)
(* qucs_tmpViewX1=0 *)
(* qucs_tmpViewY1=0 *)
(* qucs_GridX=10 *)
(* qucs_GridY=10 *)
(* qucs_GridOn=1 *)
(* qucs_DataSet="buckconverter.dat" *)
(* qucs_DataDisplay="buckconverter.dpl" *)
(* qucs_SimOpenDpl=0 *)
(* qucs_Script="buckconverter.m" *)
(* qucs_SimRunScript=0 *)
(* qucs_showFrame=0 *)
(* qucs_FrameText0="Title" *)
(* qucs_FrameText1="Drawn By:" *)
(* qucs_FrameText2="Date:" *)
(* qucs_FrameText3="Revision:" *)
module buckconverter();
    wire n_50_140;
    wire n_50_160;
    wire n_50_190;
    wire n_50_220;
    wire n_50_240;
    wire n_130_200;
    wire n_130_260;
    wire n_130_290;
    wire n_130_320;
    wire n_130_360;
    wire n_170_140;
    wire n_170_200;
    wire n_200_170;
    wire n_230_140;
    wire n_230_200;
    wire n_270_200;
    wire n_270_360;
    wire n_330_140;
    wire n_330_160;
    wire n_330_190;
    wire n_330_220;
    wire n_330_240;
    wire n_350_290;
    wire n_380_140;
    wire n_410_140;
    wire n_440_140;
    wire n_490_280;
    wire n_530_510;
    wire n_540_140;
    wire n_540_160;
    wire n_540_190;
    wire n_540_220;
    wire n_540_240;
    wire n_640_140;
    wire n_640_160;
    wire n_640_190;
    wire n_640_220;
    wire n_640_240;
    (* S0_x1=130, S0_y1=260, S0_x2=130, S0_y2=320 *) Vrect #(.U(1V),.TH(Ton),.TL(Toff),.Tr(1 ns),.Tf(1 ns),.Td(0 ns)) V2 ( n_130_260, n_130_320 );
    (* S0_x1=130, S0_y1=360 *) GND #() \*  ( n_130_360 );
    (* S0_x1=50, S0_y1=160, S0_x2=50, S0_y2=220 *) Vdc #(.U(12V)) V1 ( n_50_160, n_50_220 );
    (* S0_x1=50, S0_y1=240 *) GND #() \*  ( n_50_240 );
    (* S0_x1=170, S0_y1=200, S0_x2=170, S0_y2=140, S0_x3=230, S0_y3=140, S0_x4=230, S0_y4=200 *) Relais #(.Vt(0.5 V),.Vh(0.1 V),.Ron(1),.Roff(1e12),.Temp(26.85)) S1 ( n_170_200, n_170_140, n_230_140, n_230_200 );
    (* S0_x1=640, S0_y1=240 *) GND #() \*  ( n_640_240 );
    (* S0_x1=330, S0_y1=160, S0_x2=330, S0_y2=220 *) Diode #(.Is(1e-12 A),.N(1),.Cj0(10 fF),.M(0.5),.Vj(0.7 V),.Fc(0.5),.Cp(0.0 fF),.Isr(0.0),.Nr(2.0),.Rs(0.0 Ohm),.Tt(0.0 ps),.Ikf(0),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Bv(0),.Ibv(1 mA),.Temp(26.85),.Xti(3.0),.Eg(1.11),.Tbv(0.0),.Trs(0.0),.Ttt1(0.0),.Ttt2(0.0),.Tm1(0.0),.Tm2(0.0),.Tnom(26.85),.Area(1.0),.Symbol(normal)) D1 ( n_330_160, n_330_220 );
    (* S0_x1=380, S0_y1=140, S0_x2=440, S0_y2=140 *) L #(.L(47uH),.I(0)) L1 ( n_380_140, n_440_140 );
    (* S0_x1=540, S0_y1=160, S0_x2=540, S0_y2=220 *) C #(.C(100u),.V(0),.Symbol(neutral)) C1 ( n_540_160, n_540_220 );
    (* S0_x1=640, S0_y1=220, S0_x2=640, S0_y2=160 *) R #(.R(5),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_640_220, n_640_160 );
    (*  *) Eqn #(.Tmax(Bperiod*20),.Tstep(Bperiod/1000),.Export(yes)) Eqn2 (  );
    (*  *) \.TR  #(.Type(lin),.Start(0),.Stop(Tmax),.Points(201),.IntegrationMethod(Trapezoidal),.Order(2),.InitialStep(1 ns),.MinStep(Tstep),.MaxIter(150),.reltol(0.001),.abstol(1 pA),.vntol(1 uV),.Temp(26.85),.LTEreltol(1e-3),.LTEabstol(1e-6),.LTEfactor(1),.Solver(CroutLU),.relaxTSR(no),.initialDC(yes),.MaxStep(Tstep)) TR1 (  );
    (*  *) Eqn #(.Bfreq(40k),.Bperiod(1/Bfreq),.Bduty(50),.Ton(Bperiod*Bduty/100),.Toff(Bperiod-Ton),.Export(yes)) Eqn1 (  );
    (* S0_x1=330, S0_y1=140, S0_x2=380, S0_y2=140 *) net #() dio ( n_330_140, n_380_140 );
    (* S0_x1=330, S0_y1=140, S0_x2=330, S0_y2=160 *) net #() net1 ( n_330_140, n_330_160 );
    (* S0_x1=230, S0_y1=140, S0_x2=330, S0_y2=140 *) net #() net2 ( n_230_140, n_330_140 );
    (* S0_x1=230, S0_y1=200, S0_x2=270, S0_y2=200 *) net #() net3 ( n_230_200, n_270_200 );
    (* S0_x1=130, S0_y1=200, S0_x2=170, S0_y2=200 *) net #() net4 ( n_130_200, n_170_200 );
    (* S0_x1=270, S0_y1=200, S0_x2=270, S0_y2=360 *) net #() net5 ( n_270_200, n_270_360 );
    (* S0_x1=130, S0_y1=360, S0_x2=270, S0_y2=360 *) net #() net6 ( n_130_360, n_270_360 );
    (* S0_x1=130, S0_y1=200, S0_x2=130, S0_y2=260 *) net #() ctrl ( n_130_200, n_130_260 );
    (* S0_x1=130, S0_y1=320, S0_x2=130, S0_y2=360 *) net #() net7 ( n_130_320, n_130_360 );
    (* S0_x1=50, S0_y1=140, S0_x2=170, S0_y2=140 *) net #() net8 ( n_50_140, n_170_140 );
    (* S0_x1=50, S0_y1=140, S0_x2=50, S0_y2=160 *) net #() net9 ( n_50_140, n_50_160 );
    (* S0_x1=50, S0_y1=220, S0_x2=50, S0_y2=240 *) net #() net10 ( n_50_220, n_50_240 );
    (* S0_x1=330, S0_y1=220, S0_x2=330, S0_y2=240 *) net #() net11 ( n_330_220, n_330_240 );
    (* S0_x1=640, S0_y1=140, S0_x2=640, S0_y2=160 *) net #() net12 ( n_640_140, n_640_160 );
    (* S0_x1=640, S0_y1=220, S0_x2=640, S0_y2=240 *) net #() net13 ( n_640_220, n_640_240 );
    (* S0_x1=540, S0_y1=140, S0_x2=640, S0_y2=140 *) net #() out ( n_540_140, n_640_140 );
    (* S0_x1=540, S0_y1=140, S0_x2=540, S0_y2=160 *) net #() net14 ( n_540_140, n_540_160 );
    (* S0_x1=330, S0_y1=240, S0_x2=540, S0_y2=240 *) net #() net15 ( n_330_240, n_540_240 );
    (* S0_x1=540, S0_y1=240, S0_x2=640, S0_y2=240 *) net #() net16 ( n_540_240, n_640_240 );
    (* S0_x1=540, S0_y1=220, S0_x2=540, S0_y2=240 *) net #() net17 ( n_540_220, n_540_240 );
    (* S0_x1=440, S0_y1=140, S0_x2=540, S0_y2=140 *) net #() net18 ( n_440_140, n_540_140 );
endmodule
