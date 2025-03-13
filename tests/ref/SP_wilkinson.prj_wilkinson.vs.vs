(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="wilkinson.dat", qucs_DataDisplay="wilkinson.dpl", qucs_SimOpenDpl=1, qucs_Script="wilkinson.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module wilkinson();
    wire n_0_0;
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(1),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P1 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) TLIN #(.Z(50 Ohm),.L(100 mm),.Alpha(0 dB),.Temp(26.85)) Line1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) TLIN #(.Z(70.7 Ohm),.L(75 mm),.Alpha(0 dB),.Temp(26.85)) Line2 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) TLIN #(.Z(70.7 Ohm),.L(75 mm),.Alpha(0 dB),.Temp(26.85)) Line3 ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(2),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P2 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(3),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P3 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(100 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_m30_0, n_30_0 );
endmodule
