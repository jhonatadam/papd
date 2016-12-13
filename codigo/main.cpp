#include <iostream>

#include "exporter.h"

//////////////// parametros ////////////////
std::string DB_NAME   = "30%.db";
std::string HTML_NAME = "alocacao";
float       ALPHA     = 1.0f;
int         TIMEOUT   = 3600;

using namespace std;

int id2Index(const Table &table, const QString &id)
{
    int index = 0;

    for (const Row &row : table)
    {
        if (row[0] == id)
            return index;
        else
            index++;
    }

    return index;
}

// minimiza o choque de horarios de turmas com mais alunos em comum
// maximiza a preferencia dos professores
void funcaoObjetivo(IloModel &model, IloEnv &env, Data &data, Vars &vars) throw (IloException)
{
    const int numProfs = int(data.professores->size());
    const int numTurms =      int(data.turmas->size());

    DataInterface &di = data.di;

    IloExpr exprAlun(env);
    for (int i = 0; i < numTurms; i++)
    {
        for (int j = 0; j < numTurms; j++)
        {
            if (i != j)
                exprAlun += di.getParesTurmas(data.turmas->at(i)[0], data.turmas->at(j)[0]) * vars.turmTurm[i][j];
        }
    }

    IloExpr exprProf(env);
    for (int p = 0; p < numProfs; p++)
    {
        for (int i = 0; i < numTurms; i++)
        {
            exprProf += di.getProfPref(data.professores->at(p)[0], data.turmas->at(i)[0]) * vars.profTurm[p][i];
        }
    }

    model.add(IloMinimize(env, ALPHA * exprAlun + (ALPHA - 1) * exprProf));
    exprAlun.end();
    exprProf.end();
}

// limita a carga horaria minima e maxima dos professores
void restricoes1e2(IloModel &model, IloEnv &env, Data &data, Vars &vars) throw (IloException)
{
    const int numProfs = int(data.professores->size());
    const int numTurms =      int(data.turmas->size());

    for (int p = 0; p < numProfs; p++)
    {
        IloExpr expr(env);
        for (int i = 0; i < numTurms; i++)
        {
            expr += data.di.getCreditosDisc(data.turmas->at(i)[0]) * vars.profTurm[p][i];
        }

        // numero de creditos maximo do professor p
        model.add(expr <= data.professores->at(p)[3].toInt());
        // numero de creditos minimo do professor p
        model.add(expr >= data.professores->at(p)[2].toInt());
        expr.end();
    }
}

// limita o numero de professores de uma disciplina
void restricao3(IloModel &model, IloEnv &env, Data &data, Vars &vars) throw (IloException)
{
    const int numProfs = int(data.professores->size());
    const int numTurms =      int(data.turmas->size());

    for (int i = 0; i < numTurms; i++)
    {
        IloExpr expr(env);
        for (int P = 0; P < numProfs; P++)
        {
            expr += vars.profTurm[P][i];
        }
        model.add(expr == data.turmas->at(i)[7].toInt());
        expr.end();
    }
}

void restricao4(IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    const int numProfs = int(data.professores->size());
    const int numTurms =      int(data.turmas->size());

    for (int p = 0; p < numProfs; p++)
    {
        for (int i = 0; i < numTurms; i++)
        {
            for (int j = 0; j < numTurms; j++)
            {
                if (i != j)
                {
                    model.add(vars.profTurm[p][i] +
                              vars.profTurm[p][j] +
                              vars.turmTurm[i][j] <= 2);
                }
            }
        }
    }
}

void restricao5 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &c : *data.cursos)
    {
        for (Row &k : *data.semestres)
        {
            Table *turmas = data.di.getTurmasPorSemCursTipo(k[0], c[0], "1");

            for (Row &t1 : *turmas)
            {
                for (Row &t2 : *turmas)
                {
                    //                    int i = t1[0].toInt() - 1;
                    //                    int j = t2[0].toInt() - 1;
                    int i = id2Index(*data.turmas, t1[0]);
                    int j = id2Index(*data.turmas, t2[0]);

                    if (i != j)
                    {
                        model.add(vars.turmTurm[i][j] == 0);
                    }
                }
            }

            delete turmas;
        }
    }
}

void restricao6 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    const int numTurmas =      int(data.turmas->size());
    const int numSlots  =        int(data.slts->size());
    const int numProfs  = int(data.professores->size());

    for (int i = 0; i < numTurmas; i++)
    {
        for (int s = 0; s < numSlots; s++)
        {
            for (int p = 0; p < numProfs; p++)
            {
                model.add(vars.turmSlot[i][s] + vars.profTurm[p][i] <= vars.profSlot[p][s] + 1);
            }
        }
    }
}

