#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"

class PlayerView;

struct ExploringData
{
    int buildUnitsCount = 0;
    int rangedUnitsCount = 0;
    int meleeUnitsCount = 0;
    int buildBaseCount = 0;
    int rangedBaseCount = 0;
    int meleeBaseCount = 0;
    int housesCount = 0;
    int myResourcesCount = 0;
    int mapResourcesCount = 0;
    int maxPopulation = 0;
    int currentPopulation = 0;

    int entityCost[EntityType::TURRET + 1];
};


class IMinistry
{
public:
    void addEntity(Entity const & e);
    void removeAllEntities();

    virtual void addMinistryAction(Action &act);

    void setExploringData(const ExploringData *exploringData);
    void setPlayerView(const PlayerView *playerView);

    virtual void activate();
    virtual void deactivate();

    int resourcesCount() const;
    void setResourcesCount(int resourcesCount);

    int maxPopulation() const;
    void setMaxPopulation(int maxPopulation);

protected:
    const ExploringData *m_exploringData = nullptr;
    const PlayerView *m_playerView = nullptr;

    int m_resourcesCount;
    int m_maxPopulation;
    std::vector<Entity> m_units;
    std::vector<Entity> m_buildings;
};

class IExploringMinistry : public IMinistry
{
public:
    virtual ExploringData getExploringData(const PlayerView &playerView);
};


class IEconomicsMinistry : public IMinistry
{
public:
};

class IWarMinistry : public IMinistry
{
public:
};

class IDefenceMinistry : public IMinistry
{
public:
};

class IDistributor
{
public:
    virtual void activate();
    virtual void deactivate();

    virtual void redistribute(const PlayerView &playerView, ExploringData const &data,
                              IEconomicsMinistry *economic,
                              IWarMinistry *war,
                              IDefenceMinistry *defence);
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);
    IEconomicsMinistry* 	m_economicMinister = nullptr;
    IWarMinistry* 			m_warMinister = nullptr;
    IDefenceMinistry* 		m_defenceMinister = nullptr;
};
#endif
