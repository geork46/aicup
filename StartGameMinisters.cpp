#include "StartGameMinisters.h"

void StartGameEconomicMinister::addMinistryAction(Action &act)
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
                    if (prop.populationProvide > 0 && en.health < prop.maxHealth)
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
        } else if (m_repairMap.find(i) != m_repairMap.end())
        {

            const Entity& building = m_playerView->entities[m_repairMap[i]];
            const EntityProperties& prop = m_playerView->entityProperties.at(building.entityType);
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         Vec2Int( building.position.x + prop.size / 2 ,
                                                                  building.position.y + prop.size / 2),
                                                         true, true));
            act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr,std::shared_ptr<RepairAction>(new RepairAction(building.id)));

        } else
            if (i > m_exploringData->builderUnitsCount - 3 && f && m_exploringData->myResourcesCount > 50 && (m_exploringData->meleeUnitsCount + m_exploringData->rangedUnitsCount > 3)
                    && (m_exploringData->freePopulation < 10))
        {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         Vec2Int(x + m_exploringData->houseSize, y + m_exploringData->houseSize - 1),
                                                         true, true));
            buildAction = std::shared_ptr<BuildAction>(new BuildAction( EntityType::HOUSE, Vec2Int(x, y)));
            act.entityActions[entity.id] = EntityAction( moveAction, buildAction, nullptr, nullptr);
            continue;
        } else
        {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         Vec2Int(m_playerView->mapSize - 1, m_playerView->mapSize - 1),
                                                         true, true));

            std::vector<EntityType> validAutoAttackTargets;
            if (entity.entityType == BUILDER_UNIT) {
                validAutoAttackTargets.push_back(RESOURCE);
            }
            act.entityActions[entity.id] = EntityAction(
                        moveAction,
                        buildAction,
                        std::shared_ptr<AttackAction>(new AttackAction(
                                                          nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
                        nullptr);
        }

    }
}

void StartGameEconomicMinister::fillRepairMap()
{
}

void StartGameWarMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;

        int x = m_playerView->mapSize - 1, y = m_playerView->mapSize - 1;
        if (m_exploringData->enemies.find(m_exploringData->mainEnemy) != m_exploringData->enemies.end())
        {
            x = m_exploringData->enemies.at(m_exploringData->mainEnemy).builderBaseX;
            y = m_exploringData->enemies.at(m_exploringData->mainEnemy).builderBaseY;
            if (x < 0 || m_exploringData->enemies.at(m_exploringData->mainEnemy).dangerousLevel > 200)
            {
                x = m_exploringData->enemies.at(m_exploringData->mainEnemy).mainX;
                y = m_exploringData->enemies.at(m_exploringData->mainEnemy).mainY;
            }
        }
        if (properties.canMove) {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction( Vec2Int(x, y), true, true));
        }

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
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        std::vector<EntityType> validAutoAttackTargets;

        if (properties.build != nullptr) {
            EntityType entityType = properties.build->options[0];
            if (m_playerView->entityProperties.at(entityType).populationUse <= m_maxPopulation
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

void StartGameDefenceMinister::addMinistryAction(Action &act) { }


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
        default:
            break;
        }
    }

    m_economicMinister->setMaxPopulation(data.maxPopulation * 0.6 - data.builderUnitsCount);
    m_economicMinister->setResourcesCount(data.myResourcesCount);

    m_warMinister->setMaxPopulation(data.maxPopulation * 0.4 - data.meleeUnitsCount - data.rangedUnitsCount);
    m_warMinister->setResourcesCount(data.myResourcesCount);

    m_defenceMinister->setMaxPopulation(0);
    m_defenceMinister->setResourcesCount(0);
}
