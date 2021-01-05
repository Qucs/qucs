module main();
GND #() *(gnd);
//TR1
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(Vout, gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(Vin, Vout);
Vac #(.U(10 V), .f(1kHz), .Phase(0), .Theta(0)) V1(Vin, gnd);
endmodule // main

Sub #(.File()) Sub(unknown_net);
//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module :Paintings:();
endmodule // :Paintings:

