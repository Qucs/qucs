<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=chargepump.dpl>
  <DataSet=chargepump.dat>
  <OpenDisplay=1>
  <PosX=0>
  <PosY=0>
  <ViewScale=1>
  <ViewX1=0>
  <ViewX2=800>
  <ViewY1=0>
  <ViewY2=800>
</Properties>
<Symbol>
</Symbol>
<Components>
    <GND * 5 110 260 0 0 0 0>
    <GND * 5 330 300 0 0 0 0>
    <GND * 5 220 280 0 0 0 0>
    <C C1 1 220 250 17 -26 0 1 "10 pF" 1 "" 0 "neutral" 0>
    <R R1 1 350 160 15 -26 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
    <Vac V1 1 110 230 -72 -26 1 1 "5 V" 1 "10 MHz" 1 "0" 0 "0" 0>
    <Vac V3 1 330 270 18 -26 0 1 "-5 V" 1 "10 MHz" 1 "0" 0 "0" 0>
    <Vac V2 1 350 80 18 -26 0 1 "5 V" 1 "10 MHz" 1 "0" 0 "0" 0>
    <.TR TR1 1 60 30 0 51  0 0 "lin" 1 "0 ns" 1 "2 us" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
<350 190 350 220 "" 0 0 0 "">
<220 220 350 220 "" 0 0 0 "">
<350 110 350 130 "" 0 0 0 "">
<110 190 110 200 "" 0 0 0 "">
<110 190 190 190 "" 0 0 0 "">
<330 190 330 240 "" 0 0 0 "">
<240 190 330 190 "" 0 0 0 "">
<220 40 350 40 "" 0 0 0 "">
<350 40 350 50 "" 0 0 0 "">
<220 40 220 160 "" 0 0 0 "">
<220 220 220 220 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
