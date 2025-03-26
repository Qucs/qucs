(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="opa227.dat", qucs_DataDisplay="opa227.dpl", qucs_SimOpenDpl=1, qucs_Script="opa227.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module opa227(.P1(n_0_0), .P3(n_0_0), .P2(n_0_0));
    inout n_0_0;
    inout n_0_0;
    inout n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=-30, S0_x2=30, S0_y2=-30, S0_x3=30, S0_y3=30, S0_x4=-30, S0_y4=30 *) VCVS #(.G(1),.T(0)) SRC2 ( n_m30_m30, n_30_m30, n_30_30, n_m30_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(1 kOhm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) C #(.C(1.98944 mF),.V(),.Symbol(neutral)) C1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=-30, S0_x2=30, S0_y2=-30, S0_x3=30, S0_y3=30, S0_x4=-30, S0_y4=30 *) VCVS #(.G(1e8),.T(0)) SRC1 ( n_m30_m30, n_30_m30, n_30_30, n_m30_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) Port #(.Num(1),.Type(analog)) P1 ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) Port #(.Num(3),.Type(analog)) P3 ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) Port #(.Num(2),.Type(analog)) P2 ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
endmodule
