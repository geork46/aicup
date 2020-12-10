#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"

class PlayerView;

struct ExploringData
{

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
