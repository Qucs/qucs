(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="diode_hb.dat", qucs_DataDisplay="diode_hb.dpl", qucs_SimOpenDpl=1, qucs_Script="diode_hb.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module diode_hb();
    wire n_100_340;
    wire n_100_410;
    wire n_130_320;
    wire n_210_400;
    wire n_280_320;
    wire n_370_400;
    wire n_470_390;
    (* S0_x1=100, S0_y1=410, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_100_410 );
    (* S0_x1=100, S0_y1=310, S0_x2=100, S0_y2=370, qucs_mirroredX=0, qucs_rotated=1 *) Vac #(.U(10 V),.f(1 GHz),.Phase(0),.Theta(0)) V1 ( n_100_310, n_100_370 );
    (* S0_x1=180, S0_y1=400, S0_x2=240, S0_y2=400, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(1 pF),.V(),.Symbol(neutral)) C1 ( n_180_400, n_240_400 );
    (* S0_x1=440, S0_y1=390, S0_x2=500, S0_y2=390, qucs_mirroredX=0, qucs_rotated=0 *) Diode #(.Is(1e-15 A),.N(1),.Cj0(0 fF),.M(0.5),.Vj(0.7 V),.Fc(0.5),.Cp(0.0 fF),.Isr(0.0),.Nr(2.0),.Rs(0.0 Ohm),.Tt(0.0 ps),.Ikf(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Bv(0.7),.Ibv(1 mA),.Temp(0),.Xti(3.0),.Eg(1.11),.Tbv(0.0),.Trs(0.0),.Ttt1(0.0),.Ttt2(0.0),.Tm1(0.0),.Tm2(0.0),.Tnom(26.85),.Area(1.0),.Symbol(normal)) D1 ( n_440_390, n_500_390 );
    (* S0_x1=100, S0_y1=320, S0_x2=160, S0_y2=320, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(100),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_100_320, n_160_320 );
    (* S0_x1=250, S0_y1=320, S0_x2=310, S0_y2=320, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(100),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_250_320, n_310_320 );
    (* S0_x1=340, S0_y1=400, S0_x2=400, S0_y2=400, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(100k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R3 ( n_340_400, n_400_400 );
endmodule
