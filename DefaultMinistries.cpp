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
    if (m_resourcesCount >= 50 && (m_exploringData->freePopulation < 15))
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
    if (m_resourcesCount >= 50 && (m_exploringData->freePopulation < 15))
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


void DefaultWarMinister::addMinistryAction(Action &act)
{
}

void DefaultDefenceMinister::addMinistryAction(Action &act)
{
}
