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

    if (data.builderUnitsCount < data.maxPopulation / 3)
    {
        m_economicMinister->setResourcesCount(data.myResourcesCount);
        m_warMinister->setResourcesCount((data.myResourcesCount - data.builderUnitsCost) / 2);
        m_defenceMinister->setResourcesCount((data.myResourcesCount - data.builderUnitsCost) / 2);

        m_economicMinister->setMaxPopulation(data.freePopulation);
        m_warMinister->setMaxPopulation((data.freePopulation - (data.builderUnitsCount - 5)) / 2);
        m_defenceMinister->setMaxPopulation((data.freePopulation - (data.builderUnitsCount - 5)) / 2);
    } else
    {
        if (data.freePopulation <= 8)
        {
            m_economicMinister->setResourcesCount(data.myResourcesCount * 5 / 10 - 2);
            m_warMinister->setResourcesCount(data.myResourcesCount * 3 / 10 - 2);
            m_defenceMinister->setResourcesCount(data.myResourcesCount * 2 / 10 - 2);
        } else
        {
            m_economicMinister->setResourcesCount(data.myResourcesCount * 1 / 6 - 1);
            m_warMinister->setResourcesCount(data.myResourcesCount * 3 / 6 - 1);
            m_defenceMinister->setResourcesCount(data.myResourcesCount * 2 / 6 - 1);
        }

        m_economicMinister->setMaxPopulation(data.maxPopulation / 3 > data.builderUnitsCount?1:0);
        m_warMinister->setMaxPopulation(data.freePopulation - 1);
        m_defenceMinister->setMaxPopulation(data.freePopulation - 1);
    }

}
