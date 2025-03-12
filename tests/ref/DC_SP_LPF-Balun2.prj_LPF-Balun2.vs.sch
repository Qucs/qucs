<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=LPF-Balun2.dat>
  <DataDisplay=LPF-Balun2.dpl>
  <OpenDisplay=1>
  <Script=LPF-Balun2.m>
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
  <L L1 1 480 90 -26 10 0 0 "1.2uH" 1 "" 0>
  <L L2 1 620 90 -26 10 0 0 "470nH" 1 "" 0>
  <C C1 1 440 200 -26 17 0 0 "270pF" 1 "" 0 "neutral" 0>
  <C C2 1 580 200 -26 17 0 0 "270pF" 1 "" 0 "neutral" 0>
  <L L3 1 330 90 -26 10 0 0 "470nH" 1 "" 0>
  <L L4 1 330 260 -26 10 0 0 "470nH" 1 "" 0>
  <L L5 1 480 260 -26 10 0 0 "1.2uH" 1 "" 0>
  <L L6 1 620 260 -26 10 0 0 "470nH" 1 "" 0>
  <GND \* 5 130 270 0 0 0 0>
  <Pac P1 1 -20 160 18 -26 0 1 "1" 1 "100 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 -20 240 0 0 0 0>
  <GND \* 5 870 190 0 0 0 0>
  <Sub Balun1 1 0 0 -26 21 0 0 "IdealBalun.sch" 0>
  <Sub Balun2 1 0 0 -26 21 0 0 "IdealBalun.sch" 0>
  <GND \* 5 80 210 0 0 0 0>
  <Vdc V1 1 80 130 18 -26 0 1 "1V" 1>
  <VProbe Pr1 1 910 150 28 -31 0 0>
  <GND \* 5 930 170 0 0 0 0>
  <Pac P2 1 990 150 18 -26 0 1 "2" 1 "100 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 990 250 0 0 0 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
