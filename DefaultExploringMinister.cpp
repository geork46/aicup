#include "DefaultExploringMinister.h"

DefaultExploringMinister::DefaultExploringMinister()
{

}

ExploringData DefaultExploringMinister::getExploringData(const PlayerView &playerView)
{
    ExploringData data;

    int myId = playerView.myId;
    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (entity.entityType == EntityType::RESOURCE)
        {
            data.mapResourcesCount++;
        }

        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }

        const EntityProperties& properties = playerView.entityProperties.at(entity.entityType);

        switch (entity.entityType) {
        case HOUSE:
            data.housesCount++;
            break;
        case BUILDER_BASE:
            data.buildBaseCount++;
            break;
        case BUILDER_UNIT:
            data.buildUnitsCount++;
            break;
        case MELEE_BASE:
            data.meleeBaseCount++;
            break;
        case MELEE_UNIT:
            data.meleeUnitsCount++;
            break;
        case RANGED_BASE:
            data.rangedBaseCount++;
            break;
        case RANGED_UNIT:
            data.rangedUnitsCount++;
            break;
        default:
            break;
        }
        data.maxPopulation += properties.populationProvide;
        data.currentPopulation += properties.populationUse;
    }
    data.myResourcesCount = playerView.players[myId].resource;

    for (int i = EntityType::WALL; i <= EntityType::TURRET; ++i)
    {
        data.entityCost[i] = playerView.entityProperties.at((EntityType)i).initialCost;
        switch (i) {
        case EntityType::BUILDER_UNIT :
            data.entityCost[i] += data.buildUnitsCount;
            break;
        case EntityType::MELEE_UNIT :
            data.entityCost[i] += data.meleeUnitsCount;
            break;
        case EntityType::RANGED_UNIT :
            data.entityCost[i] += data.rangedUnitsCount;
            break;
        default:
            break;
        }

    }
}
