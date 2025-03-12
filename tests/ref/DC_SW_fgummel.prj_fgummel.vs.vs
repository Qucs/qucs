(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="fgummel.dat", qucs_DataDisplay="fgummel.dpl", qucs_SimOpenDpl=1, qucs_Script="fgummel.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module fgummel();
    wire n_40_130;
    wire n_40_190;
    wire n_150_130;
    wire n_180_190;
    wire n_270_130;
    wire n_270_190;
    wire n_360_130;
    wire n_360_190;
    (* S0_x1=40, S0_y1=100, S0_x2=40, S0_y2=160, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(Vbe)) VBE ( n_40_100, n_40_160 );
    (* S0_x1=40, S0_y1=190, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_40_190 );
    (* S0_x1=180, S0_y1=190, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_180_190 );
    (* S0_x1=360, S0_y1=100, S0_x2=360, S0_y2=160, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(Vbe)) VCE ( n_360_100, n_360_160 );
    (* S0_x1=360, S0_y1=190, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_360_190 );
    (* S0_x1=270, S0_y1=100, S0_x2=270, S0_y2=160, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(0 V)) VSUB ( n_270_100, n_270_160 );
    (* S0_x1=270, S0_y1=190, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_270_190 );
    (* S0_x1=120, S0_y1=130, S0_x2=150, S0_y2=100, S0_x3=150, S0_y3=160, S0_x4=180, S0_y4=130, qucs_mirroredX=0, qucs_rotated=0 *) BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1.3),.Ikf(40mA),.Ikr(1mA),.Vaf(40),.Var(30),.Ise(1e-16),.Ne(1.5),.Isc(1e-15),.Nc(2),.Bf(100),.Br(3.5),.Rbm(1.1),.Irb(1.5),.Rc(4),.Re(1),.Rb(5),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(2.5p),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(12p),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_120_130, n_150_100, n_150_160, n_180_130 );
endmodule
