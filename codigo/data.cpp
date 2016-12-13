#include "data.h"
#include <iostream>
Data::Data(const QString &dbName) : di(dbName)
{
    professores =  di.getTable("professor");
    turmas      = di.getTable("turma");
    cursos      =      di.getTable("curso");
    semestres   =   di.getTable("semestre");
    turnos      =      di.getTable("turno");
    slts        =       di.getTable("slot");
    grupo       = di.getTable("grupo_slot");
    casais      =     di.getTable("casais");
    reunioes    =    di.getTable("reuniao");

}

Data::~Data()
{
    delete professores;
    delete      turmas;
    delete      cursos;
    delete   semestres;
    delete      turnos;
    delete        slts;
    delete       grupo;
    delete      casais;
    delete    reunioes;
}
