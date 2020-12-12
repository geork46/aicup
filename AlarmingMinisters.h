#ifndef ALARMINGMINISTERS_H
#define ALARMINGMINISTERS_H

#include "Interfaces.h"


class AlarmingEconomicMinister : public IEconomicsMinistry
{
public:
    virtual void addMinistryAction(Action &act);
};

class AlarmingWarMinister : public IWarMinistry
{
public:
    virtual void addMinistryAction(Action &act);


};

class AlarmingDefenceMinister : public IDefenceMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class AlarmingDistributor : public IDistributor
{
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);
};



#endif // ALARMINGMINISTERS_H
