module main(_net1, _net10);
Port #(.Num(2), .Type(analog)) P2(_net10);
Port #(.Num(1), .Type(analog)) P1(_net1);
GND #() *(gnd);
C #(.C(1 pF), .V(), .Symbol(neutral)) C1(gnd, _net10);
Sub$nest2 #(.File(nest2.sch)) SC2(_net8, _net10);
R #(.R(0 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(_net1, _net8);
endmodule // main

Sub #(.File()) Sub(_net0);
