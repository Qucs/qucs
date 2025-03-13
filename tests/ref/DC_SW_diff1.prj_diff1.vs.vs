(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="diff1.dat", qucs_DataDisplay="diff1.dpl", qucs_SimOpenDpl=1, qucs_Script="diff1.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module diff1();
    wire n_0_0;
    (* S0_x1=-30, S0_y1=0, S0_x2=0, S0_y2=-30, S0_x3=0, S0_y3=30, qucs_mirrored=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(6.734f),.Nf(1),.Nr(1),.Ikf(66.78m),.Ikr(0),.Vaf(74.03),.Var(0),.Ise(6.734f),.Ne(1.259),.Isc(0),.Nc(2),.Bf(416.4),.Br(0.7371),.Rbm(0),.Irb(0),.Rc(1),.Re(0),.Rb(10),.Cje(4.493p),.Vje(0.75),.Mje(0.2593),.Cjc(3.638p),.Vjc(0.75),.Mjc(0.3085),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(301.2p),.Xtf(2),.Vtf(4),.Itf(0.4),.Tr(239.5n),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) Q2N3904_1 ( n_m30_0, n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=0, S0_y2=-30, S0_x3=0, S0_y3=30, qucs_mirrored=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(6.734f),.Nf(1),.Nr(1),.Ikf(66.78m),.Ikr(0),.Vaf(74.03),.Var(0),.Ise(6.734f),.Ne(1.259),.Isc(0),.Nc(2),.Bf(416.4),.Br(0.7371),.Rbm(0),.Irb(0),.Rc(1),.Re(0),.Rb(10),.Cje(4.493p),.Vje(0.75),.Mje(0.2593),.Cjc(3.638p),.Vjc(0.75),.Mjc(0.3085),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(301.2p),.Xtf(2),.Vtf(4),.Itf(0.4),.Tr(239.5n),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) Q2N3904_2 ( n_m30_0, n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(2k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RC1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(2k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RC2 ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Vdc #(.U(+12)) VCC ( n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(Re),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RE1 ( n_m30_0, n_30_0 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(Re),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RE2 ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Vdc #(.U(Vin)) VIN ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Vdc #(.U(Vin)) VIN1 ( n_0_m30, n_0_30 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Idc #(.I(0)) IEE ( n_0_m30, n_0_30 );
    (* S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0, qucs_mirrored=0, qucs_rotated=0 *) R #(.R(1.5k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) REE ( n_m30_0, n_30_0 );
    (* S0_x1=0, S0_y1=0, qucs_mirrored=0, qucs_rotated=0 *) GND #() \\*  ( n_0_0 );
    (* S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30, qucs_mirrored=0, qucs_rotated=1 *) Vdc #(.U(-12)) VEE ( n_0_m30, n_0_30 );
endmodule
