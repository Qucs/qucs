(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="colpitts.dat", qucs_DataDisplay="colpitts.dpl", qucs_SimOpenDpl=1, qucs_Script="colpitts.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module colpitts();
    wire n_0_0;
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(1 nF),.V(),.Symbol(neutral)) C3 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(500 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Vdc #(.U(10 V)) V1 ( n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) L #(.L(100 uH),.I()) L1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=0, S0_y2=-30, S0_x3=0, S0_y3=30, qucs_mirrored=0, qucs_rotated=0 *) JFET #(.Type(nfet),.Vt0(-0.8 V),.Beta(5e-3),.Lambda(0.0),.Rd(0.0),.Rs(0.0),.Is(1e-14),.N(1.0),.Isr(1e-14),.Nr(2.0),.Cgs(0.0),.Cgd(0.0),.Pb(1.0),.Fc(0.5),.M(0.5),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Temp(26.85),.Xti(3.0),.Vt0tc(0.0),.Betatce(0.0),.Tnom(26.85),.Area(1.0)) T1 ( n_m30_0, n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(500 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(0.5 nF),.V(),.Symbol(neutral)) C2 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(0.5 nF),.V(),.Symbol(neutral)) C1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(5 kOhm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R3 ( n_m30_0, n_30_0 );
endmodule
