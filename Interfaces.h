#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"

class PlayerView;

struct ExploringData
{

};

class IMinistry
{
public:
    void addEntity(Entity const & e);
    void removeAllEntities();

    virtual void addMinistryAction(Action &act);
protected:

    std::vector<Entity> m_units;
    std::vector<Entity> m_buildings;
};

class IExploringMinistry : public IMinistry
{
public:
    virtual ExploringData getExploringData(const PlayerView &playerView) = 0;
};


class IEconomicsMinistry : public IMinistry
{
public:
};

class IWarMinistry : public IMinistry
{
public:
};

class IDefenceMinistry : public IMinistry
{
public:
};

#endif
