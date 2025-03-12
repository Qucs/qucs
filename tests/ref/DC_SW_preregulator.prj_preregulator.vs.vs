(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="preregulator.dat", qucs_DataDisplay="preregulator.dpl", qucs_SimOpenDpl=1, qucs_Script="preregulator.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module preregulator();
    wire n_110_170;
    wire n_110_250;
    wire n_160_150;
    wire n_260_130;
    wire n_260_170;
    wire n_360_230;
    (* S0_x1=230, S0_y1=130, S0_x2=260, S0_y2=100, S0_x3=260, S0_y3=160, qucs_mirroredX=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1),.Ikf(0),.Ikr(0),.Vaf(0),.Var(0),.Ise(0),.Ne(1.5),.Isc(0),.Nc(2),.Bf(100),.Br(1),.Rbm(0),.Irb(0),.Rc(0),.Re(0),.Rb(0),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(0.0),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(0.0),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_230_130, n_260_100, n_260_160 );
    (* S0_x1=110, S0_y1=140, S0_x2=110, S0_y2=200, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(VBAT)) V1 ( n_110_140, n_110_200 );
    (* S0_x1=110, S0_y1=250, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_110_250 );
    (* S0_x1=130, S0_y1=150, S0_x2=190, S0_y2=150, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(1k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_130_150, n_190_150 );
    (* S0_x1=330, S0_y1=230, S0_x2=390, S0_y2=230, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(100),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_330_230, n_390_230 );
    (* S0_x1=230, S0_y1=170, S0_x2=290, S0_y2=170, qucs_mirroredX=0, qucs_rotated=0 *) Diode #(.Is(1e-15 A),.N(1),.Cj0(10 fF),.M(0.5),.Vj(0.7 V),.Fc(0.5),.Cp(0.0 fF),.Isr(0.0),.Nr(2.0),.Rs(0.0 Ohm),.Tt(0.0 ps),.Ikf(0),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Bv(5.9V),.Ibv(1 mA),.Temp(26.85),.Xti(3.0),.Eg(1.11),.Tbv(0.0),.Trs(0.0),.Ttt1(0.0),.Ttt2(0.0),.Tm1(0.0),.Tm2(0.0),.Tnom(26.85),.Area(1.0),.Symbol(Zener)) D1 ( n_230_170, n_290_170 );
endmodule
