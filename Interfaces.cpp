#include "Interfaces.h"

#include <math.h>

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

int ExploringData::getIndex(int x, int y) const
{
    if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
    {
        return 7 * mapSize + 7;
    }
    return y * mapSize + x;
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

double ExploringData::getDistance(const Entity &unit, const Entity &building) const
{
    double x = unit.position.x;
    double y = unit.position.y;

    const EntityProperties& properties = playerView->entityProperties.at(building.entityType);
    double px = building.position.x + properties.size / 2;
    double py = building.position.y + properties.size / 2;

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
