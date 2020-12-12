#ifndef DEFAULTEXPLORINGMINISTER_H
#define DEFAULTEXPLORINGMINISTER_H

#include "Interfaces.h"

class DefaultExploringMinister : public IExploringMinistry
{
public:
    DefaultExploringMinister();

    virtual ExploringData getExploringData(const PlayerView &playerView);

private:
    void fillMap(const PlayerView &playerView, ExploringData &data, int index);

    double getDangerousCoef(double distance);

    void enemyAnalize(const PlayerView &playerView, ExploringData &data, const Entity &entity, int index);
    void postEnemyAnalize(const PlayerView &playerView, ExploringData &data);
};

#endif // DEFAULTEXPLORINGMINISTER_H
