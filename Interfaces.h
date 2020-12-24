#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <utility>

class PlayerView;

struct EnemyInfo
{
    int mainX = 0;
    int mainY = 0;

    int builderUnitsCount = 0;
    int rangedUnitsCount = 0;
    int meleeUnitsCount = 0;

    int builderBaseX = -1;
    int builderBaseY = -1;

    double distance = 1000;

    int entityCount = 0;
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

    static int builderUnitPopulationUse;
    static int rangedUnitPopulationUse;
    static int meleeUnitPopulationUse;

    static int entityCost[EntityType::TURRET + 1];
    static int builderUnitsCost;
    static int rangedUnitsCost;
    static int meleeUnitsCost;
    static int houseSize;
    static int turretSize;
    static int rangedBaseSize;
    static int mapSize;
    static int playersCount;
    static const int MAX_ENEMIES = 5;
    static EnemyInfo enemies[MAX_ENEMIES];

    static EntityProperties entityProperties[EntityType::TURRET + 1];

    int builderBaseId = 0;
    int rangedBaseID = 0;
    int meleeBaseID = 0;
    int turretID = 0;

    int builderBaseIndex = 0;
    int rangedBaseIndex = 0;
    int meleeBaseIndex = 0;

    std::unordered_map<int, int> map;

    std::unordered_map<int, Vec2Int> sightMap;
    std::unordered_map<int, int> attackMap;

    static std::unordered_map<int, int> lastMap; // EntityType
    static std::unordered_map<int, int> lastUpdatedMap;

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


    std::vector<Vec2Int> getFreeBuildingsCoordinates(const std::vector<Vec2Int> &init, int size) const;

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

    void getXYfromIndex(int index, int&x, int&y) const;

    bool isSafetryPosition(int x, int y) const;

    void getNearestResources(const Entity &entity, int &x, int &y) const;
    bool getNearestSafertyResources(const Entity &entity, int &x, int &y) const;

    std::vector<Vec2Int> getRouteAStar(const Entity &entity, Vec2Int dest) const;

    std::vector<Vec2Int> getRouteAStarWar(const Entity &entity, Vec2Int dest) const;

    double getDistance(const Entity &unit, const Entity &building) const;
    double getDistanceSqr(const Entity &unit, const Entity &building) const;
    double getDistance(const Entity &unit, int x, int y) const;
    double getDistance(int x, int y, int px, int py) const;
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
    virtual void createEntitiesByBuildings(Action &act);

    virtual void farmResources(Action &act, const Entity& entity, int i);

    virtual void turretAttack(Action &act);
    virtual void turretAttack(Action &act, int turretId);


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
    virtual void getExploringData(const PlayerView &playerView, ExploringData &data);
};


class IEconomicsMinistry : public IMinistry
{
protected:
    virtual void createBuilderUnit(Action &act);

    virtual bool tryRepair(Action &act, const Entity& entity);


    virtual void fillRepairMap();
    virtual void getCreateUnitCoordinates(int &x, int &y);

    virtual std::vector<Vec2Int> getTurretsCoordinates() const;
    virtual std::vector<Vec2Int> getHousesCoordinates() const;
    virtual std::vector<Vec2Int> getRangedBaseCoordinates() const;

    virtual void fillBuildingMap(EntityType type);

    virtual void fillBuildRangeBaseaMap();
    virtual void fillBuildHouseMap();
    virtual void fillBuildTurrets();

    std::unordered_map<int, int> m_repairMap;
    std::unordered_map<int, std::pair<Vec2Int, Vec2Int>> m_buildHouseMap;
    std::unordered_map<int, EntityType> m_buildTypeMap;
};

class IWarMinistry : public IMinistry
{
public:
protected:
    virtual Vec2Int getNearestEnemyBuilderUnitCoords(const Entity & entity);

    virtual std::vector<Vec2Int> getDefencePositions() const;

    virtual void fillPositionMap();

    std::unordered_map<int, Vec2Int> m_positionMap;
    std::unordered_map<Vec2Int, int> m_reversePositionMap;
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
