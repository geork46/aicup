#include "DefaultExploringMinister.h"

#include "math.h"

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
            if (entity.playerId != nullptr)
            {
                enemyAnalize(playerView, data, entity);
            }
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
        } else {
            if (data.enemies.find(playerView.players[i].id) == data.enemies.end())
            {
                data.enemies[playerView.players[i].id] = EnemyInfo{};
            }
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

    postEnemyAnalize(playerView, data);

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

double DefaultExploringMinister::getDangerousCoef(double distance)
{
    if (distance < 30)
    {
        return 40;
    }
    if (distance < 60)
    {
        return 5 * ((60 - distance)/30 + 1);
    }
    return 2.5 * ((120 - distance)/60 + 1);
}

void DefaultExploringMinister::enemyAnalize(const PlayerView &playerView, ExploringData &data, const Entity &entity)
{
    const EntityProperties& properties = playerView.entityProperties.at(entity.entityType);

    double distance = sqrt(entity.position.x * entity.position.x + entity.position.y * entity.position.y);

    if (data.enemies[*entity.playerId].testDistance > distance)
    {
        switch (entity.entityType) {
        case EntityType::BUILDER_BASE :
        case EntityType::BUILDER_UNIT :
        case EntityType::MELEE_BASE :
        case EntityType::MELEE_UNIT :
        case EntityType::RANGED_BASE :
        case EntityType::RANGED_UNIT :
        case EntityType::HOUSE :
            data.enemies[*entity.playerId].mainX = entity.position.x;
            data.enemies[*entity.playerId].mainY = entity.position.y;
            data.enemies[*entity.playerId].testDistance = distance;
            break;
        default:
            break;
        }
    }

    switch (entity.entityType) {
    case EntityType::BUILDER_BASE :
        data.enemies[*entity.playerId].builderBaseX = entity.position.x;
        data.enemies[*entity.playerId].builderBaseY = entity.position.y;
        data.enemies[*entity.playerId].dangerousLevel += 2 * getDangerousCoef(distance);
        break;
    case EntityType::BUILDER_UNIT :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        data.enemies[*entity.playerId].builderUnitsCount++;
        data.enemies[*entity.playerId].dangerousLevel += 1 * getDangerousCoef(distance);

        break;
    case EntityType::MELEE_BASE :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        data.enemies[*entity.playerId].dangerousLevel += 3 * getDangerousCoef(distance);
        break;
    case EntityType::MELEE_UNIT :
        data.enemies[*entity.playerId].meleeUnitsCount++;
        data.enemies[*entity.playerId].dangerousLevel += 5 * getDangerousCoef(distance);
        break;
    case EntityType::RANGED_BASE :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        data.enemies[*entity.playerId].dangerousLevel += 3 * getDangerousCoef(distance);
        break;
    case EntityType::RANGED_UNIT :
        data.enemies[*entity.playerId].rangedUnitsCount++;
        data.enemies[*entity.playerId].dangerousLevel += 5 * getDangerousCoef(distance);
        break;
    case EntityType::HOUSE :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        break;
    case EntityType::WALL :
    case EntityType::RESOURCE :
    case EntityType::TURRET :
    default:
        break;
    }

}

void DefaultExploringMinister::postEnemyAnalize(const PlayerView &playerView, ExploringData &data)
{
    int maxLevel = -1;
    int k = -1;
    for (auto i : data.enemies)
    {
        if (i.second.dangerousLevel > maxLevel)
        {
            k = i.first;
            maxLevel = i.second.dangerousLevel;
        }
    }
    data.mainEnemy = k;
}