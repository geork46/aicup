#ifndef DEFAULTDISTRIBUTOR_H
#define DEFAULTDISTRIBUTOR_H

#include "Interfaces.h"

class DefaultDistributor : public IDistributor
{
public:
    DefaultDistributor();
protected:
    virtual void innerDistribute(const PlayerView &playerView, ExploringData const &data);
};

#endif // DEFAULTDISTRIBUTOR_H
