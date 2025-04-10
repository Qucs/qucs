<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=groupdelay_sp.dat>
  <DataDisplay=groupdelay_sp.dpl>
  <OpenDisplay=1>
  <Script=groupdelay_sp.m>
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
  <GND * 5 110 210 0 0 0 0>
  <C C1 1 220 180 0 0 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 5 220 210 0 0 0 0>
  <L L1 1 290 100 0 0 0 0 "280.9nH" 1 "" 0>
  <C C2 1 360 180 0 0 0 1 "39.01pF" 1 "" 0 "neutral" 0>
  <GND * 5 360 210 0 0 0 0>
  <Pac P1 1 110 180 0 0 0 1 "1" 1 "60 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <.SP SP1 1 110 290 0 0 0 0 "log" 1 "1MHz" 1 "200MHz" 1 "458" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Pac P2 1 500 190 0 0 0 1 "2" 1 "60 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 5 500 220 0 0 0 0>
  <Eqn Eqn1 1 310 300 0 0 0 0 "y=1" 1 "yes" 0>
</Components>
<Wires>
  <220 100 220 150 "" 0 0 0 "">
  <360 100 360 150 "" 0 0 0 "">
  <220 100 260 100 "" 0 0 0 "">
  <320 100 360 100 "" 0 0 0 "">
  <110 100 220 100 "" 0 0 0 "">
  <110 100 110 150 "" 0 0 0 "">
  <360 100 500 100 "" 0 0 0 "">
  <500 100 500 160 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
