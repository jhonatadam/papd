#ifndef DATA_H
#define DATA_H

#include "datainterface.h"

struct Data
{
    DataInterface             di;
    Table           *professores;
    Table                *turmas;
    Table                *cursos;
    Table             *semestres;
    Table                *turnos;
    Table                  *slts;
    Table                 *grupo;
    Table                *casais;
    Table              *reunioes;

    Data  (const QString &dbName);
    ~Data ();
};

#endif // DATA_H
