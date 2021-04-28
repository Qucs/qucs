<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=curtice_1.dpl>
  <DataSet=curtice_1.dat>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
  <OpenDisplay=1>
  <PosX=0>
  <PosY=0>
  <RunScript=0>
  <Script=curtice_1.m>
  <ViewScale=1.10593>
  <ViewX1=22>
  <ViewX2=861>
  <ViewY1=-52>
  <ViewY2=656>
  <showFrame=0>
</Properties>
<Symbol>
  <.PortSym -30 0  0>
  <Line -30 0 20 0 #000080 2 1>
  <Line -10 -20 0 40 #000080 2 1>
  <Line 0 40 0 -30 #000080 2 1>
  <Line 0 -10 0 -30 #000080 2 1>
  <.ID 20 -46 Curtice>
  <.PortSym 0 -40  0>
  <.PortSym 0 40  0>
  <Line -10 -10 10 0 #000080 2 1>
  <Line -10 10 10 0 #000080 2 1>
  <Line -5 35 10 -10 #000080 2 1>
</Symbol>
<Components>
    <R Rs 1 280 410 -26 15 0 0 "RS" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
    <L Ls 1 200 410 -26 10 0 0 "LS" 1 "" 0>
    <Port Source 1 90 410 -23 12 0 0 "1" 1 "analog" 0>
    <R Rin 1 240 320 -26 15 0 0 "RIN" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
    <EDD D2 1 310 180 -26 -40 1 2 "explicit" 0 "1" 0 "0" 0 "CGD*V1" 0>
    <Port Drain 1 90 180 -23 -69 1 0 "3" 1 "analog" 0>
    <R Rd 1 220 180 -39 -85 1 0 "RD" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
    <L Ld 1 140 180 -26 10 0 0 "LD" 1 "" 0>
    <R Rg 1 360 130 15 -26 0 1 "RG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
    <L Lg 1 360 60 10 -26 0 1 "LG" 1 "" 0>
    <Port Gate 1 360 30 -85 -42 0 3 "2" 1 "analog" 0>
    <Eqn Eqn1 1 490 20 -32 16 0 0 "Vt=kB/q*TK" 1 "GMIN=1e-12" 1 "TK=Temp+273.15" 1 "TnK=Tnom+273.15" 1 "yes" 0>
    <Eqn Eqn2 1 490 240 -32 16 0 0 "TR=TK/TnK" 1 "IsT=IS*exp(XTI/N*ln(TR) - EG/N/Vt*(1-TR))" 1 "yes" 0>
    <EDD D1 1 300 270 -26 -130 0 3 "explicit" 0 "4" 0 "V1<-VBR+50*Vt ? -IsT*(1+exp(-(VBR+V1)/Vt)) + GMIN*V1 : 0" 1 "0" 0 "V1>=-VBR+50*Vt && V1<-5*Vt ? -IsT+GMIN*V1 : V1>=-5*Vt ? IsT*(exp(V1/(N*Vt))-1) + GMIN*V1 : 0" 0 "0" 0 "V1-VT0>0 ? Beta*(V1-VT0)^2*(1+Lambda*V3)*tanh(Alpha*V3) : 0" 0 "CDS*V3 + TAU*I3" 0 "0" 0 "CGS*V4" 0>
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
