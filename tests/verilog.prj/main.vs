(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=0, qucs_tmpViewY1=0, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="main.dat", qucs_DataDisplay="main.dpl", qucs_SimOpenDpl=1, qucs_Script="main.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module main();
    wire n_380_290;
    wire n_440_290;
    wire n_380_350;
    wire n_440_350;
    wire n_380_410;
    (* qucs_mirrored=0, qucs_rotated=0, qucs_type="Verilog", qucs_File="sub.v", qucs_visible="0", S0_x1=380, S0_y1=290, S0_x2=440, S0_y2=290, S0_x3=380, S0_y3=350, S0_x4=440, S0_y4=350, S0_x5=380, S0_y5=410 *) mysub #() X1 ( n_380_290, n_440_290, n_380_350, n_440_350, n_380_410 );
endmodule
