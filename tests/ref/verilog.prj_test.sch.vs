// VS::printSubckt main
module main();
Verilog #(.$xposition(620), .$yposition(180), .$vflip(1), .$hflip(1), .$ty(-20), .$tx(44), .File(verilogmodel.v)) BBBB(net_590_150, net_650_150, net_590_210);
R #(.$xposition(340), .$yposition(70), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_310_70, net_370_70);
R #(.$xposition(280), .$yposition(280), .$vflip(-1), .$hflip(1), .$tx(15), .$ty(15), .R(1591 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(net_280_250, net_280_310);
R #(.$xposition(400), .$yposition(150), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(1.59 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R6(net_370_150, net_430_150);
Verilog #(.$xposition(340), .$yposition(180), .$vflip(1), .$hflip(1), .$ty(-20), .$tx(44), .File(verilogmodel.v)) AAAA(net_310_150, net_370_150, net_310_210);
C #(.$xposition(590), .$yposition(240), .$vflip(1), .$hflip(1), .$tx(17), .$ty(17), .C(0.1 uF), .V(), .Symbol(neutral)) C2(net_590_270, net_590_210);
R #(.$xposition(560), .$yposition(150), .$vflip(1), .$hflip(1), .$tx(-26), .$ty(-26), .R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(net_530_150, net_590_150);
wire #(.$xposition(160), .$yposition(150), .$vflip(1), .$hflip(1)) noname(net_160_150, net_310_150);
wire #(.$xposition(260), .$yposition(100), .$vflip(1), .$hflip(1)) noname(net_260_100, net_260_210);
wire #(.$xposition(310), .$yposition(210), .$vflip(1), .$hflip(1)) noname(net_310_210, net_260_210);
wire #(.$xposition(770), .$yposition(140), .$vflip(1), .$hflip(1)) noname(net_770_140, net_650_140);
wire #(.$xposition(650), .$yposition(150), .$vflip(1), .$hflip(1)) noname(net_650_150, net_650_140);
place #(.$xposition(590), .$yposition(150)) net_590_150(net_590_150);
place #(.$xposition(650), .$yposition(150)) net_650_150(net_650_150);
place #(.$xposition(590), .$yposition(210)) net_590_210(net_590_210);
place #(.$xposition(310), .$yposition(70)) net_310_70(net_310_70);
place #(.$xposition(370), .$yposition(70)) net_370_70(net_370_70);
place #(.$xposition(280), .$yposition(250)) net_280_250(net_280_250);
place #(.$xposition(280), .$yposition(310)) net_280_310(net_280_310);
place #(.$xposition(370), .$yposition(150)) net_370_150(net_370_150);
place #(.$xposition(430), .$yposition(150)) net_430_150(net_430_150);
place #(.$xposition(310), .$yposition(150)) net_310_150(net_310_150);
place #(.$xposition(310), .$yposition(210)) net_310_210(net_310_210);
place #(.$xposition(590), .$yposition(270)) net_590_270(net_590_270);
place #(.$xposition(530), .$yposition(150)) net_530_150(net_530_150);
place #(.$xposition(160), .$yposition(150)) net_160_150(net_160_150);
place #(.$xposition(260), .$yposition(100)) net_260_100(net_260_100);
place #(.$xposition(260), .$yposition(210)) net_260_210(net_260_210);
place #(.$xposition(770), .$yposition(140)) net_770_140(net_770_140);
place #(.$xposition(650), .$yposition(140)) net_650_140(net_650_140);
endmodule // main

