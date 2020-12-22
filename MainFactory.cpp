#include "MainFactory.h"

#include "DefaultDistributor.h"
#include "DefaultMinistries.h"
#include "AlarmingMinisters.h"
#include "DefaultExploringMinister.h"

#include "StartGameMinisters.h"
#include "StartGameMinisters2.h"

MainFactory *MainFactory::m_inst = nullptr;

MainFactory *MainFactory::instance()
{
    if (!m_inst)
    {
        m_inst = new MainFactory();
    }
    return  m_inst;
}

IExploringMinistry *MainFactory::getExploringMinister()
{
    return m_exploringMinister;
}

IEconomicsMinistry *MainFactory::getEconomicMinister()
{
    return m_economicMinister;
}

IWarMinistry *MainFactory::getWarMinister()
{
    return m_warMinister;
}

IDefenceMinistry *MainFactory::getDefenceMinister()
{
    return m_defenceMinister;
}

void MainFactory::updateMinisters(const PlayerView &playerView, const ExploringData &data)
{
//    if (playerView.fogOfWar)
//    {
//        if (data.isBaseAttacked)
//        {
//            updateMinister(m_economicMinister, m_ministers[ECONOMIC_ALARMING]);
//            updateMinister(m_warMinister, m_ministers[WAR_ALARMING]);
//            updateMinister(m_defenceMinister, m_ministers[DEFENCE_ALARMING]);
//            updateMinister(m_distributor, m_distributors[DISTRIBUTOR_ALARMING]);
//        } else
//        {
//            updateMinister(m_economicMinister, m_ministers[ECONOMIC_START2]);
//            updateMinister(m_warMinister, m_ministers[WAR_START2]);
//            updateMinister(m_defenceMinister, m_ministers[DEFENCE_START2]);
//            updateMinister(m_distributor, m_distributors[DISTRIBUTOR_START2]);
//        }
//    } else
//    {
//        if (data.isBaseAttacked)
//        {
//            updateMinister(m_economicMinister, m_ministers[ECONOMIC_ALARMING]);
//            updateMinister(m_warMinister, m_ministers[WAR_ALARMING]);
//            updateMinister(m_defenceMinister, m_ministers[DEFENCE_ALARMING]);
//            updateMinister(m_distributor, m_distributors[DISTRIBUTOR_ALARMING]);
//        } else
//        {
//            updateMinister(m_economicMinister, m_ministers[ECONOMIC_START]);
//            updateMinister(m_warMinister, m_ministers[WAR_START]);
//            updateMinister(m_defenceMinister, m_ministers[DEFENCE_START]);
//            updateMinister(m_distributor, m_distributors[DISTRIBUTOR_START]);
//        }
//    }
}

void MainFactory::redistributeResources(const PlayerView &playerView, const ExploringData &data)
{
    if (m_distributor)
    {
        m_distributor->redistribute(playerView, data, m_economicMinister, m_warMinister, m_defenceMinister);
    }
}

void MainFactory::initMinisters()
{
    for (int i = 0; i < MINISTER_NAME_COUNT; ++i)
    {
        m_ministers[i] = nullptr;
    }
    m_ministers[ECONOMIC_DEFAULT] = new DefaultEconomicMinister();
    m_ministers[WAR_DEFAULT] = new DefaultWarMinister();
    m_ministers[DEFENCE_DEFAULT] = new DefaultDefenceMinister();

    m_ministers[ECONOMIC_ALARMING] = new AlarmingEconomicMinister();
    m_ministers[WAR_ALARMING] = new AlarmingWarMinister();
    m_ministers[DEFENCE_ALARMING] = new AlarmingDefenceMinister();

    m_ministers[ECONOMIC_START] = new StartGameEconomicMinister();
    m_ministers[WAR_START] = new StartGameWarMinister();
    m_ministers[DEFENCE_START] = new StartGameDefenceMinister();

    m_ministers[ECONOMIC_START2] = new StartGameEconomicMinister2();
    m_ministers[WAR_START2] = new StartGameWarMinister2();
    m_ministers[DEFENCE_START2] = new StartGameDefenceMinister2();

    updateMinister(m_economicMinister, m_ministers[ECONOMIC_START]);
    updateMinister(m_warMinister, m_ministers[WAR_START]);
    updateMinister(m_defenceMinister, m_ministers[DEFENCE_START]);

    updateMinister(m_economicMinister, m_ministers[ECONOMIC_DEFAULT]);
    updateMinister(m_warMinister, m_ministers[WAR_DEFAULT]);
    updateMinister(m_defenceMinister, m_ministers[DEFENCE_DEFAULT]);

    updateMinister(m_exploringMinister, new DefaultExploringMinister());
}

void MainFactory::initDistributors()
{
    for (int i = 0; i < DISTRIBUTOR_NAME_COUNT; ++i)
    {
        m_distributors[i] = nullptr;
    }
    m_distributors[DISTRIBUTOR_DEFAULT] = new DefaultDistributor();
    m_distributors[DISTRIBUTOR_START] = new StartGameDistributor();
    m_distributors[DISTRIBUTOR_START2] = new StartGameDistributor2();
    m_distributors[DISTRIBUTOR_MORE_WAR] = new MoreWarDistributor();
    m_distributors[DISTRIBUTOR_ALARMING] = new AlarmingDistributor();

    updateMinister(m_distributor, m_distributors[DISTRIBUTOR_START]);
//    updateMinister(m_distributor, m_distributors[DISTRIBUTOR_DEFAULT]);
}

MainFactory::MainFactory()
{
    initMinisters();
    initDistributors();
}
