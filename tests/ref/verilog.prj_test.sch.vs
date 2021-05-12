// VS::printSubckt main
module main();
Verilog #(.$tx(-20), .$ty(44), .File(verilogmodel.v), .$xposition(620), .$yposition(180), .$vflip(1), .$hflip(1), .A(1), .B(2)) BBBB(net_590_150, net_650_150, net_590_210);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(340), .$yposition(70), .$vflip(1), .$hflip(1)) R1(net_310_70, net_370_70);
R #(.R(1591 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(15), .$tx(15), .$xposition(280), .$yposition(280), .$vflip(-1), .$hflip(1)) R3(net_280_250, net_280_310);
R #(.R(1.59 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(400), .$yposition(150), .$vflip(1), .$hflip(1)) R6(net_370_150, net_430_150);
Verilog #(.$tx(-20), .$ty(44), .File(verilogmodel.v), .$xposition(340), .$yposition(180), .$vflip(1), .$hflip(1)) AAAA(net_310_150, net_370_150, net_310_210);
C #(.C(0.1 uF), .V(), .Symbol(neutral), .$ty(17), .$tx(17), .$xposition(590), .$yposition(240), .$vflip(1), .$hflip(1)) C2(net_590_270, net_590_210);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european), .$ty(-26), .$tx(-26), .$xposition(560), .$yposition(150), .$vflip(1), .$hflip(1)) R4(net_530_150, net_590_150);
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

