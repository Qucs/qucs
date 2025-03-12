(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="colpitts_base.dat", qucs_DataDisplay="colpitts_base.dpl", qucs_SimOpenDpl=1, qucs_Script="colpitts_base.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module colpitts_base();
    wire n_80_180;
    wire n_160_260;
    wire n_190_240;
    wire n_190_280;
    wire n_280_260;
    wire n_300_220;
    wire n_340_180;
    wire n_340_260;
    (* S0_x1=270, S0_y1=220, S0_x2=330, S0_y2=220, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(5u),.I()) L1 ( n_270_220, n_330_220 );
    (* S0_x1=280, S0_y1=230, S0_x2=280, S0_y2=290, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(5 V)) Vp ( n_280_230, n_280_290 );
    (* S0_x1=160, S0_y1=230, S0_x2=160, S0_y2=290, qucs_mirroredX=0, qucs_rotated=1 *) Vdc #(.U(5 V)) Vn ( n_160_230, n_160_290 );
    (* S0_x1=190, S0_y1=280, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_190_280 );
    (* S0_x1=160, S0_y1=240, S0_x2=190, S0_y2=210, S0_x3=190, S0_y3=270, qucs_mirroredX=0, qucs_rotated=0 *) _BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1),.Ikf(0),.Ikr(0),.Vaf(0),.Var(0),.Ise(0),.Ne(1.5),.Isc(0),.Nc(2),.Bf(100),.Br(1),.Rbm(0),.Irb(0),.Rc(0),.Re(0),.Rb(100),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(0.0),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(0.0),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_160_240, n_190_210, n_190_270 );
    (* S0_x1=310, S0_y1=180, S0_x2=370, S0_y2=180, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(200 pF),.V(),.Symbol(neutral)) C2 ( n_310_180, n_370_180 );
    (* S0_x1=310, S0_y1=260, S0_x2=370, S0_y2=260, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(200p),.V(),.Symbol(neutral)) C1 ( n_310_260, n_370_260 );
    (* S0_x1=50, S0_y1=180, S0_x2=110, S0_y2=180, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(2.2k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_50_180, n_110_180 );
endmodule
