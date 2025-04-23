(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="integrity2.dat", qucs_DataDisplay="integrity2.dpl", qucs_SimOpenDpl=1, qucs_Script="integrity2.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module integrity2();
    wire n_2000_m280;
    wire n_2060_m280;
    wire n_2110_m280;
    wire n_2170_m280;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=2000, S0_y1=-280, S0_x2=2060, S0_y2=-280 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_2000_m280, n_2060_m280 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=2110, S0_y1=-280, S0_x2=2170, S0_y2=-280 *) C #(.C(1 pF),.V()) C1 ( n_2110_m280, n_2170_m280 );
endmodule
Warning: Port 0 of component R1 at position (2000,-280)!
Should be connected to n_0_0 but isn't!

