#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <string.h>
#include "privatechat.h"
#include <QThread>

TCPClient::TCPClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TCPClient)
{
    ui->setupUi(this);
    LoadConfig();
    //连接到服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    m_strDownLoadPath = "D://c++//qt//wangpanxitong//cloudDownload";
    m_downLoad = false;
}

TCPClient::~TCPClient()
{
    delete ui;
}

void TCPClient::LoadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray bstr=file.readAll();
        QString str=bstr.toStdString().c_str();
        //qDebug()<<str;
        file.close();
        str.replace("\r\n"," ");
//        qDebug()<<str;
        QStringList strList=str.split(" ");
//        foreach (str, strList) {
//            qDebug()<<str;
//        }
        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
//        qDebug()<<"ip:"<<m_strIP<<" port:"<<m_usPort;
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

TCPClient &TCPClient::getInstance()
{
    static TCPClient instance;
    return instance;
}

QTcpSocket &TCPClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TCPClient::loginName()
{
    return m_strLoginName;
}

QString TCPClient::curPath()
{
    return m_strCurPath;
}

void TCPClient::setCurPath(QString str)
{
    m_strCurPath = str;
}

QString TCPClient::DownLoadPath()
{
    return m_strDownLoadPath;
}

void TCPClient::showConnect()
{
    QMessageBox::information(this,"连接服务器器","连接服务器成功");
}

void TCPClient::recvMsg()
{
    if(!m_downLoad)
    {
        qDebug()<<m_tcpSocket.bytesAvailable();
        uint uiPDULen=0;
        m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen=uiPDULen-sizeof(PDU);
        PDU* pdu=mkPDU(uiMsgLen);
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

        switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGIST_RESPOND:
        {
            if(0 == strcmp(pdu->caData,REGIST_OK))
            {
                QMessageBox::information(this,"注册",REGIST_OK);
            }
            else if(0 == strcmp(pdu->caData,REGIST_FAILED))
            {
                QMessageBox::information(this,"注册",REGIST_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:
        {
            if(0 == strcmp(pdu->caData,LOGIN_OK))
            {
                m_strCurPath = m_strLoginName;

                //            QMessageBox::information(this,"登录",LOGIN_OK);//会打断文件列表刷新
                OpeWidget::getInstance().show();
                OpeWidget::getInstance().getFriend()->flushFriend();//只能刷新一个出来


//              OpeWidget::getInstance().getBook()->flushFile();//刷新文件列表
                this->hide();
            }
            else if(0 == strcmp(pdu->caData,LOGIN_FAILED))
            {
                QMessageBox::information(this,"登录",LOGIN_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
        {
            OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
        {
            if(0==strcmp(SEARCH_USR_NO,pdu->caData))
            {
                QString strName= OpeWidget::getInstance().getFriend()->m_strSearchName;
                QMessageBox::information(this,"搜索",QString("%1 not exist").arg(strName));
            }
            else if(0==strcmp(SEARCH_USR_ONLINE,pdu->caData))
            {
                QString strName= OpeWidget::getInstance().getFriend()->m_strSearchName;
                QMessageBox::information(this,"搜索",QString("%1 online").arg(strName));
            }
            else if(0==strcmp(SEARCH_USR_OFFLINE,pdu->caData))
            {
                QString strName= OpeWidget::getInstance().getFriend()->m_strSearchName;
                QMessageBox::information(this,"搜索",QString("%1 offline").arg(strName));
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caName[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            int ret = QMessageBox::information(&OpeWidget::getInstance(),"添加好友",QString("%1 want to add you as friend ?").arg(caName)
                                               ,QMessageBox::Yes,QMessageBox::No);
            PDU *respdu=mkPDU(0);
            mempcpy(respdu->caData,pdu->caData,64);
            if(ret == QMessageBox::Yes)
            {
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
            }
            else
            {
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            }
            m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        {
//            QMessageBox::information(&OpeWidget::getInstance(),"添加好友",pdu->caData);
            qDebug()<<pdu->caData;
            if(0 == strcmp(pdu->caData,"添加好友成功"))
            {
                OpeWidget::getInstance().getFriend()->flushFriend();
            }
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
        {
            OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        {
            QMessageBox::information(&OpeWidget::getInstance(),"删除好友",pdu->caData);
            OpeWidget::getInstance().getFriend()->flushFriend();
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            char sendName[32];
            memcpy(sendName,pdu->caData,32);
            if(PrivateChat::getInstance().isHidden())
            {
                PrivateChat::getInstance().show();
                PrivateChat::getInstance().setChatName(sendName);
            }
            PrivateChat::getInstance().updateMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
        {
            OpeWidget::getInstance().getBook()->flushFile();
            //        QMessageBox::information(this,"创建文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
        {
            OpeWidget::getInstance().getBook()->updateFileList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_OR_FILE_RESPOND:
        {
            if(0 == strcmp(pdu->caData,DEL_DIR_OR_FILE_OK))
            {
                OpeWidget::getInstance().getBook()->flushFile();
                //            QMessageBox::information(OpeWidget::getInstance().getBook(),"",DEL_DIR_OR_FILE_OK);

            }
            else if(0 == strcmp(pdu->caData,DEL_DIR_OR_FILE_FAILURED))
            {
                QMessageBox::information(OpeWidget::getInstance().getBook(),"",DEL_DIR_OR_FILE_FAILURED);

            }
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
        {
            if(0 == strcmp(pdu->caData,RENAME_FILE_OK))
            {
                OpeWidget::getInstance().getBook()->flushFile();
                //            QMessageBox::information(OpeWidget::getInstance().getBook(),"",RENAME_FILE_OK);

            }
            else if(0 == strcmp(pdu->caData,RENAME_FILE_FAILURED))
            {
                QMessageBox::information(OpeWidget::getInstance().getBook(),"",RENAME_FILE_FAILURED);

            }
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
        {
            if(0 == strcmp(pdu->caData,ENTER_DIR_FAILURED))
            {
                QMessageBox::information(OpeWidget::getInstance().getBook(),"",ENTER_DIR_FAILURED);
            }
            else
            {
                QString str = OpeWidget::getInstance().getBook()->returnEnterDir();

                m_strCurPath = m_strCurPath+"/"+str;
                qDebug()<<"当前路径更新为："<<m_strCurPath;
                OpeWidget::getInstance().getBook()->updateFileList(pdu);
            }
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
        {
            QMessageBox::information(OpeWidget::getInstance().getBook(),"上传文件",pdu->caData);
            OpeWidget::getInstance().getBook()->flushFile();
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
        {
            if(0 == strcmp(pdu->caData,"请先选择一个文件"))
            {
                QMessageBox::information(OpeWidget::getInstance().getBook(),"",pdu->caData);
            }
            else
            {
                char fileName[32]={'\0'};
                qDebug()<<"要下载的文件信息："<<pdu->caData;
                sscanf(pdu->caData,"%s %d",fileName,&m_iTotal);
                QString strFilePath = m_strDownLoadPath+"//"+fileName;
                qDebug()<<"下载到："<<strFilePath;

                int i=1;
                QString strFileName(fileName);
                qDebug()<<"FileName为："<<strFileName;
                QString strNewPath = strFilePath;
                while(QFile::exists(strNewPath))
                {
                    int index = strFileName.lastIndexOf('.');
                    QString strRight = strFileName.right(strFileName.size()-index);
                    QString strLeft = strFileName.left(index);
                    strNewPath =m_strDownLoadPath+"//"+strLeft+QString::asprintf("%d",i)+strRight;
                    qDebug()<<"更新路径为："<<strNewPath;
                    i++;
                }
                qDebug()<<strNewPath;
                m_file.setFileName(strNewPath);
                //以写的方式打开文件，若文件不存在，则自动创建文件
                if(m_file.open(QIODevice::WriteOnly))
                {
                    m_downLoad = true;
                    m_iRecved = 0;
                }
                else
                {
                    QMessageBox::information(OpeWidget::getInstance().getBook(),
                                             "","未能打开文件，下载失败");
                }
            }

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE:
        {
            char caName[32]={'\0'};
            qDebug()<<"准备接收分享";
            strncpy(caName,pdu->caData+32,32);
            qDebug()<<"由"<<caName<<"分享";
            auto ret = QMessageBox::information(&OpeWidget::getInstance(),"分享文件",QString("%1 分享文件给你，接受分享?").arg(caName)
                                               ,QMessageBox::Yes,QMessageBox::No);
//            switch (ret) {
//            case QMessageBox::Yes:
//                pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE_AGGREE;
//                break;
//            default:
//                pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE_REFUSE;
//                break;
//            }//跟下面写法效果一致
            if(ret == QMessageBox::Yes)
            {
                pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE_AGGREE;
            }
            else
            {
                pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE_REFUSE;
            }
            m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
        {
            QMessageBox::information(OpeWidget::getInstance().getBook(),"分享文件",pdu->caData);
            OpeWidget::getInstance().getBook()->flushFile();
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
        {
            QMessageBox::information(OpeWidget::getInstance().getBook(),"移动文件",pdu->caData);
            OpeWidget::getInstance().getBook()->flushFile();
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QByteArray buffer = m_tcpSocket.readAll();
        m_file.write(buffer);
        m_iRecved += buffer.size();
        if(m_iTotal<=m_iRecved)
        {
            m_file.close();
            m_downLoad = false;
            m_iTotal = 0;
            m_iRecved = 0;
            QMessageBox::information(OpeWidget::getInstance().getBook(),
                                     "","下载完成");
        }
    }

}
#if 0
void TCPClient::on_send_pb_clicked()
{
    QString strMsg=ui->lineEdit->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu=mkPDU(strMsg.size()+1);
        pdu->uiMsgType=8888;
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
        QMessageBox::warning(this,"信息发送","发送的信息不能为空");

}
#endif
void TCPClient::on_login_pb_clicked()
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    m_strLoginName=strName;//保存登录者名字
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败！用户名或密码不能为空");
    }

}

void TCPClient::on_regist_pb_clicked()
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败！用户名或密码不能为空");
    }
}

void TCPClient::on_cancel_pb_clicked()
{

}
