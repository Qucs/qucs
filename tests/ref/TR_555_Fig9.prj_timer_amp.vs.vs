(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="timer_amp.dat", qucs_DataDisplay="timer_amp.dpl", qucs_SimOpenDpl=1, qucs_Script="timer_amp.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module timer_amp(.P_vout(n_365_265), .Pamp_P(n_210_265), .Pamp_N(n_210_325));
    inout n_365_265;
    inout n_210_265;
    inout n_210_325;
    wire n_270_290;
    wire n_310_325;
    wire n_335_265;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=310, S0_y1=325 *) GND #() \\*  ( n_310_325 );
    (* qucs_mirrored=1, qucs_rotated=2, qucs_Num=3, qucs_Type="analog", S0_x1=365, S0_y1=265 *) Port #() P_vout ( n_365_265 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=305, S0_y1=265, S0_x2=365, S0_y2=265 *) R #(.R(7),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_305_265, n_365_265 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_Num=1, qucs_Type="analog", S0_x1=210, S0_y1=265 *) Port #() Pamp_P ( n_210_265 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_Num=2, qucs_Type="analog", S0_x1=210, S0_y1=325 *) Port #() Pamp_N ( n_210_325 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=240, S0_y1=260, S0_x2=300, S0_y2=260, S0_x3=300, S0_y3=320, S0_x4=240, S0_y4=320 *) VCVS #(.G(3.7),.T(0)) SRC1 ( n_240_260, n_300_260, n_300_320, n_240_320 );
endmodule
