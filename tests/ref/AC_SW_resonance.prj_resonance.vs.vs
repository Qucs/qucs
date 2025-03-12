(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="resonance.dat", qucs_DataDisplay="resonance.dpl", qucs_SimOpenDpl=1, qucs_Script="resonance.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module resonance();
    wire n_120_120;
    wire n_120_240;
    wire n_320_120;
    wire n_320_180;
    wire n_320_240;
    (* S0_x1=120, S0_y1=90, S0_x2=120, S0_y2=150, qucs_mirroredX=0, qucs_rotated=1 *) Vac #(.U(1 V),.f(1 GHz),.Phase(0),.Theta(0)) V1 ( n_120_90, n_120_150 );
    (* S0_x1=320, S0_y1=240, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_320_240 );
    (* S0_x1=120, S0_y1=240, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_120_240 );
    (* S0_x1=290, S0_y1=120, S0_x2=350, S0_y2=120, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(R_par),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_290_120, n_350_120 );
    (* S0_x1=290, S0_y1=240, S0_x2=350, S0_y2=240, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(2 pF),.V(),.Symbol(neutral)) C1 ( n_290_240, n_350_240 );
    (* S0_x1=290, S0_y1=180, S0_x2=350, S0_y2=180, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(13 nH),.I()) L1 ( n_290_180, n_350_180 );
endmodule
