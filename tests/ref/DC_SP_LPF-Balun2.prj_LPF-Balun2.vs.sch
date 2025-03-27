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
  <L L1 1 510 90 0 0 0 0 "1.2uH" 1 "" 0>
  <L L2 1 650 90 0 0 0 0 "470nH" 1 "" 0>
  <C C1 1 440 170 0 0 0 1 "270pF" 1 "" 0 "neutral" 0>
  <C C2 1 580 170 0 0 0 1 "270pF" 1 "" 0 "neutral" 0>
  <L L3 1 360 90 0 0 0 0 "470nH" 1 "" 0>
  <L L4 1 360 260 0 0 0 0 "470nH" 1 "" 0>
  <L L5 1 510 260 0 0 0 0 "1.2uH" 1 "" 0>
  <L L6 1 650 260 0 0 0 0 "470nH" 1 "" 0>
  <GND \* 5 130 270 0 0 0 0>
  <Pac P1 1 -20 190 0 0 0 1 "1" 1 "100 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 -20 240 0 0 0 0>
  <GND \* 5 870 190 0 0 0 0>
  <Sub Balun1 1 0 0 0 0 0 0 "IdealBalun.sch" 0>
  <Sub Balun2 1 0 0 0 0 1 2 "IdealBalun.sch" 0>
  <GND \* 5 80 210 0 0 0 0>
  <Vdc V1 1 80 160 0 0 0 1 "1V" 1>
  <VProbe Pr1 1 920 130 0 0 0 0>
  <GND \* 5 930 170 0 0 0 0>
  <Pac P2 1 990 180 0 0 0 1 "2" 1 "100 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 990 250 0 0 1 2>
  <Eqn Eqn1 1 0 0 0 0 0 0 "y=1" 1 "yes" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
