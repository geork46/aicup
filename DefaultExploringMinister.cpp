#include "DefaultExploringMinister.h"

DefaultExploringMinister::DefaultExploringMinister()
{

}

ExploringData DefaultExploringMinister::getExploringData(const PlayerView &playerView)
{
    ExploringData data{};

    int myId = playerView.myId;
    data.mapSize = playerView.mapSize;

    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];

        fillMap(playerView, data, i);

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
            data.builderBaseId = entity.id;
            data.builderBaseIndex = i;
            data.builderBaseCount++;
            break;
        case BUILDER_UNIT:
            data.builderUnitsCount++;
            break;
        case MELEE_BASE:
            data.meleeBaseID = entity.id;
            data.meleeBaseIndex = i;
            data.meleeBaseCount++;
            break;
        case MELEE_UNIT:
            data.meleeUnitsCount++;
            break;
        case RANGED_BASE:
            data.rangedBaseID = entity.id;
            data.rangedBaseIndex = i;
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

    for (int i = 0; i < playerView.players.size(); ++i )
    {
        if (playerView.players[i].id == myId)
        {
            data.myResourcesCount = playerView.players[i].resource;
        }
    }
    data.freePopulation = data.maxPopulation - data.currentPopulation;

    for (int i = EntityType::WALL; i <= EntityType::TURRET; ++i)
    {
        data.entityCost[i] = playerView.entityProperties.at((EntityType)i).initialCost;
        switch (i) {
        case EntityType::BUILDER_UNIT :
            data.entityCost[i] += data.builderUnitsCount;
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
    data.builderUnitsCost = data.entityCost[EntityType::BUILDER_UNIT];
    data.meleeUnitsCost = data.entityCost[EntityType::MELEE_UNIT];
    data.rangedUnitsCost = data.entityCost[EntityType::BUILDER_UNIT];

    data.houseSize = playerView.entityProperties.at(EntityType::HOUSE).size;

    return data;
}

void DefaultExploringMinister::fillMap(const PlayerView &playerView, ExploringData &data, int index)
{
    const Entity& entity = playerView.entities[index];
    const EntityProperties& properties = playerView.entityProperties.at(entity.entityType);
    for (int i = 0; i < properties.size; ++i)
    {
        for (int j = 0; j < properties.size; ++j)
        {
            data.map[data.getIndex(entity.position.x + j, entity.position.y + i)] = index;
        }
    }
}
