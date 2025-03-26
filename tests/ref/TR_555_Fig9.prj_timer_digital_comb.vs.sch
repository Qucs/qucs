<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=timer_digital_comb.dat>
  <DataDisplay=timer_digital_comb.dpl>
  <OpenDisplay=1>
  <Script=timer_digital_comb.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym 40 20 3 0>
  <.PortSym 40 60 2 0>
  <.PortSym 40 100 4 0>
  <.PortSym 40 140 5 0>
  <.PortSym 40 180 1 0>
</Symbol>
<Components>
  <Port P_trig 1 0 0 0 0 0 0 "3" 1 "analog" 0>
  <Port P_tresh 1 0 0 0 0 0 0 "2" 1 "analog" 0>
  <GND \* 5 0 0 0 0 0 0>
  <Port P_Q 1 0 0 0 0 0 0 "4" 1 "analog" 0>
  <GND \* 5 0 0 0 0 0 0>
  <Inv Y5 1 0 0 0 0 0 0 "1 V" 0 "0" 0 "10" 0 "DIN40900" 0>
  <Port P_QB 1 0 0 0 0 0 0 "5" 1 "analog" 0>
  <GND \* 5 0 0 0 0 0 0>
  <Port P_reset 1 0 0 0 0 0 0 "1" 1 "analog" 0>
  <GND \* 5 0 0 0 0 0 0>
  <GND \* 5 0 0 0 0 0 0>
  <R R1 1 0 0 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R5 1 0 0 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 0 0 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 0 0 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 0 0 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Inv Y4 1 0 0 0 0 0 0 "1 V" 0 "0" 0 "10" 0 "DIN40900" 0>
  <OR Y1 1 0 0 0 0 0 0 "2" 0 "1 V" 0 "0" 0 "10" 0 "old" 0>
  <NOR Y3 1 0 0 0 0 0 0 "2" 0 "1 V" 0 "0" 0 "10" 0 "old" 0>
  <NOR Y2 1 0 0 0 0 0 0 "2" 0 "1 V" 0 "5 ns" 0 "10" 0 "old" 0>
  <C C1 1 0 0 0 0 0 0 "0.5nF" 1 "" 0 "neutral" 0>
  <C C5 1 0 0 0 0 0 0 "0.5nF" 1 "" 0 "neutral" 0>
  <C C4 1 0 0 0 0 0 0 "5nF" 1 "" 0 "neutral" 0>
  <C C3 1 0 0 0 0 0 0 "0.09nF" 1 "" 0 "neutral" 0>
  <C C2 1 0 0 0 0 0 0 "0.5nF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
