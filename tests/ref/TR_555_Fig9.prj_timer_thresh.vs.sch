<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=-7,-10,480,300,1.74333,0,0>
  <Grid=5,5,0>
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
  <Port PinP 1 60 120 0 0 0 0 "1" 1 "analog" 0>
  <Port PinN 1 60 180 0 0 0 0 "2" 1 "analog" 0>
  <GND * 5 245 35 0 0 0 0>
  <Idc I1 1 160 60 0 0 0 1 "0.1 uA" 1>
  <R R4 1 320 150 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port POUT 1 390 150 0 0 0 2 "3" 1 "analog" 0>
  <GND * 5 370 235 0 0 0 0>
  <OpAmp OP1 1 225 150 0 0 0 0 "1e6" 1 "1 V" 1>
  <C C1 1 370 205 0 0 0 1 "1 nF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <60 120 160 120 "" 0 0 0 "">
  <245 30 245 35 "" 0 0 0 "">
  <160 30 245 30 "" 0 0 0 "">
  <160 90 160 120 "" 0 0 0 "">
  <160 120 170 120 "" 0 0 0 "">
  <265 150 290 150 "" 0 0 0 "">
  <350 150 370 150 "" 0 0 0 "">
  <370 150 390 150 "" 0 0 0 "">
  <370 150 370 175 "" 0 0 0 "">
  <170 130 195 130 "" 0 0 0 "">
  <170 120 170 130 "" 0 0 0 "">
  <60 180 195 180 "" 0 0 0 "">
  <195 170 195 180 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
