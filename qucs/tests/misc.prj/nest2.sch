<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=short_circuit.dpl>
  <DataSet=short_circuit.dat>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
  <OpenDisplay=1>
  <PosX=0>
  <PosY=0>
  <RunScript=0>
  <Script=short_circuit.m>
  <ViewScale=1>
  <ViewX1=0>
  <ViewX2=800>
  <ViewY1=0>
  <ViewY2=800>
  <showFrame=0>
</Properties>
<Symbol>
  <.ID -20 14 SC>
  <.PortSym -30 0  0>
  <Line -20 -10 40 0 #000080 2 1>
  <Line 20 -10 0 20 #000080 2 1>
  <Line -20 10 40 0 #000080 2 1>
  <Line -20 -10 0 20 #000080 2 1>
  <Line -30 0 10 0 #000080 2 1>
  <Line 20 0 10 0 #000080 2 1>
  <.PortSym 30 0  0>
  <Line -20 0 40 0 #0000ff 3 1>
</Symbol>
<Components>
    <Port P2 1 400 180 4 -52 0 2 "2" 1 "analog" 0>
    <Port P1 1 180 180 -23 -52 1 0 "1" 1 "analog" 0>
    <R R1 1 290 260 -26 15 0 0 "0 Ohm" 0 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
    <GND * 5 380 360 0 0 0 0>
    <C C1 1 380 280 17 -26 0 1 "1 pF" 0 "" 0 "neutral" 0>
</Components>
<Wires>
<240 260 260 260 "" 0 0 0 "">
<320 260 340 260 "" 0 0 0 "">
<380 180 400 180 "" 0 0 0 "">
<380 180 380 250 "" 0 0 0 "">
<380 310 380 360 "" 0 0 0 "">
<340 180 380 180 "" 0 0 0 "">
<340 180 340 260 "" 0 0 0 "">
<180 180 240 180 "" 0 0 0 "">
<240 180 240 260 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 190 60 14 #ff5500 0 "simple subcircuit which just\nshorts the two ports">
  <Text 140 430 12 #ff5500 0 "C1 is used just to have a\ncomponent in the subcircuit netlist\n(does not change the simulation/netlisting results)">
</Paintings>
