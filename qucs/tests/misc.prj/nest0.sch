<Qucs Schematic 0.0.19>
<Properties>
  <View=86,0,820,483,1.23842,0,0>
  <Grid=10,10,1>
  <DataSet=test_circuit.dat>
  <DataDisplay=test_circuit.dpl>
  <OpenDisplay=0>
  <Script=test_circuit.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Vdc V1 1 140 250 18 -26 0 1 "1 V" 1>
  <GND * 1 140 320 0 0 0 0>
  <R R1 1 340 250 15 -26 1 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.DC DC1 1 140 380 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Sub SC1 1 250 200 -20 14 0 0 "nest1.sch" 0>
</Components>
<Wires>
  <140 280 140 300 "" 0 0 0 "">
  <140 300 140 320 "" 0 0 0 "">
  <140 300 340 300 "" 0 0 0 "">
  <340 280 340 300 "" 0 0 0 "">
  <140 200 140 220 "" 0 0 0 "">
  <140 200 220 200 "in" 210 170 33 "">
  <340 200 340 220 "" 0 0 0 "">
  <280 200 340 200 "" 0 0 0 "">
  <340 200 340 200 "out" 370 170 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 140 40 14 #ff5500 0 "- if the subcircuit is a plain short (i.e. just a net connecting the two nodes)\nthe simulation result is wrong\n- if the subcircuit shorts the two nodes via a 0 ohm resistor, results are correct">
</Paintings>
