#ifndef MAINFACTORY_H
#define MAINFACTORY_H

#include "Interfaces.h"

enum MinistersName
{
    ECONOMIC_1 = 0,
    WAR_1,
    DEFENCE_1,
    MINISTER_NAME_COUNT
};

enum DistributorsName
{
    DISTRIBUTOR_1 = 0,
    DISTRIBUTOR_NAME_COUNT
};


class MainFactory
{
public:
    static MainFactory* instance();


    IExploringMinistry* getExploringMinister();

    IEconomicsMinistry* getEconomicMinister();
    IWarMinistry* getWarMinister();
    IDefenceMinistry* getDefenceMinister();

    void updateMinisters(const PlayerView &playerView, ExploringData const &data);
    void redistributeResources(const PlayerView &playerView, ExploringData const &data);

private:
    void initMinisters();
    void initDistributors();

    template<class T>
    void updateMinister(T &oldMinister, T newMinister) {
        if (oldMinister != newMinister)
        {
            oldMinister->deactivate();
            oldMinister = newMinister;
            newMinister->activate();
        }
    }

    IMinistry* m_ministers[MINISTER_NAME_COUNT];
    IDistributor* m_distributors[DISTRIBUTOR_NAME_COUNT];

    IEconomicsMinistry* 	m_economicMinister = nullptr;
    IWarMinistry* 			m_warMinister = nullptr;
    IDefenceMinistry* 		m_defenceMinister = nullptr;
    IDistributor* 		m_distributor = nullptr;

    MainFactory();
    static MainFactory *m_inst;
};

#endif // MAINFACTORY_H
