(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="integrity3.dat", qucs_DataDisplay="integrity3.dpl", qucs_SimOpenDpl=1, qucs_Script="integrity3.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module integrity3();
    wire n_2001_0;
    wire n_2061_0;
    wire n_2110_m280;
    wire n_2170_m280;
    (* qucs_mirrored=0, qucs_rotated=0, qucs_visible="100000", S0_x1=2001, S0_y1=0, S0_x2=2061, S0_y2=0 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_2001_0, n_2061_0 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_visible="100", S0_x1=2110, S0_y1=-280, S0_x2=2170, S0_y2=-280 *) C #(.C(1 pF),.V()) C1 ( n_2110_m280, n_2170_m280 );
endmodule
Warning: R1, Port 0 at (2001,0): not connected to n_2000_m280

Warning: R1, Port 1 at (2061,0): not connected to n_2060_m280

