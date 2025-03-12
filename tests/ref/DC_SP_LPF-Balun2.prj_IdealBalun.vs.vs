(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="IdealBalun.dat", qucs_DataDisplay="IdealBalun.dpl", qucs_SimOpenDpl=1, qucs_Script="IdealBalun.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module IdealBalun(.P3(n_460_280), .P4(n_460_140), .P5(n_340_390), .P2(n_260_360), .P1(n_260_180));
    inout n_460_280;
    inout n_460_140;
    inout n_340_390;
    inout n_260_360;
    inout n_260_180;
    wire n_410_140;
    (* S0_x1=380, S0_y1=70, S0_x2=440, S0_y2=110, S0_x3=440, S0_y3=170, S0_x4=380, S0_y4=210, S0_x5=380, S0_y5=150, S0_x6=380, S0_y6=130, qucs_mirroredX=0, qucs_rotated=0 *) sTr #(.T1(0.5),.T2(0.5)) Tr1 ( n_380_70, n_440_110, n_440_170, n_380_210, n_380_150, n_380_130 );
    (* S0_x1=460, S0_y1=280, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(3),.Type(analog)) P3 ( n_460_280 );
    (* S0_x1=460, S0_y1=140, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(4),.Type(analog)) P4 ( n_460_140 );
    (* S0_x1=340, S0_y1=390, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(5),.Type(analog)) P5 ( n_340_390 );
    (* S0_x1=260, S0_y1=360, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(2),.Type(analog)) P2 ( n_260_360 );
    (* S0_x1=260, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(1),.Type(analog)) P1 ( n_260_180 );
endmodule
