# Qucs 0.1.0  ../../tests/AC_bandpass.prj/opa227.sch

VCVS #(.G(1), .T(0)) SRC2(_net6, _net1, _net2, gnd);
GND #() *(gnd);
R #(.R(1 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net10, _net6);
C #(.C(1.98944 mF), .V(), .Symbol(neutral)) C1(gnd, _net6);
VCVS #(.G(1e8), .T(0)) SRC1(_net13, _net10, gnd, _net15);
Port #(.Num(1), .Type(analog)) P1(_net13);
Port #(.Num(3), .Type(analog)) P3(_net1);
Port #(.Num(2), .Type(analog)) P2(_net15);
GND #() *(_net2);
