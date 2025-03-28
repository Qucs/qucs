(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="wattmeter.dat", qucs_DataDisplay="wattmeter.dpl", qucs_SimOpenDpl=1, qucs_Script="wattmeter.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module wattmeter();
    wire n_56_277;
    wire n_56_437;
    wire n_216_187;
    wire n_306_257;
    wire n_406_257;
    wire n_406_377;
    wire n_516_257;
    wire n_516_377;
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=56, S0_y1=247, S0_x2=56, S0_y2=307 *) Vac #(.U(150 V),.f(1 GHz),.Phase(0),.Theta(0)) V1 ( n_56_247, n_56_307 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=56, S0_y1=437 *) GND #() \\*  ( n_56_437 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=306, S0_y1=287, S0_x2=306, S0_y2=227 *) R #(.R(15 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_306_287, n_306_227 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=406, S0_y1=287, S0_x2=406, S0_y2=227 *) R #(.R(6 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_406_287, n_406_227 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=516, S0_y1=287, S0_x2=516, S0_y2=227 *) R #(.R(8 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R3 ( n_516_287, n_516_227 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=516, S0_y1=407, S0_x2=516, S0_y2=347 *) C #(.C(177 uF),.V()) C1 ( n_516_407, n_516_347 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=186, S0_y1=187, S0_x2=246, S0_y2=187, S0_x3=206, S0_y3=207, S0_x4=226, S0_y4=207 *) WProbe #() PF ( n_186_187, n_246_187, n_206_207, n_226_207 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=406, S0_y1=407, S0_x2=406, S0_y2=347 *) L #(.L(0.1 H),.I()) L1 ( n_406_407, n_406_347 );
endmodule
