<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=microstrip.dat>
  <DataDisplay=microstrip.dpl>
  <OpenDisplay=1>
  <Script=microstrip.m>
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
  <Pac P3 1 340 320 18 -26 0 1 "3" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 340 380 0 0 0 0>
  <GND \* 5 240 290 0 0 0 0>
  <Pac P2 1 550 140 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 550 200 0 0 0 0>
  <Pac P1 1 100 140 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 100 200 0 0 0 0>
  <C C1 1 260 290 -26 17 0 0 "30 pF" 1 "" 0 "neutral" 0>
  <MLIN MS1 1 200 130 -26 15 0 0 "Aluminia" 1 "1 mm" 1 "10 mm" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <MLIN MS2 1 420 130 -26 15 0 0 "Aluminia" 1 "1 mm" 1 "10 mm" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
  <SUBST Aluminia 1 0 0 -30 24 0 0 "9.8" 1 "1 mm" 1 "35 um" 1 "1e-3" 1 "0.022e-6" 1 "0.15e-6" 1>
  <MTEE MS4 1 310 130 -26 -105 0 0 "Aluminia" 1 "1 mm" 1 "1 mm" 1 "0.5 mm" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0 "showNumbers" 0>
  <MLIN Stub 1 340 270 -26 15 0 0 "Aluminia" 1 "0.5 mm" 1 "10 mm" 1 "Hammerstad" 0 "Kirschning" 0 "26.85" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
