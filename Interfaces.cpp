#include "Interfaces.h"

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
