#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include <QtSql>
#include <string>
#include <vector>

typedef std::vector<QString> Row;
typedef std::vector<Row> Table;

class DataInterface
{
public:
    DataInterface(const QString &fileName);
    ~DataInterface();

    ///////////////////////////////////////////////////////
    /// \brief getTable
    /// \param table
    /// \return retorna uma tabela do bd
    ///
    Table * getTable(const QString &table);

    ///////////////////////////////////////////////////////
    /// \brief getParesTurmas
    /// \param i
    /// \param j
    /// \return numero de alunos que podem fazer a
    ///         disc i e j
    ///
    int getParesTurmas(QString &i, QString &j);

    ///////////////////////////////////////////////////////
    /// \brief getCargaHorariaDisc
    /// \param disc
    /// \return carga horaria da disciplinas recebida
    ///
    int getCreditosDisc(const QString &turma);

    ///////////////////////////////////////////////////////
    /// \brief getTurmasPorSemCursTipo
    /// \param sem
    /// \param curso
    /// \param tipo (0 == obrigatoria; 1 == optativa)
    /// \return tabela de turmas (ob ou op) de um semestre
    ///         de um curso
    ///
    Table * getTurmasPorSemCursTipo(const QString &sem, const QString &curso, const QString &tipo_id);

    ///////////////////////////////////////////////////////
    /// \brief getSlotsPorTurno
    /// \param turno
    /// \return tabela de slots de um turno
    ///
    Table * getSlotsPorTurno(const QString &turno);

    ///////////////////////////////////////////////////////
    /// \brief getTurmasPorCursTipo
    /// \param curso
    /// \param tipo
    /// \return tumas de um curso de um topo
    ///
    Table * getTurmasPorCursTipo(const QString curso, const QString tipo);

    ///////////////////////////////////////////////////////
    /// \brief getGruposPorCreditos
    /// \param creditos
    /// \return retorna grupos de slots com o mesmo numero
    ///         de créditos
    ///
    Table * getGruposPorCreditos(const QString &creditos);

    ///////////////////////////////////////////////////////
    /// \brief getSlotPorGrupo
    /// \param grupo
    /// \return slots de um grupo
    ///
    Table * getSlotsPorGrupo(const QString &grupo);

    ///////////////////////////////////////////////////////
    /// \brief getPreReqPorTurma
    /// \param turma
    /// \return pre-requisitos de uma disciplina
    ///
    Table * getPreReqPorTurma(const QString &turma);

    ///////////////////////////////////////////////////////
    /// \brief getProfPorReuniao
    /// \param reuniao
    /// \return professores de um reuniao
    ///
    Table * getProfPorReuniao(const QString &reuniao);

    ///////////////////////////////////////////////////////
    /// \brief getSlotsPorReuniao
    /// \param reuniao
    /// \return slots de um reuniao
    ///
    Table * getSlotsPorReuniao(const QString &reuniao);

    ///////////////////////////////////////////////////////
    /// \brief getProfPref
    /// \param prof
    /// \param disc
    /// \return preferencia de um professor por uma
    ///         disciplina
    ///
    float getProfPref(QString &prof, QString &disc);

    ///////////////////////////////////////////////////////
    /// \brief getSlotsPorTurma
    /// \param turma
    /// \return slots de prefixados de um disciplina
    ///
    Table * getSlotsPorTurma(const QString &turma);

private:
    QSqlDatabase db;

    Table * queryToTable(QSqlQuery &query);

};

#endif // DATAINTERFACE_H
