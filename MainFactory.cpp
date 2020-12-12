#include "MainFactory.h"

#include "DefaultDistributor.h"
#include "DefaultMinistries.h"
#include "DefaultExploringMinister.h"

#include "StartGameMinisters.h"

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
    if (data.myResourcesCount < 200)
    {
        updateMinister(m_economicMinister, m_ministers[ECONOMIC_START]);
        updateMinister(m_warMinister, m_ministers[WAR_START]);
        updateMinister(m_defenceMinister, m_ministers[DEFENCE_START]);
        updateMinister(m_distributor, m_distributors[DISTRIBUTOR_START]);
    } else
    {
        updateMinister(m_economicMinister, m_ministers[ECONOMIC_START]);
        updateMinister(m_warMinister, m_ministers[WAR_START]);
        updateMinister(m_defenceMinister, m_ministers[DEFENCE_START]);
        updateMinister(m_distributor, m_distributors[DISTRIBUTOR_MORE_WAR]);

    }

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

    m_ministers[ECONOMIC_START] = new StartGameEconomicMinister();
    m_ministers[WAR_START] = new StartGameWarMinister();
    m_ministers[DEFENCE_START] = new StartGameDefenceMinister();

    updateMinister(m_economicMinister, m_ministers[ECONOMIC_START]);
    updateMinister(m_warMinister, m_ministers[WAR_START]);
    updateMinister(m_defenceMinister, m_ministers[DEFENCE_START]);

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
    m_distributors[DISTRIBUTOR_MORE_WAR] = new MoreWarDistributor();
    updateMinister(m_distributor, m_distributors[DISTRIBUTOR_START]);
}

MainFactory::MainFactory()
{
    initMinisters();
    initDistributors();
}
