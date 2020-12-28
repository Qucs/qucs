<Qucs Schematic 0.0.19>
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
  <.ID -20 14 SC>
  <.PortSym -30 0 2 0>
  <Line -20 -10 40 0 #000080 2 1>
  <Line 20 -10 0 20 #000080 2 1>
  <Line -20 10 40 0 #000080 2 1>
  <Line -20 -10 0 20 #000080 2 1>
  <Line -30 0 10 0 #000080 2 1>
  <Line 20 0 10 0 #000080 2 1>
  <.PortSym 30 0 1 180>
  <Line -20 0 40 0 #0000ff 3 1>
</Symbol>
<Components>
  <Port P2 1 400 180 4 -52 0 2 "2" 1 "analog" 0>
  <Port P1 1 180 180 -23 -52 1 0 "1" 1 "analog" 0>
  <R R1 0 290 260 -26 15 0 0 "0 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND * 1 380 360 0 0 0 0>
  <C C1 0 380 280 17 -26 0 1 "1 pF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <180 180 240 180 "" 0 0 0 "">
  <240 260 260 260 "" 0 0 0 "">
  <240 180 240 260 "" 0 0 0 "">
  <320 260 340 260 "" 0 0 0 "">
  <340 180 380 180 "" 0 0 0 "">
  <340 180 340 260 "" 0 0 0 "">
  <240 180 340 180 "" 0 0 0 "">
  <380 180 400 180 "" 0 0 0 "">
  <380 180 380 250 "" 0 0 0 "">
  <380 310 380 360 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 190 60 14 #ff5500 0 "simple subcircuit which just\nshorts the two ports">
  <Text 140 430 12 #ff5500 0 "C1 is used just to have a\ncomponent in the subcircuit netlist\n(does not change the simulation/netlisting results)">
</Paintings>
