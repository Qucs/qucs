<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
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
  <R Rs 1 250 410 -26 15 0 0 "RS" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Ls 1 170 410 -26 10 0 0 "LS" 1 "" 0>
  <Port Source 1 90 410 -23 12 0 0 "1" 1 "analog" 0>
  <R Rin 1 210 320 -26 15 0 0 "RIN" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <EDD D2 1 340 180 -26 -72 0 0 "explicit" 0 "1" 0 "0" 1 "CGD*V1" 0>
  <Port Drain 1 90 180 -23 12 0 0 "3" 1 "analog" 0>
  <R Rd 1 190 180 -26 15 0 0 "RD" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Ld 1 110 180 -26 10 0 0 "LD" 1 "" 0>
  <R Rg 1 360 160 -26 15 0 0 "RG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Lg 1 360 90 -26 10 0 0 "LG" 1 "" 0>
  <Port Gate 1 360 30 -23 12 0 0 "2" 1 "analog" 0>
  <EDD D1 1 390 240 -26 -72 0 0 "explicit" 0 "4" 0 "V1<-VBR+50*Vt ? -IsT*(1+exp(-(VBR+V1)/Vt)) + GMIN*V1 : 0" 1 "0" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
