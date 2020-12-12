#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"
#include <unordered_map>
#include <vector>
#include <list>

class PlayerView;

struct EnemyInfo
{
    int mainX = 0;
    int mainY = 0;

    double dangerousLevel = 0;

    int builderUnitsCount = 0;
    int rangedUnitsCount = 0;
    int meleeUnitsCount = 0;

    int builderBaseX = -1;
    int builderBaseY = -1;

    double testDistance = 1000;
};

struct ExploringData
{
    int builderUnitsCount = 0;
    int rangedUnitsCount = 0;
    int meleeUnitsCount = 0;
    int builderBaseCount = 0;
    int rangedBaseCount = 0;
    int meleeBaseCount = 0;
    int housesCount = 0;
    int myResourcesCount = 0;
    int mapResourcesCount = 0;
    int maxPopulation = 0;
    int currentPopulation = 0;
    int freePopulation = 0;

    int builderUnitPopulationUse = 1;
    int rangedUnitPopulationUse = 1;
    int meleeUnitPopulationUse = 1;

    int builderBaseId = 0;
    int rangedBaseID = 0;
    int meleeBaseID = 0;

    int builderBaseIndex = 0;
    int rangedBaseIndex = 0;
    int meleeBaseIndex = 0;

    int entityCost[EntityType::TURRET + 1];
    int builderUnitsCost = 0;
    int rangedUnitsCost = 0;
    int meleeUnitsCost = 0;

    int houseSize = 0;
    int mapSize = 0;

    std::unordered_map<int, int> map;

    std::unordered_map<int, EnemyInfo> enemies;
    std::vector<int> needRepairBuildings;

    bool isBaseAttacked = false;
    std::vector<int> attackedEnemyUnits;
    std::vector<int> enemyUnits;

    std::vector<int> myBuildings;

    std::list<int> safertyResources;

    const PlayerView *playerView;

    int mainEnemy = 0;

    bool getFreeHouseCoordinate(int &x, int &y) const;

    int getIndex(int x, int y) const;

    void getNearestResources(const Entity &entity, int &x, int &y) const;

private:
    double getDistance(const Entity &unit, const Entity &building) const;
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

    double getDistance(const Entity &unit, const Entity &building);
    double getDistance(const Entity &unit, int x, int y);

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
protected:
    virtual void createBuilderUnit(Action &act);

    virtual void fillRepairMap();
    virtual void getCreateUnitCoordinates(int &x, int &y);

    std::unordered_map<int, int> m_repairMap;
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
