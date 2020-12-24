#include "Interfaces.h"

#include <math.h>
#include <queue>
#include <algorithm>
#include "DrawerHolder.h"

int ExploringData::builderUnitPopulationUse;
int ExploringData::rangedUnitPopulationUse;
int ExploringData::meleeUnitPopulationUse;
int ExploringData::entityCost[EntityType::TURRET + 1];
int ExploringData::builderUnitsCost;
int ExploringData::rangedUnitsCost;
int ExploringData::meleeUnitsCost;
int ExploringData::houseSize;
int ExploringData::turretSize;
int ExploringData::rangedBaseSize;
int ExploringData::mapSize;
int ExploringData::playersCount;

std::unordered_map<int, int> ExploringData::lastMap; // EntityType
std::unordered_map<int, int> ExploringData::lastUpdatedMap;

EnemyInfo ExploringData::enemies[ExploringData::MAX_ENEMIES];

EntityProperties ExploringData::entityProperties[EntityType::TURRET + 1];

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

void IMinistry::createEntitiesByBuildings(Action &act)
{
    for (size_t i = 0; i < m_buildings.size(); i++) {
        const Entity& entity = m_buildings[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        std::shared_ptr<BuildAction> buildAction = nullptr;

        if (properties.build == nullptr) {
            continue;
        }
        EntityType entityType = properties.build->options[0];
        if (m_exploringData->entityProperties[entityType].populationUse <= m_maxPopulation
                && m_exploringData->entityCost[entityType] <= m_resourcesCount + m_exploringData->builderUnitsCount)
        {

            std::vector<Vec2Int> v = m_exploringData->getFreeCoordinateForBuilding(entity.position, properties.size);
            if (v.size() > 0)
            {
                buildAction = std::shared_ptr<BuildAction>(new BuildAction( entityType, Vec2Int(v[0].x, v[0].y)));
                m_resourcesCount -= m_exploringData->entityCost[entityType];
            }
        }
        act.entityActions[entity.id] = EntityAction(nullptr, buildAction, nullptr, nullptr);
    }
}

void IMinistry::turretAttack(Action &act)
{
    if (m_exploringData->turretCount > 0)
    {
        turretAttack(act, m_exploringData->turretID);
    }
}

void IMinistry::turretAttack(Action &act, int turretId)
{
    std::shared_ptr<AutoAttack> autoAttack;
    std::shared_ptr<AttackAction> attackAction;
    std::vector<EntityType> validAutoAttackTargets;

    const EntityProperties& properties = m_exploringData->entityProperties[TURRET];
    autoAttack.reset(new AutoAttack(properties.sightRange, validAutoAttackTargets));
    attackAction.reset(new AttackAction( nullptr, autoAttack));
    act.entityActions[turretId] = EntityAction( nullptr, nullptr, attackAction, nullptr);

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

void IExploringMinistry::getExploringData(const PlayerView &playerView, ExploringData & data)
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

std::vector<Vec2Int> ExploringData::getFreeBuildingsCoordinates(const std::vector<Vec2Int> &init, int size) const
{
    std::vector<Vec2Int> result{};

    for (int k = 0; k < init.size(); ++k)
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                if (map.find(getIndex(init[k].x + i, init[k].y + j)) != map.end() ||
                lastUpdatedMap.find(getIndex(init[k].x + i, init[k].y + j)) == lastUpdatedMap.end())
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
    for (int i = size - 1; i >= 0; --i)
    {
        add.push_back(Vec2Int(size, i));
        add.push_back(Vec2Int(i, size));
    }
    for (int i = size - 1; i >= 0; --i)
    {
        add.push_back(Vec2Int(-1, i));
        add.push_back(Vec2Int(i, -1));
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

void ExploringData::getXYfromIndex(int index, int &x, int &y) const
{
    y = index / mapSize;
    x = index % mapSize;
}

//int ExploringData::getIndex(int x, int y) const

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
    const EntityProperties& properties = entityProperties[entity.entityType];

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

    for (int i = 0; i < 1000 && !f && queue.size() > 0; ++i)
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
        if (current == current2)
        {
            f = false;
        }
        x = current2.x;
        y = current2.y;
    } else
    {
        getNearestResources(entity, x, y);
    }
    return f;
}


std::vector<Vec2Int> ExploringData::getRouteAStar(const Entity &entity, Vec2Int dest) const
{
    const EntityProperties& properties = entityProperties[entity.entityType];
    int resourcePenalty = 5;
    if (properties.attack != nullptr)
    {
        resourcePenalty = entityProperties[RESOURCE].maxHealth / properties.attack->damage + 1;
    }

    std::unordered_map<Vec2Int, Vec2Int> traking;

    std::unordered_map<Vec2Int, int> currentDistance;

    std::vector<Vec2Int> add{};
    add.push_back(Vec2Int(0, 1));
    add.push_back(Vec2Int(0, -1));
    add.push_back(Vec2Int(1, 0));
    add.push_back(Vec2Int(-1, 0));

    std::vector<Vec2Int> queue;
    auto comp =  [&](Vec2Int const &a, Vec2Int const &b) {
        int distA = currentDistance[a] + getDistance(a.x, a.y, dest.x, dest.y);
        int distB = currentDistance[b] + getDistance(b.x, b.y, dest.x, dest.y);
        return distA > distB;
    };
    std::make_heap(queue.begin(), queue.end(), comp );

    std::vector<Vec2Int> log;
    queue.push_back(entity.position);
    currentDistance[entity.position] = 0;
    std::push_heap(queue.begin(), queue.end(), comp );

    Vec2Int finish;
    bool f = false;

    for (int i = 0; i < 3000 && !f && queue.size() > 0; ++i)
    {
        std::pop_heap(queue.begin(), queue.end(), comp );
        Vec2Int current = queue.back();
        queue.pop_back();
        DrawerHolder::instance()->getDrawer()->fillColorCell(current.x, current.y, 0x23E9E9);

        log.push_back(current);
        for (int j = 0; j < add.size(); ++j)
        {
            if (traking.find(Vec2Int(current.x + add[j].x, current.y + add[j].y)) != traking.end())
            {
                continue;
            }
            traking[Vec2Int(current.x + add[j].x, current.y + add[j].y)] = current;

            if (current == dest)
            {
                finish = dest;
                f = true;
                break;
            }

            if (isSafetryPosition(current.x + add[j].x, current.y + add[j].y) &&
                    (lastMap.find(getIndex(current.x + add[j].x, current.y + add[j].y)) == lastMap.end() ||
                     lastMap.at(getIndex(current.x + add[j].x, current.y + add[j].y)) == -1))
            {
                queue.push_back(Vec2Int(current.x + add[j].x, current.y + add[j].y));
                currentDistance[Vec2Int(current.x + add[j].x, current.y + add[j].y)] = currentDistance[current] + 1;
                std::push_heap(queue.begin(), queue.end(), comp );
                continue;
            }
            if (isSafetryPosition(current.x + add[j].x, current.y + add[j].y) &&
                    (lastMap.find(getIndex(current.x + add[j].x, current.y + add[j].y)) != lastMap.end() &&
                     lastMap.at(getIndex(current.x + add[j].x, current.y + add[j].y)) == RESOURCE))
            {
                if (map.find(getIndex(current.x + add[j].x, current.y + add[j].y)) != map.end())
                {
                    int health = playerView->entities[map.at(getIndex(current.x + add[j].x, current.y + add[j].y))].health;
                    resourcePenalty = health / properties.attack->damage + 1;
                }
                queue.push_back(Vec2Int(current.x + add[j].x, current.y + add[j].y));
                currentDistance[Vec2Int(current.x + add[j].x, current.y + add[j].y)] = currentDistance[current] + resourcePenalty;
                std::push_heap(queue.begin(), queue.end(), comp );
                continue;
            }
        }
    }
    std::vector<Vec2Int> res;
    if (f)
    {
        Vec2Int current = finish;
        Vec2Int current2 = finish;
        while (!(current == entity.position))
        {
            res.push_back(current);
            DrawerHolder::instance()->getDrawer()->fillColorCell(current.x, current.y, 0x5AFF04);
            current2 = current;
            current = traking[Vec2Int(current.x, current.y)];
        }
        if (current == current2)
        {
            f = false;
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}



double ExploringData::getDistance(const Entity &unit, int x, int y) const
{
    double px = unit.position.x;
    double py = unit.position.y;
    return abs(px - x) + abs(py - y);
}

double ExploringData::getDistance(int x, int y, int px, int py) const
{
    return abs(px - x) + abs(py - y);
}


double ExploringData::getDistanceSqr(const Entity &unit, const Entity &building) const
{
    double x = unit.position.x + 0.5;
    double y = unit.position.y + 0.5;

    const EntityProperties& properties = entityProperties[building.entityType];
    double px = building.position.x + properties.size / 2.0;
    double py = building.position.y + properties.size / 2.0;

    return (px - x)*(px - x) + (py - y) * (py - y);
}

double ExploringData::getDistance(const Entity &unit, const Entity &building) const
{
    return sqrt(getDistanceSqr(unit, building));
}

void IEconomicsMinistry::createBuilderUnit(Action &act)
{
    if (m_exploringData->builderBaseCount > 0)
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
                const EntityProperties& prop = m_exploringData->entityProperties[en.entityType];
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

void IMinistry::farmResources(Action &act, const Entity &entity, int i)
{
    const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
    std::shared_ptr<MoveAction> moveAction = nullptr;
    int x = m_playerView->mapSize - 10;
    int y = m_playerView->mapSize - 10;

    if (i % 2 != 0)
    {
        x = m_playerView->mapSize / 4 + 1;
        y = m_playerView->mapSize - 10;
    } else if (i % 4 == 0)
    {
         x = m_playerView->mapSize - 10;
         y = m_playerView->mapSize / 4 + 1;
    }

    std::vector<EntityType> validAutoAttackTargets;
    if (entity.entityType == BUILDER_UNIT) {
        validAutoAttackTargets.push_back(BUILDER_UNIT);
        validAutoAttackTargets.push_back(RESOURCE);
    }

    if (!m_exploringData->isSafetryPosition(entity.position.x, entity.position.y))
    {
        x = 0;
        y = 0;
        moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(x, y), true, true));
        act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr, nullptr);
        return;

    }

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
            double distance = m_exploringData->getDistance(entity, building);
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
            double distance = m_exploringData->getDistance(entity, building);
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
    const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];

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
        if (m_exploringData->getDistance(t, be) < m)
        {
            m = m_exploringData->getDistance(t, be);
            k = i;
        }
    }
    x = entity.position.x + a[k];
    y = entity.position.y + b[k];

//    x = entity.position.x + properties.size;
    //    y = entity.position.y + properties.size - 1;
}

