#ifndef DRAWERHOLDER_H
#define DRAWERHOLDER_H

class IDrawer
{
public:
    virtual void drawBuilderUnit(int x, int y, int player);
    virtual void drawBuilderBase(int x, int y, int player);

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
