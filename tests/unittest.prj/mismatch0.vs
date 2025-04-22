(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=1010, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=0, qucs_tmpViewY1=0, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="ee.dat", qucs_DataDisplay="ee.dpl", qucs_SimOpenDpl=1, qucs_Script="ee.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module ee();
    wire a;
    wire c;
    wire b;
    (* S0_x1=780, S0_y1=580, S0_x2=1010, S0_y2=580 *) net #() net1 ( a, b );
    (* S0_x1=1010, S0_y1=440, S0_x2=9999, S0_y2=580 *) net #() net2 ( c, b );
endmodule
