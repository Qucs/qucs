(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="timer_amp.dat", qucs_DataDisplay="timer_amp.dpl", qucs_SimOpenDpl=1, qucs_Script="timer_amp.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module timer_amp(.P_vout(n_365_265), .Pamp_P(n_210_265), .Pamp_N(n_210_325));
    inout n_365_265;
    inout n_210_265;
    inout n_210_325;
    wire n_240_260;
    wire n_305_265;
    wire n_310_325;
    (* S0_x1=310, S0_y1=325, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_310_325 );
    (* S0_x1=365, S0_y1=265, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(3),.Type(analog)) P_vout ( n_365_265 );
    (* S0_x1=275, S0_y1=265, S0_x2=335, S0_y2=265, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(7),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_275_265, n_335_265 );
    (* S0_x1=210, S0_y1=265, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(1),.Type(analog)) Pamp_P ( n_210_265 );
    (* S0_x1=210, S0_y1=325, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(2),.Type(analog)) Pamp_N ( n_210_325 );
    (* S0_x1=210, S0_y1=230, S0_x2=270, S0_y2=230, S0_x3=270, S0_y3=290, S0_x4=210, S0_y4=290, qucs_mirroredX=0, qucs_rotated=0 *) VCVS #(.G(3.7),.T(0)) SRC1 ( n_210_230, n_270_230, n_270_290, n_210_290 );
endmodule
