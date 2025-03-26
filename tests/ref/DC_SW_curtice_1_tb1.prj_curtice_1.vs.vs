(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="curtice_1.dat", qucs_DataDisplay="curtice_1.dpl", qucs_SimOpenDpl=1, qucs_Script="curtice_1.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module curtice_1(.Source(n_0_0), .Drain(n_0_0), .Gate(n_0_0));
    inout n_0_0;
    inout n_0_0;
    inout n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(RS),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) Rs ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) L #(.L(LS),.I()) Ls ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) Port #(.Num(1),.Type(analog)) Source ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(RIN),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) Rin ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) EDD #(.Type(explicit),.Branches(1),.I1(0),.Q1(CGD*V1)) D2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) Port #(.Num(3),.Type(analog)) Drain ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(RD),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) Rd ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) L #(.L(LD),.I()) Ld ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(RG),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) Rg ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) L #(.L(LG),.I()) Lg ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) Port #(.Num(2),.Type(analog)) Gate ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) EDD #(.Type(explicit),.Branches(4),.I1(V1<-VBR+50*Vt ? -IsT*(1+exp(-(VBR+V1)/Vt)) + GMIN*V1 : 0),.Q1(0)) D1 ( n_m30_0, n_30_0 );
endmodule
