<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=test_diode_3f5_dc.dpl>
  <DataSet=test_diode_3f5_dc.dat>
  <FrameText0=Test d_3f5 d.c characteristics.nTwo sweep process:n1 dcsweep 0.3 <= 1.2 V in 0.02V steps.n2 Step diode resistance rs over rangen    0.2 to 1.2 Ohm.>>
  <FrameText1=Drawn By:Mike Brinson>
  <FrameText2=Date:2 March 2021>
  <FrameText3=Revision:R1>
  <OpenDisplay=1>
  <PosX=0>
  <PosY=180>
  <RunScript=0>
  <Script=test_diode_3f5_dc.m>
  <ViewScale=1>
  <ViewX1=10>
  <ViewX2=1733>
  <ViewY1=-44>
  <ViewY2=1003>
  <showFrame=3>
</Properties>
<Symbol>
</Symbol>
<Components>
    <GND * 5 490 380 0 0 0 0>
    <Lib d_3f52 1 490 340 40 -21 0 0 "Xyce" 0 "d_3f5" 0>
    <Lib ammeter1 1 400 300 -30 19 0 0 "Xyce" 0 "ammeter" 0>
    <GND * 5 330 360 0 0 0 0>
    <Lib v_dc1 1 350 350 -50 46 0 0 "Xyce" 0 "v_dc" 0>
</Components>
<Wires>
<440 300 490 300 "" 0 0 0 "">
<330 300 380 300 "" 0 0 0 "">
<330 300 330 310 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 730 421 359 241 0 #c0c0c0 1 00 1 0.3 0.2 1.2 1 -0.336365 1 4 1 -1 0.5 1 0 0 0 "" "" "" "0">
	<"xyce/I(XAMMETER1_VPROBE)" #0000ff 2 3 0 0 0>
  </Rect>
  <Rect 730 743 351 243 0 #c0c0c0 1 01 1 0.3 0.2 1.2 1 1e-09 1 10 1 -1 1 1 0 0 0 "" "" "" "0">
	<"xyce/I(XAMMETER1_VPROBE)"     0 0>
	<"xyce/I(XAMMETER1_VPROBE)" #0000ff 2 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
