<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=LPF-Balun3.dat>
  <DataDisplay=LPF-Balun3.dpl>
  <OpenDisplay=1>
  <Script=LPF-Balun3.m>
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
  <Pac P1 1 40 140 0 0 0 1 "1" 1 "100 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <L L1 1 510 80 0 0 0 0 "1.2uH" 1 "" 0>
  <L L2 1 650 80 0 0 0 0 "470nH" 1 "" 0>
  <C C1 1 440 160 0 0 0 1 "270pF" 1 "" 0 "neutral" 0>
  <C C2 1 580 160 0 0 0 1 "270pF" 1 "" 0 "neutral" 0>
  <L L3 1 360 80 0 0 0 0 "470nH" 1 "" 0>
  <GND \* 5 40 190 0 0 0 0>
  <Eqn Eqn1 1 0 0 0 0 0 0 "y=1" 1 "yes" 0>
  <Pac P2 1 960 130 0 0 0 1 "2" 1 "100 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 960 200 0 0 1 2>
  <L L4 1 360 250 0 0 0 0 "470nH" 1 "" 0>
  <L L5 1 510 250 0 0 0 0 "1.2uH" 1 "" 0>
  <L L6 1 650 250 0 0 0 0 "470nH" 1 "" 0>
  <sTr Tr5 1 200 150 0 0 1 2 "0.5" 1 "0.5" 1>
  <sTr Tr6 1 820 150 0 0 0 0 "0.5" 1 "0.5" 1>
  <GND \* 5 890 220 0 0 1 2>
  <GND \* 5 140 190 0 0 0 0>
  <Vdc V1 1 300 180 0 0 0 1 "1V" 1>
  <GND \* 5 300 210 0 0 0 0>
  <VProbe Pr1 1 930 270 0 0 0 0>
  <GND \* 5 940 310 0 0 1 2>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
