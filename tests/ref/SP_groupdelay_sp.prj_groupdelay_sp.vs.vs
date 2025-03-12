(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="groupdelay_sp.dat", qucs_DataDisplay="groupdelay_sp.dpl", qucs_SimOpenDpl=1, qucs_Script="groupdelay_sp.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module groupdelay_sp();
    wire n_110_150;
    wire n_110_210;
    wire n_220_210;
    wire n_260_100;
    wire n_360_210;
    wire n_500_160;
    wire n_500_220;
    (* S0_x1=110, S0_y1=210, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_110_210 );
    (* S0_x1=190, S0_y1=210, S0_x2=250, S0_y2=210, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(39.01pF),.V(),.Symbol(neutral)) C1 ( n_190_210, n_250_210 );
    (* S0_x1=220, S0_y1=210, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_220_210 );
    (* S0_x1=230, S0_y1=100, S0_x2=290, S0_y2=100, qucs_mirroredX=0, qucs_rotated=0 *) L #(.L(280.9nH),.I()) L1 ( n_230_100, n_290_100 );
    (* S0_x1=330, S0_y1=210, S0_x2=390, S0_y2=210, qucs_mirroredX=0, qucs_rotated=0 *) C #(.C(39.01pF),.V(),.Symbol(neutral)) C2 ( n_330_210, n_390_210 );
    (* S0_x1=360, S0_y1=210, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_360_210 );
    (* S0_x1=110, S0_y1=120, S0_x2=110, S0_y2=180, qucs_mirroredX=0, qucs_rotated=1 *) Pac #(.Num(1),.Z(60 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P1 ( n_110_120, n_110_180 );
    (* S0_x1=500, S0_y1=130, S0_x2=500, S0_y2=190, qucs_mirroredX=0, qucs_rotated=1 *) Pac #(.Num(2),.Z(60 Ohm),.P(0 dBm),.f(1 GHz),.Temp(26.85)) P2 ( n_500_130, n_500_190 );
    (* S0_x1=500, S0_y1=220, qucs_mirroredX=0, qucs_rotated=0 *) GND #() \\*  ( n_500_220 );
endmodule
