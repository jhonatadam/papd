#ifndef VARS_H
#define VARS_H

#include <vector>

#include <ilcplex/ilocplex.h>

#include "data.h"

typedef std::vector< std::vector<IloBoolVar> > MatNumVar;

struct Vars
{
    MatNumVar               turmTurm; // Xij
    MatNumVar               turmSlot; // yis
    MatNumVar               profTurm; // zpi
    MatNumVar               profSlot; // wps
    MatNumVar               semeCurs; // ukc
    MatNumVar               turmGrup; // fig
    std::vector<IloBoolVar>     prof; // vp

    Vars (IloEnv &env, Data &data);

    void initMatNumVar(MatNumVar &m, IloEnv &env);
};
#endif // VARS_H
