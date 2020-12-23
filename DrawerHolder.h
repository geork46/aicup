#ifndef DRAWERHOLDER_H
#define DRAWERHOLDER_H

#include "model/PlayerView.hpp"
#include "model/Model.hpp"

#include "Interfaces.h"

class IDrawer
{
public:
    virtual void drawPlayerView(PlayerView const &playerView);
    virtual void drawAction(PlayerView const &playerView, Action const &action);

    virtual void drawExploringData(PlayerView const &playerView, ExploringData const &data);

    virtual void turningEnd();

};


class DrawerHolder
{
public:
    static DrawerHolder* instance();
    IDrawer *getDrawer();
private:
    DrawerHolder();

    IDrawer *m_drawer = nullptr;
    static DrawerHolder *m_inst;
};

#endif // DRAWERHOLDER_H
