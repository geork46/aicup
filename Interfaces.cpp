#include "Interfaces.h"

#include <math.h>
#include <queue>

void IMinistry::addEntity(const Entity &e)
{
    if (!this)
    {
        return;
    }

    switch (e.entityType) {
    case EntityType::WALL :
    case EntityType::HOUSE :
    case EntityType::BUILDER_BASE :
    case EntityType::MELEE_BASE :
    case EntityType::RANGED_BASE :
    case EntityType::TURRET :
        m_buildings.push_back(e);
        break;
    case EntityType::BUILDER_UNIT :
    case EntityType::MELEE_UNIT :
    case EntityType::RANGED_UNIT :
        m_units.push_back(e);
        break;
    default:
        break;
    }
}

void IMinistry::removeAllEntities()
{
    m_buildings.clear();
    m_units.clear();
}

void IMinistry::addMinistryAction(Action &act)
{
}

void IMinistry::setExploringData(const ExploringData *exploringData)
{
    m_exploringData = exploringData;
}

void IMinistry::setPlayerView(const PlayerView *playerView)
{
    m_playerView = playerView;
}

void IMinistry::activate() {  }

void IMinistry::deactivate() {  }

int IMinistry::resourcesCount() const
{
    return m_resourcesCount;
}

void IMinistry::setResourcesCount(int resourcesCount)
{
    m_resourcesCount = resourcesCount;
}

int IMinistry::maxPopulation() const
{
    return m_maxPopulation;
}

void IMinistry::setMaxPopulation(int maxPopulation)
{
    m_maxPopulation = maxPopulation;
}

double IMinistry::getDistance(const Entity &unit, const Entity &building)
{
    double x = unit.position.x;
    double y = unit.position.y;

    const EntityProperties& properties = m_playerView->entityProperties.at(building.entityType);
    double px = building.position.x + properties.size / 2;
    double py = building.position.y + properties.size / 2;

    return sqrt((px - x)*(px - x) + (py - y) * (py - y));
}

double IMinistry::getDistance(const Entity &unit, int x, int y)
{
    double px = unit.position.x;
    double py = unit.position.y;

    return sqrt((px - x)*(px - x) + (py - y) * (py - y));
}

void IMinistry::createEntitiesByBuildings(Action &act)
{
    for (size_t i = 0; i < m_buildings.size(); i++) {
        const Entity& entity = m_buildings[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        std::shared_ptr<BuildAction> buildAction = nullptr;

        if (properties.build != nullptr) {
            EntityType entityType = properties.build->options[0];
            if (m_playerView->entityProperties.at(entityType).populationUse <= m_maxPopulation
                    && m_exploringData->entityCost[entityType] <= m_resourcesCount + m_exploringData->builderUnitsCount)
            {
                buildAction = std::shared_ptr<BuildAction>(new BuildAction(
                                                               entityType,
                                                               Vec2Int(entity.position.x + properties.size, entity.position.y + properties.size - 1)));
                m_resourcesCount -= m_exploringData->entityCost[entityType];
            }
        }
        act.entityActions[entity.id] = EntityAction(nullptr, buildAction, nullptr, nullptr);
    }
}

void IDistributor::activate() {  }

void IDistributor::deactivate() {  }

void IDistributor::redistribute(const PlayerView &playerView, const ExploringData &data, IEconomicsMinistry *economic, IWarMinistry *war, IDefenceMinistry *defence)
{
    m_warMinister = war;
    m_economicMinister = economic;
    m_defenceMinister = defence;
    if (war)
    {
        war->removeAllEntities();
        war->setExploringData(&data);
        war->setPlayerView(&playerView);
    }
    if (economic)
    {
        economic->removeAllEntities();
        economic->setExploringData(&data);
        economic->setPlayerView(&playerView);
    }
    if (defence)
    {
        defence->removeAllEntities();
        defence->setExploringData(&data);
        defence->setPlayerView(&playerView);
    }

    innerDistribute(playerView, data);
}

void IDistributor::innerDistribute(const PlayerView &playerView, const ExploringData &data)
{
}

ExploringData IExploringMinistry::getExploringData(const PlayerView &playerView)
{

}


bool ExploringData::getFreeHouseCoordinate(int &x, int &y) const
{
    static const int n = 19;
    static int a[n] = {0, 3, 6, 9, 12, 15, 18, 21, 0, 0, 0,  0,  0,  0,  0, 11, 11,  4,  8};
    static int b[n] = {0, 0, 0, 0,  0,  0,  0,  0, 4, 7, 10, 13, 16, 19, 22,  4,  8, 11, 11};

    for (int k = 0; k < n; ++k)
    {
        for (int i = 0; i < houseSize; ++i)
        {
            for (int j = 0; j < houseSize; ++j)
            {
                if (map.find(getIndex(a[k] + i, b[k] + j)) != map.end())
                {
                    goto next;
                }
            }
        }
        x = a[k];
        y = b[k];
        return true;
        next:
        continue;
    }
    return false;
}

std::vector<Vec2Int> ExploringData::getFreeHouseCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(0, 0));
    init.push_back(Vec2Int(0, 3));
    init.push_back(Vec2Int(0, 6));
    init.push_back(Vec2Int(0, 9));
    init.push_back(Vec2Int(0, 12));
    init.push_back(Vec2Int(0, 15));
    init.push_back(Vec2Int(0, 18));
    init.push_back(Vec2Int(0, 21));
    init.push_back(Vec2Int(4, 0));
    init.push_back(Vec2Int(7, 0));
    init.push_back(Vec2Int(10, 0));
    init.push_back(Vec2Int(13, 0));
    init.push_back(Vec2Int(16, 0));
    init.push_back(Vec2Int(19, 0));
    init.push_back(Vec2Int(22, 0));
