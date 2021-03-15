<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=curtice_1_tb1.dpl>
  <DataSet=curtice_1_tb1.dat>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
  <OpenDisplay=0>
  <PosX=0>
  <PosY=0>
  <RunScript=0>
  <Script=curtice_1_tb1.m>
  <ViewScale=0.9631>
  <ViewX1=-10>
  <ViewX2=1075>
  <ViewY1=-14>
  <ViewY2=849>
  <showFrame=0>
</Properties>
<Symbol>
  <.ID -20 -16 SUB>
  <Line -20 20 40 0 #000080 2 1>
  <Line 20 20 0 -40 #000080 2 1>
  <Line -20 -20 40 0 #000080 2 1>
  <Line -20 20 0 -40 #000080 2 1>
</Symbol>
<Components>
    <Vdc V2 1 80 160 18 -26 0 1 "Vgs" 1>
    <GND * 5 80 190 0 0 0 0>
    <.DC DC1 1 40 250 0 46  0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0 "" 1>
    <Eqn Eqn1 1 70 390 -32 16 0 0 "Id=-V1.I" 1 "yes" 0>
    <GND * 5 380 160 0 0 0 0>
    <Vdc V1 1 380 120 18 -26 0 1 "Vds" 1>
    <GND * 5 210 160 0 0 0 0>
    <IProbe Pr1 1 320 60 -26 16 1 2>
    <Sub Curtice1 1 210 120 20 -46 0 0 "curtice_1.sch" 0>
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
  <Rect 140 724 386 294 3 #c0c0c0 1 00 1 0 2 10 1 -0.0151191 0.05 0.16631 1 -1 0.5 1 315 0 225 "" "" "" "">
	<"Id" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 590 80 13 #000000 0 "Author: Mike Brinson">
</Paintings>
