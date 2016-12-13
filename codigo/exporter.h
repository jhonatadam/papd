#ifndef EXPORTER_H
#define EXPORTER_H

#include <fstream>

#include "vars.h"

using namespace std;

class Exporter
{
public:
    Exporter(IloCplex &cplex, Vars &vars, Data &data);

    void exportHTML (const QString &fileName);
    void exportProfHTML (const QString &fileName);

private:
    Data &data;
    vector <vector <bool> > profTurm;
    vector <vector <bool> > profSlot;
    vector <vector <bool> > turmSlot;

    string htmlHeader =
            "<head>"
            "<meta charset=\"UTF-8\">"
            "<style>"
            "table, td, th {"
                "border: 1px solid #ddd;"
                "text-align: left;"
            "}"
            "table {"
                "border-collapse: collapse;"
                "width: 100%;"
            "}"
            "th, td {"
                "padding: 15px;"
            "}"
            "</style>"
            "<title>Alocação UFC-Quixadá</title>"
            "</head>";

};

#endif // EXPORTER_H
