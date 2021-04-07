// VS::printSubckt main
module main(P1, P2, P3, P4, P5);
sTr #(.$xposition(380), .$yposition(210), .$vflip(-1), .$hflip(1), .$tx(-29), .$ty(-29), .T1(0.5), .T2(0.5)) Tr1(net_410_140, net_350_180, net_350_240, net_410_280, net_410_220, net_410_200);
Port #(.$xposition(460), .$yposition(280), .$vflip(-1), .$hflip(1), .$tx(4), .$ty(4), .Num(3), .Type(analog)) P3(P3, net_460_280);
Port #(.$xposition(460), .$yposition(140), .$vflip(-1), .$hflip(1), .$tx(4), .$ty(4), .Num(4), .Type(analog)) P4(P4, net_460_140);
Port #(.$xposition(340), .$yposition(390), .$vflip(1), .$hflip(1), .$tx(12), .$ty(12), .Num(5), .Type(analog)) P5(P5, net_340_390);
Port #(.$xposition(260), .$yposition(360), .$vflip(1), .$hflip(1), .$tx(-23), .$ty(-23), .Num(2), .Type(analog)) P2(P2, net_260_360);
Port #(.$xposition(260), .$yposition(180), .$vflip(1), .$hflip(1), .$tx(-23), .$ty(-23), .Num(1), .Type(analog)) P1(P1, net_260_180);
wire #(.$xposition(410), .$yposition(220), .$vflip(1), .$hflip(1)) noname(net_410_220, net_430_220);
wire #(.$xposition(410), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_410_200, net_430_200);
wire #(.$xposition(430), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_430_200, net_430_220);
wire #(.$xposition(410), .$yposition(280), .$vflip(1), .$hflip(1)) noname(net_410_280, net_460_280);
wire #(.$xposition(410), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_410_140, net_460_140);
wire #(.$xposition(340), .$yposition(240), .$vflip(1), .$hflip(1)) noname(net_340_240, net_350_240);
wire #(.$xposition(340), .$yposition(240), .$vflip(1), .$hflip(1)) noname(net_340_240, net_340_390);
wire #(.$xposition(430), .$yposition(220), .$vflip(1), .$hflip(1)) noname(net_430_220, net_430_360);
wire #(.$xposition(260), .$yposition(360), .$vflip(1), .$hflip(1)) noname(net_260_360, net_430_360);
wire #(.$xposition(260), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_260_180, net_350_180);
place #(.$xposition(410), .$yposition(140)) net_410_140(net_410_140);
place #(.$xposition(350), .$yposition(180)) net_350_180(net_350_180);
place #(.$xposition(350), .$yposition(240)) net_350_240(net_350_240);
place #(.$xposition(410), .$yposition(280)) net_410_280(net_410_280);
place #(.$xposition(410), .$yposition(220)) net_410_220(net_410_220);
place #(.$xposition(410), .$yposition(200)) net_410_200(net_410_200);
place #(.$xposition(460), .$yposition(280)) net_460_280(net_460_280);
place #(.$xposition(460), .$yposition(140)) net_460_140(net_460_140);
place #(.$xposition(340), .$yposition(390)) net_340_390(net_340_390);
place #(.$xposition(260), .$yposition(360)) net_260_360(net_260_360);
place #(.$xposition(260), .$yposition(180)) net_260_180(net_260_180);
place #(.$xposition(430), .$yposition(220)) net_430_220(net_430_220);
place #(.$xposition(430), .$yposition(200)) net_430_200(net_430_200);
place #(.$xposition(340), .$yposition(240)) net_340_240(net_340_240);
place #(.$xposition(430), .$yposition(360)) net_430_360(net_430_360);
endmodule // main