std::vector<Vec2Int> IEconomicsMinistry::getTurretsCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(3, 25));
    init.push_back(Vec2Int(6, 25));
    init.push_back(Vec2Int(9, 25));
    init.push_back(Vec2Int(25, 3));
    init.push_back(Vec2Int(25, 6));
    init.push_back(Vec2Int(25, 9));
    return init;
}

std::vector<Vec2Int> IEconomicsMinistry::getHousesCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(0, 0));
    init.push_back(Vec2Int(0, 3));
    init.push_back(Vec2Int(0, 6));
    init.push_back(Vec2Int(0, 18));
//    init.push_back(Vec2Int(0, 21));
    init.push_back(Vec2Int(4, 0));
    init.push_back(Vec2Int(7, 0));
    init.push_back(Vec2Int(16, 0));
    init.push_back(Vec2Int(19, 0));
    init.push_back(Vec2Int(22, 0));


    if (m_exploringData->rangedBaseCount + m_exploringData->meleeBaseCount > 1)
    {
        init.push_back(Vec2Int(0, 9));
        init.push_back(Vec2Int(0, 12));
        init.push_back(Vec2Int(0, 15));
        init.push_back(Vec2Int(10, 0));
        init.push_back(Vec2Int(13, 0));
        init.push_back(Vec2Int(11, 4));
        init.push_back(Vec2Int(11, 8));
        init.push_back(Vec2Int(4, 11));
        init.push_back(Vec2Int(7, 11));
    }

