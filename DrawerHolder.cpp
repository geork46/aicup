#include "DrawerHolder.h"

#ifdef GUI_TEST
#include "DrawerImpl.h"
#endif

class EmptyDrawer : public IDrawer
{
public:

};

DrawerHolder *DrawerHolder::m_inst = nullptr;

DrawerHolder *DrawerHolder::instance()
{
    if (m_inst == nullptr)
    {
        m_inst = new DrawerHolder();
    }
    return m_inst;
}

DrawerHolder::DrawerHolder()
{
#ifdef GUI_TEST
    m_drawer = new DrawerImpl();
#elif
    m_drawer = new EmptyDrawer();
#endif

}

IDrawer *DrawerHolder::getDrawer()
{
    return m_drawer;
}

void IDrawer::drawPlayerView(const PlayerView &playerView)
{

}

void IDrawer::turningEnd()
{

}

void IDrawer::drawBuilderUnit(int x, int y, int player)
{

}

void IDrawer::drawBuilderBase(int x, int y, int player)
{

}
