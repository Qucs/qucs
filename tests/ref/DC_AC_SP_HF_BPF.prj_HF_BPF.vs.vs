(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="HF_BPF.dat", qucs_DataDisplay="HF_BPF.dpl", qucs_SimOpenDpl=1, qucs_Script="HF_BPF.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module HF_BPF();
    wire n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) INDQ #(.L(1.82uH),.Q(Ql),.f(f0),.Mode(SquareRoot),.Temp(26.85)) INDQ1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) INDQ #(.L(221.8nH),.Q(Ql),.f(f0),.Mode(SquareRoot),.Temp(26.85)) INDQ3 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) CAPQ #(.C(560 pF),.Q(Qc),.f(f0),.Mode(Linear),.Temp(26.85)) CAPQ3 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Pac #(.Num(2),.Z(50 Ohm),.P(0 W),.f(1 GHz),.Temp(-273.15)) P1 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) INDQ #(.L(221.8nH),.Q(Ql),.f(f0),.Mode(SquareRoot),.Temp(26.85)) INDQ2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) CAPQ #(.C(68 pF),.Q(Qc),.f(f0),.Mode(Linear),.Temp(26.85)) CAPQ1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) CAPQ #(.C(560 pF),.Q(Qc),.f(f0),.Mode(Linear),.Temp(26.85)) CAPQ2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0 *) Eqn #(.y(1),.Export(yes)) Eqn2 (  );
    (* qucs_mirrored=0, qucs_rotated=0 *) Eqn #(.y(1),.Export(yes)) Eqn1 (  );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Pac #(.Num(1),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(-273.15)) P2 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Vdc #(.U(1.1 V)) V1 ( n_0_m30, n_0_30 );
endmodule
