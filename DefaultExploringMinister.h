#ifndef DEFAULTEXPLORINGMINISTER_H
#define DEFAULTEXPLORINGMINISTER_H

#include "Interfaces.h"

class DefaultExploringMinister : public IExploringMinistry
{
public:
    DefaultExploringMinister();

    virtual void getExploringData(const PlayerView &playerView, ExploringData &data);

private:
    void clearExploringData(ExploringData &data);
    void fillExploringDataFromPlayerView(ExploringData &data, const PlayerView &playerView);

    void exploring1(const PlayerView &playerView, ExploringData &data);
    void exploring2(const PlayerView &playerView, ExploringData &data);

    void exploring3(const PlayerView &playerView, ExploringData &data);


    void fillSightMap(const PlayerView &playerView, ExploringData &data, const Entity &entity);
    void fillAttackMap(const PlayerView &playerView, ExploringData &data, const Entity &entity);

    void updateLastMap(const PlayerView &playerView, ExploringData &data);
    void updateLastMap(const PlayerView &playerView, ExploringData &data, int x, int y);

    void exploring1Fill(const PlayerView &playerView, ExploringData &data);

    void clearEnemyInfo(ExploringData &data);

    void fillMap(const PlayerView &playerView, ExploringData &data, int index);

    void enemyAnalize(const PlayerView &playerView, ExploringData &data, const Entity &entity, int index);

    void postEnemyAnalize(const PlayerView &playerView, ExploringData &data);

    void resourcesAnalize(const PlayerView &playerView, ExploringData &data);
};

#endif // DEFAULTEXPLORINGMINISTER_H
