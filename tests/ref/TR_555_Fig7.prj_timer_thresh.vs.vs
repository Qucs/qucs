(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="timer_thresh.dat", qucs_DataDisplay="timer_thresh.dpl", qucs_SimOpenDpl=1, qucs_Script="timer_thresh.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module timer_thresh(.PinP(n_60_120), .PinN(n_60_180), .POUT(n_390_150));
    inout n_60_120;
    inout n_60_180;
    inout n_390_150;
    wire n_160_30;
    wire n_195_170;
    wire n_245_35;
    wire n_290_150;
    wire n_370_235;
    (* S0_x1=60, S0_y1=120, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(1),.Type(analog)) PinP ( n_60_120 );
    (* S0_x1=60, S0_y1=180, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(2),.Type(analog)) PinN ( n_60_180 );
    (* S0_x1=245, S0_y1=35, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_245_35 );
    (* S0_x1=160, S0_y1=0, S0_x2=160, S0_y2=60, qucs_mirroredX=0, qucs_rotated=1 *) Idc #(.I(0.1 uA)) I1 ( n_160_0, n_160_60 );
    (* S0_x1=260, S0_y1=150, S0_x2=320, S0_y2=150, qucs_mirroredX=0, qucs_rotated=0 *) R #(.R(1k),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) R4 ( n_260_150, n_320_150 );
    (* S0_x1=390, S0_y1=150, qucs_mirroredX=0, qucs_rotated=0 *) Port #(.Num(3),.Type(analog)) POUT ( n_390_150 );
    (* S0_x1=370, S0_y1=235, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_370_235 );
    (* S0_x1=165, S0_y1=190, S0_x2=165, S0_y2=150, S0_x3=235, S0_y3=170, qucs_mirroredX=0, qucs_rotated=0 *) OpAmp #(.G(1e6),.Umax(1 V)) OP1 ( n_165_190, n_165_150, n_235_170 );
    (* S0_x1=340, S0_y1=235, S0_x2=400, S0_y2=235, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(1 nF),.V(),.Symbol(neutral)) C1 ( n_340_235, n_400_235 );
endmodule
