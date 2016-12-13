#include "vars.h"

Vars::Vars(IloEnv &env, Data &data) :
    turmTurm(data.turmas->size(),      std::vector<IloBoolVar>(data.turmas->size())),
    turmSlot(data.turmas->size(),        std::vector<IloBoolVar>(data.slts->size())),
    profTurm(data.professores->size(), std::vector<IloBoolVar>(data.turmas->size())),
    profSlot(data.professores->size(),   std::vector<IloBoolVar>(data.slts->size())),
    semeCurs(data.semestres->size(),   std::vector<IloBoolVar>(data.cursos->size())),
    turmGrup(data.turmas->size(),       std::vector<IloBoolVar>(data.grupo->size())),
    prof    (data.professores->size())
{
    initMatNumVar(turmTurm, env);
    initMatNumVar(turmSlot, env);
    initMatNumVar(profTurm, env);
    initMatNumVar(profSlot, env);
    initMatNumVar(semeCurs, env);
    initMatNumVar(turmGrup, env);

    for (auto &i : prof)
        i = IloBoolVar(env);
}

void Vars::initMatNumVar(MatNumVar &m, IloEnv &env)
{
    for (auto &i : m)
        for (auto &j : i)
            j = IloBoolVar(env);
}
