
#include <cstdio>
#include "qucs-core/qucs_interface.h"

using namespace qucs;

void testmessage(int level, const char * message, ...);

int main (int argc, char ** argv)
{

    char infile[] = "/home/richard/src/qucs-qucs_namespace/examples/cpp-interface/boostconverter.net";

//    char infile[] = "m-test-3.net";

    //object::object myobject;
    //trsolver mytr;

    trsolver_interface theqtr (infile);

//    e_trsolver * the_e_trsolver;

//    theqtr.prepare_netlist(infile);

//    the_e_trsolver = (e_trsolver *)theqtr.getETR();

    // make the output message function our custom function
    theqtr.setMessageFcn( &testmessage );

    //theqtr.debug();
    double tend = 5.0e-4;
    double start = 0.0;
    double delta = (tend - start) / 200.0;

    theqtr.init(start, delta/100, ETR_MODE_ASYNC);

    theqtr.printSolution ();

    for (double t = start; t <= tend; t += delta)
    {
        theqtr.stepsolve_async(t);

        theqtr.printSolution ();

        theqtr.acceptstep_async();
    }

    //theqtr.debug();
//    delta = 0.01;
//    start = 0.0;

//    theqtr.init(start, delta, ETR_MODE_SYNC);
//
//    int N = theqtr.getN();
//
//    int M = theqtr.getM();
//
//    theqtr.printx();
//
//    for (double t = start+(NR_TINY*10); t <= tend; t += delta)
//    {
//        theqtr.stepsolve_sync(t);
//
//        theqtr.acceptstep_sync();
//
//        theqtr.printx();
//    }

//    theqtr.stepsolve_sync(0.01);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//    theqtr.stepsolve_sync(0.02);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//        theqtr.stepsolve_sync(0.03);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//        theqtr.stepsolve_sync(0.04);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//        theqtr.stepsolve_sync(0.05);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//    theqtr.stepsolve_sync(0.06);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//    theqtr.stepsolve_sync(0.15);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();
//
//    theqtr.stepsolve_sync(0.3);
//
//    theqtr.acceptstep_sync();
//
//    theqtr.printx();



    return 0;

}

void testmessage(int level, const char * message, ...)
{
    std::printf("%s\n", message);
}
