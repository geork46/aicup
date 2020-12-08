#include "Interfaces.h"

void IMinistry::addEntity(const Entity &e)
{
    switch (e.entityType) {
    case EntityType::WALL :
    case EntityType::HOUSE :
    case EntityType::BUILDER_BASE :
    case EntityType::MELEE_BASE :
    case EntityType::RANGED_BASE :
    case EntityType::TURRET :
        m_buildings.push_back(e);
        break;
    case EntityType::BUILDER_UNIT :
    case EntityType::MELEE_UNIT :
    case EntityType::RANGED_UNIT :
        m_units.push_back(e);
        break;
    default:
        break;
    }
}

void IMinistry::removeAllEntities()
{
    m_buildings.clear();
    m_units.clear();
}

void IMinistry::addMinistryAction(Action &act)
{
}
