#include "exporter.h"

Exporter::Exporter(IloCplex &cplex, Vars &vars, Data &data) :
    data(data)
{
    const int numProf = int(data.professores->size());
    const int numTurm = int(data.turmas->size());
    const int numSlot = int(data.slts->size());

    for (int p = 0; p < numProf; p++)
    {
        profTurm.push_back(vector<bool>(numTurm));

        for (int t = 0; t < numTurm; t++)
        {
            profTurm.back()[t] = IloAbs(cplex.getValue(vars.profTurm[p][t]));
        }
    }

    for (int p = 0; p < numProf; p++)
    {
        profSlot.push_back(vector<bool>(numSlot));

        for (int s = 0; s < numSlot; s++)
        {
            profSlot[p][s] = IloAbs(cplex.getValue(vars.profSlot[p][s]));
        }
    }

    for (int t = 0; t < numTurm; t++)
    {
        turmSlot.push_back(vector<bool>(numSlot));

        for (int s = 0; s < numSlot; s++)
        {
            turmSlot.back()[s] = IloAbs(cplex.getValue(vars.turmSlot[t][s]));
        }
    }
}

void Exporter::exportHTML(const QString &fileName)
{
    const int numHorarios = 6;
    const int numDias = 5;

    std::filebuf fb;
    fb.open (fileName.toStdString().c_str(), std::ios::out);
    std::ostream os(&fb);

    os << "<!DOCTYPE html>\n";
    os << "<html>\n";
    os << htmlHeader + "\n";
    os << "<body>\n";

    for (Row &c : *data.cursos)
    {
        for (Row &sem : *data.semestres)
        {
            os << "<table style=\"width: 100%\" border-collapse: \"collapse\">\n";
            // cabeçalho da tabela
            os << "<tr><th colspan= \"7\">";
            os << c[1].toStdString() << " - " << sem[0].toStdString() << "º Semestre";
            os << "</th></tr>\n";
            os << "<tr><th colspan= \"2\">--</th><th>SEGUNDA</th><th>TERÇA</th><th>QUARTA</th><th>QUINTA</th><th>SEXTA</th></tr>\n";

            for (int horario = 0; horario < numHorarios; horario++)
            {
                os << "<tr>\n";
                if ((horario % 2) == 0)
                {
                    if ((horario / 2) == 0)
                        os << "<th rowspan=\"2\">MANHÃ</th>\n";
                    if ((horario / 2) == 1)
                        os << "<th rowspan=\"2\">TARDE</th>\n";
                    if ((horario / 2) == 2)
                        os << "<th rowspan=\"2\">NOITE</th>\n";
                    os << "<th>AB</th>\n";
                }
                else
                {
                    os << "<th>CD</th>\n";
                }

                for (int dia = 0; dia < numDias; dia++)
                {
                    os << "<td>";
                    const int slot = (dia * numHorarios) + horario;

                    for (int turma = 0; turma < int(turmSlot.size()); turma++)
                    {
                        if (turmSlot[turma][slot])
                        {
                            if (data.turmas->at(turma)[4] == c[0] && data.turmas->at(turma)[6] == sem[0])
                            {
                                os << data.turmas->at(turma)[1].toStdString() + "<br>";
                                os << "(";

                                bool temUm = false;
                                for (int prof = 0; prof < int(data.professores->size()); prof++)
                                {
                                    if (profTurm[prof][turma])
                                    {
                                        if (temUm)
                                            os << ", ";

                                        os << data.professores->at(prof)[1].toStdString();

                                        temUm = true;
                                    }
                                }
                                os << ")<br>";

                            }
                        }
                    }

                    os << "</td>\n";
                }

                os << "</tr>\n";
            }

            os << "</table>";
            os << "<br>\n";
        }

        os << "<br><br><br>\n";
    }

    os << "</body>\n";
    os << "</html>\n";

    fb.close();
}

void Exporter::exportProfHTML(const QString &fileName)
{
    const int numHorarios = 6;
    const int numDias = 5;

    std::filebuf fb;
    fb.open (fileName.toStdString().c_str(), std::ios::out);
    std::ostream os(&fb);

    os << "<!DOCTYPE html>\n";
    os << "<html>\n";
    os << htmlHeader + "\n";
    os << "<body>\n";

    for (int p = 0; p < int(data.professores->size()); p++)
    {
        os << "<table style=\"width: 100%\" border-collapse: \"collapse\">\n";
        // cabeçalho da tabela
        os << "<tr><th colspan= \"7\">";
        os << data.professores->at(p)[1].toStdString();
        os << "</th></tr>\n";
        os << "<tr><th colspan= \"2\">--</th><th>SEGUNDA</th><th>TERÇA</th><th>QUARTA</th><th>QUINTA</th><th>SEXTA</th></tr>\n";

        for (int horario = 0; horario < numHorarios; horario++)
        {
            os << "<tr>\n";
            if ((horario % 2) == 0)
            {
                if ((horario / 2) == 0)
                    os << "<th rowspan=\"2\">MANHÃ</th>\n";
                if ((horario / 2) == 1)
                    os << "<th rowspan=\"2\">TARDE</th>\n";
                if ((horario / 2) == 2)
                    os << "<th rowspan=\"2\">NOITE</th>\n";
                os << "<th>AB</th>\n";
            }
            else
            {
                os << "<th>CD</th>\n";
            }

            for (int dia = 0; dia < numDias; dia++)
            {
                os << "<td>";
                const int slot = (dia * numHorarios) + horario;

                if (profSlot[p][slot])
                {
                    os << data.professores->at(p)[1].toStdString() << "<br>";
                    os << "(";

                    for (int t = 0; t < int(data.turmas->size()); t++)
                    {
                        if (profTurm[p][t] && turmSlot[t][slot])
                        {
                            os << data.turmas->at(t)[1].toStdString();
                            break;
                        }
                    }
                    os << ")<br>";
                }

                os << "</td>\n";
            }

            os << "</tr>\n";
        }

        os << "</table>";
        os << "<br><br><br>\n";
    }



    os << "</body>\n";
    os << "</html>\n";

    fb.close();
}
