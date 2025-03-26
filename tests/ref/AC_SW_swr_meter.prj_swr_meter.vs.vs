(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="swr_meter.dat", qucs_DataDisplay="swr_meter.dpl", qucs_SimOpenDpl=1, qucs_Script="swr_meter.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module swr_meter();
    wire n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R4 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Vac #(.U(2 V),.f(1 GHz),.Phase(0),.Theta(0)) V1 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=-30, S0_x2=30, S0_y2=-30, S0_x3=30, S0_y3=30, S0_x4=-30, S0_y4=30 *) Tr #(.T(25)) Tr2 ( n_m30_m30, n_30_m30, n_30_30, n_m30_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(R_load),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) Load ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=-30, S0_x2=30, S0_y2=-30, S0_x3=30, S0_y3=30, S0_x4=-30, S0_y4=30 *) Tr #(.T(25)) Tr1 ( n_m30_m30, n_30_m30, n_30_30, n_m30_30 );
endmodule
