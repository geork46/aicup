#include "MainFactory.h"

#include "DefaultDistributor.h"
#include "DefaultMinistries.h"
#include "DefaultExploringMinister.h"

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
    m_ministers[ECONOMIC_1] = new DefaultEconomicMinister();
    m_ministers[WAR_1] = new DefaultWarMinister();
    m_ministers[DEFENCE_1] = new DefaultDefenceMinister();
    updateMinister(m_economicMinister, m_ministers[ECONOMIC_1]);
    updateMinister(m_warMinister, m_ministers[WAR_1]);
    updateMinister(m_defenceMinister, m_ministers[DEFENCE_1]);
    updateMinister(m_exploringMinister, new DefaultExploringMinister());
}

void MainFactory::initDistributors()
{
    for (int i = 0; i < DISTRIBUTOR_NAME_COUNT; ++i)
    {
        m_distributors[i] = nullptr;
    }
    m_distributors[DISTRIBUTOR_1] = new DefaultDistributor();
    updateMinister(m_distributor, m_distributors[DISTRIBUTOR_1]);
}

MainFactory::MainFactory()
{
    initMinisters();
    initDistributors();
}
