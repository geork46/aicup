#include "DefaultMinistries.h"

#include <iostream>
#include <math.h>

void DefaultEconomicMinister::addMinistryAction(Action &act)
{
    int myId = m_playerView->myId;

    fillRepairMap();
    createBuilderUnit(act);

    static int tick = 0;
    tick++;
    if (tick % 3 == 0)
    {
        m_buildHouseMap.clear();
        m_buildTypeMap.clear();
    }
    if (m_exploringData->rangedBaseCount < 1 &&
            m_resourcesCount + m_exploringData->builderUnitsCount >= m_exploringData->entityCost[EntityType::RANGED_BASE])
    {
        if (m_buildHouseMap.size() < 3)
        {
            fillBuildRangeBaseaMap();
        }
    }
    if (m_resourcesCount >= 50)// && (m_exploringData->freePopulation < 15))
    {
        if (m_buildHouseMap.size() < 2)
        {
            fillBuildHouseMap();
        } else if (m_buildHouseMap.size() < 3 &&
                   m_resourcesCount + m_exploringData->builderUnitsCount >= 200)
        {
            fillBuildHouseMap();
        }
    }
    if (m_resourcesCount >= 50 && m_exploringData->maxPopulation > 50 && m_exploringData->rangedBaseCount > 0)
    {
        if (m_buildHouseMap.size() < 2)
        {
            fillBuildTurrets();
        } else if (m_buildHouseMap.size() < 3 && m_resourcesCount + m_exploringData->builderUnitsCount >= 200)
        {
            fillBuildTurrets();
        }
    }

    int x, y;
    bool f = m_exploringData->getFreeHouseCoordinate(x, y);

    std::shared_ptr<BuildAction> buildAction = nullptr;
    std::shared_ptr<MoveAction> moveAction = nullptr;

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        buildAction = nullptr;

        if (m_buildHouseMap.find(i) != m_buildHouseMap.end() &&
                m_resourcesCount + m_exploringData->builderUnitsCount > m_exploringData->entityCost[m_buildTypeMap[i]])
        {
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         m_buildHouseMap[i].second,
                                                         true, true));
            buildAction = std::shared_ptr<BuildAction>(new BuildAction(m_buildTypeMap[i] , m_buildHouseMap[i].first));
            act.entityActions[entity.id] = EntityAction( moveAction, buildAction, nullptr, nullptr);
            m_resourcesCount -= m_exploringData->entityCost[m_buildTypeMap[i]];
            if (m_buildHouseMap[i].second == entity.position)
            {
                    m_buildHouseMap.erase(i);
            }
            continue;
        }

        if (tryRepair(act, entity))
        {
            continue;
        }

        if (m_repairMap.find(i) != m_repairMap.end())
        {

            const Entity& building = m_playerView->entities[m_repairMap[i]];
            const EntityProperties& prop = m_exploringData->entityProperties[building.entityType];
            moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                                         Vec2Int( building.position.x + prop.size / 2 ,
                                                                  building.position.y + prop.size / 2),
                                                         true, true));
            act.entityActions[entity.id] = EntityAction( moveAction, nullptr, nullptr,std::shared_ptr<RepairAction>(new RepairAction(building.id)));
            continue;
        }

        farmResources(act, entity, i);
    }
}

std::vector<Vec2Int> DefaultWarMinister::getDefencePositions() const
{
    std::vector<Vec2Int> init{};
//    init.push_back(Vec2Int(0, 26));
//    init.push_back(Vec2Int(1, 25));
//    init.push_back(Vec2Int(2, 24));

//    init.push_back(Vec2Int(4, 22));
//    init.push_back(Vec2Int(5, 21));
//    init.push_back(Vec2Int(6, 20));

//    init.push_back(Vec2Int(8, 18));
//    init.push_back(Vec2Int(9, 17));
//    init.push_back(Vec2Int(10, 16));

//    init.push_back(Vec2Int(12, 14));
//    init.push_back(Vec2Int(13, 13));
//    init.push_back(Vec2Int(14, 12));

//    init.push_back(Vec2Int(16, 10));
//    init.push_back(Vec2Int(17, 9));
//    init.push_back(Vec2Int(18, 8));

//    init.push_back(Vec2Int(20, 6));
//    init.push_back(Vec2Int(21, 5));
//    init.push_back(Vec2Int(22, 4));

//    init.push_back(Vec2Int(24, 2));
//    init.push_back(Vec2Int(25, 1));
//    init.push_back(Vec2Int(26, 0));

//    init.push_back(Vec2Int(0, 25));
//    init.push_back(Vec2Int(1, 24));

//    init.push_back(Vec2Int(4, 21));
//    init.push_back(Vec2Int(5, 20));

//    init.push_back(Vec2Int(8, 17));
//    init.push_back(Vec2Int(9, 16));

//    init.push_back(Vec2Int(12, 13));
//    init.push_back(Vec2Int(13, 12));

//    init.push_back(Vec2Int(16, 9));
//    init.push_back(Vec2Int(17, 8));

//    init.push_back(Vec2Int(20, 5));
//    init.push_back(Vec2Int(21, 4));
    int x = 0;
    int y = 26;
    while (y > 1)
    {
        init.push_back(Vec2Int(x++, y--));
        x += 1;
        y -= 1;
        init.push_back(Vec2Int(x++, y--));
        x += 1;
        y -= 1;
    }
    x = 0;
    y = 25;
    while (y > 1)
    {
        init.push_back(Vec2Int(x++, y--));
//        init.push_back(Vec2Int(x++, y--));
        x += 1;
        y -= 1;
        x += 2;
        y -= 2;
    }


    x = 0;
    y = 38;
    while (y > 1)
    {
        init.push_back(Vec2Int(x++, y--));
        init.push_back(Vec2Int(x++, y--));
        x += 2;
        y -= 2;
    }
    x = 0;
    y = 37;
    while (y > 1)
    {
        init.push_back(Vec2Int(x++, y--));
//        init.push_back(Vec2Int(x++, y--));
        x += 1;
        y -= 1;
        x += 2;
        y -= 2;
    }

//    init.push_back(Vec2Int(, ));
//    init.push_back(Vec2Int(, ));

//    init.push_back(Vec2Int(, ));

    return init;

}

