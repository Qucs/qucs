// electrical models for qucs legacy symbols
// mostly picked from Gnucsator.
// caneda style attribute for use with ngspice

// net. a net simply connects the end points
// in qucs, we only need one with two ports.

(* spice_caneda="R%label %port{a} %port{b} 0. ; TODO" *) module net(.a(i), .b(i));
endmodule

// ground: connect to ground.

(* spice_caneda="X%port{p} %port{p} gndhack" *) module GND(.p(g));
	ground g;
endmodule

// a port device does nothing (but could)

(* spice_caneda=" " *) module Port(.p(g));
	parameter Type;
	parameter Num;
endmodule

// wrapper modules from Gnucsator
// make sense of Qucs "lumped" components in terms of
// standard primitives from section E.

(* spice_caneda="C%label %port{p} %port{n} %property{C}" *) module C(p, n);
	parameter C;
	parameter V;
	capacitor #(.c(C)) dev(p, n);
endmodule // C

module L(p, n);
	parameter L;
	inductor #(.l(L)) dev(p, n);
endmodule // L

(* spice_caneda="R%label %port{p} %port{n} %property{R}" *) module R(p, n);
	parameter R;
	parameter Temp=26.85;
	parameter Tnom=26.85;
	parameter Tc1=0;
	parameter Tc2=0;

	// local?
	parameter dT=Temp-Tnom;

	resistor #(.r(R * (1. + dT*(Tc1 + dT*Tc2)))) dev(p, n);
endmodule // R

// wrap spice sinusoidal source. accessible from verilog netlist
// as per LRM see Annex E.
.subckt Vac(1 2)
.parameter U=1
.parameter f=1
.parameter Phase=0
.parameter Theta=0

V1 1 2 dc=0 ac={U} tran sin amplitude=U frequency=f delay={(-Phase/360.-10)/f}
.ends
