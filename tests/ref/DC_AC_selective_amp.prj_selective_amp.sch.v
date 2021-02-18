module main();
GND #() *(gnd);
GND #() *(gnd);
GND #() *(gnd);
R #(.R(100k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(Input, _net10);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(gnd, Input);
R #(.R(500), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(gnd, _net11);
//DC1
GND #() *(gnd);
_BJT #(.Type(npn), .Is(1e-16), .Nf(1), .Nr(1), .Ikf(0), .Ikr(0), .Vaf(0), .Var(0), .Ise(0), .Ne(1.5), .Isc(0), .Nc(2), .Bf(100), .Br(1), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(0), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T1(Input, Output, _net11, Output);
C #(.C(1.5n), .V(), .Symbol(neutral)) C1(Output, _net10);
R #(.R(100k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(Output, _net10);
L #(.L(80u), .I()) L1(Output, _net10);
Vac #(.U(1 uV), .f(1 GHz), .Phase(0), .Theta(0)) V2(_net0, gnd);
C #(.C(100n), .V(), .Symbol(neutral)) C2(gnd, _net11);
GND #() *(gnd);
Vdc #(.U(12 V)) V1(_net10, gnd);
//AC1
Eqn #(.Gain(dB(Output.v/Input.v)), .Export(yes)) Eqn1();
endmodule // main

//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module :Paintings:();
endmodule // :Paintings:

