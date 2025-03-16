(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="tline0.dat", qucs_DataDisplay="tline0.dpl", qucs_SimOpenDpl=1, qucs_Script="tline0.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module tline0();
    wire n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*3  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*2  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*4  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) TLIN #(.Z(50 Ohm),.L(.1),.Alpha(0 dB),.Temp(26.85)) Line1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-10, S0_y1=20, S0_x2=10, S0_y2=20 *) VProbe #() Pr2 ( n_m10_20, n_10_20 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-10, S0_y1=20, S0_x2=10, S0_y2=20 *) VProbe #() Pr1 ( n_m10_20, n_10_20 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-10, S0_y1=20, S0_x2=10, S0_y2=20 *) VProbe #() Pr3 ( n_m10_20, n_10_20 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*1  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Vpulse #(.U1(0 V),.U2(1 V),.T1(1 ns),.T2(2 ns),.Tr(.1 ns),.Tf(.1 ns)) V1 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_m30_0, n_30_0 );
endmodule
