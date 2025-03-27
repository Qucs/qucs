(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="fullwaverectifier_1.dat", qucs_DataDisplay="fullwaverectifier_1.dpl", qucs_SimOpenDpl=1, qucs_Script="fullwaverectifier_1.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module fullwaverectifier_1();
    wire n_0_0;
    wire n_50_180;
    wire n_50_250;
    wire n_200_110;
    wire n_200_250;
    wire n_320_180;
    wire n_430_180;
    wire n_430_250;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=290, S0_y1=110, S0_x2=350, S0_y2=150, S0_x3=350, S0_y3=210, S0_x4=290, S0_y4=250, S0_x5=290, S0_y5=190, S0_x6=290, S0_y6=170 *) sTr #(.T1(1),.T2(1)) Tr1 ( n_290_110, n_350_150, n_350_210, n_290_250, n_290_190, n_290_170 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=50, S0_y1=210, S0_x2=50, S0_y2=150 *) R #(.R(1k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_50_210, n_50_150 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=50, S0_y1=250 *) GND #() \\*  ( n_50_250 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=430, S0_y1=250 *) GND #() \\*  ( n_430_250 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=430, S0_y1=150, S0_x2=430, S0_y2=210 *) Vac #(.U(Vamp),.f(Vfreq),.Phase(0),.Theta(0)) V1 ( n_430_150, n_430_210 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=170, S0_y1=110, S0_x2=230, S0_y2=110 *) Diode #(.Is(1e-15 A),.N(1),.Cj0(10 fF),.M(0.5),.Vj(0.7 V),.Fc(0.5),.Cp(0.0 fF),.Isr(0.0),.Nr(2.0),.Rs(0.0 Ohm),.Tt(0.0 ps),.Ikf(0),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Bv(0),.Ibv(1 mA),.Temp(26.85),.Xti(3.0),.Eg(1.11),.Tbv(0.0),.Trs(0.0),.Ttt1(0.0),.Ttt2(0.0),.Tm1(0.0),.Tm2(0.0),.Tnom(26.85),.Area(1.0),.Symbol(normal)) D1 ( n_170_110, n_230_110 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=170, S0_y1=250, S0_x2=230, S0_y2=250 *) Diode #(.Is(1e-15 A),.N(1),.Cj0(10 fF),.M(0.5),.Vj(0.7 V),.Fc(0.5),.Cp(0.0 fF),.Isr(0.0),.Nr(2.0),.Rs(0.0 Ohm),.Tt(0.0 ps),.Ikf(0),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Bv(0),.Ibv(1 mA),.Temp(26.85),.Xti(3.0),.Eg(1.11),.Tbv(0.0),.Trs(0.0),.Ttt1(0.0),.Ttt2(0.0),.Tm1(0.0),.Tm2(0.0),.Tnom(26.85),.Area(1.0),.Symbol(normal)) D2 ( n_170_250, n_230_250 );
    (* qucs_mirrored=0, qucs_rotated=0 *) Eqn #(.y(1),.Export(yes)) Eqn1 (  );
endmodule
