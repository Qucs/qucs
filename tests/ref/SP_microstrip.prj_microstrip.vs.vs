(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="microstrip.dat", qucs_DataDisplay="microstrip.dpl", qucs_SimOpenDpl=1, qucs_Script="microstrip.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module microstrip();
    wire n_0_0;
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Pac #(.Num(3),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P3 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Pac #(.Num(2),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P2 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Pac #(.Num(1),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P1 ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) C #(.C(30 pF),.V(),.Symbol(neutral)) C1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) MLIN #(.Subst(Aluminia),.W(1 mm),.L(10 mm),.Model(Hammerstad),.DispModel(Kirschning),.Temp(26.85)) MS1 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) MLIN #(.Subst(Aluminia),.W(1 mm),.L(10 mm),.Model(Hammerstad),.DispModel(Kirschning),.Temp(26.85)) MS2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0 *) SUBST #(.er(9.8),.h(1 mm),.t(35 um),.tand(1e-3),.rho(0.022e-6),.D(0.15e-6)) Aluminia (  );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, S0_x3=0, S0_y3=30 *) MTEE #(.Subst(Aluminia),.W1(1 mm),.W2(1 mm),.W3(0.5 mm),.MSModel(Hammerstad),.MSDispModel(Kirschning),.Temp(26.85),.Symbol(showNumbers)) MS4 ( n_m30_0, n_30_0, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) MLIN #(.Subst(Aluminia),.W(0.5 mm),.L(10 mm),.Model(Hammerstad),.DispModel(Kirschning),.Temp(26.85)) Stub ( n_m30_0, n_30_0 );
endmodule
