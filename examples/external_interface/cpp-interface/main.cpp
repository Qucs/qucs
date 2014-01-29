#include <iostream>
#include <cmath>
#include "qucs-core/qucs_interface.h"

using namespace qucs;

// message function used to print messages from the solver
void testmessage(int level, const char * message, ...);

// example
void example_probe_and_subcircuit_access_async (void);
void example_ecvs_async (void);

int main (int argc, char ** argv)
{

//    example_probe_and_subcircuit_access_async ();

    example_ecvs_async ();

    return 0;

}

void testmessage(int level, const char * message, ...)
{
    printf("%s\n", message);
}

void example_probe_and_subcircuit_access_async (void)
{
    char infile[] = "example_probe_and_subcircuit_access.net";

    char * wirelabel = "test_label";
    char * vprobename = "VProbe1";
    char * iprobename = "IProbe1";
    char * iprobename2 = "IProbe2";
    char * subcktvprobe = "SUB1.SUB1.VProbeS1";

    nr_double_t node_voltage = 0;
    nr_double_t probe_voltage;
    nr_double_t probe_current;

    int exists = -1;

    qucsint thequcsint;
    e_trsolver * the_e_trsolver;

    thequcsint.prepare_netlist (infile);

    the_e_trsolver = (qucs::e_trsolver *)thequcsint.getETR();

    the_e_trsolver->messagefcn = &testmessage;

    double tend = 1e-3;
    double start = 0.0;
    double delta = (tend - start) / 113.0;

    the_e_trsolver->init (start, delta/100, ETR_MODE_ASYNC);

    the_e_trsolver->printx ();

    for (double t = start; t <= tend; t += delta)
    {
        the_e_trsolver->stepsolve_async (t);

        // prints the whole solution, all node voltages and branch currents
        the_e_trsolver->printx ();

        exists = the_e_trsolver->getNodeV (wirelabel, node_voltage);

        if (exists == 0)
        {
            printf("%s node voltage: %f\n", wirelabel, node_voltage);
        }

        exists = the_e_trsolver->getVProbeV (vprobename, probe_voltage);

        if (exists == 0)
        {
            printf ("%s probe voltage: %f\n", vprobename, probe_voltage);
        }

        exists = the_e_trsolver->getIProbeI (iprobename, probe_current);

        if (exists == 0)
        {
            printf ("%s probe current: %f\n", iprobename, probe_current);
        }

        exists = the_e_trsolver->getIProbeI (iprobename2, probe_current);

        if (exists == 0)
        {
            printf ("%s probe current: %f\n", iprobename2, probe_current);
        }


        exists = the_e_trsolver->getVProbeV (subcktvprobe, probe_voltage);

        if (exists == 0)
        {
            printf ("%s probe voltage: %f\n", subcktvprobe, probe_voltage);
        }

        the_e_trsolver->acceptstep_async ();

    }

    the_e_trsolver->finish ();
}


void example_ecvs_async (void)
{
    char infile[] = "example_ecvs.net";

    char * ecvsname = "ECVS1";
    char * iprobename = "Pr1";

    nr_double_t probe_current;

    int exists = -1;

    qucsint thequcsint;
    e_trsolver * the_e_trsolver;

    thequcsint.prepare_netlist (infile);

    the_e_trsolver = (qucs::e_trsolver *)thequcsint.getETR();

    the_e_trsolver->messagefcn = &testmessage;

    double tend = 1;
    double start = 0.0;
    double delta = (tend - start) / 10;

    the_e_trsolver->init (start, delta, ETR_MODE_ASYNC);

    exists = the_e_trsolver->setECVSVoltage(ecvsname, 0);

    the_e_trsolver->printx ();

    for (double t = start; t <= tend; t += delta)
    {
        exists = the_e_trsolver->setECVSVoltage (ecvsname, std::sin(2 * 3.142 * 1.0 * t));

        the_e_trsolver->stepsolve_async (t);

        // prints the whole solution, all node voltages and branch currents
        the_e_trsolver->printx ();

        exists = the_e_trsolver->getIProbeI (iprobename, probe_current);

        if (exists == 0)
        {
            printf ("%s probe current: %f\n", iprobename, probe_current);
        }

        // the solution must be explicitly accepted for it to become a
        // part of the circuit solution history
        the_e_trsolver->acceptstep_async ();
    }
}

