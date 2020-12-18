#include "DefaultExploringMinister.h"

#include "math.h"

DefaultExploringMinister::DefaultExploringMinister()
{

}

void DefaultExploringMinister::getExploringData(const PlayerView &playerView, ExploringData &data)
{
    m_playerView = &playerView;
    m_exploringData = &data;

    clearExploringData(data);
    fillExploringDataFromPlayerView(data, playerView);

    if (playerView.fogOfWar)
    {
        if (data.playersCount > 2)
        {
            exploring2(playerView, data);
        } else {
            exploring3(playerView, data);
        }

    } else {
        exploring1(playerView, data);
    }

}

void DefaultExploringMinister::clearExploringData(ExploringData &data)
{
    data.builderUnitsCount = 0;
    data.rangedUnitsCount = 0;
    data.meleeUnitsCount = 0;
    data.builderBaseCount = 0;
    data.rangedBaseCount = 0;
    data.meleeBaseCount = 0;
    data.housesCount = 0;
    data.turretCount = 0;
    data.myResourcesCount = 0;
    data.mapResourcesCount = 0;
    data.maxPopulation = 0;
    data.currentPopulation = 0;
    data.freePopulation = 0;

    data.isBaseAttacked = false;

    data.map.clear();
    data.enemies.clear();
    data.needRepairBuildings.clear();
    data.attackingEnemyUnits.clear();
    data.enemyUnits.clear();
    data.enemyBuilderUnits.clear();
    data.myBuildings.clear();
    data.safertyResources.clear();

    data.mainEnemy = 0;
}

void DefaultExploringMinister::fillExploringDataFromPlayerView(ExploringData &data, const PlayerView &playerView)
{
    data.playerView = &playerView;

    static bool onlyOne = true;

    if (onlyOne)
    {
        for (int i = WALL; i <= TURRET; ++i)
        {
            data.entityProperties[i] = playerView.entityProperties.at((EntityType)i);
        }

        data.mapSize = playerView.mapSize;
        data.builderUnitPopulationUse = data.entityProperties[EntityType::BUILDER_UNIT].populationUse;
        data.rangedUnitPopulationUse = data.entityProperties[EntityType::RANGED_UNIT].populationUse;
        data.meleeUnitPopulationUse = data.entityProperties[EntityType::MELEE_UNIT].populationUse;

        data.houseSize = data.entityProperties[EntityType::HOUSE].size;
        data.rangedBaseSize = data.entityProperties[EntityType::RANGED_BASE].size;


        data.playersCount = playerView.players.size();
        onlyOne = false;
    }
}

void DefaultExploringMinister::exploring1(const PlayerView &playerView, ExploringData &data)
{
    int myId = playerView.myId;

    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];

        fillMap(playerView, data, i);

        if (entity.entityType == EntityType::RESOURCE)
        {
            data.mapResourcesCount++;
            data.safertyResources.push_back(i);
        }

        if (entity.playerId == nullptr || *entity.playerId != myId) {
            if (entity.playerId != nullptr)
            {
                enemyAnalize(playerView, data, entity, i);
            }
            continue;
        }

        const EntityProperties& properties = data.entityProperties[entity.entityType];

        if ((!properties.canMove && properties.size > 1) || entity.entityType == EntityType::BUILDER_UNIT)
        {
            data.myBuildings.push_back(i);
        }

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
        case TURRET:
            data.turretID = entity.id;
            data.turretCount++;
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
        if (entity.health < properties.maxHealth && !properties.canMove)
        {
            data.needRepairBuildings.push_back(i);
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
        data.entityCost[i] = data.entityProperties[(EntityType)i].initialCost;
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


    postEnemyAnalize(playerView, data);
    resourcesAnalize(playerView, data);
}

void DefaultExploringMinister::exploring2(const PlayerView &playerView, ExploringData &data)
{

}

void DefaultExploringMinister::exploring3(const PlayerView &playerView, ExploringData &data)
{
    exploring2(playerView, data);
}

void DefaultExploringMinister::fillMap(const PlayerView &playerView, ExploringData &data, int index)
{
    const Entity& entity = playerView.entities[index];
    const EntityProperties& properties = data.entityProperties[entity.entityType];
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
    if (distance < 35)
    {
        return 45;
    }
    if (distance < 60)
    {
        return 5 * ((60 - distance)/30 + 1);
    }
    return 2.5 * ((120 - distance)/60 + 1);
}

void DefaultExploringMinister::enemyAnalize(const PlayerView &playerView, ExploringData &data, const Entity &entity, int index)
{
    const EntityProperties& properties = data.entityProperties[entity.entityType];

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
        data.enemyUnits.push_back(index);
        data.enemyBuilderUnits.push_back(index);
        break;
    case EntityType::MELEE_BASE :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        data.enemies[*entity.playerId].dangerousLevel += 3 * getDangerousCoef(distance);
        break;
    case EntityType::MELEE_UNIT :
        data.enemies[*entity.playerId].meleeUnitsCount++;
        data.enemies[*entity.playerId].dangerousLevel += 5 * getDangerousCoef(distance);
        data.enemyUnits.push_back(index);
        break;
    case EntityType::RANGED_BASE :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        data.enemies[*entity.playerId].dangerousLevel += 3 * getDangerousCoef(distance);
        break;
    case EntityType::RANGED_UNIT :
        data.enemies[*entity.playerId].rangedUnitsCount++;
        data.enemies[*entity.playerId].dangerousLevel += 5 * getDangerousCoef(distance);
        data.enemyUnits.push_back(index);
        break;
    case EntityType::HOUSE :
        data.enemies[*entity.playerId].mainX = entity.position.x;
        data.enemies[*entity.playerId].mainY = entity.position.y;
        data.enemies[*entity.playerId].dangerousLevel += 0.1 * getDangerousCoef(distance);
        break;
    case EntityType::TURRET :
        data.enemies[*entity.playerId].dangerousLevel += 0 * getDangerousCoef(distance);
        data.enemyUnits.push_back(index);
        break;
    case EntityType::WALL :
    case EntityType::RESOURCE :
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


    for (int i : data.enemyUnits)
    {
        for (int j : data.myBuildings)
        {
            const EntityProperties& properties = data.entityProperties[playerView.entities[j].entityType];
            if (m_exploringData->getDistance(playerView.entities[i], playerView.entities[j]) < 8 + properties.size / 2
                    && m_exploringData->getDistance(playerView.entities[j], 8, 8) < 40)
            {
                data.isBaseAttacked = true;
                data.attackingEnemyUnits.push_back(i);
            }
        }
    }

}

void DefaultExploringMinister::resourcesAnalize(const PlayerView &playerView, ExploringData &data)
{
    std::vector<int> l = data.safertyResources;
    data.safertyResources.clear();
    for (int i : l)
    {
        const Entity& entity = playerView.entities[i];
        int x = entity.position.x;
        int y = entity.position.y;

        bool f = true;
        f = f && (data.map.find(data.getIndex(x - 1, y)) == data.map.end() ||
                  data.map.find(data.getIndex(x + 1, y)) == data.map.end() ||
                  data.map.find(data.getIndex(x, y - 1)) == data.map.end() ||
                  data.map.find(data.getIndex(x, y + 1)) == data.map.end() );
        if (f)
        {
            for (int j : data.enemyUnits)
            {
                const Entity& enemy = playerView.entities[j];
                if (data.getDistanceSqr(entity, enemy) < 7*7 && enemy.entityType != EntityType::BUILDER_UNIT)
                {
                    f = false;
                    break;
                }
            }
        }
        if (f)
        {
            data.safertyResources.push_back(i);
        }
    }

}
