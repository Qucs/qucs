// VS::printSubckt main
module main(P1, P2);
Port #(.Num(2), .Type(analog), .$ty(4), .$tx(4), .$xposition(400), .$yposition(180), .$vflip(1), .$hflip(1)) P2(P2, net_400_180);
Port #(.Num(1), .Type(analog), .$ty(-23), .$tx(-23), .$xposition(180), .$yposition(180), .$vflip(-1), .$hflip(1)) P1(P1, net_180_180);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US), .$ty(-26), .$tx(-26), .$xposition(290), .$yposition(260), .$vflip(1), .$hflip(1)) R1(net_260_260, net_320_260);
GND #(.$xposition(380), .$yposition(360), .$vflip(1), .$hflip(1)) anonymous_gnd_hack_0(net_380_360);
C #(.C(1 pF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(380), .$yposition(280), .$vflip(1), .$hflip(1)) C1(net_380_310, net_380_250);
wire #(.$xposition(180), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_180_180, net_240_180);
wire #(.$xposition(240), .$yposition(260), .$vflip(1), .$hflip(1)) noname(net_240_260, net_260_260);
wire #(.$xposition(240), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_240_180, net_240_260);
wire #(.$xposition(320), .$yposition(260), .$vflip(1), .$hflip(1)) noname(net_320_260, net_340_260);
wire #(.$xposition(340), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_340_180, net_380_180);
wire #(.$xposition(340), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_340_180, net_340_260);
wire #(.$xposition(240), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_240_180, net_340_180);
wire #(.$xposition(380), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_380_180, net_400_180);
wire #(.$xposition(380), .$yposition(180), .$vflip(1), .$hflip(1)) noname(net_380_180, net_380_250);
wire #(.$xposition(380), .$yposition(310), .$vflip(1), .$hflip(1)) noname(net_380_310, net_380_360);
place #(.$xposition(400), .$yposition(180)) net_400_180(net_400_180);
place #(.$xposition(180), .$yposition(180)) net_180_180(net_180_180);
place #(.$xposition(260), .$yposition(260)) net_260_260(net_260_260);
place #(.$xposition(320), .$yposition(260)) net_320_260(net_320_260);
place #(.$xposition(380), .$yposition(360)) net_380_360(net_380_360);
place #(.$xposition(380), .$yposition(310)) net_380_310(net_380_310);
place #(.$xposition(380), .$yposition(250)) net_380_250(net_380_250);
place #(.$xposition(240), .$yposition(180)) net_240_180(net_240_180);
place #(.$xposition(240), .$yposition(260)) net_240_260(net_240_260);
place #(.$xposition(340), .$yposition(260)) net_340_260(net_340_260);
place #(.$xposition(340), .$yposition(180)) net_340_180(net_340_180);
place #(.$xposition(380), .$yposition(180)) net_380_180(net_380_180);
endmodule // main

