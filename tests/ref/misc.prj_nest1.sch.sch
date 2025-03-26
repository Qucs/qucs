<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=short_circuit.dat>
  <DataDisplay=short_circuit.dpl>
  <OpenDisplay=1>
  <Script=short_circuit.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 14 SC1>
  <.PortSym 0 50 2 0>
  <.PortSym 0 90 1 0>
</Symbol>
<Components>
  <Port P2 1 400 180 4 -52 0 2 "2" 1 "analog" 0>
  <Port P1 1 180 180 -23 -52 1 0 "1" 1 "analog" 0>
  <GND * 5 380 360 0 0 0 0>
  <C C1 1 380 280 17 -26 0 1 "1 pF" 0 "" 0 "neutral" 0>
  <Sub SC2 1 310 180 -20 14 0 0 "nest2.sch" 0>
  <R R1 1 230 180 -26 15 0 0 "0 Ohm" 0 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <380 180 400 180 "" 0 0 0 "">
  <380 180 380 250 "" 0 0 0 "">
  <380 310 380 360 "" 0 0 0 "">
  <340 180 380 180 "" 0 0 0 "">
  <180 180 200 180 "" 0 0 0 "">
  <260 180 280 180 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 190 60 14 #ff5500 0 "simple subcircuit which just\nshorts the two ports">
  <Text 140 430 12 #ff5500 0 "C1 is used just to have a\ncomponent in the subcircuit netlist\n(does not change the simulation/netlisting results)">
</Paintings>
