module main();
Vdc #(.U(1 V)) V1(_net4, gnd);
GND #() *(gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(_net0, gnd);
(empty) #(.Lib(), .Component()) IB1();
//DC1
endmodule // main

module :SymbolSection:();
endmodule // :SymbolSection:

module :Paintings:();
endmodule // :Paintings:

