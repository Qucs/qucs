// Qucs

Vdc #(.U(0.9 V)) V1(_net10, gnd);
GND #() *(gnd);
Pac #(.Num(1), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P1(_net2, gnd);
Pac #(.Num(2), .Z(50 Ohm), .P(0 dBm), .f(1 GHz), .Temp(26.85)) P2(_net4, gnd);
Vdc #(.U(2 V)) V2(_net9, gnd);
Eqn #(.NFmin(10*log10(Fmin)), .NF(10*log10(F)), .Export(yes)) Eqn1();
