#ifndef DEFAULTEXPLORINGMINISTER_H
#define DEFAULTEXPLORINGMINISTER_H

#include "Interfaces.h"

class DefaultExploringMinister : public IExploringMinistry
{
public:
    DefaultExploringMinister();

    virtual ExploringData getExploringData(const PlayerView &playerView);
};

#endif // DEFAULTEXPLORINGMINISTER_H
