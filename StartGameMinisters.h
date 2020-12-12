#ifndef STARTGAMEMINISTERS_H
#define STARTGAMEMINISTERS_H

#include "Interfaces.h"


class StartGameEconomicMinister : public IEconomicsMinistry
{
public:
    virtual void addMinistryAction(Action &act);
};

class StartGameWarMinister : public IWarMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class StartGameDefenceMinister : public IDefenceMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class StartGameDistributor : public IDistributor
{
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);
};

class MoreWarDistributor : public IDistributor
{
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);
};

#endif // STARTGAMEMINISTERS_H
