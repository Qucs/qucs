<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=22,-52,861,656,1.10593,0,0>
  <Grid=10,10,1>
  <DataSet=curtice_1.dat>
  <DataDisplay=curtice_1.dpl>
  <OpenDisplay=1>
  <Script=curtice_1.m>
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
  <R Rs 1 280 410 0 0 0 0 "RS" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Ls 1 200 410 0 0 0 0 "LS" 1 "" 0>
  <Port Source 1 90 410 0 0 0 0 "1" 1 "analog" 0>
  <R Rin 1 240 320 0 0 0 0 "RIN" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <EDD D2 1 310 180 0 0 1 2 "explicit" 0 "1" 0 "0" 1 "CGD*V1" 1>
  <Port Drain 1 90 180 0 0 1 0 "3" 1 "analog" 0>
  <R Rd 1 220 180 0 0 1 0 "RD" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Ld 1 140 180 0 0 0 0 "LD" 1 "" 0>
  <R Rg 1 360 130 0 0 0 1 "RG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Lg 1 360 60 0 0 0 1 "LG" 1 "" 0>
  <Port Gate 1 360 30 0 0 0 3 "2" 1 "analog" 0>
  <Eqn Eqn1 1 490 20 0 0 0 0 "y=1" 1 "yes" 1>
  <Eqn Eqn2 1 490 240 0 0 0 0 "y=1" 1 "yes" 1>
  <EDD D1 1 390 270 0 0 0 3 "explicit" 0 "4" 0 "V1<-VBR+50*Vt ? -IsT*(1+exp(-(VBR+V1)/Vt)) + GMIN*V1 : 0" 1 "0" 1>
</Components>
<Wires>
  <270 300 270 320 "" 0 0 0 "">
  <390 300 390 320 "" 0 0 0 "">
  <270 320 330 320 "" 0 0 0 "">
  <330 320 390 320 "" 0 0 0 "">
  <330 300 330 320 "" 0 0 0 "">
  <330 220 330 240 "" 0 0 0 "">
  <390 220 390 240 "" 0 0 0 "">
  <330 220 360 220 "" 0 0 0 "">
  <210 300 210 320 "" 0 0 0 "">
  <210 220 330 220 "" 0 0 0 "">
  <210 220 210 240 "" 0 0 0 "">
  <330 320 330 410 "" 0 0 0 "">
  <310 410 330 410 "" 0 0 0 "">
  <230 410 250 410 "" 0 0 0 "">
  <90 410 170 410 "" 0 0 0 "">
  <360 220 390 220 "" 0 0 0 "">
  <360 180 360 220 "" 0 0 0 "">
  <340 180 360 180 "" 0 0 0 "">
  <270 180 270 240 "" 0 0 0 "">
  <270 180 280 180 "" 0 0 0 "">
  <250 180 270 180 "" 0 0 0 "">
  <170 180 190 180 "" 0 0 0 "">
  <90 180 110 180 "" 0 0 0 "">
  <360 160 360 180 "" 0 0 0 "">
  <360 90 360 100 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 700 30 13 #000000 0 "Author: Mike Brinson">
</Paintings>
