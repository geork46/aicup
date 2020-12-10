#include "DefaultDistributor.h"

DefaultDistributor::DefaultDistributor()
{
}

void DefaultDistributor::innerDistribute(const PlayerView &playerView, const ExploringData &data)
{
    int population = 0;
    int myId = playerView.myId;
    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }

        const EntityProperties& properties = playerView.entityProperties.at(entity.entityType);
        if (properties.build != nullptr)
        {
            population += properties.populationProvide;
        }

        if (entity.entityType == EntityType::BUILDER_BASE || entity.entityType == EntityType::BUILDER_UNIT)
        {
            m_economicMinister->addEntity(entity);
        } else
        if (entity.entityType == EntityType::RANGED_BASE || entity.entityType == EntityType::RANGED_UNIT)
        {
            m_warMinister->addEntity(entity);
        } else
        if (entity.entityType == EntityType::MELEE_BASE || entity.entityType == EntityType::MELEE_UNIT)
        {
            m_defenceMinister->addEntity(entity);
        }
    }

    int resources = playerView.players[playerView.myId].resource;
    m_economicMinister->setResourcesCount(resources / 3);
    m_warMinister->setResourcesCount(resources / 3);
    m_defenceMinister->setResourcesCount(resources / 3);

    m_economicMinister->setMaxPopulation(population / 3);
    m_warMinister->setMaxPopulation(population / 3);
    m_defenceMinister->setMaxPopulation(population / 3);
}