std::vector<Vec2Int> DefaultEconomicMinister::getTurretsCoordinates() const
{
    std::vector<Vec2Int> init{};
//    init.push_back(Vec2Int(2, 22));
//    init.push_back(Vec2Int(6, 18));
//    init.push_back(Vec2Int(10, 14));
//    init.push_back(Vec2Int(14, 10));
//    init.push_back(Vec2Int(18, 6));
//    init.push_back(Vec2Int(22, 2));

    int x = 2;
    int y = 22;
//    while (y > 1)
//    {
//        init.push_back(Vec2Int(x, y));
//        x += 3;
//        y -= 3;
//    }

    x = 2;
    y = 34;
    while (y > 1)
    {
        init.push_back(Vec2Int(x, y));
        x += 4;
        y -= 4;
    }
//    init.push_back(Vec2Int(, ));
    return init;

}

std::vector<Vec2Int> DefaultEconomicMinister::getHousesCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(1, 1));
    init.push_back(Vec2Int(1, 4));
    init.push_back(Vec2Int(1, 8));
    init.push_back(Vec2Int(1, 11));
    init.push_back(Vec2Int(1, 14));
    init.push_back(Vec2Int(1, 17));
    init.push_back(Vec2Int(4, 1));
    init.push_back(Vec2Int(8, 1));
    init.push_back(Vec2Int(11, 1));
    init.push_back(Vec2Int(14, 1));
    init.push_back(Vec2Int(17, 1));
//    init.push_back(Vec2Int(, ));

    if (m_exploringData->turretCount > 8)
    {
        init.push_back(Vec2Int(2, 25));
        init.push_back(Vec2Int(2, 28));

        init.push_back(Vec2Int(6, 21));
        init.push_back(Vec2Int(6, 24));

        init.push_back(Vec2Int(10, 17));
        init.push_back(Vec2Int(10, 20));

        init.push_back(Vec2Int(14, 13));
        init.push_back(Vec2Int(14, 16));

        init.push_back(Vec2Int(18, 9));
        init.push_back(Vec2Int(18, 12));

        init.push_back(Vec2Int(22, 5));
        init.push_back(Vec2Int(22, 8));

//        init.push_back(Vec2Int(, ));
//        init.push_back(Vec2Int(, ));

    }
    return init;
}

std::vector<Vec2Int> DefaultEconomicMinister::getRangedBaseCoordinates() const
{
    std::vector<Vec2Int> init{};
    init.push_back(Vec2Int(4, 10));
    init.push_back(Vec2Int(10, 4));
//    init.push_back(Vec2Int(, ));
    return init;
}


void DefaultWarMinister::addMinistryAction(Action &act)
{
    fillPositionMap();

    for (size_t i = 0; i < m_units.size(); i++) {
        const Entity& entity = m_units[i];
        const EntityProperties& properties = m_exploringData->entityProperties[entity.entityType];
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;

//        int x = m_playerView->mapSize - 5, y = m_playerView->mapSize - 5;

        int x =  25, y = 25;

        if (m_positionMap.find(i) != m_positionMap.end())
        {
            Vec2Int v = m_positionMap[i];
            x = v.x;
            y = v.y;
        }

        moveAction = std::shared_ptr<MoveAction>(new MoveAction( Vec2Int(x, y), true, true));

        std::vector<EntityType> validAutoAttackTargets;
        act.entityActions[entity.id] = EntityAction(
                    moveAction,
                    buildAction,
                    std::shared_ptr<AttackAction>(new AttackAction(
                                                      nullptr, std::shared_ptr<AutoAttack>(new AutoAttack(properties.attack->attackRange - 1, validAutoAttackTargets)))),
                    nullptr);
    }

    createEntitiesByBuildings(act);

}


void DefaultWarMinister::fillPositionMap()
{
    m_positionMap.clear();
    m_reversePositionMap.clear();
    std::vector<Vec2Int> positions = getDefencePositions();

    for (int i = 0; i < m_units.size(); ++i)
    {
        const Entity & entity = m_units[i];
        double maxDistance = 1000000;
        Vec2Int p;

        for(Vec2Int v : positions)
        {
            if (m_reversePositionMap.find(v) != m_reversePositionMap.end())
            {
                continue;
            }
            double sqrDistance = m_exploringData->getDistanceSqr(entity, v.x, v.y);
            if (sqrDistance < maxDistance)
            {
                maxDistance = sqrDistance;
                p = v;
                if (maxDistance < 1)
                {
                    goto iter_end;
                }
            }
        }
        iter_end:
        m_positionMap[i] = p;
        m_reversePositionMap[p] = i;
        continue;
    }
}

void DefaultDefenceMinister::addMinistryAction(Action &act)
{
    for (size_t i = 0; i < m_buildings.size(); i++) {
        act.entityActions[m_buildings[i].id] = EntityAction( nullptr, nullptr, nullptr, nullptr);
    }
    for (int i = 0; i < m_buildings.size(); ++i)
    {
        if (m_buildings[i].entityType == TURRET)
        {
            turretAttack(act, m_buildings[i].id);
        }
    }
}
