(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="fet_noise.dat", qucs_DataDisplay="fet_noise.dpl", qucs_SimOpenDpl=1, qucs_Script="fet_noise.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module fet_noise();
    wire n_0_0;
    wire n_40_150;
    wire n_40_210;
    wire n_100_110;
    wire n_180_210;
    wire n_240_130;
    wire n_400_110;
    wire n_400_170;
    (* S0_x1=240, S0_y1=130, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_240_130 );
    (* , qucs_mirroredX=0, qucs_rotated=0 *) Sub #(.File(fet.sch)) SUB1 (  );
    (* S0_x1=70, S0_y1=110, S0_x2=130, S0_y2=110, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(1.1 nH),.I()) L1 ( n_70_110, n_130_110 );
    (* S0_x1=150, S0_y1=210, S0_x2=210, S0_y2=210, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(4.0 nH),.I()) L2 ( n_150_210, n_210_210 );
    (* S0_x1=180, S0_y1=210, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_180_210 );
    (* S0_x1=40, S0_y1=210, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_40_210 );
    (* S0_x1=40, S0_y1=120, S0_x2=40, S0_y2=180, qucs_mirroredX=0, qucs_rotated=1 *) Pac #(.Num(1),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P1 ( n_40_120, n_40_180 );
    (* S0_x1=400, S0_y1=170, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_400_170 );
    (* S0_x1=400, S0_y1=80, S0_x2=400, S0_y2=140, qucs_mirroredX=0, qucs_rotated=1 *) Pac #(.Num(2),.Z(50 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P2 ( n_400_80, n_400_140 );
endmodule
