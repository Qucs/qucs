<Qucs Schematic 0.0.22>
<Properties>
  <View=10,-44,1733,1003,1,0,180>
  <Grid=10,10,1>
  <DataSet=test_diode_3f5_dc.dat>
  <DataDisplay=test_diode_3f5_dc.dpl>
  <OpenDisplay=1>
  <Script=test_diode_3f5_dc.m>
  <RunScript=0>
  <showFrame=3>
  <FrameText0=Test d_3f5 d.c characteristics.\nTwo sweep process:\n1 dcsweep 0.3 <= 1.2 V in 0.02V steps.\n2 Step diode resistance rs over range\n    0.2 to 1.2 Ohm.>
  <FrameText1=Drawn By:Mike Brinson>
  <FrameText2=Date:2 March 2021>
  <FrameText3=Revision:R1>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 490 380 0 0 0 0>
  <Lib d_3f52 1 490 340 40 -21 0 0 "Xyce" 0 "d_3f5" 0 "1" 1 "0" 1 "0" 0 "0" 1 "26.85" 1 "1e-14" 1 "res" 1 "0" 0 "1.0" 1 "0.0" 0 "1.0" 0 "0.5" 0 "1.11" 0 "3.0" 0 "0.0" 0 "1.0" 0 "1e99" 0 "0.001" 0 "26.58" 0 "0.5" 0>
  <Lib ammeter1 1 400 300 -30 19 0 0 "Xyce" 0 "ammeter" 0>
  <GND * 1 330 360 0 0 0 0>
  <Lib v_dc1 1 350 350 -50 46 0 0 "Xyce" 0 "v_dc" 0 "dcsweep" 1>
  <.XYCESCR XYCESCR1 1 300 470 0 51 0 0 "\n.global_param dcsweep = 0.0\n.global_param res=0.1\n.dc lin dcsweep 0.3 1.2 0.02\n.step lin res 0.1 1.2 0.2\n.print dc format=std file=dc.txt dcsweep\n+ i(xammeter1:vprobe)" 1 "" 0 "dc.txt" 0>
</Components>
<Wires>
  <440 300 490 300 "" 0 0 0 "">
  <330 300 380 300 "" 0 0 0 "">
  <330 300 330 310 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 730 421 359 241 2 #c0c0c0 1 00 1 0.3 0.2 1.2 1 -0.336365 1 4 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/I(XAMMETER1_VPROBE)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 730 743 351 243 2 #c0c0c0 1 01 1 0.3 0.2 1.2 1 1e-09 1 10 1 -1 1 1 315 0 225 "" "" "">
	<"xyce/I(XAMMETER1_VPROBE)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
