<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=Spiral_BPF.dat>
  <DataDisplay=Spiral_BPF.dpl>
  <OpenDisplay=1>
  <Script=Spiral_BPF.m>
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
  <C C1 1 180 370 -26 17 0 0 "195 fF" 1 "" 0 "neutral" 0>
  <SPIRALIND SPIRALIND1 1 260 200 -30 25 0 0 "Subst1" 1 "Circular" 1 "25 um" 0 "91 um" 0 "25 um" 0 "2" 0 "26.85" 0>
  <C C2 1 260 260 -26 17 0 0 "155 fF" 1 "" 0 "neutral" 0>
  <SUBST Subst1 1 0 0 -30 24 0 0 "4.5" 1 "0.8 mm" 1 "35 um" 1 "2e-4" 1 "0.022e-6" 1 "0.15e-6" 1>
  <GND \* 5 180 410 0 0 0 0>
  <GND \* 5 400 410 0 0 0 0>
  <GND \* 5 620 410 0 0 0 0>
  <GND \* 5 730 410 0 0 0 0>
  <C C3 1 400 370 -26 17 0 0 "320 fF" 1 "" 0 "neutral" 0>
  <C C5 1 620 370 -26 17 0 0 "82 fF" 1 "" 0 "neutral" 0>
  <CIRCULARLOOP CIRCULARLOOP1 1 480 200 -30 10 0 0 "Subst1" 1 "25 um" 0 "195 um" 0 "26.85" 0>
  <C C4 1 480 260 -26 17 0 0 "600 fF" 1 "" 0 "neutral" 0>
  <Pac P2 1 730 310 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 W" 0 "1 GHz" 0 "-273.15" 0>
  <Pac P1 1 30 270 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "-273.15" 0>
  <Vdc V1 1 30 350 18 -26 0 1 "1.1 V" 1>
  <GND \* 5 30 410 0 0 0 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
