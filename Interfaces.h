#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"
#include <unordered_map>
#include <vector>
#include <list>
#include <utility>

class PlayerView;

const EntityProperties& getEntityProperties(PlayerView const &playerView, EntityType type);

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
    int turretCount = 0;
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
    int turretID = 0;

    int builderBaseIndex = 0;
    int rangedBaseIndex = 0;
    int meleeBaseIndex = 0;

    int entityCost[EntityType::TURRET + 1];
    int builderUnitsCost = 0;
    int rangedUnitsCost = 0;
    int meleeUnitsCost = 0;

    int houseSize = 0;
    int rangedBaseSize = 0;
    int mapSize = 0;

    std::unordered_map<int, int> map;

    std::unordered_map<int, EnemyInfo> enemies;
    std::vector<int> needRepairBuildings;

    bool isBaseAttacked = false;
    std::vector<int> attackingEnemyUnits;
    std::vector<int> enemyUnits;

    std::vector<int> enemyBuilderUnits;

    std::vector<int> myBuildings;

    std::vector<int> safertyResources;

    const PlayerView *playerView;

    int mainEnemy = 0;

    bool getFreeHouseCoordinate(int &x, int &y) const;
    std::vector<Vec2Int> getFreeHouseCoordinates() const;

    std::vector<Vec2Int> getFreeRangeBaseCoordinates() const;


    std::vector<Vec2Int> getFreeCoordinateForBuilding(Vec2Int point, size_t size) const;
    std::vector<Vec2Int> getFreeCoordinateForHouseBuild(Vec2Int point) const;

    inline int getIndex(int x, int y) const
    {
        if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
        {
            return 7 * mapSize + 7;
        }
        return y * mapSize + x;
    }

    bool isSafetryPosition(int x, int y) const;

    void getNearestResources(const Entity &entity, int &x, int &y) const;
//    bool getNearestSafertyResources(const Entity &entity, int &x, int &y) const;

    double getDistance(const Entity &unit, const Entity &building) const;
    double getDistance(const Entity &unit, int x, int y) const;
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
    double getDistanceSqr(const Entity &unit, const Entity &building);

    double getDistance(const Entity &unit, int x, int y);

protected:
    virtual void createEntitiesByBuildings(Action &act);
    virtual void turretAttack(Action &act);

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



    virtual bool tryRepair(Action &act, const Entity& entity);

    virtual void farmResources(Action &act, const Entity& entity, int i);

    virtual void fillRepairMap();
    virtual void getCreateUnitCoordinates(int &x, int &y);
    virtual void fillBuildHouseMap();

    virtual void fillBuildRangeBaseaMap();

    std::unordered_map<int, int> m_repairMap;
    std::unordered_map<int, std::pair<Vec2Int, Vec2Int>> m_buildHouseMap;
    std::unordered_map<int, EntityType> m_buildTypeMap;
};

class IWarMinistry : public IMinistry
{
public:
protected:
    virtual Vec2Int getNearestEnemyBuilderUnitCoords(const Entity & entity);
};

class IDefenceMinistry : public IMinistry
{
public:

protected:

    virtual void fillAttackMap();

    std::unordered_map<int, int> m_attackMap;
    std::unordered_map<int, int> m_attackMapCounter;
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