void restricao7 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    const int numTurmas =      int(data.turmas->size());
    const int numSlots  =        int(data.slts->size());
    const int numProfs  = int(data.professores->size());

    for (int p = 0; p < numProfs; p++)
    {
        IloExpr exprSlots(env);
        for (int s = 0; s < numSlots; s++)
        {
            exprSlots += 2 * vars.profSlot[p][s];
        }

        IloExpr exprTurms(env);
        for (int i = 0; i < numTurmas; i++)
        {
            exprTurms += data.di.getCreditosDisc(data.turmas->at(i)[0]) * vars.profTurm[p][i];
        }

        model.add(exprSlots == exprTurms);

        exprSlots.end();
        exprTurms.end();
    }

}

void restricao8 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    //    Table * incmp_slots = di.getTable("incomp_slots");
    //    for (Row &r : *incmp_slots)
    //    {
    //        int s1 = r[0].toInt();
    //        int s2 = r[1].toInt();
    //        int p = r[2].toInt() - 1;
    //        model.add(profSlot[p][s1] + profSlot[p][s2] <= 1);
    //        numRest += 1;
    //    }
}

void restricao9 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    const int numTurmas = int(data.turmas->size());
    const int numSlots  =   int(data.slts->size());

    for (int i = 0; i < numTurmas; i++)
    {
        for (int j = 0; j < numTurmas; j++)
        {
            if (i != j)
            {
                for (int s = 0; s < numSlots; s++)
                {
                    model.add(vars.turmSlot[i][s] + vars.turmSlot[j][s] <= vars.turmTurm[i][j] + 1);
                }
            }
        }
    }
}

void restricao10e11 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &c : *data.cursos)
    {
        Table *slts = data.di.getSlotsPorTurno(c[2]);

        for (Row &k : *data.semestres)
        {
            Table *turmas = data.di.getTurmasPorSemCursTipo(k[0], c[0], "1");
            // talvez fique melhor verificar se há alguma turma para cirar a restricao

            // contando creditos
            int totalCreditos = 0;
            for (Row &t : *turmas)
            {
                totalCreditos += t[3].toInt();
            }

            IloExpr expr(env);
            for (Row &t : *turmas)
            {
                for (Row &s : *slts)
                {
                    int turma = id2Index(*data.turmas, t[0]);
                    int slot  = id2Index(*data.slts, s[0]);

                    expr += vars.turmSlot[turma][slot];
                }
            }

            int semestre = id2Index(*data.semestres, k[0]);
            int curso    = id2Index(*data.cursos, c[0]);

            // restricao 10
            model.add((2 * expr) >= (totalCreditos * vars.semeCurs[semestre][curso]));
            // restricao 11
            model.add( expr >= (1 - vars.semeCurs[semestre][curso]) * 10);

            expr.end();
            delete turmas;
        }

        delete slts;
    }
}

void restricao12 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &c : *data.cursos)
    {
        // slots to turno secundario
        Table *slts = data.di.getSlotsPorTurno(c[3]);

        for (Row &k : *data.semestres)
        {
            Table *turmas = data.di.getTurmasPorSemCursTipo(k[0], c[0], "1");
            // talvez fique melhor verificar se há alguma turma para cirar a restricao

            // contando creditos
            int totalCreditos = 0;
            for (Row &t : *turmas)
            {
                totalCreditos += t[3].toInt();
            }

            IloExpr expr(env);
            for (Row &t : *turmas)
            {
                for (Row &s : *slts)
                {
                    int turma = id2Index(*data.turmas, t[0]);
                    int slot  = id2Index(*data.slts, s[0]);

                    expr += vars.turmSlot[turma][slot];
                }
            }

            int semestre = id2Index(*data.semestres, k[0]);
            int curso    = id2Index(*data.cursos, c[0]);

            model.add((2 * expr) >= (1 - vars.semeCurs[semestre][curso]) * (totalCreditos - 20));

            expr.end();
            delete turmas;
        }

        delete slts;
    }
}

void restricao13 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    const int numSlots = int(data.slts->size());

    for (Row &t : *data.turmas)
    {
        IloExpr expr(env);
        int turma = id2Index(*data.turmas, t[0]);
        int creditos =  data.di.getCreditosDisc(t[0]);

        for (int s = 0; s < numSlots; s++)
        {
            expr += vars.turmSlot[turma][s];
        }

        model.add((2 * expr) == creditos);

        expr.end();
    }
}

