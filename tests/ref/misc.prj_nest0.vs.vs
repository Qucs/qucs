(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="nest0.dat", qucs_DataDisplay="nest0.dpl", qucs_SimOpenDpl=1, qucs_Script="nest0.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module nest0();
    wire n_0_0;
    wire n_140_250;
    wire n_140_320;
    wire n_340_250;
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=140, S0_y1=220, S0_x2=140, S0_y2=280 *) Vdc #(.U(1 V)) V1 ( n_140_220, n_140_280 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=140, S0_y1=320 *) GND #() \\*  ( n_140_320 );
    (* qucs_mirrored=1, qucs_rotated=3, S0_x1=340, S0_y1=220, S0_x2=340, S0_y2=280 *) R #(.R(50 Ohm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_340_220, n_340_280 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_Type="Sub", qucs_File="nest1.sch" *) nest1 #() SC1 (  );
endmodule
