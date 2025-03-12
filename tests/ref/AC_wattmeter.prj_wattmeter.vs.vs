(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="wattmeter.dat", qucs_DataDisplay="wattmeter.dpl", qucs_SimOpenDpl=1, qucs_Script="wattmeter.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module wattmeter();
    wire n_56_247;
    wire n_56_437;
    wire n_186_187;
    wire n_306_287;
    wire n_406_287;
    wire n_406_407;
    wire n_516_287;
    wire n_516_407;
    (* S0_x1=56, S0_y1=217, S0_x2=56, S0_y2=277, qucs_mirroredX=0, qucs_rotated=1 *) Vac #(.U(150 V),.f(1 GHz),.Phase(0),.Theta(0)) V1 ( n_56_217, n_56_277 );
    (* S0_x1=56, S0_y1=437, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_56_437 );
    (* S0_x1=276, S0_y1=287, S0_x2=336, S0_y2=287, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(15 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_276_287, n_336_287 );
    (* S0_x1=376, S0_y1=287, S0_x2=436, S0_y2=287, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(6 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_376_287, n_436_287 );
    (* S0_x1=486, S0_y1=287, S0_x2=546, S0_y2=287, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(8 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R3 ( n_486_287, n_546_287 );
    (* S0_x1=486, S0_y1=407, S0_x2=546, S0_y2=407, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(177 uF),.V(),.Symbol(neutral)) C1 ( n_486_407, n_546_407 );
    (* S0_x1=156, S0_y1=187, S0_x2=216, S0_y2=187, S0_x3=176, S0_y3=207, S0_x4=196, S0_y4=207, qucs_mirroredX=0, qucs_rotated=0 *) WProbe #() PF ( n_156_187, n_216_187, n_176_207, n_196_207 );
    (* S0_x1=376, S0_y1=407, S0_x2=436, S0_y2=407, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(0.1 H),.I()) L1 ( n_376_407, n_436_407 );
endmodule
