# Qucs 0.1.0  ../../tests/netlist_bug.prj/short_circuit.sch
#ifdef QUCS_INACTIVE
#endif // QUCS_INACTIVE
#ifdef QUCS_INACTIVE
#endif // QUCS_INACTIVE

Port #(.Num(2), .Type(analog)) P2(_net1);
Port #(.Num(1), .Type(analog)) P1(_net1);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(_net1, _net1);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, _net1);
