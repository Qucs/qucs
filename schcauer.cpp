#include "schcauer.h"

SchCauer::SchCauer(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par) :
    Filter(ffunc_, type_, par)
{
    Nr = 4;
    Nopamp = 3;
    Nc = 2;
}

void SchCauer::calcLowPass()
{

}


void SchCauer::calcHighPass()
{

}


void SchCauer::createLowPassSchematic(QString &s)
{

}

void SchCauer::createHighPassSchematic(QString &s)
{

}
