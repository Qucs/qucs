(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="curtice_1_tb1.dat", qucs_DataDisplay="curtice_1_tb1.dpl", qucs_SimOpenDpl=1, qucs_Script="curtice_1_tb1.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module curtice_1_tb1();
    wire n_0_0;
    wire n_80_160;
    wire n_80_190;
    wire n_210_160;
    wire n_320_60;
    wire n_380_120;
    wire n_380_160;
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=80, S0_y1=130, S0_x2=80, S0_y2=190 *) Vdc #(.U(Vgs)) V2 ( n_80_130, n_80_190 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=80, S0_y1=190 *) GND #() \\*  ( n_80_190 );
    (* qucs_mirrored=0, qucs_rotated=0 *) Eqn #(.y(1),.Export(yes)) Eqn1 (  );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=380, S0_y1=160 *) GND #() \\*  ( n_380_160 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=380, S0_y1=90, S0_x2=380, S0_y2=150 *) Vdc #(.U(Vds)) V1 ( n_380_90, n_380_150 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=210, S0_y1=160 *) GND #() \\*  ( n_210_160 );
    (* qucs_mirrored=1, qucs_rotated=2, S0_x1=350, S0_y1=60, S0_x2=290, S0_y2=60 *) IProbe #() Pr1 ( n_350_60, n_290_60 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_type="Sub", qucs_file="curtice_1.sch" *) curtice_1 #() Curtice1 (  );
endmodule
