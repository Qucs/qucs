module main();
Vdc #(.U(1 V)) V1(in, gnd);
GND #() *(gnd);
R #(.R(50 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(US)) R1(out, gnd);
//DC1
Sub$nest1 #(.File(nest1.sch)) SC1(in, out);
endmodule // main

Sub #(.File()) Sub((null));