void restricao14 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &t : *data.turmas)
    {
        int turma = t[0].toInt() - 1;
        int cred = t[3].toInt();
        Table *grupos = data.di.getGruposPorCreditos(t[3]);

        for (Row &g : *grupos)
        {
            Table *slts = data.di.getSlotsPorGrupo(g[0]);
            IloExpr expr(env);
            int grupo = g[0].toInt() - 1;

            for (Row &s : *slts)
            {
                int slot  = s[0].toInt() - 1;

                expr += vars.turmSlot[turma][slot];
            }

            model.add((2 * expr) >= (cred * vars.turmGrup[turma][grupo]));

            expr.end();
            delete slts;
        }

        delete grupos;
    }
}

void restricao15 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &t : *data.turmas)
    {
        int turma = t[0].toInt() - 1;
        Table *grupos = data.di.getGruposPorCreditos(t[3]);
        IloExpr expr(env);

        for (Row &g : *grupos)
        {
            int grupo = g[0].toInt() - 1;

            expr += vars.turmGrup[turma][grupo];
        }

        model.add(expr == 1);

        expr.end();
        delete grupos;
    }
}

void restricao16 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    int numProfs = int(data.professores->size());
    vector<int> dias = {0, 6, 12, 18, 24};
    vector<int> turnos = {0, 2, 4};

    for (int p = 0; p < numProfs; p++)
    {
        for (int s : dias)
        {
            IloExpr expr(env);

            for (int q : turnos)
            {
                expr += IloMax(vars.profSlot[p][s+q], vars.profSlot[p][s+q+1]);
            }

            model.add(expr <= 2);

            expr.end();
        }
    }
}

void restricao17 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    int numProfs = int(data.professores->size());
    vector<int> dias = {5, 11, 17, 23};

    for (int p = 0; p < numProfs; p++)
    {
        for (int s : dias)
        {
            model.add(vars.profSlot[p][s] + vars.profSlot[p][s+1] <= 1);
        }
    }
}

void restricao18 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    int numProfs = int(data.professores->size());
    vector<int> dias = {5, 11, 17, 23};

    for (int p = 0; p < numProfs; p++)
    {
        model.add(vars.profSlot[p][13] + vars.profSlot[p][14] <= 1);
    }
}

void restricao19e20 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    int numProfs = int(data.professores->size());
    vector<int> slotsSeg = {0,1,2,3,4,5};
    vector<int> slotsSex = {24,25,26,27,28,29};

    for (int p = 0; p < numProfs; p++)
    {
        IloExpr exprSeg(env);
        IloExpr exprSex(env);

        for (int s : slotsSeg)
        {
            exprSeg += vars.profSlot[p][s];
        }

        for (int s : slotsSex)
        {
            exprSex += vars.profSlot[p][s];
        }

        // rest 19
        model.add(exprSeg <= (6 * vars.prof[p]));

        // rest 20
        model.add(exprSex <= (6 * (1 - vars.prof[p])));

        exprSeg.end();
        exprSex.end();
    }
}

// casais
void restricao21 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &c : *data.casais)
    {
        //        int p1 = c[0].toInt() - 1;
        //        int p2 = c[1].toInt() - 1;
        int p1 = id2Index(*data.professores, c[0]);
        int p2 = id2Index(*data.professores, c[1]);

        model.add(vars.prof[p1] == vars.prof[p2]);
    }
}

// pre-requisitos
void restricao22 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &t : *data.turmas)
    {
        Table *pr = data.di.getPreReqPorTurma(t[0]);

        for (Row &r : *pr)
        {

            int i = id2Index(*data.turmas, t[0]);
            int j = id2Index(*data.turmas, r[1]);

            model.add(vars.turmTurm[i][j] == 1);
        }
    }
}

// horarios fixados das disciplinas
void restricao23 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &t : *data.turmas)
    {
        Table *slts = data.di.getSlotsPorTurma(t[0]);
        int turma =  id2Index(*data.turmas, t[0]);

        for (Row &s : *slts)
        {
            int slot = id2Index(*data.slts, s[1]);

            model.add(vars.turmSlot[turma][slot] == 1);
        }

        delete slts;
    }
}

// reunioes
void restricao24 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    for (Row &r : *data.reunioes)
    {
        Table *profs = data.di.getProfPorReuniao(r[0]);
        const int horario  = r[2].toInt() - 1;

        for (Row &p : *profs)
        {
            //            const int prof = p[0].toInt() - 1;

            const int prof = id2Index(*data.professores, p[1]);

            model.add(vars.profSlot[prof][horario] == 0);
        }

        delete profs;
    }
}

