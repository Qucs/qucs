(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="timer_Discharge.dat", qucs_DataDisplay="timer_Discharge.dpl", qucs_SimOpenDpl=1, qucs_Script="timer_Discharge.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module timer_Discharge(.P_control_in(n_255_150), .P_GND(n_260_260), .P_Discharge(n_260_320));
    inout n_255_150;
    inout n_260_260;
    inout n_260_320;
    wire n_350_150;
    wire n_370_320;
    wire n_410_150;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=320, S0_y1=150, S0_x2=380, S0_y2=150 *) R #(.R(10K),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R1 ( n_320_150, n_380_150 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_Num=1, qucs_Type="analog", S0_x1=255, S0_y1=150 *) Port #() P_control_in ( n_255_150 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_Num=3, qucs_Type="analog", S0_x1=260, S0_y1=260 *) Port #() P_GND ( n_260_260 );
    (* qucs_mirrored=0, qucs_rotated=0, qucs_Num=2, qucs_Type="analog", S0_x1=260, S0_y1=320 *) Port #() P_Discharge ( n_260_320 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=380, S0_y1=150, S0_x2=410, S0_y2=120, S0_x3=410, S0_y3=180 *) _BJT #(.Type(npn),.Is(1e-16),.Nf(1),.Nr(1),.Ikf(0),.Ikr(0),.Vaf(0),.Var(0),.Ise(0),.Ne(1.5),.Isc(0),.Nc(2),.Bf(100),.Br(1),.Rbm(0),.Irb(0),.Rc(0),.Re(0),.Rb(0),.Cje(0),.Vje(0.75),.Mje(0.33),.Cjc(0),.Vjc(0.75),.Mjc(0.33),.Xcjc(1.0),.Cjs(0),.Vjs(0.75),.Mjs(0),.Fc(0.5),.Tf(1e-9),.Xtf(0.0),.Vtf(0.0),.Itf(0.0),.Tr(0.0),.Temp(26.85),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Kb(0.0),.Ab(1.0),.Fb(1.0),.Ptf(0.0),.Xtb(0.0),.Xti(3.0),.Eg(1.11),.Tnom(26.85),.Area(1.0)) T1 ( n_380_150, n_410_120, n_410_180 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=340, S0_y1=320, S0_x2=400, S0_y2=320 *) R #(.R(200),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R2 ( n_340_320, n_400_320 );
endmodule
