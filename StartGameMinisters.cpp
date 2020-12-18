#include "StartGameMinisters.h"

void StartGameEconomicMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;


    fillRepairMap();
    createBuilderUnit(act);

    m_buildHouseMap.clear();
//    if (m_exploringData->rangedBaseCount < 1 &&
//            m_resourcesCount + m_exploringData->builderUnitsCount >= m_exploringData->entityCost[EntityType::RANGED_BASE])
//    {
//        if (m_buildHouseMap.size() == 0)
//        {
//            fillBuildRangeBaseaMap();
//        }
//    } else
    if (m_resourcesCount + m_exploringData->builderUnitsCount >= 50 && (m_exploringData->freePopulation < 10))
    {
        if (m_buildHouseMap.size() == 0)
        {
            fillBuildHouseMap();
        }
    }
    int x, y;
    bool f = m_exploringData->getFreeHouseCoordinate(x, y);

    std::shared_ptr<BuildAction> buildAction = nullptr;
    std::shared_ptr<MoveAction> moveAction = nullptr;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        buildAction = nullptr;

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
                    m_buildHouseMap.clear();
                    m_buildTypeMap.clear();
            }
            continue;
        }

        if (tryRepair(act, entity))
        {
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
//        if (i > m_exploringData->builderUnitsCount - 3 && f && m_exploringData->myResourcesCount > 50
//                && (m_exploringData->freePopulation < 10))
//        {
//            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
//                                                         Vec2Int(x + m_exploringData->houseSize, y + m_exploringData->houseSize - 1),
//                                                         true, true));
//            buildAction = std::shared_ptr<BuildAction>(new BuildAction( EntityType::HOUSE, Vec2Int(x, y)));
//            act.entityActions[entity.id] = EntityAction( moveAction, buildAction, nullptr, nullptr);
//            continue;
//        }

        farmResources(act, entity, i);
    }
}

void StartGameWarMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;

        int x = m_playerView->mapSize - 1, y = m_playerView->mapSize - 1;

//        if (m_exploringData->rangedUnitsCount > 8 && i < m_units.size() - m_units.size() / 3 + 1)
//        {

        int maxD = 500;
        for (auto i : m_exploringData->enemies)
        {
            if (i.second.dangerousLevel > 0)
            {
                if (i.second.meleeUnitsCount + i.second.rangedUnitsCount < maxD)
                {
                    x = i.second.mainX;
                    y = i.second.mainY;
                    maxD = i.second.meleeUnitsCount + i.second.rangedUnitsCount;
                }
                break;
            }
        }
//        } else
//        {
//            x = 15;
//            y = 15;
//        }
        if (maxD > m_exploringData->meleeUnitsCount + m_exploringData->rangedUnitsCount + 1)
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
                    buildAction,
                    std::shared_ptr<AttackAction>(new AttackAction(
                                                      nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
                    nullptr);
    }

    for (size_t i = 0; i < m_buildings.size(); i++) {
        const Entity& entity = m_buildings[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        std::vector<EntityType> validAutoAttackTargets;

        if (properties.build != nullptr) {
            EntityType entityType = properties.build->options[0];
            if (m_exploringData->entityProperties[entityType].populationUse <= m_maxPopulation
                    && m_exploringData->entityCost[entityType] <= m_resourcesCount)
            {
                buildAction = std::shared_ptr<BuildAction>(new BuildAction(
                                                               entityType,
                                                               Vec2Int(entity.position.x + properties.size, entity.position.y + properties.size - 1)));
            }
        }
        act.entityActions[entity.id] = EntityAction(
                    moveAction,
                    buildAction,
                    nullptr,
                    nullptr);
    }

}

void StartGameDefenceMinister::addMinistryAction(Action &act)
{
    if (m_exploringData->meleeBaseCount > 0)
    {
        act.entityActions[m_exploringData->meleeBaseID] = EntityAction( nullptr, nullptr, nullptr, nullptr);
    }
}


void StartGameDistributor::innerDistribute(const PlayerView &playerView, const ExploringData &data)
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
            m_warMinister->addEntity(entity);
            break;
        case EntityType::MELEE_BASE :
            m_defenceMinister->addEntity(entity);
        default:
            break;
        }
    }

    if (data.isBaseAttacked)
    {
        m_economicMinister->setMaxPopulation(0);
        m_warMinister->setMaxPopulation(data.freePopulation);
    } else if (data.builderUnitsCount < 12)
    {
        m_economicMinister->setMaxPopulation(data.maxPopulation * 0.8 - data.builderUnitsCount);
        m_warMinister->setMaxPopulation(data.maxPopulation * 0.2 - data.meleeUnitsCount - data.rangedUnitsCount);
    } else
    {
        if (data.myResourcesCount < 200)
        {
            m_economicMinister->setMaxPopulation(data.maxPopulation * 0.6 - data.builderUnitsCount);
            m_warMinister->setMaxPopulation(data.maxPopulation * 0.4 - data.meleeUnitsCount - data.rangedUnitsCount);
        } else
        {
            m_economicMinister->setMaxPopulation(data.maxPopulation * 0.4 - data.builderUnitsCount);
            m_warMinister->setMaxPopulation(data.maxPopulation * 0.6 - data.meleeUnitsCount - data.rangedUnitsCount);
        }

    }

    m_economicMinister->setResourcesCount(data.myResourcesCount);
    m_warMinister->setResourcesCount(data.myResourcesCount);

    m_defenceMinister->setMaxPopulation(0);
    m_defenceMinister->setResourcesCount(0);
}


void MoreWarDistributor::innerDistribute(const PlayerView &playerView, const ExploringData &data)
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
            m_warMinister->addEntity(entity);
            break;
        case EntityType::MELEE_BASE :
        default:
            break;
        }
    }

    m_economicMinister->setMaxPopulation(data.maxPopulation * 0.4 - data.builderUnitsCount);
    m_economicMinister->setResourcesCount(data.myResourcesCount);

    m_warMinister->setMaxPopulation(data.maxPopulation * 0.6 - data.meleeUnitsCount - data.rangedUnitsCount);
    m_warMinister->setResourcesCount(data.myResourcesCount);

    m_defenceMinister->setMaxPopulation(0);
    m_defenceMinister->setResourcesCount(0);

}
