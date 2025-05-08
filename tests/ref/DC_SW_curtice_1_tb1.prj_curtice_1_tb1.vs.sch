<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=curtice_1_tb1.dat>
  <DataDisplay=curtice_1_tb1.dpl>
  <OpenDisplay=1>
  <Script=curtice_1_tb1.m>
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
  <Vdc V2 1 80 160 0 0 0 1 "Vgs" 1>
  <GND * 5 80 190 0 0 0 0>
  <.DC DC1 1 40 250 0 0 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Eqn Eqn1 1 70 390 0 0 0 0 "y=1" 1 "yes" 0>
  <.SW SW1 1 380 220 0 0 0 0 "DC1" 1 "lin" 1 "R1" 1 "Vds" 1 "-10" 1 "41" 1>
  <.SW SW2 1 540 220 0 0 0 0 "SW1" 1 "lin" 1 "R1" 1 "Vgs" 1 "-5" 1 "6" 1>
  <GND * 5 380 160 0 0 0 0>
  <Vdc V1 1 380 120 0 0 0 1 "Vds" 1>
  <GND * 5 210 160 0 0 0 0>
  <IProbe Pr1 1 320 60 0 0 1 2>
</Components>
<Wires>
  <380 150 380 160 "" 0 0 0 "">
  <380 60 380 90 "" 0 0 0 "">
  <210 60 290 60 "" 0 0 0 "">
  <80 120 80 130 "" 0 0 0 "">
  <80 120 180 120 "" 0 0 0 "">
  <210 60 210 80 "" 0 0 0 "">
  <350 60 380 60 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 590 80 12 #000000 0 "Author: Mike Brinson">
</Paintings>
