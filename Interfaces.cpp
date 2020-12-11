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
    static const int n = 15;
    static int a[n] = {0, 4, 8, 12, 16, 20, 0, 0,  0,  0,  0, 11, 11,  4,  8};
    static int b[n] = {0, 0, 0,  0,  0,  0, 4, 8, 12, 16, 20,  4,  8, 11, 11};

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
    return y * mapSize + x;
}
