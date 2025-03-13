(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="Spiral_BPF.dat", qucs_DataDisplay="Spiral_BPF.dpl", qucs_SimOpenDpl=1, qucs_Script="Spiral_BPF.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module Spiral_BPF();
    wire n_0_0;
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(195 fF),.V(),.Symbol(neutral)) C1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) SPIRALIND #(.Subst(Subst1),.Geometry(Circular),.W(25 um),.Di(91 um),.S(25 um),.N(2),.Temp(26.85)) SPIRALIND1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(155 fF),.V(),.Symbol(neutral)) C2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0 *) SUBST #(.er(4.5),.h(0.8 mm),.t(35 um),.tand(2e-4),.rho(0.022e-6),.D(0.15e-6)) Subst1 (  );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(320 fF),.V(),.Symbol(neutral)) C3 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(82 fF),.V(),.Symbol(neutral)) C5 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) CIRCULARLOOP #(.Subst(Subst1),.W(25 um),.a(195 um),.Temp(26.85)) CIRCULARLOOP1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) C #(.C(600 fF),.V(),.Symbol(neutral)) C4 ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(2),.Z(50 Ohm),.P(0 W),.f(1 GHz),.Temp(-273.15)) P2 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(1),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(-273.15)) P1 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Vdc #(.U(1.1 V)) V1 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
endmodule
