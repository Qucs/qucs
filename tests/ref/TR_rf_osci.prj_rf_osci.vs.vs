(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="rf_osci.dat", qucs_DataDisplay="rf_osci.dpl", qucs_SimOpenDpl=1, qucs_Script="rf_osci.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module rf_osci();
    wire n_140_160;
    wire n_140_220;
    wire n_290_250;
    wire n_320_180;
    wire n_360_380;
    wire n_400_180;
    wire n_480_180;
    wire n_510_250;
    (* S0_x1=450, S0_y1=180, S0_x2=510, S0_y2=180, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(10 nF),.V(),.Symbol(neutral)) C1 ( n_450_180, n_510_180 );
    (* S0_x1=360, S0_y1=380, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_360_380 );
    (* S0_x1=370, S0_y1=180, S0_x2=430, S0_y2=180, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(0.5 mH),.I()) L1 ( n_370_180, n_430_180 );
    (* S0_x1=330, S0_y1=380, S0_x2=390, S0_y2=380, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(50 kOhm),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_330_380, n_390_380 );
    (* S0_x1=260, S0_y1=250, S0_x2=290, S0_y2=220, S0_x3=290, S0_y3=280, qucs_mirroredX=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1),.Ikf(0),.Ikr(0),.Vaf(0),.Var(0),.Ise(0),.Ne(1.5),.Isc(0),.Nc(2),.Bf(200),.Br(1),.Rbm(0),.Irb(0),.Rc(0),.Re(0),.Rb(0),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(0.0),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(0.0),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_260_250, n_290_220, n_290_280 );
    (* S0_x1=480, S0_y1=250, S0_x2=510, S0_y2=220, S0_x3=510, S0_y3=280, qucs_mirroredX=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1),.Ikf(0),.Ikr(0),.Vaf(0),.Var(0),.Ise(0),.Ne(1.5),.Isc(0),.Nc(2),.Bf(200),.Br(1),.Rbm(0),.Irb(0),.Rc(0),.Re(0),.Rb(0),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(0.0),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(0.0),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T2 ( n_480_250, n_510_220, n_510_280 );
    (* S0_x1=140, S0_y1=220, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_140_220 );
    (* S0_x1=140, S0_y1=130, S0_x2=140, S0_y2=190, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(10 V)) V1 ( n_140_130, n_140_190 );
    (* S0_x1=290, S0_y1=180, S0_x2=350, S0_y2=180, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(200k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_290_180, n_350_180 );
endmodule
