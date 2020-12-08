#include "MainFactory.h"

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

MainFactory::MainFactory()
{
}
