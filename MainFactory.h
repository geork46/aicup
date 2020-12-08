#ifndef MAINFACTORY_H
#define MAINFACTORY_H

#include "Interfaces.h"



class MainFactory
{
public:
    MainFactory* instance();


    IExploringMinistry* getExploringMinister();
    IEconomicsMinistry* getEconomicMinister();
    IWarMinistry* getWarMinister();
    IDefenceMinistry* getDefenceMinister();

private:

    MainFactory();
    static MainFactory *m_inst;
};

#endif // MAINFACTORY_H