//    init.push_back(Vec2Int(11, 4));
//    init.push_back(Vec2Int(11, 8));
//    init.push_back(Vec2Int(4, 11));
//    init.push_back(Vec2Int(7, 11));
    init.push_back(Vec2Int(22, 3));
    init.push_back(Vec2Int(22, 6));
    init.push_back(Vec2Int(22, 9));
    init.push_back(Vec2Int(23, 12));
    init.push_back(Vec2Int(3, 22));
    init.push_back(Vec2Int(6, 22));
    init.push_back(Vec2Int(9, 22));
    init.push_back(Vec2Int(12, 23));

    std::vector<Vec2Int> result{};

    for (int k = 0; k < init.size(); ++k)
    {
        for (int i = 0; i < houseSize; ++i)
        {
            for (int j = 0; j < houseSize; ++j)
            {
                if (map.find(getIndex(init[k].x + i, init[k].y + j)) != map.end())
                {
                    goto next;
                }
            }
        }
        result.push_back(init[k]);
        next:
        continue;
    }
    return result;
}

std::vector<Vec2Int> ExploringData::getFreeRangeBaseCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(11, 11));
//    init.push_back(Vec2Int(11, 6));
    init.push_back(Vec2Int(11, 7));
//    init.push_back(Vec2Int(11, 8));
    init.push_back(Vec2Int(11, 9));
//    init.push_back(Vec2Int(11, 10));
    init.push_back(Vec2Int(6, 11));
    init.push_back(Vec2Int(7, 11));
//    init.push_back(Vec2Int(8, 11));
    init.push_back(Vec2Int(9, 11));
//    init.push_back(Vec2Int(10, 11));
    init.push_back(Vec2Int(12, 12));

    std::vector<Vec2Int> result{};

    for (int k = 0; k < init.size(); ++k)
    {
        for (int i = 0; i < rangedBaseSize; ++i)
        {
            for (int j = 0; j < rangedBaseSize; ++j)
            {
                if (map.find(getIndex(init[k].x + i, init[k].y + j)) != map.end())
                {
                    goto next;
                }
            }
        }
        result.push_back(init[k]);
        next:
        continue;
    }
    return result;

}

std::vector<Vec2Int> ExploringData::getFreeCoordinateForBuilding(Vec2Int point, size_t size) const
{
    std::vector<Vec2Int> add{};
    for (int i = 0; i < size; ++i)
    {
        add.push_back(Vec2Int(-1, i));
        add.push_back(Vec2Int(i, -1));
        add.push_back(Vec2Int(size, i));
        add.push_back(Vec2Int(i, size));
    }

    std::vector<Vec2Int> result{};

    for (int k = 0; k < add.size(); ++k)
    {
        if (map.find(getIndex(add[k].x + point.x, add[k].y + point.y)) == map.end())
        {
            result.push_back(Vec2Int(add[k].x + point.x, add[k].y + point.y));
        }
    }
    return result;
}

