<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=timer_trig.dat>
  <DataDisplay=timer_trig.dpl>
  <OpenDisplay=1>
  <Script=timer_trig.m>
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
  <Port Pcomp_vp1 1 140 140 0 0 0 0 "1" 1 "analog" 0>
  <Port Pcomp_vn1 1 140 200 0 0 0 0 "2" 1 "analog" 0>
  <GND \* 5 120 80 0 0 0 0>
  <Idc I1 1 160 110 0 0 1 3 "500 nA" 1>
  <R R4 1 350 165 0 0 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND \* 5 410 245 0 0 0 0>
  <Port comp_vout1 1 440 165 0 0 0 2 "3" 1 "analog" 0>
  <OpAmp OP1 1 240 165 0 0 0 0 "1e6" 1 "1 V" 0>
  <C C1 1 410 210 0 0 0 1 "1 nF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
