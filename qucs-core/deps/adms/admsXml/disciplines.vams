/*
  Copyright C 2007 Accellera Organization, Inc.
  Standard definitions
  This file contains the standard definition package disciplines.vams for Verilog-AMS HDL.
*/

`ifdef DISCIPLINES_VAMS
`else
`define DISCIPLINES_VAMS 1
//
// Natures and Disciplines
//
discipline logic
  domain discrete;
enddiscipline
/*
* Default absolute tolerances may be overridden by setting the
* appropriate _ABSTOL prior to including this file
*/
// Electrical
// Current in amperes
nature Current
  units = "A";
  access = I;
  idt_nature = Charge;
`ifdef CURRENT_ABSTOL
  abstol = `CURRENT_ABSTOL;
`else
  abstol = 1e-12;
`endif
endnature
// Charge in coulombs
nature Charge
  units = "coul";
  access = Q;
  ddt_nature = Current;
`ifdef CHARGE_ABSTOL
  abstol = `CHARGE_ABSTOL;
`else
  abstol = 1e-14;
`endif
endnature
// Potential in volts
nature Voltage
  units = "V";
  access = V;
  idt_nature = Flux;
`ifdef VOLTAGE_ABSTOL
  abstol = `VOLTAGE_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Flux in Webers
nature Flux
  units = "Wb";
  access = Phi;
  ddt_nature = Voltage;
`ifdef FLUX_ABSTOL
  abstol = `FLUX_ABSTOL;
`else
  abstol = 1e-9;
`endif
endnature
// Conservative discipline
discipline electrical
  potential Voltage;
  flow Current;
enddiscipline
// Signal flow disciplines
discipline voltage
  potential Voltage;
enddiscipline
discipline current
  potential Current;
enddiscipline
// Magnetic
// Magnetomotive force in Ampere-Turns.
nature Magneto_Motive_Force
  units = "A*turn";
  access = MMF;
`ifdef MAGNETO_MOTIVE_FORCE_ABSTOL
  abstol = `MAGNETO_MOTIVE_FORCE_ABSTOL;
`else
  abstol = 1e-12;
`endif
endnature
// Conservative discipline
discipline magnetic
  potential Magneto_Motive_Force;
  flow Flux;
enddiscipline
// Thermal
// Temperature in Kelvin
nature Temperature
  units = "K";
  access = Temp;
`ifdef TEMPERATURE_ABSTOL
  abstol = `TEMPERATURE_ABSTOL;
`else
  abstol = 1e-4;
`endif
endnature
// Power in Watts
nature Power
  units = "W";
  access = Pwr;
`ifdef POWER_ABSTOL
  abstol = `POWER_ABSTOL;
`else
  abstol = 1e-9;
`endif
endnature
// Conservative discipline
discipline thermal
  potential Temperature;
  flow Power;
enddiscipline
// Kinematic
// Position in meters
nature Position
  units = "m";
  access = Pos;
  ddt_nature = Velocity;
`ifdef POSITION_ABSTOL
  abstol = `POSITION_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Velocity in meters per second
nature Velocity
  units = "m/s";
  access = Vel;
  ddt_nature = Acceleration;
  idt_nature = Position;
`ifdef VELOCITY_ABSTOL
  abstol = `VELOCITY_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Acceleration in meters per second squared
nature Acceleration
  units = "m/s^2";
  access = Acc;
  ddt_nature = Impulse;
  idt_nature = Velocity;
`ifdef ACCELERATION_ABSTOL
  abstol = `ACCELERATION_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Impulse in meters per second cubed
nature Impulse
  units = "m/s^3";
  access = Imp;
  idt_nature = Acceleration;
`ifdef IMPULSE_ABSTOL
  abstol = `IMPULSE_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Force in Newtons
nature Force
  units = "N";
  access = F;
`ifdef FORCE_ABSTOL
  abstol = `FORCE_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Conservative disciplines
discipline kinematic
  potential Position;
  flow Force;
enddiscipline
discipline kinematic_v
  potential Velocity;
  flow Force;
enddiscipline
// Rotational
// Angle in radians
nature Angle
  units = "rads";
  access = Theta;
  ddt_nature = Angular_Velocity;
`ifdef ANGLE_ABSTOL
  abstol = `ANGLE_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Angular Velocity in radians per second
nature Angular_Velocity
  units = "rads/s";
  access = Omega;
  ddt_nature = Angular_Acceleration;
  idt_nature = Angle;
`ifdef ANGULAR_VELOCITY_ABSTOL
  abstol = `ANGULAR_VELOCITY_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Angular acceleration in radians per second squared
nature Angular_Acceleration
  units = "rads/s^2";
  access = Alpha;
  idt_nature = Angular_Velocity;
`ifdef ANGULAR_ACCELERATION_ABSTOL
  abstol = `ANGULAR_ACCELERATION_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Torque in Newtons
nature Angular_Force
  units = "N*m";
  access = Tau;
`ifdef ANGULAR_FORCE_ABSTOL
  abstol = `ANGULAR_FORCE_ABSTOL;
`else
  abstol = 1e-6;
`endif
endnature
// Conservative disciplines
discipline rotational
  potential Angle;
  flow Angular_Force;
enddiscipline
discipline rotational_omega
  potential Angular_Velocity;
  flow Angular_Force;
enddiscipline
`endif
