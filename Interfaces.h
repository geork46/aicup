#ifndef INTERFACES
#define INTERFACES

#include "model/Model.hpp"

class PlayerView;

struct ExploringData
{

};

class IMinistry
{

};

class IExploringMinistry
{
public:
    virtual ExploringData getExploringData(const PlayerView &playerView) = 0;
};


class IEconomicsMinistry
{
public:
};

class IWarMinistry
{
public:
};

class IDefenceMinistry
{
public:
};

#endif
