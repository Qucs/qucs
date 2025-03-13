(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="mscoupler.dat", qucs_DataDisplay="mscoupler.dpl", qucs_SimOpenDpl=1, qucs_Script="mscoupler.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module mscoupler();
    wire n_0_0;
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(1),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P1 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(2),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P2 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(3),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P3 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Pac #(.Num(4),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P4 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0 *) SUBST #(.er(9.8),.h(0.635 mm),.t(17.5 um),.tand(0.0001),.rho(2.43902e-08),.D(1.5e-07)) SubstTC1 (  );
    (* S0_x1=-30, S0_y1=-30, S0_x2=30, S0_y2=-30, S0_x3=30, S0_y3=30, S0_x4=-30, S0_y4=30, qucs_mirrored=0, qucs_rotated=0 *) MCOUPLED #(.Subst(SubstTC1),.W(0.518 mm),.L(14.94 mm),.S(0.185 mm),.Model(Kirschning),.DispModel(Kirschning),.Temp(26.85)) MSTC1 ( n_m30_m30, n_30_m30, n_30_30, n_m30_30 );
endmodule
