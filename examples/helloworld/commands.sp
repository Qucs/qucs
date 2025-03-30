
.include helloworld.sp

* .ground p does not work..
.subckt gndhack ( p )
R1 p 0 0.
.ends

X1 in out helloworld

* .options acct
* .options reltol=1e-2
.options nopage
.tran .1 1
.print tran v(in) v(out)
