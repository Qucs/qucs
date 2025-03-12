(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="short_circuit.dat", qucs_DataDisplay="short_circuit.dpl", qucs_SimOpenDpl=1, qucs_Script="short_circuit.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module short_circuit(.P2(n_400_180), .P1(n_180_180));
    inout n_400_180;
    inout n_180_180;
    wire n_260_260;
    wire n_380_310;
    wire n_380_360;
    (* S0_x1=400, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(2),.Type(analog)) P2 ( n_400_180 );
    (* S0_x1=180, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(1),.Type(analog)) P1 ( n_180_180 );
    (* S0_x1=230, S0_y1=260, S0_x2=290, S0_y2=260, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(0 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_230_260, n_290_260 );
    (* S0_x1=380, S0_y1=360, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_380_360 );
    (* S0_x1=350, S0_y1=310, S0_x2=410, S0_y2=310, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(1 pF),.V(),.Symbol(neutral)) C1 ( n_350_310, n_410_310 );
endmodule
