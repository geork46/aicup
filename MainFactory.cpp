#include "MainFactory.h"

#include "DefaultDistributor.h"

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
}

void MainFactory::initDistributors()
{
    for (int i = 0; i < MINISTER_NAME_COUNT; ++i)
    {
        m_distributors[i] = nullptr;
    }
    m_distributors[DISTRIBUTOR_1] = new DefaultDistributor();
    updateMinister(m_distributor, m_distributors[DISTRIBUTOR_1]);
}

MainFactory::MainFactory()
{
    initMinisters();
}