std::vector<Vec2Int> ExploringData::getFreeCoordinateForHouseBuild(Vec2Int point) const
{
    return getFreeCoordinateForBuilding(point, houseSize);
}

int ExploringData::getIndex(int x, int y) const
{
    if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
    {
        return 7 * mapSize + 7;
    }
    return y * mapSize + x;
}

bool ExploringData::isSafetryPosition(int x, int y) const
{
    bool f = true;
    for (int i : enemyUnits)
    {
        if (playerView->entities[i].entityType == EntityType::BUILDER_UNIT)
        {
            continue;
        }
        double d = getDistance(playerView->entities[i], x, y);
        if (d < 9)
        {
            f = false;
            break;
        }
    }
    return f;
}

void ExploringData::getNearestResources(const Entity &entity, int &x, int &y) const
{
    const EntityProperties& properties = playerView->entityProperties.at(entity.entityType);

    double dist = 1000;
    int k = 0;
    for (int i : safertyResources)
    {
        if (getDistance(entity, playerView->entities[i]) < dist)
        {
            dist = getDistance(entity, playerView->entities[i]);
            k = i;
        }
    }
    x = playerView->entities[k].position.x;
    y = playerView->entities[k].position.y;
}





bool ExploringData::getNearestSafertyResources(const Entity &entity, int &x, int &y) const
{
    std::unordered_map<Vec2Int, Vec2Int> traking;

    std::vector<Vec2Int> add{};
    add.push_back(Vec2Int(0, 1));
    add.push_back(Vec2Int(0, -1));
    add.push_back(Vec2Int(1, 0));
    add.push_back(Vec2Int(-1, 0));

    std::queue<Vec2Int> queue;
    std::vector<Vec2Int> log;
    queue.push(entity.position);

    Vec2Int finish;
    bool f = false;

    for (int i = 0; i < 5000 && !f && queue.size() > 0; ++i)
    {
        Vec2Int current = queue.front();
        log.push_back(current);
        queue.pop();
        for (int j = 0; j < add.size(); ++j)
        {
            if (traking.find(Vec2Int(current.x + add[j].x, current.y + add[j].y)) != traking.end())
            {
                continue;
            }
            traking[Vec2Int(current.x + add[j].x, current.y + add[j].y)] = current;

            if (map.find(getIndex(current.x + add[j].x, current.y + add[j].y)) != map.end())
            {
                if (playerView->entities[map.at(getIndex(current.x + add[j].x, current.y + add[j].y))].entityType == EntityType::RESOURCE
                        && isSafetryPosition(current.x, current.y) && isSafetryPosition(current.x + add[j].x, current.y + add[j].y))
                {
                    finish = current;
                    f = true;
                    break;
                }
                continue;
            }

            if (isSafetryPosition(current.x + add[j].x, current.y + add[j].y))
            {
                queue.push(Vec2Int(current.x + add[j].x, current.y + add[j].y));
            }
        }
    }
    if (f)
    {
        Vec2Int current = finish;
        Vec2Int current2 = finish;
        while (!(current == entity.position))
        {
            current2 = current;
            current = traking[Vec2Int(current.x, current.y)];
        }
//        if (current == current2)
//        {
//            f = false;
//        }
        x = current2.x;
        y = current2.y;
    } else
    {
        getNearestResources(entity, x, y);
    }
    return f;
}

double ExploringData::getDistance(const Entity &unit, const Entity &building) const
{
    double x = unit.position.x;
    double y = unit.position.y;

    const EntityProperties& properties = playerView->entityProperties.at(building.entityType);
    double px = building.position.x + properties.size / 2;
    double py = building.position.y + properties.size / 2;

    return sqrt((px - x)*(px - x) + (py - y) * (py - y));
}

double ExploringData::getDistance(const Entity &unit, int x, int y) const
{
    double px = unit.position.x;
    double py = unit.position.y;

    return sqrt((px - x)*(px - x) + (py - y) * (py - y));
}

