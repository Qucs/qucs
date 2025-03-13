(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="timer_trig.dat", qucs_DataDisplay="timer_trig.dpl", qucs_SimOpenDpl=1, qucs_Script="timer_trig.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module timer_trig(.Pcomp_vp1(n_0_0), .Pcomp_vn1(n_0_0), .comp_vout1(n_0_0));
    inout n_0_0;
    inout n_0_0;
    inout n_0_0;
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) Port #(.Num(1),.Type(analog)) Pcomp_vp1 ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) Port #(.Num(2),.Type(analog)) Pcomp_vn1 ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Idc #(.I(500 nA)) I1 ( n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(1k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R4 ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) Port #(.Num(3),.Type(analog)) comp_vout1 ( n_0_0 );
    (* S0_x1=-30, S0_y1=20, S0_x2=-30, S0_y2=-20, S0_x3=40, S0_y3=0, qucs_mirrored=0, qucs_rotated=0 *) OpAmp #(.G(1e6),.Umax(1 V)) OP1 ( n_m30_20, n_m30_m20, n_40_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(1 nF),.V(),.Symbol(neutral)) C1 ( n_m30_0, n_30_0 );
endmodule
