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
        exploring1Fill(playerView, data);
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

    data.sightMap.clear();
    data.attackMap.clear();

    data.needRepairBuildings.clear();
    data.attackingEnemyUnits.clear();
    data.enemyUnits.clear();
    data.myWarriorUnits.clear();
    data.enemyBuilderUnits.clear();
    data.myBuildings.clear();
    data.safertyResources.clear();

    data.mainEnemy = 0;

    clearEnemyInfo(data);
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
        data.turretSize = data.entityProperties[EntityType::TURRET].size;
        data.rangedBaseSize = data.entityProperties[EntityType::RANGED_BASE].size;

        data.lastMap.clear();
        data.lastUpdatedMap.clear();

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
                fillAttackMap(playerView, data, entity);
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
            data.myWarriorUnits.push_back(i);
            break;
        case RANGED_BASE:
            data.rangedBaseID = entity.id;
            data.rangedBaseIndex = i;
            data.rangedBaseCount++;
            break;
        case RANGED_UNIT:
            data.rangedUnitsCount++;
            data.myWarriorUnits.push_back(i);
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
                fillAttackMap(playerView, data, entity);
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
            data.myWarriorUnits.push_back(i);
            break;
        case RANGED_BASE:
            data.rangedBaseID = entity.id;
            data.rangedBaseIndex = i;
            data.rangedBaseCount++;
            break;
        case RANGED_UNIT:
            data.rangedUnitsCount++;
            data.myWarriorUnits.push_back(i);
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

    updateLastMap(playerView, data);

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

void DefaultExploringMinister::exploring3(const PlayerView &playerView, ExploringData &data)
{
    exploring2(playerView, data);
}

void DefaultExploringMinister::fillSightMap(const PlayerView &playerView, ExploringData &data, const Entity &entity)
{
    const EntityProperties& properties = data.entityProperties[entity.entityType];
    int size = properties.size;
    int sight = properties.sightRange;

    for (int i = -sight; i <= sight; ++i)
    {
        for (int j = -sight + abs(i); j <= sight - abs(i); ++j)
        {
            if (size > 0)
            {
                for (int k = 0; k < size; ++k)
                {
                    int x, y;
                    x = entity.position.x + k;
                    y = entity.position.y;
                    updateLastMap(playerView, data, x + i, y + j);
                    data.sightMap[data.getIndex(x + i, y + j)] = Vec2Int(x + i, y + j);
                    x = entity.position.x;
                    y = entity.position.y + k;
                    updateLastMap(playerView, data, x + i, y + j);
                    data.sightMap[data.getIndex(x + i, y + j)] = Vec2Int(x + i, y + j);
                    x = entity.position.x + k;
                    y = entity.position.y + size - 1;
                    updateLastMap(playerView, data, x + i, y + j);
                    data.sightMap[data.getIndex(x + i, y + j)] = Vec2Int(x + i, y + j);
                    x = entity.position.x + size - 1;
                    y = entity.position.y + k;
                    updateLastMap(playerView, data, x + i, y + j);
                    data.sightMap[data.getIndex(x + i, y + j)] = Vec2Int(x + i, y + j);
                }
            } else {
                int x, y;
                x = entity.position.x;
                y = entity.position.y;
                updateLastMap(playerView, data, x + i, y + j);
                data.sightMap[data.getIndex(x + i, y + j)] = Vec2Int(x + i, y + j);
            }
        }
    }

}

void DefaultExploringMinister::fillAttackMap(const PlayerView &playerView, ExploringData &data, const Entity &entity)
{
    const EntityProperties& properties = data.entityProperties[entity.entityType];
    int size = properties.size;
    if (properties.attack == nullptr || properties.attack->damage < 2)
    {
        return;
    }


    int sight = properties.attack->attackRange + 1;

    std::unordered_set<Vec2Int> set;
    int x, y;

    for (int i = -sight; i <= sight; ++i)
    {
        for (int j = -sight + abs(i); j <= sight - abs(i); ++j)
        {
            int damage = properties.attack->damage;
            if (abs(i) + abs(j) == sight)
            {
                damage /= 2;
            }
            if (size > 1)
            {
                for (int k = 0; k < size; ++k)
                {
                    x = entity.position.x + k;
                    y = entity.position.y;
                    if (set.find(Vec2Int(x + i, y + j)) == set.end())
                    {
                        data.attackMap[data.getIndex(x + i, y + j)] += damage;
                        set.insert(Vec2Int(x + i, y + j));
                    }
                    x = entity.position.x;
                    y = entity.position.y + k;
                    if (set.find(Vec2Int(x + i, y + j)) == set.end())
                    {
                        data.attackMap[data.getIndex(x + i, y + j)] += damage;
                        set.insert(Vec2Int(x + i, y + j));
                    }
                    x = entity.position.x + k;
                    y = entity.position.y + size - 1;
                    if (set.find(Vec2Int(x + i, y + j)) == set.end())
                    {
                        data.attackMap[data.getIndex(x + i, y + j)] += damage;
                        set.insert(Vec2Int(x + i, y + j));
                    }
                    x = entity.position.x + size - 1;
                    y = entity.position.y + k;
                    if (set.find(Vec2Int(x + i, y + j)) == set.end())
                    {
                        data.attackMap[data.getIndex(x + i, y + j)] += damage;
                        set.insert(Vec2Int(x + i, y + j));
                    }
                }
            } else {
                x = entity.position.x;
                y = entity.position.y;
                data.attackMap[data.getIndex(x + i, y + j)] +=  damage;
            }
        }
    }

}

