module main();
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
C #(.C(10 pF), .V(), .Symbol(neutral)) C1(gnd, Source);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(Source, _net9);
Vac #(.U(5 V), .f(10 MHz), .Phase(0), .Theta(0)) V1(Gate, gnd);
Vac #(.U(-5 V), .f(10 MHz), .Phase(0), .Theta(0)) V3(_net6, gnd);
Vac #(.U(5 V), .f(10 MHz), .Phase(0), .Theta(0)) V2(Drain, _net9);
//TR1
endmodule // main

module Symbol();
endmodule // Symbol

