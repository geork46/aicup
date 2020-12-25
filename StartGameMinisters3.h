#ifndef STARTGAMEMINISTERS3_H
#define STARTGAMEMINISTERS3_H


#include "Interfaces.h"


class StartGameEconomicMinister3 : public IEconomicsMinistry
{
public:
    virtual void activate();
    virtual void addMinistryAction(Action &act);
protected:
    virtual void farmResources(Action &act, const Entity& entity, int i);
    virtual std::vector<Vec2Int> getTurretsCoordinates() const;
};

class StartGameWarMinister3 : public IWarMinistry
{
public:
    virtual void addMinistryAction(Action &act);

private:
    int m_spyCounter;

    void addSpyAction(Action &act, const Entity &entity);
};

class StartGameDefenceMinister3 : public IDefenceMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

class StartGameDistributor3 : public IDistributor
{
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);

};

#endif // STARTGAMEMINISTERS3_H
