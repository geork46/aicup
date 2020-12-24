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
#else
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

void IDrawer::drawAction(const PlayerView &playerView, const Action &action)
{

}

void IDrawer::drawExploringData(const PlayerView &playerView, const ExploringData &data)
{

}

void IDrawer::turningEnd()
{

}

void IDrawer::selectLayer(int layer)
{

}

void IDrawer::fillColorCell(int x, int y, uint32_t color)
{

}

void IDrawer::drawPolyLine(std::vector<Vec2Int> points)
{

}

void IDrawer::drawPolyLine(std::vector<Vec2Int> points, uint32_t color1, uint32_t color2)
{

}
