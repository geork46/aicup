#include "StartGameMinisters3.h"

#include "DrawerHolder.h"




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
    if (m_exploringData->rangedBaseCount > 0 && m_resourcesCount >= 70)
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

void StartGameEconomicMinister3::farmResources(Action &act, const Entity &entity, int i)
{
    const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
    std::shared_ptr<MoveAction> moveAction = nullptr;
    int x = m_playerView->mapSize - 10;
    int y = m_playerView->mapSize - 10;

    if (i % 3 == 0)
    {
        x = 10;
        y = m_playerView->mapSize - 10;
    } else if (i % 3 == 1)
    {
         x = m_playerView->mapSize - 10;
         y = 10;
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

std::vector<Vec2Int> StartGameEconomicMinister3::getTurretsCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(9, 25));
    init.push_back(Vec2Int(25, 9));
    return init;

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

        int x = m_playerView->mapSize - 5, y = m_playerView->mapSize - 6;
        moveAction = std::shared_ptr<MoveAction>(new MoveAction( Vec2Int(x, y), true, true));

        if (i < 10)
        {
            DrawerHolder::instance()->getDrawer()->selectLayer(6);


            std::vector<Vec2Int> v;
            if (m_exploringData->enemyBuilderUnits.size() > 0)
            {
                m_exploringData->getNearestEnemyBuilder(entity, x, y);
                v = m_exploringData->getRouteAStarAttackBuilder(entity, Vec2Int(x, y));
            }

            if (v.size() == 0)
            {
                v = m_exploringData->getRouteAStar(entity, Vec2Int(x, y));
            }

            if (v.size() > 0)
            {
                moveAction = std::shared_ptr<MoveAction>(new MoveAction( v[0], true, true));
            }
        }


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

void StartGameWarMinister3::addSpyAction(Action &act, const Entity &entity)
{
    std::vector<Vec2Int> positions = ExploringData::getSpyPositions();
    int counter = 0;
    for (auto p : positions)
    {
        if (m_exploringData->lastUpdatedMap.find(m_exploringData->getIndex(p.x, p.y))
                != m_exploringData->lastUpdatedMap.end())
        {
            continue;
        }
        if (counter <  m_spyCounter)
        {
            counter++;
            continue;
        }
        DrawerHolder::instance()->getDrawer()->selectLayer(9);
        std::vector<Vec2Int> v = m_exploringData->getRouteAStar(entity, p);

        if (v.size() > 0)
        {
            if (m_exploringData->map.find(m_exploringData->getIndex(v[0].x, v[0].y)) != m_exploringData->map.end())
            {
                std::shared_ptr<MoveAction> moveAction;
                moveAction.reset(new MoveAction(Vec2Int(v[0].x, v[0].y), true, true));
                std::shared_ptr<int> attackId;
                attackId.reset(new int(m_playerView->entities[m_exploringData->map.at(m_exploringData->getIndex(v[0].x, v[0].y)) ].id));

                std::vector<EntityType> validAutoAttackTargets;
                if (entity.entityType == BUILDER_UNIT) {
                    validAutoAttackTargets.push_back(RESOURCE);
                }
                act.entityActions[entity.id] = EntityAction( moveAction, nullptr,
                                                             std::shared_ptr<AttackAction>(new AttackAction(attackId, nullptr)), nullptr);


            }

            std::shared_ptr<MoveAction> moveAction = nullptr;
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(v[0].x, v[0].y), true, true));
            act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr, nullptr);
            return;
        }
        std::shared_ptr<MoveAction> moveAction = nullptr;
        moveAction = std::shared_ptr<MoveAction>(new MoveAction(Vec2Int(p.x, p.y), true, true));
        act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr, nullptr);

        std::vector<EntityType> validAutoAttackTargets;
        if (entity.entityType == BUILDER_UNIT) {
            validAutoAttackTargets.push_back(BUILDER_UNIT);
            validAutoAttackTargets.push_back(RESOURCE);
        }

        act.entityActions[entity.id] = EntityAction( moveAction, nullptr,
                                                     std::shared_ptr<AttackAction>(new AttackAction( nullptr,
                                                                                                     std::shared_ptr<AutoAttack>(new AutoAttack(1, validAutoAttackTargets)))), nullptr);
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
            if (needSpy(data) && ((counter == 6) || (data.maxPopulation < 20 && data.myResourcesCount > 70  && counter == 1)))
            {
                m_warMinister->addEntity(entity);
            } else
            {
                m_economicMinister->addEntity(entity);
            }
            counter++;
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
    }

    if (data.builderUnitsCount < 12 || (data.builderUnitsCount < 30 && (data.rangedBaseCount + data.meleeBaseCount < 1)))
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

