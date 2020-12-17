#ifndef STARTGAMEMINISTERS2_H
#define STARTGAMEMINISTERS2_H


#include "Interfaces.h"


class StartGameEconomicMinister2 : public IEconomicsMinistry
{
public:
    virtual void addMinistryAction(Action &act);
};

class StartGameWarMinister2 : public IWarMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class StartGameDefenceMinister2 : public IDefenceMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class StartGameDistributor2 : public IDistributor
{
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);
};


#endif // STARTGAMEMINISTERS2_H