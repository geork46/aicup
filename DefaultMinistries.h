#ifndef DEFAULTMINISTRIES_H
#define DEFAULTMINISTRIES_H

#include "Interfaces.h"


class DefaultEconomicMinister : public IEconomicsMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class DefaultWarMinister : public IWarMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class DefaultDefenceMinister : public IDefenceMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};
#endif // DEFAULTMINISTRIES_H
