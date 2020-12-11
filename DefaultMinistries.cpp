#include "DefaultMinistries.h"

#include <iostream>

void DefaultEconomicMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    std::shared_ptr<BuildAction> buildAction = nullptr;
    std::shared_ptr<MoveAction> moveAction = nullptr;

    if (m_playerView->entityProperties.at(EntityType::BUILDER_UNIT).populationUse <= m_maxPopulation
            && m_exploringData->builderUnitsCost <= m_resourcesCount)
    {
        const Entity& entity = m_playerView->entities[m_exploringData->builderBaseIndex];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);

        buildAction = std::shared_ptr<BuildAction>(new BuildAction(
                                                       EntityType::BUILDER_UNIT,
                                                       Vec2Int(entity.position.x + properties.size, entity.position.y + properties.size - 1)));
        m_resourcesCount -= m_exploringData->builderUnitsCost;
        act.entityActions[m_exploringData->builderBaseId] = EntityAction( nullptr, buildAction, nullptr, nullptr);
    } else
    {
        act.entityActions[m_exploringData->builderBaseId] = EntityAction( nullptr, nullptr, nullptr, nullptr);
    }

    int x, y;
    bool f = m_exploringData->getFreeHouseCoordinate(x, y);

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
        } else
        if (i > m_exploringData->builderUnitsCount - 3 && f && m_exploringData->myResourcesCount > 50 && (m_exploringData->meleeUnitsCount + m_exploringData->rangedUnitsCount > 3)
                && (m_exploringData->freePopulation < 8))
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

void DefaultWarMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        if (properties.canMove) {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                Vec2Int(m_playerView->mapSize - 1, m_playerView->mapSize - 1),
                true,
                true));
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

void DefaultDefenceMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        if (properties.canMove) {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                Vec2Int(m_playerView->mapSize - 1, m_playerView->mapSize - 1),
                true,
                true));
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

        if (properties.build != nullptr) {
            EntityType entityType = properties.build->options[0];
            size_t currentUnits = m_units.size();
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