void IEconomicsMinistry::createBuilderUnit(Action &act)
{
    std::shared_ptr<BuildAction> buildAction = nullptr;

    if (m_exploringData->builderUnitPopulationUse <= m_maxPopulation
            && m_exploringData->builderUnitsCost <= m_resourcesCount + m_exploringData->builderUnitsCount)
    {

        int x, y;
        getCreateUnitCoordinates(x, y);
        buildAction = std::shared_ptr<BuildAction>(new BuildAction(EntityType::BUILDER_UNIT, Vec2Int(x, y)));
        m_resourcesCount -= m_exploringData->builderUnitsCost;
    }
    act.entityActions[m_exploringData->builderBaseId] = EntityAction( nullptr, buildAction, nullptr, nullptr);
}

bool IEconomicsMinistry::tryRepair(Action &act, const Entity &entity)
{
    bool f2 = false;
    int target = -1;
    {
        int a[4] = {0, 0, 1, -1};
        int b[4] = {1, -1, 0, 0};
        int m = -1;
        for (int k = 0; k < 4; ++k)
        {
            if  (m_exploringData->map.find(m_exploringData->getIndex(entity.position.x + a[k], entity.position.y + b[k])) !=
                 m_exploringData->map.end())
            {
                int d = m_exploringData->map.at(m_exploringData->getIndex(entity.position.x + a[k], entity.position.y + b[k]));
                const Entity& en = m_playerView->entities[d];
                const EntityProperties& prop = m_playerView->entityProperties.at(en.entityType);
                if (en.health < prop.maxHealth && en.entityType != EntityType::RESOURCE
                        && *en.playerId == m_playerView->myId)
                {
                    f2 = true;
                    target = en.id;
                    break;
                }
            }
        }
    }
    if (f2)
    {
        act.entityActions[entity.id] = EntityAction(
                    nullptr, nullptr, nullptr, std::shared_ptr<RepairAction>(new RepairAction(target)));
    }
    return f2;
}

void IEconomicsMinistry::farmResources(Action &act, const Entity &entity, int i)
{
    const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
    std::shared_ptr<MoveAction> moveAction = nullptr;
    int x = m_playerView->mapSize - 1;
    int y = m_playerView->mapSize - 1;


    bool succes = false;
    if (i % 2 != 0)
    {
//        m_exploringData->getNearestResources(entity, x, y);
//        succes = m_exploringData->getNearestSafertyResources(entity, x, y);
        int x = m_playerView->mapSize / 4 + 1;
        int y = m_playerView->mapSize / 4;
    } else if (i % 4 == 0)
    {
        int x = m_playerView->mapSize - 1;
        int y = m_playerView->mapSize / 4 + 1;
    }

    std::vector<EntityType> validAutoAttackTargets;
    if (entity.entityType == BUILDER_UNIT) {
        validAutoAttackTargets.push_back(BUILDER_UNIT);
        validAutoAttackTargets.push_back(RESOURCE);
    }

    if (!m_exploringData->isSafetryPosition(entity.position.x, entity.position.y))
    {
        if (!succes)
        {
            x = 0;
            y = 0;
        }
        moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(x, y), true, true));
        act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr, nullptr);
        return;

    }

//    for (int i : m_exploringData->attackedEnemyUnits)
//    {
//        if (getDistance(entity, m_playerView->entities[i]) < 8 && m_playerView->entities[i].entityType != EntityType::BUILDER_UNIT)
//        {
//        }
//    }

    moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(x, y), true, true));

    act.entityActions[entity.id] = EntityAction( moveAction, nullptr,
        std::shared_ptr<AttackAction>(new AttackAction( nullptr,
        std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))), nullptr);
}

void IEconomicsMinistry::fillRepairMap()
{
    m_repairMap.clear();

    for (int ind : m_exploringData->needRepairBuildings)
    {
        double dmax = 1000;
        int k = -1;
        const Entity& building = m_playerView->entities[ind];

        for (size_t i = 0; i < m_units.size(); i++) {
            if (m_repairMap.find(i) != m_repairMap.end())
            {
                continue;
            }

            const Entity& entity = m_units[i];
            double distance = getDistance(entity, building);
            if (distance < dmax)
            {
                dmax = distance;
                k = i;
            }
        }
        m_repairMap[k] = ind;

        dmax = 1000;
        k = -1;

        for (size_t i = 0; i < m_units.size(); i++) {
            if (m_repairMap.find(i) != m_repairMap.end())
            {
                continue;
            }

            const Entity& entity = m_units[i];
            double distance = getDistance(entity, building);
            if (distance < dmax)
            {
                dmax = distance;
                k = i;
            }
        }
        m_repairMap[k] = ind;
    }
}

