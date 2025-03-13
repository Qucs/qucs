<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=timer_555.dat>
  <DataDisplay=timer_555.dpl>
  <OpenDisplay=1>
  <Script=timer_555.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym 40 20 1 0>
  <.PortSym 40 60 3 0>
  <.PortSym 40 100 4 0>
  <.PortSym 40 140 5 0>
  <.PortSym 40 180 7 0>
  <.PortSym 40 220 8 0>
  <.PortSym 40 260 2 0>
  <.PortSym 40 300 6 0>
</Symbol>
<Components>
  <Port P_GND 1 0 0 -23 12 0 0 "1" 1 "analog" 0>
  <Port P_OUTPUT 1 0 0 -23 12 0 0 "3" 1 "analog" 0>
  <R R1 1 0 0 -26 15 0 0 "5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port P_RESET 1 0 0 -23 12 0 0 "4" 1 "analog" 0>
  <R R2 1 0 0 -26 15 0 0 "5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 0 0 -26 15 0 0 "5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port P_CONTROL 1 0 0 -23 12 0 0 "5" 1 "analog" 0>
  <Port P_DISCHARGE 1 0 0 -23 12 0 0 "7" 1 "analog" 0>
  <Port P_VCC 1 0 0 -23 12 0 0 "8" 1 "analog" 0>
  <Port P_TRIGGER 1 0 0 -23 12 0 0 "2" 1 "analog" 0>
  <Port P_THRESH 1 0 0 -23 12 0 0 "6" 1 "analog" 0>
  <Sub SUB5 1 0 0 -26 21 0 0 "timer_Discharge.sch" 0>
  <Sub SUB3 1 0 0 -26 21 0 0 "timer_digital_comb.sch" 0>
  <Sub SUB4 1 0 0 -26 21 0 0 "timer_amp.sch" 0>
  <Sub SUB6 1 0 0 -26 21 0 0 "timer_trig.sch" 0>
  <Sub SUB7 1 0 0 -26 21 0 0 "timer_thresh.sch" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
