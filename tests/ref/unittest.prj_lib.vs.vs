(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="lib.dat", qucs_DataDisplay="lib.dpl", qucs_SimOpenDpl=1, qucs_Script="lib.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module lib();
    wire n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, qucs_type="Lib", S0_x1=0, S0_y1=0 *) foo #(.Lib(),.Comp(foo)) inst ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_type="Lib", S0_x1=0, S0_y1=0 *) Lib #(.Lib(),.Comp(Lib)) inst ( n_0_0 );
endmodule