void IEconomicsMinistry::getCreateUnitCoordinates(int &x, int &y)
{
    const Entity& entity = m_playerView->entities[m_exploringData->builderBaseIndex];
    const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);

    int px, py;
    m_exploringData->getNearestResources(entity, px, py);

    const int N = 20;
    int a[N] = {  0,  1,  2,  3,  4, -1, -1, -1, -1, -1,  5,  5,  5,  5,  5, 0,  1,  2,  3,  4};
    int b[N] = { -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  0,  1,  2,  3,  4, 5,  5,  5,  5,  5};

    int k = 0;
    double m = 1000;

    for (int i = 0; i < N; ++i)
    {
        Entity t;
        t.position.x = px;
        t.position.y = py;
        t.entityType = EntityType::RESOURCE;
        Entity be;
        be.position.x = entity.position.x + a[i];
        be.position.y = entity.position.y + b[i];
        be.entityType = EntityType::RESOURCE;
        if (getDistance(t, be) < m)
        {
            m = getDistance(t, be);
            k = i;
        }
    }
    x = entity.position.x + a[k];
    y = entity.position.y + b[k];

//    x = entity.position.x + properties.size;
    //    y = entity.position.y + properties.size - 1;
}

void IEconomicsMinistry::fillBuildHouseMap()
{
//    m_buildHouseMap.clear();
//    m_buildTypeMap.clear();
    double maxDistance = 1000;
    int num = -1;
    Vec2Int p1, p2;
    std::vector<Vec2Int> freeHousePoints = m_exploringData->getFreeHouseCoordinates();
    for(Vec2Int v : freeHousePoints)
    {
        std::vector<Vec2Int> freeNearPoints = m_exploringData->getFreeCoordinateForHouseBuild(v);
        for (Vec2Int v2 : freeNearPoints)
        {
            for (int i = 0; i < m_units.size(); ++i)
            {
                const Entity & entity = m_units[i];
                if (getDistance(entity, v2.x, v2.y) < maxDistance)
                {
                    maxDistance = getDistance(entity, v2.x, v2.y);
                    num = i;
                    p1 = v;
                    p2 = v2;
                }
            }
        }
    }
    if (num >= 0)
    {
        m_buildHouseMap[num] = std::pair<Vec2Int, Vec2Int>(p1, p2);
        m_buildTypeMap[num] = EntityType::HOUSE;
    }
}

void IEconomicsMinistry::fillBuildRangeBaseaMap()
{
//    m_buildHouseMap.clear();
//    m_buildTypeMap.clear();
    double maxDistance = 1000;
    int num = -1;
    Vec2Int p1, p2;
    std::vector<Vec2Int> freeHousePoints = m_exploringData->getFreeRangeBaseCoordinates();
    for(Vec2Int v : freeHousePoints)
    {
        std::vector<Vec2Int> freeNearPoints = m_exploringData->getFreeCoordinateForBuilding(v, m_exploringData->rangedBaseSize);
        for (Vec2Int v2 : freeNearPoints)
        {
            for (int i = 0; i < m_units.size(); ++i)
            {
                const Entity & entity = m_units[i];
                if (getDistance(entity, v2.x, v2.y) < maxDistance)
                {
                    maxDistance = getDistance(entity, v2.x, v2.y);
                    num = i;
                    p1 = v;
                    p2 = v2;
                }
            }
        }
    }
    if (num >= 0)
    {
        m_buildHouseMap[num] = std::pair<Vec2Int, Vec2Int>(p1, p2);
        m_buildTypeMap[num] = EntityType::RANGED_BASE;
    }

}

Vec2Int IWarMinistry::getNearestEnemyBuilderUnitCoords(const Entity &entity)
{
    Vec2Int res;
    double maxDistance = 1000;
    for (int i : m_exploringData->enemyBuilderUnits)
    {
        if (getDistance(m_playerView->entities[i], entity) < maxDistance)
        {
            maxDistance = getDistance(m_playerView->entities[i], entity);
            res = m_playerView->entities[i].position;
        }
    }
    return res;

}