//    init.push_back(Vec2Int(22, 3));
    init.push_back(Vec2Int(22, 6));
    init.push_back(Vec2Int(22, 9));
    init.push_back(Vec2Int(23, 12));
    init.push_back(Vec2Int(3, 22));
    init.push_back(Vec2Int(6, 22));
    init.push_back(Vec2Int(9, 22));
    init.push_back(Vec2Int(12, 23));

    init.push_back(Vec2Int(18, 4));
    init.push_back(Vec2Int(18, 8));
    init.push_back(Vec2Int(18, 12));

    init.push_back(Vec2Int(4, 18));
    init.push_back(Vec2Int(8, 18));
    init.push_back(Vec2Int(12, 18));

    return init;
}

std::vector<Vec2Int> IEconomicsMinistry::getRangedBaseCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(11, 11));
    init.push_back(Vec2Int(11, 6));
    init.push_back(Vec2Int(11, 7));
    init.push_back(Vec2Int(11, 8));
    init.push_back(Vec2Int(11, 9));
    init.push_back(Vec2Int(11, 10));
    init.push_back(Vec2Int(6, 11));
    init.push_back(Vec2Int(7, 11));
    init.push_back(Vec2Int(8, 11));
    init.push_back(Vec2Int(9, 11));
    init.push_back(Vec2Int(10, 11));
    init.push_back(Vec2Int(12, 12));

    init.push_back(Vec2Int(0, 11));
    init.push_back(Vec2Int(1, 11));
    init.push_back(Vec2Int(2, 11));
    init.push_back(Vec2Int(3, 11));
    init.push_back(Vec2Int(4, 11));
    init.push_back(Vec2Int(5, 11));
    init.push_back(Vec2Int(11, 0));
    init.push_back(Vec2Int(11, 1));
    init.push_back(Vec2Int(11, 2));
    init.push_back(Vec2Int(11, 3));
    init.push_back(Vec2Int(11, 4));
    init.push_back(Vec2Int(11, 5));

    return init;
}