// professor turma
void restricao25 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{
    Table * profTurm = data.di.getTable("prof_turm");

    for (Row &i : *profTurm)
    {
        int p = id2Index(*data.professores, i[0]);
        int t = id2Index(*data.turmas, i[1]);

        model.add(vars.profTurm[p][t] == 1);
    }

    delete profTurm;
}

void novaRest1 (IloModel &model, IloEnv &env, Data &data, Vars &vars)
{

    for (Row &t : *data.turmas)
    {
        //        int turma = t[0].toInt() - 1;
        int turma = id2Index(*data.turmas, t[0]);
        int cred = t[3].toInt();

        if (cred == 4)
        {
            Table *grupos = data.di.getGruposPorCreditos(t[3]);

            for (Row &g : *grupos)
            {
                Table *slts = data.di.getSlotsPorGrupo(g[0]);
                int slot_a  = slts->at(0)[0].toInt() - 1;
                int slot_b  = slts->at(1)[0].toInt() - 1;

                model.add(vars.turmSlot[turma][slot_a] == vars.turmSlot[turma][slot_b]);

                delete slts;
            }

            delete grupos;
        }
        else if (cred == 6)
        {
            vector <vector <int> > grupos =
            {
                {1, 7, 12},
                {13, 18, 24},
                {3, 9, 14},
                {15, 20, 26},
                {5, 11, 16},
                {17, 22, 28}
            };

            vector <int> invalidos = {0,6,19,25,2,8,21,27,4,10,23,29};

            for (auto &g : grupos)
            {

                model.add(vars.turmSlot[turma][g[0]] == vars.turmSlot[turma][g[1]]);
                model.add(vars.turmSlot[turma][g[0]] == vars.turmSlot[turma][g[2]]);
            }

            for (int i : invalidos)
            {
                model.add(vars.turmSlot[turma][i] == 0);
            }
        }
    }
}


int main(int argc, char * argv[])
{
    if (argc == 5)
    {
        DB_NAME   = argv[1];
        HTML_NAME = argv[2];
        ALPHA     = stof(argv[3]);
        TIMEOUT   = stoi(argv[4]);
    }

    //////////////// abrindo dados de entrada ////////////////
    Data data(DB_NAME.c_str());

    //////////////// cplex setup ////////////////
    IloEnv           env;
    IloModel  model(env);

    //////////////// criando variaveis de decisao ////////////////
    Vars vars(env, data);

    try
    {

        //////////////// construindo o modelo ////////////////


        funcaoObjetivo (model, env, data, vars);
        restricoes1e2  (model, env, data, vars);
        restricao3     (model, env, data, vars);
        restricao4     (model, env, data, vars);
        restricao5     (model, env, data, vars);
        restricao6     (model, env, data, vars);
        restricao7     (model, env, data, vars);
        restricao8     (model, env, data, vars);
        restricao9     (model, env, data, vars);
        restricao10e11 (model, env, data, vars);
        restricao12    (model, env, data, vars);
        restricao13    (model, env, data, vars);
        //        restricao14    (model, env, data, vars);
        //        restricao15    (model, env, data, vars);
        restricao16    (model, env, data, vars);
        restricao17    (model, env, data, vars);
        restricao18    (model, env, data, vars);
        restricao19e20 (model, env, data, vars);
        restricao21    (model, env, data, vars);
        restricao22    (model, env, data, vars);
        restricao23    (model, env, data, vars);
        restricao24    (model, env, data, vars);
        restricao25    (model, env, data, vars);
        novaRest1      (model, env, data, vars);

        //////////////// criando solver ////////////////
        IloCplex cplex(model);

        cplex.setParam(IloCplex::TiLim, TIMEOUT);

        //////////////// salvando o modelo ////////////////
//        cplex.exportModel("papdmodelo.lp");

        //////////////// resolvendo o modelo ////////////////
        if (!cplex.solve())
        {
            cerr << "Failed to solve PAPD" << endl;
            throw(-1);
        }

        env.out() << "Solution status = " << cplex.getStatus() << endl;
        env.out() << "Solution value = " << cplex.getObjValue() << endl;

        Exporter exporter(cplex, vars, data);

        exporter.exportHTML((HTML_NAME + "_curs.html").c_str());
        exporter.exportProfHTML((HTML_NAME + "_prof.html").c_str());


    }
    catch (IloException& e)
    {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception caught." << endl;
    }

    env.end();

    return 0;
}
