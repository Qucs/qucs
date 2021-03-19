<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=test_circuit.dpl>
  <DataSet=test_circuit.dat>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
  <OpenDisplay=0>
  <PosX=NA(-2147483648)>
  <PosY=483>
  <RunScript=0>
  <Script=test_circuit.m>
  <ViewScale=NA(-2147483648)>
  <ViewX1=Y2>
  <ViewX2=NA(-2147483648)>
  <ViewY1=NA(-2147483648)>
  <ViewY2=NA(-2147483648)>
  <showFrame=0>
</Properties>
<Symbol>
</Symbol>
<Components>
    <Vdc V1 1 140 250 18 -26 0 1 "1 V" 1>
    <GND * 5 140 320 0 0 0 0>
    <R R1 1 340 250 -22 15 1 3 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
    <Lib IB1 1 250 200 0 0 0 0 "Ideal" 0 "Notch2" 0 "1E3" 0 "1" 0>
    <.DC DC1 1 140 380 0 43  0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0 "" 1 "" 1 "" 1 "" 1 "" 1 "" 1 "" 1 "" 1 "" 1 "" 1>
</Components>
<Wires>
<140 280 140 300 "" 0 0 0 "">
<140 300 140 320 "" 0 0 0 "">
<140 300 340 300 "" 0 0 0 "">
<340 280 340 300 "" 0 0 0 "">
<140 200 140 220 "" 0 0 0 "">
<140 200 220 200 "" 0 0 0 "">
<340 200 340 220 "" 0 0 0 "">
<280 200 340 200 "" 0 0 0 "">
<340 200 340 200 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 360 368 164 49 1 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 0 0 0 "" "" "" "0">
	<"in.V" #0000ff 0 3 0 0 0>
	<"out.V" #0000ff 0 3 0 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 140 40 14 #ff5500 0 "- if the subcircuit is a plain short (i.e. just a net connecting the two nodes)\nthe simulation result is wrong\n- if the subcircuit shorts the two nodes via a 0 ohm resistor, results are correct">
</Paintings>
