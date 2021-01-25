module main();
Vdc #(.U(0.9 V)) V1(_net13, gnd);
GND #() *(gnd);
IProbe #() Ib(_net6, _net4);
IProbe #() Ic(_net5, _net7);
//DC1
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net8, gnd);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net10, gnd);
Vdc #(.U(2 V)) V2(_net0, gnd);
Eqn #(.NFmin(10*log10(Fmin)), .NF(10*log10(F)), .Export(yes)) Eqn1();
endmodule // main

Sub #(.File()) Sub(unknown_net);
//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module :Paintings:();
painting incomplete
endmodule // :Paintings:

