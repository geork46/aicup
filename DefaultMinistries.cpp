#include "DefaultMinistries.h"


void DefaultEconomicMinister::addMinistryAction(Action &act)
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
    for (size_t i = 0; i < m_buildings.size(); i++) {
        const Entity& entity = m_buildings[i];
        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        std::vector<EntityType> validAutoAttackTargets;

        if (properties.build != nullptr) {
            EntityType entityType = properties.build->options[0];
            size_t currentUnits = m_units.size();
            if (m_playerView->entityProperties.at(entityType).populationUse * (currentUnits + 1) <= m_maxPopulation
                    && m_playerView->entityProperties.at(entityType).initialCost + currentUnits <= m_resourcesCount)
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
            size_t currentUnits = m_units.size();
            if (m_playerView->entityProperties.at(entityType).populationUse * (currentUnits + 1) <= m_maxPopulation
                    && m_playerView->entityProperties.at(entityType).initialCost + currentUnits <= m_resourcesCount)
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
//    int myId = m_playerView->myId;

//    for (size_t i = 0; i < m_units.size(); i++) {
//        const Entity& entity = m_units[i];
//        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
//        std::shared_ptr<MoveAction> moveAction = nullptr;
//        std::shared_ptr<BuildAction> buildAction = nullptr;
//        if (properties.canMove) {
//            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
//                Vec2Int(m_playerView->mapSize - 1, m_playerView->mapSize - 1),
//                true,
//                true));
//        }

//        std::vector<EntityType> validAutoAttackTargets;
//        act.entityActions[entity.id] = EntityAction(
//            moveAction,
//            buildAction,
//            std::shared_ptr<AttackAction>(new AttackAction(
//                nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
//            nullptr);
//    }
//    for (size_t i = 0; i < m_buildings.size(); i++) {
//        const Entity& entity = m_buildings[i];
//        const EntityProperties& properties = m_playerView->entityProperties.at(entity.entityType);
//        std::shared_ptr<MoveAction> moveAction = nullptr;
//        std::shared_ptr<BuildAction> buildAction = nullptr;
//        std::vector<EntityType> validAutoAttackTargets;

//        if (properties.build != nullptr) {
//            EntityType entityType = properties.build->options[0];
//            size_t currentUnits = m_units.size();
//            if ((currentUnits + 1) * m_playerView->entityProperties.at(entityType).populationUse <= properties.populationProvide) {
//                buildAction = std::shared_ptr<BuildAction>(new BuildAction(
//                                                               entityType,
//                                                               Vec2Int(entity.position.x + properties.size, entity.position.y + properties.size - 1)));
//            }
//        }
//        act.entityActions[entity.id] = EntityAction(
//                    moveAction,
//                    buildAction,
//                    nullptr,
//                    nullptr);
//    }

}
