(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="charac.dat", qucs_DataDisplay="charac.dpl", qucs_SimOpenDpl=1, qucs_Script="charac.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module charac();
    wire n_210_190;
    wire n_210_250;
    wire n_290_180;
    wire n_320_210;
    wire n_460_140;
    wire n_500_160;
    wire n_500_220;
    (* S0_x1=430, S0_y1=140, S0_x2=490, S0_y2=140, qucs_mirroredX=0, qucs_rotated=0 *) IProbe #() Collector ( n_430_140, n_490_140 );
    (* S0_x1=500, S0_y1=220, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_500_220 );
    (* S0_x1=320, S0_y1=210, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_320_210 );
    (* S0_x1=210, S0_y1=250, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_210_250 );
    (* S0_x1=500, S0_y1=130, S0_x2=500, S0_y2=190, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(Uce)) V1 ( n_500_130, n_500_190 );
    (* S0_x1=210, S0_y1=160, S0_x2=210, S0_y2=220, qucs_mirroredX=0, qucs_rotated=1 *) Idc #(.I(Ib)) Basis ( n_210_160, n_210_220 );
    (* S0_x1=260, S0_y1=180, S0_x2=290, S0_y2=150, S0_x3=290, S0_y3=210, qucs_mirroredX=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1),.Ikf(0),.Ikr(0),.Vaf(10),.Var(0),.Ise(0),.Ne(1.5),.Isc(0),.Nc(2),.Bf(100),.Br(1),.Rbm(0),.Irb(0),.Rc(0),.Re(0),.Rb(0),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(0.0),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(0.0),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_260_180, n_290_150, n_290_210 );
endmodule
