(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="rgummel.dat", qucs_DataDisplay="rgummel.dpl", qucs_SimOpenDpl=1, qucs_Script="rgummel.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module rgummel();
    wire n_60_180;
    wire n_170_120;
    wire n_200_180;
    wire n_290_120;
    wire n_290_180;
    wire n_380_120;
    wire n_380_180;
    (* S0_x1=200, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_200_180 );
    (* S0_x1=380, S0_y1=90, S0_x2=380, S0_y2=150, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(Vce)) VCE ( n_380_90, n_380_150 );
    (* S0_x1=380, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_380_180 );
    (* S0_x1=290, S0_y1=90, S0_x2=290, S0_y2=150, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(0 V)) VSUB ( n_290_90, n_290_150 );
    (* S0_x1=290, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_290_180 );
    (* S0_x1=60, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_60_180 );
    (* S0_x1=140, S0_y1=120, S0_x2=170, S0_y2=90, S0_x3=170, S0_y3=150, S0_x4=200, S0_y4=120, qucs_mirroredX=0, qucs_rotated=0 *) BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1.3),.Ikf(40mA),.Ikr(1mA),.Vaf(40),.Var(30),.Ise(1e-16),.Ne(1.5),.Isc(1e-15),.Nc(2),.Bf(100),.Br(3.5),.Rbm(1.1),.Irb(1.5),.Rc(4),.Re(1),.Rb(5),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(2.5p),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(12p),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_140_120, n_170_90, n_170_150, n_200_120 );
    (* S0_x1=60, S0_y1=150, S0_x2=60, S0_y2=210, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(0 V)) VBE ( n_60_150, n_60_210 );
endmodule
