#include "StartGameMinisters3.h"




void StartGameEconomicMinister3::activate()
{
    m_buildHouseMap.clear();
    m_buildTypeMap.clear();
}

void StartGameEconomicMinister3::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    fillRepairMap();
    createBuilderUnit(act);



    static int tick = 0;
    tick++;
    if (tick % 3 == 0)
    {
        m_buildHouseMap.clear();
        m_buildTypeMap.clear();
    }
    if (m_exploringData->rangedBaseCount < 1 &&
            m_resourcesCount + m_exploringData->builderUnitsCount >= m_exploringData->entityCost[EntityType::RANGED_BASE])
    {
        if (m_buildHouseMap.size() < 3)
        {
            fillBuildRangeBaseaMap();
        }
    }
    if (m_resourcesCount >= 50 && (m_exploringData->freePopulation < 15))
    {
        if (m_buildHouseMap.size() < 2)
        {
            fillBuildHouseMap();
        } else if (m_buildHouseMap.size() < 3 &&
                   m_resourcesCount + m_exploringData->builderUnitsCount >= 200)
        {
            fillBuildHouseMap();
        }
    }
    if (m_resourcesCount >= 50 && (m_exploringData->freePopulation < 15))
    {
        if (m_buildHouseMap.size() < 2)
        {
            fillBuildTurrets();
        } else if (m_buildHouseMap.size() < 3 && m_resourcesCount + m_exploringData->builderUnitsCount >= 200)
        {
            fillBuildTurrets();
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
                m_resourcesCount + m_exploringData->builderUnitsCount > m_exploringData->entityCost[m_buildTypeMap[i]])
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

        farmResources(act, entity, i);
    }

}

void StartGameWarMinister3::addMinistryAction(Action &act)
{
    m_spyCounter = 0;
    int myId = m_playerView->myId;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];

        if (entity.entityType == BUILDER_UNIT)
        {
            addSpyAction(act, entity);
            m_spyCounter++;
            continue;
        }

        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;

        int x = m_playerView->mapSize - 5, y = m_playerView->mapSize - 5;

        moveAction = std::shared_ptr<MoveAction>(new MoveAction( Vec2Int(x, y), true, true));

        std::vector<EntityType> validAutoAttackTargets;
        act.entityActions[entity.id] = EntityAction(
                    moveAction,
                    buildAction,
                    std::shared_ptr<AttackAction>(new AttackAction(
                                                      nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
                    nullptr);
    }

    createEntitiesByBuildings(act);

}

std::vector<Vec2Int> StartGameWarMinister3::getSpyPositions()
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(31, 31));
    init.push_back(Vec2Int(42, 15));
    init.push_back(Vec2Int(15, 42));
    return init;
}

void StartGameWarMinister3::addSpyAction(Action &act, const Entity &entity)
{
    std::vector<Vec2Int> positions = getSpyPositions();
    int counter = 0;
    for (auto p : positions)
    {
//        if (m_exploringData->lastUpdatedMap.find(m_exploringData->getIndex(p.x, p.y))
//                != m_exploringData->lastUpdatedMap.end())
//        {
//            continue;
//        }
        if (counter <  m_spyCounter)
        {
            counter++;
            continue;
        }
        std::vector<Vec2Int> v = m_exploringData->getRouteAStar(entity, p);

        if (v.size() > 0)
        {
            std::shared_ptr<MoveAction> moveAction = nullptr;
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(v[0].x, v[0].y), true, true));
            act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr, nullptr);
            return;
        }
        std::shared_ptr<MoveAction> moveAction = nullptr;
        moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(p.x, p.y), true, true));
        act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr, nullptr);
        return;
    }
    farmResources(act, entity, m_spyCounter);
}

void StartGameDefenceMinister3::addMinistryAction(Action &act)
{
    for (size_t i = 0; i < m_buildings.size(); i++) {
        act.entityActions[m_buildings[i].id] = EntityAction( nullptr, nullptr, nullptr, nullptr);
    }
    for (int i = 0; i < m_buildings.size(); ++i)
    {
        if (m_buildings[i].entityType == TURRET)
        {
            turretAttack(act, m_buildings[i].id);
        }
    }

}

void StartGameDistributor3::innerDistribute(const PlayerView &playerView, const ExploringData &data)
{
    int counter = 0;
    int myId = playerView.myId;
    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }

        switch (entity.entityType) {
        case EntityType::BUILDER_BASE :
            m_economicMinister->addEntity(entity);
            break;
        case EntityType::BUILDER_UNIT :
            if (needSpy(data) && (counter >5 || entity.id % 3 == 0))
            {
                m_warMinister->addEntity(entity);
            } else
            {
                m_economicMinister->addEntity(entity);
            }
            break;
        case EntityType::RANGED_BASE :
        case EntityType::RANGED_UNIT :
        case EntityType::MELEE_UNIT :
            m_warMinister->addEntity(entity);
            break;
        case EntityType::MELEE_BASE :
        case EntityType::TURRET :
            m_defenceMinister->addEntity(entity);
        default:
            break;
        }
        counter++;
    }

    if (data.isBaseAttacked)
    {
        m_economicMinister->setMaxPopulation(0);
        m_warMinister->setMaxPopulation(data.freePopulation);
    } else if (data.builderUnitsCount < 12)
    {
        m_economicMinister->setMaxPopulation(data.maxPopulation - data.builderUnitsCount);
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

bool StartGameDistributor3::needSpy(ExploringData const &data)
{
    std::vector<Vec2Int> positions = StartGameWarMinister3::getSpyPositions();
    bool f = false;
    for (auto p : positions)
    {
        f = f || (data.lastUpdatedMap.find(data.getIndex(p.x, p.y))
                == data.lastUpdatedMap.end());
    }
    return f;
}
