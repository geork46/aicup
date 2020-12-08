#include "DefaultDistributor.h"

DefaultDistributor::DefaultDistributor()
{
}

void DefaultDistributor::innerDistribute(const PlayerView &playerView, const ExploringData &data)
{
    int myId = playerView.myId;
    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }

        if (entity.entityType == EntityType::BUILDER_BASE || entity.entityType == EntityType::BUILDER_UNIT)
        {
            m_economicMinister->addEntity(entity);
        }

    }

}
