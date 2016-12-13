#include "datainterface.h"

DataInterface::DataInterface(const QString &fileName)
{
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);
    if(!db.open())
    {
        qDebug() << "aqui";
        throw std::runtime_error("Could not open file");
    }
}

DataInterface::~DataInterface()
{
    db.close();
}

Table *DataInterface::getTable(const QString &table)
{
    QString sql = QString("SELECT * FROM %1").arg(table);
    QSqlQuery query(sql);
    return queryToTable(query);
}

int DataInterface::getParesTurmas(QString &i, QString &j)
{

    QString sql = "SELECT num_alunos FROM pares_turmas "
                  "WHERE disc_a = :i and disc_b = :j";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":i", i);
    query.bindValue(":j", j);
    query.exec();

    if(query.next())
    {
        QString value = query.value(query.record().indexOf("num_alunos")).toString();
        return value.toInt();
    }

    query.prepare(sql);
    query.bindValue(":i", j);
    query.bindValue(":j", i);
    query.exec();

    if(query.next())
    {
        QString value = query.value(query.record().indexOf("num_alunos")).toString();
        return value.toInt();
    }

    return 0;

}

int DataInterface::getCreditosDisc(const QString &turma)
{

    QString sql = "SELECT creditos FROM turma "
                  "WHERE id = :id";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":id", turma);
    query.exec();

    if(query.next())
    {
        QString value = query.value(query.record().indexOf("creditos")).toString();
        return value.toInt();
    }

    return 0;
}

Table *DataInterface::getTurmasPorSemCursTipo(const QString &sem,
        const QString &curso,
        const QString &tipo_id)
{
    QString sql = "SELECT * FROM turma "
                  "WHERE curso_id = :curso and "
                  "semestre_id = :sem and tipo = :tipo";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":curso", curso);
    query.bindValue(":sem", sem);
    query.bindValue(":tipo", tipo_id);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getSlotsPorTurno(const QString &turno)
{
    QString sql = "SELECT * FROM slot "
                  "WHERE turno = :turno";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":turno", turno);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getTurmasPorCursTipo(const QString curso, const QString tipo)
{
    QString sql = "SELECT * FROM turma "
                  "WHERE curso_id = :curso and tipo = :tipo";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":curso", curso);
    query.bindValue(":tipo", tipo);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getGruposPorCreditos(const QString &creditos)
{
    QString sql = "SELECT * FROM grupo_slot "
                  "WHERE creditos = :creditos";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":creditos", creditos);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getSlotsPorGrupo(const QString &grupo)
{
    QString sql = "SELECT * FROM slot_grupo_slot "
                  "WHERE grupo = :grupo";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":grupo", grupo);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getPreReqPorTurma(const QString &turma)
{
    QString sql = "SELECT * FROM pre_requisito "
                  "WHERE disciplina = :turma";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":turma", turma);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getProfPorReuniao(const QString &reuniao)
{
    QString sql = "SELECT * FROM reuniao_professor "
                  "WHERE reuniao = :reuniao";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":reuniao", reuniao);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::getSlotsPorReuniao(const QString &reuniao)
{
    QString sql = "SELECT * FROM reuniao_professor "
                  "WHERE reuniao = :reuniao";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":reuniao", reuniao);
    query.exec();

    return queryToTable(query);
}

float DataInterface::getProfPref(QString &prof, QString &disc)
{

    QString sql = "SELECT valor FROM prof_pref "
                  "WHERE prof_id = :prof and disc_id = :disc";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":prof", prof);
    query.bindValue(":disc", disc);
    query.exec();

    if(query.next())
    {
        QString value = query.value(query.record().indexOf("valor")).toString();
        return value.toFloat();
    }

    return 0.0f;
}

Table *DataInterface::getSlotsPorTurma(const QString &turma)
{
    QString sql = "SELECT * FROM turma_slot "
                  "WHERE turma_id = :turma";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":turma", turma);
    query.exec();

    return queryToTable(query);
}

Table *DataInterface::queryToTable(QSqlQuery &query)
{
    Table *tb = new Table;
    QSqlRecord record = query.record();

    while (query.next())
    {
        tb->push_back(Row());
        for (int i = 0; i < record.count(); i++)
        {
            QString field = record.fieldName(i);
            tb->back().push_back(
                        query.value(record.indexOf(field)).toString());
        }
    }

    return tb;
}

