<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=gyrator.dat>
  <DataDisplay=gyrator.dpl>
  <OpenDisplay=1>
  <Script=gyrator.m>
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
  <GND * 5 120 320 0 0 0 0>
  <Vdc V1 1 120 290 0 0 0 1 "10 V" 1>
  <Vac V2 1 120 230 0 0 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <GND * 5 570 320 0 0 0 0>
  <R Load 1 570 290 0 0 0 1 "1000 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.DC DC1 1 190 350 0 0 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.AC AC1 1 320 350 0 0 0 0 "log" 1 "1 Hz" 1 "100 kHz" 1 "100" 1 "no" 0>
  <_BJT T1 1 360 130 0 0 0 1 "npn" 1 "1e-16" 1 "1" 1 "1" 0 "0" 0 "0" 0 "0" 1 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "250" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <R R1 1 310 160 0 0 0 0 "20 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 410 160 0 0 0 0 "200 uF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <280 130 330 130 "" 0 0 0 "">
  <280 130 280 160 "" 0 0 0 "">
  <340 160 360 160 "" 0 0 0 "">
  <440 130 440 160 "" 0 0 0 "">
  <390 130 440 130 "" 0 0 0 "">
  <440 160 570 160 "" 0 0 0 "">
  <570 160 570 260 "" 0 0 0 "">
  <120 160 280 160 "" 0 0 0 "">
  <120 160 120 200 "" 0 0 0 "">
  <360 160 380 160 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 210 240 12 #000000 0 "This circuit is a primitive gyrator. It simulates an inductor\nby taking a capacitor and an transistor inverter. At the\noutput there should be only little ac voltage.">
</Paintings>
