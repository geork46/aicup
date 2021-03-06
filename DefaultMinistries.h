#ifndef DEFAULTMINISTRIES_H
#define DEFAULTMINISTRIES_H

#include "Interfaces.h"

#include <unordered_map>

class DefaultEconomicMinister : public IEconomicsMinistry
{
public:
    virtual void addMinistryAction(Action &act);

protected:

    virtual std::vector<Vec2Int> getTurretsCoordinates() const;
    virtual std::vector<Vec2Int> getHousesCoordinates() const;
    virtual std::vector<Vec2Int> getRangedBaseCoordinates() const;


};

class DefaultWarMinister : public IWarMinistry
{
public:
    virtual void addMinistryAction(Action &act);
    virtual std::vector<Vec2Int> getDefencePositions() const;
protected:
    virtual void fillPositionMap();

};

class DefaultDefenceMinister : public IDefenceMinistry
{
public:
    virtual void addMinistryAction(Action &act);

};

#endif // DEFAULTMINISTRIES_H
