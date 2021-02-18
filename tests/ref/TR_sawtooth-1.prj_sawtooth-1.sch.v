module main();
Lib$Z-Diodes$1N4732A #(.Lib(Z-Diodes), .Component(1N4732A)) D1();
_BJT #(.Type(npn), .Is(15.5f), .Nf(1), .Nr(1), .Ikf(54.7m), .Ikr(0.135), .Vaf(127), .Var(28), .Ise(3.9p), .Ne(2), .Isc(0), .Nc(2), .Bf(524), .Br(4), .Rbm(0), .Irb(0), .Rc(0.116), .Re(0.29), .Rb(1.16), .Cje(22.7p), .Vje(1.1), .Mje(0.5), .Cjc(7.34p), .Vjc(0.3), .Mjc(0.3), .Xcjc(1), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(698p), .Xtf(0), .Vtf(0), .Itf(0), .Tr(121n), .Temp(26.85), .Kf(0), .Af(1), .Ffe(1), .Kb(0), .Ab(1), .Fb(1), .Ptf(0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T_2DC4617R1(_net4, _net16, gnd, _net16);
_BJT #(.Type(pnp), .Is(15.2f), .Nf(1), .Nr(1), .Ikf(54.7m), .Ikr(0.135), .Vaf(127), .Var(24), .Ise(3.88p), .Ne(2), .Isc(0), .Nc(2), .Bf(523), .Br(4), .Rbm(0), .Irb(0), .Rc(0.284), .Re(0.710), .Rb(2.84), .Cje(42.6p), .Vje(1.1), .Mje(0.5), .Cjc(13.7p), .Vjc(0.3), .Mjc(0.3), .Xcjc(1), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(453p), .Xtf(0), .Vtf(0), .Itf(0), .Tr(96.4n), .Temp(26.85), .Kf(0), .Af(1), .Ffe(1), .Kb(0), .Ab(1), .Fb(1), .Ptf(0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T_2DA1774R1(_net16, _net4, Out, _net4);
GND #() *(gnd);
GND #() *(gnd);
C #(.C(1n), .V(), .Symbol(neutral)) C1(gnd, Out);
GND #() *(gnd);
Vdc #(.U(10)) V1(_net9, gnd);
GND #() *(gnd);
R #(.R(400), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(Out, _net9);
R #(.R(12), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net16, _net9);
//TR1
endmodule // main

//else?
module :SymbolSection:();
endmodule // :SymbolSection:

//else?
module :Paintings:();
endmodule // :Paintings:

