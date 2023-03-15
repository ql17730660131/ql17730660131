#include "mytcpsocket.h"
#include <string.h>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>
MyTcpSocket::MyTcpSocket()
{
    m_upLoad=false;
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
    connect(&m_Timer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));
}

MyTcpSocket::~MyTcpSocket()
{
    qDebug()<<"MyTcpSocket被析构";
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    if(!m_upLoad)
    {
        qDebug()<<this->bytesAvailable();
        uint uiPDULen=0;
        this->read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen=uiPDULen-sizeof(PDU);
        PDU* pdu=mkPDU(uiMsgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

        switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            char caName[32]={'\0'};
            char caPwd[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);

            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,REGIST_OK);
                QDir dir;//为用户创建文件夹
                dir.mkdir(QString("D:/c++/qt/wangpanxitong/网盘数据/%1").arg(caName));
            }
            else
            {
                strcpy(respdu->caData,REGIST_FAILED);
            }
            qDebug()<<caName<<" "<<caPwd;
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char caName[32]={'\0'};
            char caPwd[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);

            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,LOGIN_OK);
                m_strName = caName;
            }
            else
            {
                strcpy(respdu->caData,LOGIN_FAILED);
            }
            qDebug()<<caName<<" "<<caPwd;
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret=OpeDB::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size()*32;
            PDU* respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for(int i=0;i<ret.size();i++)
            {
                mempcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).toStdString().size()+1);
                //中文名字显示不乱码

                //            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),
                //                   ret.at(i).size());//显示中文会乱码
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
        {
            int ret=OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
            if(ret==-1)
            {
                strcpy(respdu->caData,SEARCH_USR_NO);
            }
            if(ret==1)
            {
                strcpy(respdu->caData,SEARCH_USR_ONLINE);
            }
            if(ret==0)
            {
                strcpy(respdu->caData,SEARCH_USR_OFFLINE);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char perName[32]={'\0'};
            char Name[32]={'\0'};
            strncpy(perName,pdu->caData,32);
            strncpy(Name,pdu->caData+32,32);
            int ret=OpeDB::getInstance().handleAddFriend(perName,Name);
            PDU* respdu;
            if(-1== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,UNKNOW_ERROR);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;

            }
            else if(0== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,EXISTED_FRIEND);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }
            else if(1== ret)
            {
                MyTcpServer::getInstance().resend(perName,pdu);
            }
            else if(2== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }
            else if(3== ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_NO_EXIST);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            char perName[32]={'\0'};
            char Name[32]={'\0'};
            strncpy(perName,pdu->caData,32);
            strncpy(Name,pdu->caData+32,32);
            int ret=OpeDB::getInstance().handleAgreeAddFriend(perName,Name);
            if(ret)
            {
                PDU* respdu = mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,"添加好友成功");
                MyTcpServer::getInstance().resend(Name,respdu);
                free(respdu);
            }

        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE://有bug 加好友成功后也会调用
        {
            char perName[32]={'\0'};
            char Name[32]={'\0'};
            strncpy(perName,pdu->caData,32);
            strncpy(Name,pdu->caData+32,32);
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            QString str = QString("%1拒绝添加你为好友").arg(perName);
            strcpy(respdu->caData,str.toStdString().c_str());
            MyTcpServer::getInstance().resend(Name,respdu);
            free(respdu);
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char Name[32]={'\0'};
            strncpy(Name,pdu->caData,32);
            QStringList strlist= OpeDB::getInstance().handleFlushFriend(Name);
            uint uiMsgLen=strlist.size()*32;
            PDU* respdu=mkPDU(uiMsgLen);
            respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i=0;i<strlist.size();i++)
            {
                mempcpy((char*)(respdu->caMsg)+i*32,strlist.at(i).toStdString().c_str(),32);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char selfName[32]={'\0'};
            char friendName[32]={'\0'};
            strncpy(selfName,pdu->caData,32);
            strncpy(friendName,pdu->caData+32,32);
            int ret=OpeDB::getInstance().handleDelFriend(selfName,friendName);
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            if(ret)
            {
                QString str = QString("删除好友成功");
                strcpy(respdu->caData,str.toStdString().c_str());
            }
            else
            {
                QString str = QString("删除好友失败");
                strcpy(respdu->caData,str.toStdString().c_str());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            char caPerName[32];
            memcpy(caPerName,pdu->caData+32,32);
            qDebug()<<"私聊对象："<<caPerName;
            MyTcpServer::getInstance().resend(caPerName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            char Name[32];
            memcpy(Name,pdu->caData,32);
            QStringList strOnlineFriend= OpeDB::getInstance().handleFlushFriend(Name);

            QString temp;
            for(int i=0;i<strOnlineFriend.size();i++)
            {
                temp=strOnlineFriend.at(i);
                MyTcpServer::getInstance().resend(temp.toStdString().c_str(),pdu);
            }

            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {
            QDir dir;
            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1").arg((char*)(pdu->caMsg));
            qDebug()<<"路径：:"<<strCurPath;
            bool ret = dir.exists(strCurPath);
            PDU *respdu = NULL;
            if(ret)//目录存在
            {
                char caNewDir[32];
                memcpy(caNewDir,pdu->caData+32,32);
                QString strNewPath =strCurPath+"/"+caNewDir;
                qDebug()<<"新建文件夹路径：:"<<strNewPath;
                if(dir.exists(strNewPath))
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, FILE_NAME_EXIST);
                }
                else
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    if(dir.mkdir(strNewPath))
                    {
                        strcpy(respdu->caData, CREAT_DIR_OK);
                    }
                    else
                    {
                        strcpy(respdu->caData, "创建文件夹失败");
                    }
                }

            }
            else
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData, DIR_NO_EXIST);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        {
            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1").arg((char*)(pdu->caMsg));
            qDebug()<<"刷新路径：:"<<strCurPath;
            QDir dir(strCurPath);
            QFileInfoList  fileInfoList = dir.entryInfoList();

            int fileCount = fileInfoList.size()-2;
            PDU *respdu = mkPDU(sizeof(FileInfo)*fileCount);
            respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo *fileInfo=NULL;
            QString strFileName;
            for(int i=0;i<fileCount;i++)
            {
                fileInfo = (FileInfo*)(respdu->caMsg)+i;
                strFileName = fileInfoList.at(i+2).fileName();
                memcpy(fileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.toStdString().size());
                if(fileInfoList.at(i+2).isDir())
                {
                    fileInfo->iFileType=0;//表示文件夹
                }
                else if(fileInfoList.at(i+2).isFile())
                {
                    fileInfo->iFileType=1;//表示文件
                }
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_OR_FILE_REQUEST:
        {
            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1").arg((char*)(pdu->caMsg));
            qDebug()<<"路径：:"<<strCurPath;

            char caDelDir[32];
            strcpy(caDelDir,pdu->caData);
            QString strPath =strCurPath+"/"+caDelDir;
            qDebug()<<"删除文件夹路径：:"<<strPath;
            QFileInfo fileInfo(strPath);
            bool ret = false;
            if(fileInfo.isDir())
            {
                QDir dir(strPath);
                ret = dir.removeRecursively();
            }
            else if(fileInfo.isFile())//常规文件
            {
                QDir dir;
                ret = dir.remove(strPath);
            }
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_OR_FILE_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,DEL_DIR_OR_FILE_OK);
            }
            else
            {
                strcpy(respdu->caData,DEL_DIR_OR_FILE_FAILURED);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        {
            char oldName[32]={'\0'};
            char newName[32]={'\0'};
            strncpy(oldName,pdu->caData,32);
            strncpy(newName,pdu->caData+32,32);

            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1").arg((char*)(pdu->caMsg));
            QString strOldPath =strCurPath+"/"+oldName;
            QString strNewPath =strCurPath+"/"+newName;
            PDU* respdu = mkPDU(0);
            qDebug()<<"旧路径：:"<<strOldPath;
            qDebug()<<"新路径：:"<<strNewPath;
            respdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            bool ret = false;
            QDir dir;
            ret = dir.rename(strOldPath,strNewPath);
            if(ret)
            {
                strcpy(respdu->caData,RENAME_FILE_OK);
            }
            else
            {
                strcpy(respdu->caData,RENAME_FILE_FAILURED);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
        {
            char dirName[32]={'\0'};
            strncpy(dirName,pdu->caData,32);

            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1").arg((char*)(pdu->caMsg));

            QString strNewPath =strCurPath+"/"+dirName;

            PDU *respdu = NULL;
            QFileInfo fileInfo(strNewPath);
            if(fileInfo.isDir())
            {
                qDebug()<<"路径：:"<<strNewPath;
                qDebug()<<"文件夹";
                QDir dir(strNewPath);
                QFileInfoList  fileInfoList = dir.entryInfoList();

                int fileCount = fileInfoList.size()-2;
                respdu = mkPDU(sizeof(FileInfo)*fileCount);
                respdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                FileInfo *file_Info=NULL;
                QString strFileName;
                for(int i=0;i<fileCount;i++)
                {

                    file_Info = (FileInfo*)(respdu->caMsg)+i;
                    strFileName = fileInfoList.at(i+2).fileName();
                    qDebug()<<"文件"<<i<<":"<<strFileName;
                    memcpy(file_Info->caFileName,strFileName.toStdString().c_str(),strFileName.toStdString().size());
                    if(fileInfoList.at(i+2).isDir())
                    {
                        file_Info->iFileType=0;//表示文件夹
                    }
                    else if(fileInfoList.at(i+2).isFile())
                    {
                        file_Info->iFileType=1;//表示文件
                    }
                }
                qDebug()<<"完好";
            }
            else if(fileInfo.isFile())//常规文件
            {
                qDebug()<<"不是文件夹";
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(respdu->caData,ENTER_DIR_FAILURED);
            }
            qDebug()<<"最后完好";
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        {
            char fileName[32]={'\0'};
            quint32 fileSize;
            sscanf(pdu->caData,"%s %d",fileName,&fileSize);
            qDebug()<<fileName<<" "<<fileSize;

            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1/%2").arg((char*)(pdu->caMsg)).arg(fileName);
            qDebug()<<strCurPath;
            int i=1;
            QString strFileName(fileName);
            QString strpath=QString("%1/").arg((char*)(pdu->caMsg));
            QString strNewPath = strCurPath;

            while(QFile::exists(strNewPath))
            {
                int index = strFileName.lastIndexOf('.');
                QString strRight = strFileName.right(strFileName.size()-index);
                QString strLeft = strFileName.left(index);
                strNewPath ="D:/c++/qt/wangpanxitong/网盘数据/"+strpath+ strLeft+QString::asprintf("%d",i)+strRight;
                qDebug()<<"更新路径为："<<strNewPath;
                i++;
            }
            qDebug()<<strNewPath;
            m_file.setFileName(strNewPath);
            //以写的方式打开文件，若文件不存在，则自动创建文件
            if(m_file.open(QIODevice::WriteOnly))
            {
                m_upLoad = true;
                m_iTotal = fileSize;
                m_iRecved = 0;
            }
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        {
            char fileName[32]={'\0'};

            strcpy(fileName,pdu->caData);
            qDebug()<<fileName;

            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1/%2").arg((char*)(pdu->caMsg)).arg(fileName);
            qDebug()<<strCurPath;
            m_file.setFileName(strCurPath);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;

            QFileInfo fileInfo(strCurPath);
            if(fileInfo.isDir())
            {
                strcpy(respdu->caData,"请先选择一个文件");
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if(fileInfo.isFile())//常规文件
            {
                quint32 fileSize =  fileInfo.size();
                sprintf(respdu->caData,"%s %d",fileName,fileSize);
                qDebug()<<respdu->caData;
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                m_Timer.start(1000);

            }
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        {
            char perName[32]={'\0'};
            char Name[32]={'\0'};
            strcpy(perName,pdu->caData);
            strcpy(Name,pdu->caData+32);
            qDebug()<<"要分享给："<<perName;
            QString strCurPath = "D:/c++/qt/wangpanxitong/网盘数据/"+QString("%1").arg((char*)(pdu->caMsg));
            qDebug()<<strCurPath;


            PDU *respdu = mkPDU(strCurPath.toStdString().size()+1);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(respdu->caData,perName);
            strcpy(respdu->caData+32,Name);
            memcpy(respdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
            MyTcpServer::getInstance().resend(perName,respdu);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_AGGREE:
        {
            char Name[32]={'\0'};
            strcpy(Name,pdu->caData+32);

            QString strShareFilePath = QString("%1").arg((char*)(pdu->caMsg));
            qDebug()<<"分享文件路径："<<strShareFilePath;
            int index = strShareFilePath.lastIndexOf('/');
            QString fileName = strShareFilePath.right(strShareFilePath.size()-index-1);
            qDebug()<<"文件名称："<<fileName;
            QString strRecvPath = "D:/c++/qt/wangpanxitong/网盘数据/"+m_strName+"/"+fileName;
            QString strRecvNewPath = strRecvPath;
            int i =1;
            while(QFile::exists(strRecvNewPath))
            {
                int index = fileName.lastIndexOf('.');
                QString strRight = fileName.right(fileName.size()-index);
                QString strLeft = fileName.left(index);
                strRecvNewPath ="D:/c++/qt/wangpanxitong/网盘数据/"+m_strName+"/"+strLeft+QString::asprintf("%d",i)+strRight;
                qDebug()<<"更新路径为："<<strRecvNewPath;
                i++;
            }

            qDebug()<<"接收文件路径："<<strRecvNewPath;
            QFile::copy(strShareFilePath,strRecvNewPath);


            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData,"对方成功接收文件！");
            MyTcpServer::getInstance().resend(Name,respdu);

            strcpy(respdu->caData,"成功接收文件！");//通知自己接收文件成功，通过客户端刷新进行显示
            write((char*)respdu, respdu->uiPDULen);

            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_REFUSE:
        {
            char Name[32]={'\0'};
            strcpy(Name,pdu->caData+32);
            qDebug()<<"拒绝接受文件";
            qDebug()<<"通知："<<Name;
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData,"对方拒绝接收文件！");
            MyTcpServer::getInstance().resend(Name,respdu);
            free(respdu);
            respdu = NULL;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        {
            int srclen;
            int destlen;
            char fileName[32];
            sscanf(pdu->caData,"%d %d %s",&srclen,&destlen,fileName);
            QString strMoveFileName = QString("%1").arg(fileName);
            qDebug()<<srclen<<' '<<destlen;
            qDebug()<<"文件名："<<strMoveFileName;

            QString strMoveFilePath = QString("%1").arg((char*)(pdu->caMsg));
            QString strMoveFileOldPath = "D:/c++/qt/wangpanxitong/网盘数据/"+strMoveFilePath;

            QString strDestPath = QString("%1").arg((char*)(pdu->caMsg)+srclen+1);
            qDebug()<<"移动文件路径："<<strMoveFilePath;
            qDebug()<<"目标路径："<<strDestPath;
            QString strDestFilePath = "D:/c++/qt/wangpanxitong/网盘数据/"+strDestPath+"/"+fileName;
            QString strDestNewPath = strDestFilePath;
            int i =1;
            while(QFile::exists(strDestNewPath))
            {
                int index = strMoveFileName.lastIndexOf('.');
                QString strRight = strMoveFileName.right(strMoveFileName.size()-index);
                QString strLeft = strMoveFileName.left(index);
                strDestNewPath ="D:/c++/qt/wangpanxitong/网盘数据/"+strDestPath+"/"+strLeft+QString::asprintf("%d",i)+strRight;
                qDebug()<<"更新路径为："<<strDestNewPath;
                i++;
            }
            qDebug()<<"原路径："<<strMoveFileOldPath;
            qDebug()<<"目标路径："<<strDestNewPath;
            bool ret = QFile::rename(strMoveFileOldPath,strDestNewPath);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
            strcpy(respdu->caData,"对方成功接收文件！");
            if(ret)
            {
                strcpy(respdu->caData,"移动文件成功！");
            }
            else
            {
                strcpy(respdu->caData,"移动文件成功！");
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu=NULL;
        //    qDebug()<< caName <<caPwd <<pdu->uiMsgType;
    }
    else
    {
        QByteArray buffer = readAll();
        m_file.write(buffer);
        m_iRecved += buffer.size();
        if(m_iTotal<=m_iRecved)
        {
            m_file.close();
            m_upLoad = false;
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(respdu->caData,"上传文件成功");
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
    }
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

void MyTcpSocket::sendFileToClient()
{
    m_Timer.stop();
    if(m_file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"打开文件，准备发送";
        char *pBuffer = new char[4096];
        quint32 ret =0;
        while(true)
        {
            ret = m_file.read(pBuffer,4096);
            if(ret>0 &&ret<=4096)
            {
                write(pBuffer,ret);
            }
            else
            {
                break;
            }
        }
        delete []pBuffer;
        pBuffer = NULL;
    }

    m_file.close();
}
