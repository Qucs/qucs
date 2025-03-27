(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="nest1.dat", qucs_DataDisplay="nest1.dpl", qucs_SimOpenDpl=1, qucs_Script="nest1.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module nest1(.P2(n_400_180), .P1(n_180_180));
    inout n_400_180;
    inout n_180_180;
    wire n_0_0;
    wire n_230_180;
    wire n_380_280;
    wire n_380_360;
    (* qucs_mirrored=0, qucs_rotated=2, S0_x1=400, S0_y1=180 *) Port #(.Num(2),.Type(analog)) P2 ( n_400_180 );
    (* qucs_mirrored=1, qucs_rotated=0, S0_x1=180, S0_y1=180 *) Port #(.Num(1),.Type(analog)) P1 ( n_180_180 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=380, S0_y1=360 *) GND #() \\*  ( n_380_360 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=380, S0_y1=310, S0_x2=380, S0_y2=250 *) C #(.C(1 pF),.V(),.Symbol(neutral)) C1 ( n_380_310, n_380_250 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_type="Sub", qucs_file="nest2.sch" *) nest2 #() SC2 (  );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=200, S0_y1=180, S0_x2=260, S0_y2=180 *) R #(.R(0 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_200_180, n_260_180 );
endmodule
