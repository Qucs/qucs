(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="curtice_1_tb1.dat", qucs_DataDisplay="curtice_1_tb1.dpl", qucs_SimOpenDpl=1, qucs_Script="curtice_1_tb1.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module curtice_1_tb1();
    wire n_0_0;
    wire n_80_130;
    wire n_80_190;
    wire n_210_160;
    wire n_350_60;
    wire n_380_90;
    wire n_380_160;
    (* S0_x1=80, S0_y1=100, S0_x2=80, S0_y2=160, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(Vgs)) V2 ( n_80_100, n_80_160 );
    (* S0_x1=80, S0_y1=190, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_80_190 );
    (* S0_x1=380, S0_y1=160, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_380_160 );
    (* S0_x1=380, S0_y1=60, S0_x2=380, S0_y2=120, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(Vds)) V1 ( n_380_60, n_380_120 );
    (* S0_x1=210, S0_y1=160, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_210_160 );
    (* S0_x1=320, S0_y1=60, S0_x2=380, S0_y2=60, qucs_mirroredX=0, qucs_rotated=0 *) IProbe #() Pr1 ( n_320_60, n_380_60 );
    (* , qucs_mirroredX=0, qucs_rotated=0 *) Sub #(.File(curtice_1.sch)) Curtice1 (  );
endmodule
