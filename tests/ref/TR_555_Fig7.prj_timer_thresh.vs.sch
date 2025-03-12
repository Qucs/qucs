<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=timer_thresh.dat>
  <DataDisplay=timer_thresh.dpl>
  <OpenDisplay=1>
  <Script=timer_thresh.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym 40 20 1 0>
  <.PortSym 40 60 2 0>
  <.PortSym 40 100 3 0>
</Symbol>
<Components>
  <Port PinP 1 60 120 -23 12 0 0 "1" 1 "analog" 0>
  <Port PinN 1 60 180 -23 12 0 0 "2" 1 "analog" 0>
  <GND \* 5 245 35 0 0 0 0>
  <Idc I1 1 160 30 18 -26 0 1 "0.1 uA" 1>
  <R R4 1 290 150 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port POUT 1 390 150 -23 12 0 0 "3" 1 "analog" 0>
  <GND \* 5 370 235 0 0 0 0>
  <OpAmp OP1 1 195 170 -26 42 0 0 "1e6" 1 "1 V" 0>
  <C C1 1 370 235 -26 17 0 0 "1 nF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
