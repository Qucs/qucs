// VS::printSubckt main
module main();
Vdc #(.$xposition(140), .$yposition(250), .$vflip(1), .$hflip(1), .$tx(18), .$ty(18), .U(1 V)) V1(net_140_220, net_140_280);
GND #(.$xposition(140), .$yposition(320), .$vflip(1), .$hflip(1), .$tx(0), .$ty(0)) anonymous_gnd_hack_0(net_140_320);
R #(.$xposition(340), .$yposition(250), .$vflip(-1), .$hflip(1), .$tx(15), .$ty(15), .R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(net_340_220, net_340_280);
//DC1
Sub #(.$xposition(250), .$yposition(200), .$vflip(1), .$hflip(1), .$ty(-20), .$tx(14), .File(nest1.sch)) SC1(net_220_200, net_280_200);
wire #(.$xposition(140), .$yposition(280), .$vflip(1), .$hflip(1)) noname(net_140_280, net_140_300);
wire #(.$xposition(140), .$yposition(300), .$vflip(1), .$hflip(1)) noname(net_140_300, net_140_320);
wire #(.$xposition(140), .$yposition(300), .$vflip(1), .$hflip(1)) noname(net_140_300, net_340_300);
wire #(.$xposition(340), .$yposition(280), .$vflip(1), .$hflip(1)) noname(net_340_280, net_340_300);
wire #(.$xposition(140), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_140_200, net_140_220);
wire #(.$xposition(140), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_140_200, net_220_200);
wire #(.$xposition(340), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_340_200, net_340_220);
wire #(.$xposition(280), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_280_200, net_340_200);
wire #(.$xposition(340), .$yposition(200), .$vflip(1), .$hflip(1)) noname(net_340_200, net_340_200);
place #(.$xposition(140), .$yposition(220), .$vflip(1), .$hflip(1)) net_140_220(net_140_220);
place #(.$xposition(140), .$yposition(280), .$vflip(1), .$hflip(1)) net_140_280(net_140_280);
place #(.$xposition(140), .$yposition(320), .$vflip(1), .$hflip(1)) net_140_320(net_140_320);
place #(.$xposition(340), .$yposition(220), .$vflip(1), .$hflip(1)) net_340_220(net_340_220);
place #(.$xposition(340), .$yposition(280), .$vflip(1), .$hflip(1)) net_340_280(net_340_280);
place #(.$xposition(220), .$yposition(200), .$vflip(1), .$hflip(1)) net_220_200(net_220_200);
place #(.$xposition(280), .$yposition(200), .$vflip(1), .$hflip(1)) net_280_200(net_280_200);
place #(.$xposition(140), .$yposition(300), .$vflip(1), .$hflip(1)) net_140_300(net_140_300);
place #(.$xposition(340), .$yposition(300), .$vflip(1), .$hflip(1)) net_340_300(net_340_300);
place #(.$xposition(140), .$yposition(200), .$vflip(1), .$hflip(1)) net_140_200(net_140_200);
place #(.$xposition(340), .$yposition(200), .$vflip(1), .$hflip(1)) net_340_200(net_340_200);
endmodule // main

