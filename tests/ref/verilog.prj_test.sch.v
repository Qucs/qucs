module main();
Verilog$verilogmodel #(.File(verilogmodel.v), .A(1), .B(2)) BBBB(_net0, _net10, _net2);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(_net3, _net4);
R #(.R(1591 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net5, _net6);
R #(.R(1.59 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R6(_net7, _net8);
Verilog$verilogmodel #(.File(verilogmodel.v)) AAAA(_net13, _net7, _net9);
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C2(_net11, _net2);
R #(.R(10 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(_net12, _net0);
endmodule // main

module Symbol();
endmodule // Symbol

module Verilog$verilogmodel(_net0, _net1, _net2);
module something(_net0, _net1, _net2);
// 	parameter A;
// 	parameter B;
endmodule // something

endmodule // Verilog:verilogmodel

