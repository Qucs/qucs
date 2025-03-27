(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="diode.dat", qucs_DataDisplay="diode.dpl", qucs_SimOpenDpl=1, qucs_Script="diode.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module diode();
    wire n_170_170;
    wire n_260_280;
    wire n_350_170;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=260, S0_y1=280 *) GND #() \\*  ( n_260_280 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=170, S0_y1=140, S0_x2=170, S0_y2=200 *) Idc #(.I(Idiode)) I1 ( n_170_140, n_170_200 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=350, S0_y1=200, S0_x2=350, S0_y2=140 *) Diode #(.Is(1e-15 A),.N(1),.Cj0(15 fF),.M(0.5),.Vj(0.7 V),.Fc(0.5),.Cp(0.0 fF),.Isr(0.0),.Nr(2.0),.Rs(0.0 Ohm),.Tt(0.0 ps),.Ikf(0),.Kf(0.0),.Af(1.0),.Ffe(1.0),.Bv(0),.Ibv(1 mA),.Temp(26.85),.Xti(3.0),.Eg(1.11),.Tbv(0.0),.Trs(0.0),.Ttt1(0.0),.Ttt2(0.0),.Tm1(0.0),.Tm2(0.0),.Tnom(26.85),.Area(1.0),.Symbol(normal)) D1 ( n_350_200, n_350_140 );
endmodule
