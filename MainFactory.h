#ifndef MAINFACTORY_H
#define MAINFACTORY_H

#include "Interfaces.h"

enum MinistersName
{
    ECONOMIC_DEFAULT = 0,
    WAR_DEFAULT,
    DEFENCE_DEFAULT,
    ECONOMIC_START,
    WAR_START,
    DEFENCE_START,
    ECONOMIC_START2,
    WAR_START2,
    DEFENCE_START2,
    ECONOMIC_START3,
    WAR_START3,
    DEFENCE_START3,
    ECONOMIC_ALARMING,
    WAR_ALARMING,
    DEFENCE_ALARMING,
    ECONOMIC_ALARMING2,
    WAR_ALARMING2,
    DEFENCE_ALARMING2,
    MINISTER_NAME_COUNT
};

enum DistributorsName
{
    DISTRIBUTOR_DEFAULT = 0,
    DISTRIBUTOR_START,
    DISTRIBUTOR_START2,
    DISTRIBUTOR_START3,
    DISTRIBUTOR_ALARMING,
    DISTRIBUTOR_ALARMING2,
    DISTRIBUTOR_MORE_WAR,
    DISTRIBUTOR_NAME_COUNT
};

enum State
{
    STATE_START = 0,
    STATE_DEFAULT,
    STATE_BASE_ATTACKED,
    STATE_COUNT

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

    template<class T, class TT>
    void updateMinister(T &oldMinister, TT newMinister) {
        if (oldMinister != newMinister)
        {
            if (oldMinister)
            {
                oldMinister->deactivate();
            }
            oldMinister = dynamic_cast<T>(newMinister);
            if (newMinister)
            {
                newMinister->activate();
            }
        }
    }

    IMinistry* m_ministers[MINISTER_NAME_COUNT];
    IDistributor* m_distributors[DISTRIBUTOR_NAME_COUNT];

    IExploringMinistry* 	m_exploringMinister = nullptr;
    IEconomicsMinistry* 	m_economicMinister = nullptr;
    IWarMinistry* 			m_warMinister = nullptr;
    IDefenceMinistry* 		m_defenceMinister = nullptr;
    IDistributor* 		m_distributor = nullptr;

    State   m_currentState = STATE_START;

    MainFactory();
    static MainFactory *m_inst;
};

#endif // MAINFACTORY_H
