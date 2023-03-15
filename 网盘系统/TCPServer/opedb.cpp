#include "opedb.h"
#include <QMessageBox>
#include <QDebug>
OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\c++\\qt\\wangpanxitong\\TCPServer\\cloud.db");
    if(m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while (query.next())
        {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug()<<data;
        }
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if(name==NULL || pwd==NULL)
        return false;

    QString data=QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    QSqlQuery query;
    return query.exec(data);//数据库中名字是唯一的，若没有重名，则成功返回true
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(name==NULL || pwd==NULL)
        return false;
    QString data=QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        QString data=QString("update usrInfo set online=1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        QSqlQuery query;
        bool ret= query.exec(data);
        return ret;
    }
    else
    {
        return false;
    }
}

void OpeDB::handleOffline(const char* name)
{
    if(name==NULL)
        return;
    QString data=QString("update usrInfo set online=0 where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline()
{
    QString data=QString("select * from usrInfo where online=1");
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();

    while(query.next())
    {
        result.append(query.value(1).toString());
    }
    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if(name == NULL)
        return -1;
    qDebug()<<"搜索"<<name;
    QString data=QString("select * from usrInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        int ret=query.value(3).toInt();
        QString str=query.value(1).toString();
        qDebug()<<str<<"online="<<ret;
        if(ret == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
        return -1;

}

int OpeDB::handleAddFriend(const char *perName, const char *name)
{
    if(NULL==perName || NULL==name )
        return -1;
    QString data = QString("select * from friendInfo where (id=(select id from usrInfo where name=\'%1\') and friendId = (select id from usrInfo where name=\'%2\')) "
                               "or (id=(select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo where name=\'%4\'))").arg(perName).arg(name).arg(name).arg(perName);
        qDebug() << data;
        QSqlQuery query;
        query.exec(data);
        if (query.next())
        {
            qDebug()<<perName<< name<<"双方已经是好友";
            return 0;  //双方已经是好友
        }
        else
        {
            data = QString("select online from usrInfo where name=\'%1\'").arg(perName);
            QSqlQuery query;
            query.exec(data);
            if (query.next())
            {
                int ret = query.value(0).toInt();
                if (1 == ret)
                {
                    return 1;   //在线
                }
                else if (0 == ret)
                {
                    return 2;  //不在线
                }
                else
                    return -1;//出错
            }
            else
            {
                return 3;   //不存在
            }
        }

}

bool OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    if (NULL == pername || NULL == name)
    {
        return false;
    }
    qDebug()<<pername<<"要添加"<<name;
    QString data = QString("insert into friendInfo(id, friendId) values((select id from usrInfo where name=\'%1\'), (select id from usrInfo where name=\'%2\'))").arg(pername).arg(name);
    QSqlQuery query;
    qDebug()<<data;
    if(query.exec(data))
    {
        qDebug()<<"添加好友成功";
        return true;
    }
    else
    {
        qDebug()<<"添加好友失败";
        return false;
    }
}

QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(name==NULL)
        return strFriendList;
    QString data=QString("select id from friendInfo where friendId=(select id from usrInfo where name=\'%1\')").arg(name);
    QSqlQuery query;
    query.exec(data);

    while (query.next())
    {
        int t=query.value(0).toInt();
        QString data1=QString::asprintf("select name from usrInfo where online=1 and id=%d",t);
        QSqlQuery query1;
        query1.exec(data1);
        while(query1.next())
        {
            QString str= query1.value(0).toString();
            strFriendList.append(str);
        }
    }
    data=QString("select friendId from friendInfo where id=(select id from usrInfo where name=\'%1\')").arg(name);
    query.exec(data);
    while (query.next())
    {
        int t=query.value(0).toInt();
        QString data1=QString::asprintf("select name from usrInfo where online=1 and id=%d",t);
        QSqlQuery query1;
        query1.exec(data1);
        while(query1.next())
        {
            QString str= query1.value(0).toString();
            strFriendList.append(str);
        }
    }
    qDebug()<<name<<"的在线好友有：";
    foreach(auto str,strFriendList)
    {
        qDebug()<<str;
    }
    return strFriendList;
}

bool OpeDB::handleDelFriend(const char *name, const char *friendname)
{
    if (NULL == friendname || NULL == name)
    {
        return false;
    }
    QString data = QString("delete from friendInfo where id=(select id from usrInfo where name=\'%1\') and friendId=(select id from usrInfo where name=\'%2\')").arg(friendname).arg(name);
    QString data1 = QString("delete from friendInfo where id=(select id from usrInfo where name=\'%1\') and friendId=(select id from usrInfo where name=\'%2\')").arg(name).arg(friendname);
    QSqlQuery query,query1;
    int t=query.exec(data);
    int t1=query1.exec(data1);
    if(t && t1)
    {
        qDebug()<<"删除好友成功";
        return true;
    }
    else
    {
        qDebug()<<"删除好友失败";
        return false;
    }
}
