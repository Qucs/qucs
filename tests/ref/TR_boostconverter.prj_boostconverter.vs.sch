<Qucs Schematic 0.0.21-dev>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=boostconverter.dat>
  <DataDisplay=boostconverter.dpl>
  <OpenDisplay=1>
  <Script=boostconverter.m>
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
  <L L1 1 170 150 0 0 0 0 "47uH" 1 "0" 0>
  <Vdc V2 1 70 200 0 0 0 1 "12V" 1>
  <GND \* 5 70 250 0 0 0 0>
  <Eqn Eqn1 1 0 0 0 0 0 0 "y=1" 1 "yes" 0>
  <Eqn Eqn2 1 0 0 0 0 0 0 "y=1" 1 "yes" 0>
  <Relais S1 1 350 200 0 0 0 0 "0.5 V" 0 "0.1 V" 0 "1" 0 "1e12" 0 "26.85" 0>
  <Vrect V1 1 220 300 0 0 0 1 "1V" 1 "Ton" 1 "Toff" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0>
  <GND \* 5 220 370 0 0 0 0>
  <Diode D1 1 490 150 0 0 0 2 "1e-12 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND \* 5 660 250 0 0 0 0>
  <C C1 1 560 200 0 0 1 3 "100u" 1 "0" 0 "neutral" 0>
  <R R1 1 660 200 0 0 0 1 "5" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
