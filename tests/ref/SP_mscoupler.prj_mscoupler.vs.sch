<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=mscoupler.dat>
  <DataDisplay=mscoupler.dpl>
  <OpenDisplay=1>
  <Script=mscoupler.m>
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
  <Pac P1 1 0 0 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P2 1 0 0 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P3 1 0 0 18 -26 0 1 "3" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <Pac P4 1 0 0 18 -26 0 1 "4" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND \* 5 0 0 0 0 0 0>
  <GND \* 5 0 0 0 0 0 0>
  <GND \* 5 0 0 0 0 0 0>
  <GND \* 5 0 0 0 0 0 0>
  <SUBST SubstTC1 1 0 0 -30 24 0 0 "9.8" 1 "0.635 mm" 1 "17.5 um" 1 "0.0001" 1 "2.43902e-08" 1 "1.5e-07" 1>
  <MCOUPLED MSTC1 1 0 0 -26 37 0 0 "SubstTC1" 1 "0.518 mm" 1 "14.94 mm" 1 "0.185 mm" 1 "Kirschning" 0 "Kirschning" 0 "26.85" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