void IEconomicsMinistry::fillBuildHouseMap()
{
    fillBuildingMap(HOUSE);
}

void IEconomicsMinistry::fillBuildRangeBaseaMap()
{
    fillBuildingMap(RANGED_BASE);
}

void IEconomicsMinistry::fillBuildingMap(EntityType type)
{
    double maxDistance = 1000000;
    int num = -1;
    Vec2Int p1, p2;
    std::vector<Vec2Int> freeHousePoints;
    int size = 3;

    switch (type) {
    case TURRET:
        size = m_exploringData->turretSize;
        freeHousePoints = m_exploringData->getFreeBuildingsCoordinates(
                    getTurretsCoordinates(), size);
        break;
    case HOUSE:
        size = m_exploringData->houseSize;
        freeHousePoints = m_exploringData->getFreeBuildingsCoordinates(
                    getHousesCoordinates(), size);
        break;
    case RANGED_BASE:
        size = m_exploringData->rangedBaseSize;
        freeHousePoints = m_exploringData->getFreeBuildingsCoordinates(
                    getRangedBaseCoordinates(), size);
        break;
    default:
        break;
    }
    for(Vec2Int v : freeHousePoints)
    {
        std::vector<Vec2Int> freeNearPoints = m_exploringData->getFreeCoordinateForBuilding(v, size);
        for (Vec2Int v2 : freeNearPoints)
        {
            for (int i = 0; i < m_units.size(); ++i)
            {
                if (m_buildHouseMap.find(i) != m_buildHouseMap.end())
                {
                    continue;
                }
                const Entity & entity = m_units[i];
                double distance = m_exploringData->getDistance(entity, v2.x, v2.y);
                if (distance < maxDistance)
                {
                    maxDistance = distance;
                    num = i;
                    p1 = v;
                    p2 = v2;
                    if (maxDistance < 2)
                    {
                        m_buildHouseMap[num] = std::pair<Vec2Int, Vec2Int>(p1, p2);
                        m_buildTypeMap[num] = type;
                        return;
                    }
                }
            }
        }
    }
    if (num >= 0)
    {
        m_buildHouseMap[num] = std::pair<Vec2Int, Vec2Int>(p1, p2);
        m_buildTypeMap[num] = type;
    }


}

void IEconomicsMinistry::fillBuildTurrets()
{
    fillBuildingMap(TURRET);
}

Vec2Int IWarMinistry::getNearestEnemyBuilderUnitCoords(const Entity &entity)
{
    Vec2Int res(0, 0);
    double maxDistance = 1000;
    for (int i : m_exploringData->enemyBuilderUnits)
    {
        if (m_exploringData->getDistance(m_playerView->entities[i], entity) < maxDistance)
        {
            maxDistance = m_exploringData->getDistance(m_playerView->entities[i], entity);
            res = m_playerView->entities[i].position;
        }
    }
    return res;

}

void IDefenceMinistry::fillAttackMap()
{
    m_attackMap.clear();
    m_attackMapCounter.clear();
    int counter = 0;
    for (int ii = 0; ii < 1 && counter < m_units.size(); ++ii)
    {
        for (int i : m_exploringData->attackingEnemyUnits)
        {
            double D = 1000;
            int k = -1;
            for (int j = 0; j < m_units.size(); ++j)
            {
                if (m_attackMap.find(j) == m_attackMap.end() &&
                        m_exploringData->getDistance(m_units[j], m_playerView->entities[i]) < D)
                {
                    D = m_exploringData->getDistance(m_units[j], m_playerView->entities[i]);
                    k = j;
                }
            }
            if (k >= 0)
            {
                counter++;
                m_attackMap[k] = i;
                m_attackMapCounter[i] = m_attackMapCounter[i] + 1;
            }
        }
    }

    for (int j = 0; j < m_units.size() && counter < m_units.size(); ++j)
    {
        if (m_attackMap.find(j) == m_attackMap.end())
        {
            double D = 1000;
            int k = -1;

            for (int i : m_exploringData->attackingEnemyUnits)
            {
                if (m_exploringData->getDistance(m_units[j], m_playerView->entities[i]) < D)
                {
                    D = m_exploringData->getDistance(m_units[j], m_playerView->entities[i]);
                    k = i;
                }
            }
            if (k >= 0)
            {
                counter++;
                m_attackMap[j] = k;
                m_attackMapCounter[k] = m_attackMapCounter[k] + 1;
            }
        }
    }


}

std::vector<Vec2Int> IWarMinistry::getDefencePositions() const
{

}

void IWarMinistry::fillPositionMap()
{

}
