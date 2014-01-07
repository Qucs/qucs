
#include "config.h"
#include "qucs-core/qucs_interface.h"
#include "qucs-core/e_trsolver.h"

using namespace qucs;

void testmessage(int level, const char * message, ...);

int main (int argc, char ** argv)
{

    char infile[] = "boostconverter.net";
//    char infile[] = "m-test-3.net";

    //object::object myobject;
    //trsolver mytr;
    qucsint thequcsint;
    e_trsolver * the_e_trsolver;

    thequcsint.prepare_netlist(infile);

    the_e_trsolver = (e_trsolver *)thequcsint.getETR();

    // make the output message function our custom function
    the_e_trsolver->messagefcn = &testmessage;

    //the_e_trsolver->debug();
    double tend = 5.0e-4;
    double start = 0.0;
    double delta = (tend - start) / 200.0;

    the_e_trsolver->init(start, delta/100, ETR_MODE_ASYNC);

    the_e_trsolver->printx();

    for (double t = start; t <= tend; t += delta)
    {
        the_e_trsolver->stepsolve_async(t);

        the_e_trsolver->printx();

        the_e_trsolver->acceptstep_async();
    }

    //the_e_trsolver->debug();
//    delta = 0.01;
//    start = 0.0;

//    the_e_trsolver->init(start, delta, ETR_MODE_SYNC);
//
//    int N = the_e_trsolver->getN();
//
//    int M = the_e_trsolver->getM();
//
//    the_e_trsolver->printx();
//
//    for (double t = start+(NR_TINY*10); t <= tend; t += delta)
//    {
//        the_e_trsolver->stepsolve_sync(t);
//
//        the_e_trsolver->acceptstep_sync();
//
//        the_e_trsolver->printx();
//    }

//    the_e_trsolver->stepsolve_sync(0.01);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//    the_e_trsolver->stepsolve_sync(0.02);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//        the_e_trsolver->stepsolve_sync(0.03);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//        the_e_trsolver->stepsolve_sync(0.04);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//        the_e_trsolver->stepsolve_sync(0.05);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//    the_e_trsolver->stepsolve_sync(0.06);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//    the_e_trsolver->stepsolve_sync(0.15);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();
//
//    the_e_trsolver->stepsolve_sync(0.3);
//
//    the_e_trsolver->acceptstep_sync();
//
//    the_e_trsolver->printx();



    return 0;

}

void testmessage(int level, const char * message, ...)
{
    printf("%s\n", message);
}
