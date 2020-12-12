#include "AlarmingMinisters.h"


void AlarmingEconomicMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    fillRepairMap();
    createBuilderUnit(act);

    int x, y;
    bool f = m_exploringData->getFreeHouseCoordinate(x, y);

    std::shared_ptr<BuildAction> buildAction = nullptr;
    std::shared_ptr<MoveAction> moveAction = nullptr;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        buildAction = nullptr;

        if (tryRepair(act, entity))
        {
            continue;
        }

        if (m_repairMap.find(i) != m_repairMap.end())
        {

            const Entity& building = m_playerView->entities[m_repairMap[i]];
            const EntityProperties& prop = m_playerView->entityProperties.at(building.entityType);
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         Vec2Int( building.position.x + prop.size / 2 ,
                                                                  building.position.y + prop.size / 2),
                                                         true, true));
            act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr,std::shared_ptr<RepairAction>(new RepairAction(building.id)));
            continue;
        }
        farmResources(act, entity, i);
    }
}


void AlarmingWarMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);

        int x = m_playerView->mapSize - 1, y = m_playerView->mapSize - 1;

        if (m_exploringData->attackedEnemyUnits.size() > 0)
        {
            x = m_playerView->entities[m_exploringData->attackedEnemyUnits[0]].position.x;
            y = m_playerView->entities[m_exploringData->attackedEnemyUnits[0]].position.y;
        }

        std::shared_ptr<MoveAction> moveAction = std::shared_ptr<MoveAction>(new MoveAction( Vec2Int(x, y), true, true));

        std::vector<EntityType> validAutoAttackTargets;
        act.entityActions[entity.id] = EntityAction(
                    moveAction,
                    nullptr,
                    std::shared_ptr<AttackAction>(new AttackAction(
                                                      nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
                    nullptr);
    }

    createEntitiesByBuildings(act);



}

void AlarmingDefenceMinister::addMinistryAction(Action &act) {  }

void AlarmingDistributor::innerDistribute(const PlayerView &playerView, const ExploringData &data)
{
    int myId = playerView.myId;
    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }

        switch (entity.entityType) {
        case EntityType::BUILDER_BASE :
        case EntityType::BUILDER_UNIT :
            m_economicMinister->addEntity(entity);
            break;
        case EntityType::RANGED_BASE :
        case EntityType::RANGED_UNIT :
        case EntityType::MELEE_UNIT :
        case EntityType::MELEE_BASE :
            m_warMinister->addEntity(entity);
            break;
        default:
            break;
        }
    }

    if (data.meleeBaseCount > 0 || data.rangedBaseCount > 0)
    {
        m_economicMinister->setMaxPopulation(0);
        m_economicMinister->setResourcesCount(data.myResourcesCount / 2);
    } else
    {
        m_economicMinister->setMaxPopulation(data.freePopulation);
        m_economicMinister->setResourcesCount(data.myResourcesCount);
    }
    m_warMinister->setMaxPopulation(data.freePopulation);
    m_warMinister->setResourcesCount(data.myResourcesCount);

    m_defenceMinister->setMaxPopulation(0);
    m_defenceMinister->setResourcesCount(0);
}
