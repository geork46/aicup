#include "AlarmingMinisters.h"


void AlarmingEconomicMinister::activate()
{
    m_buildHouseMap.clear();
    m_buildTypeMap.clear();
}

void AlarmingEconomicMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    fillRepairMap();
    createBuilderUnit(act);

    m_buildHouseMap.clear();
    if (m_resourcesCount >= 50 && (m_exploringData->freePopulation < 10))
    {
        m_buildHouseMap.clear();
        m_buildTypeMap.clear();
        fillBuildHouseMap();
    }
    int x, y;
    bool f = m_exploringData->getFreeHouseCoordinate(x, y);

    std::shared_ptr<BuildAction> buildAction = nullptr;
    std::shared_ptr<MoveAction> moveAction = nullptr;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        buildAction = nullptr;

//        if (!m_exploringData->isSafetryPosition(entity.position.x, entity.position.y))
//        {
//            farmResources(act, entity, i);
//            continue;
//        }

        if (tryRepair(act, entity))
        {
            continue;
        }

        if (m_buildHouseMap.find(i) != m_buildHouseMap.end() &&
                m_resourcesCount + m_exploringData->builderUnitsCount >= m_exploringData->entityCost[m_buildTypeMap[i]])
        {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         m_buildHouseMap[i].second,
                                                         true, true));
            buildAction = std::shared_ptr<BuildAction>(new BuildAction(m_buildTypeMap[i] , m_buildHouseMap[i].first));
            act.entityActions[entity.id] = EntityAction( moveAction, buildAction, nullptr, nullptr);
            m_resourcesCount -= m_exploringData->entityCost[m_buildTypeMap[i]];
            if (m_buildHouseMap[i].second == entity.position)
            {
                    m_buildHouseMap.erase(i);
            }
            continue;
        }

        if (m_repairMap.find(i) != m_repairMap.end())
        {

            const Entity& building = m_playerView->entities[m_repairMap[i]];
            const EntityProperties& prop = m_exploringData->entityProperties[building.entityType];
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
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        std::shared_ptr<MoveAction> moveAction = nullptr;

        int x = m_playerView->mapSize - 1, y = m_playerView->mapSize - 1;

        int maxD = 500;
        for (int i = 0; i < m_exploringData->MAX_ENEMIES; ++i)
        {
            if (m_exploringData->enemies[i].entityCount > 0)
            {
                if (m_exploringData->enemies[i].meleeUnitsCount + m_exploringData->enemies[i].rangedUnitsCount < maxD)
                {
                    x = m_exploringData->enemies[i].mainX;
                    y = m_exploringData->enemies[i].mainY;
                    maxD = m_exploringData->enemies[i].meleeUnitsCount + m_exploringData->enemies[i].rangedUnitsCount;
                }
            }
        }

        if (maxD > m_units.size())
        {
            x = 15;
            y = 15;
        }
        if (i < 3)
        {
            Vec2Int v = getNearestEnemyBuilderUnitCoords(entity);
            x = v.x;
            y = v.y;
        }

        moveAction = std::shared_ptr<MoveAction>(new MoveAction( Vec2Int(x, y), true, true));

        std::vector<EntityType> validAutoAttackTargets;
        act.entityActions[entity.id] = EntityAction(
                    moveAction,
                    nullptr,
                    std::shared_ptr<AttackAction>(new AttackAction(
                                                      nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
                    nullptr);
    }


}

void AlarmingDefenceMinister::addMinistryAction(Action &act)
{
    fillAttackMap();

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];

        int x = m_playerView->mapSize - 1, y = m_playerView->mapSize - 1;

        if (m_attackMap.find(i) != m_attackMap.end())
        {
            int k = m_attackMap[i];
            x = m_playerView->entities[k].position.x;
            y = m_playerView->entities[k].position.y;
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

    for (int i = 0; i < m_buildings.size(); ++i)
    {
        if (m_buildings[i].entityType == TURRET)
        {
            turretAttack(act, m_buildings[i].id);
        }
    }
}


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
            if (data.mapResourcesCount < 5)
            {
                m_warMinister->addEntity(entity);
            } else {
                m_economicMinister->addEntity(entity);
            }
            break;
        case EntityType::RANGED_UNIT :
        case EntityType::MELEE_UNIT :
            if (entityNearByAttackingEnemy(playerView, data, entity))
            {
                m_defenceMinister->addEntity(entity);
            } else {
                m_warMinister->addEntity(entity);
            }
            break;
        case EntityType::RANGED_BASE :
        case EntityType::MELEE_BASE :
        case EntityType::TURRET :
            m_defenceMinister->addEntity(entity);
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

    m_defenceMinister->setMaxPopulation(data.freePopulation);
    m_defenceMinister->setResourcesCount(data.myResourcesCount);

    m_warMinister->setMaxPopulation(0);
    m_warMinister->setResourcesCount(0);
}

bool AlarmingDistributor::entityNearByAttackingEnemy(const PlayerView &playerView, const ExploringData &data, const Entity &entity)
{
    for (int i : data.attackingEnemyUnits)
    {
        if (data.getDistance(entity, playerView.entities[i]) < 40)
        {
            return true;
        }
    }
    return false;
}

bool AlarmingDistributor::needMoreSolders(const PlayerView &playerView, const ExploringData &data)
{

}
