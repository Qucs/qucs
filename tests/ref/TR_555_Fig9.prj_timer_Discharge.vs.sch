<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=timer_Discharge.dat>
  <DataDisplay=timer_Discharge.dpl>
  <OpenDisplay=1>
  <Script=timer_Discharge.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym 40 20 1 0>
  <.PortSym 40 60 3 0>
  <.PortSym 40 100 2 0>
</Symbol>
<Components>
  <R R1 1 350 150 0 0 0 0 "10K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Port P_control_in 1 255 150 0 0 0 0 "1" 0 "analog" 0>
  <Port P_GND 1 260 260 0 0 0 0 "3" 0 "analog" 0>
  <Port P_Discharge 1 260 320 0 0 0 0 "2" 0 "analog" 0>
  <_BJT T1 1 410 150 0 0 0 0 "npn" 1 "1e-16" 1 "1" 1 "1" 0 "0" 0 "0" 0 "0" 1 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "100" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "1e-9" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <R R2 1 370 320 0 0 0 0 "200" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <255 150 320 150 "" 0 0 0 "">
  <260 260 410 260 "" 0 0 0 "">
  <410 180 410 260 "" 0 0 0 "">
  <260 320 340 320 "" 0 0 0 "">
  <410 120 500 120 "" 0 0 0 "">
  <500 120 500 320 "" 0 0 0 "">
  <400 320 500 320 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