void DefaultExploringMinister::updateLastMap(const PlayerView &playerView, ExploringData &data)
{
    int myId = playerView.myId;

    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];

        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }
        fillSightMap(playerView, data, entity);
    }
}

void DefaultExploringMinister::updateLastMap(const PlayerView &playerView, ExploringData &data, int x, int y)
{
    int index = data.getIndex(x, y);
    if (data.lastUpdatedMap.find(index) == data.lastUpdatedMap.end())
    {
        int res = -1;
        if (data.map.find(index) != data.map.end())
        {
            res = playerView.entities[data.map[index]].entityType;
        }

        data.lastMap[index] = res;
        data.lastUpdatedMap[index] = playerView.currentTick;
        int tx;

        if (data.playersCount < 4)
        {
            x = data.mapSize - 1 -x;
            y = data.mapSize - 1 -y;
            index = data.getIndex(x, y);
            data.lastMap[index] = res;
            data.lastUpdatedMap[index] = playerView.currentTick;
        } else {
            for (int i = 0; i < 3; ++i)
            {
                tx = x;
                x = data.mapSize - 1 -y;
                y = tx;
                index = data.getIndex(x, y);
                data.lastMap[index] = res;
                data.lastUpdatedMap[index] = playerView.currentTick;
            }
        }

    } else {
        if (data.map.find(index) == data.map.end())
        {
            data.lastMap[index] = -1;
        } else {
            data.lastMap[index] = playerView.entities[data.map[index]].entityType;
        }
        data.lastUpdatedMap[index] = playerView.currentTick;
    }
}

void DefaultExploringMinister::exploring1Fill(const PlayerView &playerView, ExploringData &data)
{
    data.lastMap.clear();
    for (int i = 0; i < playerView.mapSize; ++i)
    {
        for (int j = 0; j < playerView.mapSize; ++j)
        {
            data.lastUpdatedMap[data.getIndex(i, j)] = 1000;
        }
    }

}

void DefaultExploringMinister::clearEnemyInfo(ExploringData &data)
{
    for (int i = 0; i < data.MAX_ENEMIES; ++i)
    {
        data.enemies[i].mainX = 0;
        data.enemies[i].mainY = 0;
        data.enemies[i].builderBaseX = 0;
        data.enemies[i].builderBaseY = 0;
        data.enemies[i].builderUnitsCount = 0;
        data.enemies[i].rangedUnitsCount = 0;
        data.enemies[i].meleeUnitsCount = 0;
        data.enemies[i].distance = 1000;
        data.enemies[i].entityCount = 0;
    }
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
            data.lastMap[data.getIndex(entity.position.x + j, entity.position.y + i)] = entity.entityType;
        }
    }
}

void DefaultExploringMinister::enemyAnalize(const PlayerView &playerView, ExploringData &data, const Entity &entity, int index)
{
    double distance = data.getDistance(entity, 0, 0);

    int playerId = *entity.playerId;

    data.enemies[playerId].entityCount++;

    if (data.enemies[playerId].distance > distance)
    {
        switch (entity.entityType) {
        case EntityType::BUILDER_BASE :
        case EntityType::BUILDER_UNIT :
        case EntityType::MELEE_BASE :
        case EntityType::MELEE_UNIT :
        case EntityType::RANGED_BASE :
        case EntityType::RANGED_UNIT :
        case EntityType::HOUSE :
            data.enemies[playerId].mainX = entity.position.x;
            data.enemies[playerId].mainY = entity.position.y;
            data.enemies[playerId].distance = distance;
            break;
        default:
            break;
        }
    }

    switch (entity.entityType) {
    case EntityType::BUILDER_BASE :
        data.enemies[playerId].builderBaseX = entity.position.x;
        data.enemies[playerId].builderBaseY = entity.position.y;
        break;
    case EntityType::BUILDER_UNIT :
        data.enemies[playerId].mainX = entity.position.x;
        data.enemies[playerId].mainY = entity.position.y;
        data.enemies[playerId].builderUnitsCount++;
        data.enemyUnits.push_back(index);
        data.enemyBuilderUnits.push_back(index);
        break;
    case EntityType::MELEE_BASE :
        data.enemies[playerId].mainX = entity.position.x;
        data.enemies[playerId].mainY = entity.position.y;
        break;
    case EntityType::MELEE_UNIT :
        data.enemies[playerId].meleeUnitsCount++;
        data.enemyUnits.push_back(index);
        break;
    case EntityType::RANGED_BASE :
        data.enemies[playerId].mainX = entity.position.x;
        data.enemies[playerId].mainY = entity.position.y;
        break;
    case EntityType::RANGED_UNIT :
        data.enemies[playerId].rangedUnitsCount++;
        data.enemyUnits.push_back(index);
        break;
    case EntityType::HOUSE :
        data.enemies[playerId].mainX = entity.position.x;
        data.enemies[playerId].mainY = entity.position.y;
        break;
    case EntityType::TURRET :
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
    for (int i : data.enemyUnits)
    {
        for (int j : data.myBuildings)
        {
            const EntityProperties& properties = data.entityProperties[playerView.entities[j].entityType];

            double distance = 8 + properties.size / 2;

            if (m_exploringData->getDistance(playerView.entities[i], playerView.entities[j]) < distance
                    && m_exploringData->getDistance(playerView.entities[j], 8, 8) < 40)
            {
                data.isBaseAttacked = true;
                data.attackingEnemyUnits.push_back(i);
                break;
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
                if (data.getDistance(entity, enemy) < 8 && enemy.entityType != EntityType::BUILDER_UNIT)
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
