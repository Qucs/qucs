<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=swr_meter.dat>
  <DataDisplay=swr_meter.dpl>
  <OpenDisplay=1>
  <Script=swr_meter.m>
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
  <GND \* 5 380 180 0 0 0 0>
  <GND \* 5 320 240 0 0 0 2>
  <R R1 1 260 350 0 0 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 440 350 0 0 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND \* 5 440 380 0 0 0 0>
  <GND \* 5 260 380 0 0 0 0>
  <R R4 1 170 120 0 0 1 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND \* 5 140 220 0 0 0 0>
  <GND \* 5 620 220 0 0 0 0>
  <Vac V1 1 140 190 0 0 0 1 "2 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <Tr Tr2 1 350 270 0 0 1 3 "25" 1>
  <R Load 1 620 190 0 0 0 1 "R_load" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Tr Tr1 1 350 150 0 0 0 1 "25" 1>
  <Eqn Eqn1 1 0 0 0 0 0 0 "y=1" 1 "yes" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
