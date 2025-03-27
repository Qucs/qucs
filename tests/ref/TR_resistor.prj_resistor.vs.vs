(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="resistor.dat", qucs_DataDisplay="resistor.dpl", qucs_SimOpenDpl=1, qucs_Script="resistor.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module resistor();
    wire n_450_320;
    wire n_530_270;
    wire n_610_270;
    wire n_660_370;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=660, S0_y1=370 *) GND #() \\*  ( n_660_370 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=580, S0_y1=270, S0_x2=640, S0_y2=270 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_580_270, n_640_270 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=500, S0_y1=270, S0_x2=560, S0_y2=270 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_500_270, n_560_270 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=450, S0_y1=290, S0_x2=450, S0_y2=350 *) Vac #(.U(10 V),.f(1kHz),.Phase(0),.Theta(0)) V1 ( n_450_290, n_450_350 );
endmodule
