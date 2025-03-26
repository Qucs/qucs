(* qucs_ViewX1=0, qucs_ViewY1=0, qucs_ViewX2=800, qucs_ViewY2=800, qucs_Scale=1, qucs_tmpViewX1=-200, qucs_tmpViewY1=-200, qucs_GridX=10, qucs_GridY=10, qucs_GridOn=1, qucs_DataSet="Puls3b.dat", qucs_DataDisplay="Puls3b.dpl", qucs_SimOpenDpl=1, qucs_Script="Puls3b.m", qucs_SimRunScript=0, qucs_showFrame=0, qucs_FrameText0="Title", qucs_FrameText1="Drawn By:", qucs_FrameText2="Date:", qucs_FrameText3="Revision:" *) module Puls3b();
    wire n_0_0;
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(200m),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RD ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=0, S0_y1=0 *) GND #() \\*  ( n_0_0 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Vdc #(.U(13.5V)) VDC ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=1, S0_x1=0, S0_y1=-30, S0_x2=0, S0_y2=30 *) Vdc #(.U(200V)) VGEN ( n_0_m30, n_0_30 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) C #(.C(40nF),.V(),.Symbol(neutral)) CS ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(400m),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RE ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) C #(.C(40nF),.V(),.Symbol(neutral)) CL ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) R #(.R(50),.Temp(26.85),.Tc1(0.0),.Tc2(0.0),.Tnom(26.85)) RI ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) Switch #(.init(off),.time(10ns),.Ron(0),.Roff(1e9),.Temp(26.85),.MaxDuration(1e-6),.Transition(spline)) S2 ( n_m30_0, n_30_0 );
    (* qucs_mirrored=0, qucs_rotated=0, S0_x1=-30, S0_y1=0, S0_x2=30, S0_y2=0 *) Switch #(.init(on),.time(9ns),.Ron(0),.Roff(1e9),.Temp(26.85),.MaxDuration(1e-6),.Transition(spline)) S1 ( n_m30_0, n_30_0 );
endmodule
