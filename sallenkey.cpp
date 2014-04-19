#include "sallenkey.h"

SallenKey::SallenKey(QVector< std::complex<float> > poles_, bool HighPass_) :
    Filter(poles_, HighPass_)
{
}

QString* SallenKey::createSchematic()
{

}


void SallenKey::calcFilter()
{

}
