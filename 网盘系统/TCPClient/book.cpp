#include "book.h"
#include "ui_book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "opewidget.h"
#include "sharefile.h"
book::book(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::book)
{
    ui->setupUi(this);
    m_strEnterDir.clear();

    ui->ReturnPB->setEnabled(false);
    ui->DelDirPB->setEnabled(false);
    ui->RenamePB->setEnabled(false);
    ui->ShareFilePB->setEnabled(false);
    ui->DownLoadPB->setEnabled(false);
    ui->moveFile_pb->setEnabled(false);
    ui->moveFilePath_pb->hide();
    connect(&m_pTimer,SIGNAL(timeout()),this,SLOT(uploadFileData()));
}

book::~book()
{
    delete ui;
}

void book::flushFile()
{
    ui->DelDirPB->setEnabled(false);
    ui->RenamePB->setEnabled(false);
    ui->ShareFilePB->setEnabled(false);
    ui->DownLoadPB->setEnabled(false);
    ui->moveFile_pb->setEnabled(false);

    QString strCurPath = TCPClient::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString book::returnEnterDir()
{
    return m_strEnterDir;
}

void book::updateFileList(const PDU *pdu)
{
    if(pdu == NULL)
        return;
    FileInfo *fileInfo=NULL;
    int count = pdu->uiMsgLen/sizeof(FileInfo);
    QString strFileName;
    ui->BookListW->clear();
    for(int i=0;i<count;i++)
    {
        fileInfo = (FileInfo*)(pdu->caMsg)+i;
        strFileName = fileInfo->caFileName;
        qDebug()<<strFileName<<" 类型："<<fileInfo->iFileType;

        ui->BookListW->addItem(strFileName);
        if(fileInfo->iFileType==0)
        {
            ui->BookListW->item(i)->setIcon(QIcon(":/map/dir.jpg"));
            ui->BookListW->item(i)->setWhatsThis(QString("文件夹"));
        }
        else if(fileInfo->iFileType==1)
        {
            ui->BookListW->item(i)->setIcon(QIcon(":/map/reg.jpg"));
            ui->BookListW->item(i)->setWhatsThis(QString("文件"));
        }
    }
}

QString book::getShareFileName()
{
    return strShareFileName;
}




void book::uploadFileData()
{
    m_pTimer.stop();
    QFile file(m_strUploadFilePath);
    if(file.open(QIODevice::ReadOnly))
    {
        char *pBuffer = new char[4096];
        quint32 ret =0;
        while(true)
        {
            ret = file.read(pBuffer,4096);
            if(ret>0 &&ret<=4096)
            {
                TCPClient::getInstance().getTcpSocket().write(pBuffer,ret);
            }
            else
            {
                break;
            }
        }
        delete []pBuffer;
        pBuffer = NULL;
    }
    else
    {
        QMessageBox::information(this,"","文件上传失败！");
        return;
    }
    file.close();
}

void book::on_CreateDirPB_clicked()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","输入新文件夹名称");
    if(strNewDir.isEmpty())
    {
        QMessageBox::information(this,"新建文件夹","文件夹名称不能为空！");
        return;
    }
    if(strNewDir.toStdString().size()>=32)
    {
        QMessageBox::information(this,"新建文件夹","文件夹名称过长！");
        return;
    }
    QString strName=TCPClient::getInstance().loginName();
    QString strCurPath=TCPClient::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.toStdString().size());
    memcpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.toStdString().size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void book::on_FlushFilePB_clicked()
{
    this->flushFile();
}

void book::on_DelDirPB_clicked()
{
    QString strCurPath=TCPClient::getInstance().curPath();
    if(ui->BookListW->currentItem())
    {
        QString strDelName = ui->BookListW->currentItem()->text();
        PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_OR_FILE_REQUEST;

        memcpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.toStdString().size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
        TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::information(this,"","先选择一个文件夹");
    }
}

void book::on_RenamePB_clicked()
{
    QString strCurPath=TCPClient::getInstance().curPath();
    if(ui->BookListW->currentItem())
    {
        QString strOldName = ui->BookListW->currentItem()->text();
        QString strNewName = QInputDialog::getText(this,"重命名","输入新名称");
        if(strNewName.isEmpty())
        {
            QMessageBox::information(this,"","名称不能为空！");
            return;
        }
        if(strNewName.toStdString().size()>=32)
        {
            QMessageBox::information(this,"","名称过长！");
            return;
        }
        PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_REQUEST;

        memcpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.toStdString().size());
        memcpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.toStdString().size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
        TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::information(this,"重命名文件","先选择一个文件夹");
    }
}

void book::on_BookListW_doubleClicked(const QModelIndex &index)
{
    ui->DelDirPB->setEnabled(false);
    ui->RenamePB->setEnabled(false);
    ui->moveFile_pb->setEnabled(false);
    QString strDirname = index.data().toString();
    qDebug()<<index.data().toString();
    QString strCurPath=TCPClient::getInstance().curPath();
    m_strEnterDir = strDirname;
    PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    memcpy(pdu->caData,strDirname.toStdString().c_str(),strDirname.toStdString().size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    ui->ReturnPB->setEnabled(true);

}

void book::on_ReturnPB_clicked()
{
    QString strCurPath = TCPClient::getInstance().curPath();
    int index=strCurPath.lastIndexOf('/');
    strCurPath.remove(index,strCurPath.size()-index);
    TCPClient::getInstance().setCurPath(strCurPath);
    qDebug()<<"上级目录为："<<strCurPath;

    flushFile();
    if(TCPClient::getInstance().loginName()==TCPClient::getInstance().curPath())
    {
        ui->ReturnPB->setEnabled(false);
    }
}

void book::on_BookListW_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
//    qDebug()<<"item类型："<< item->whatsThis();
    ui->DelDirPB->setEnabled(true);
    ui->RenamePB->setEnabled(true);

    if(!(item->whatsThis() == QString("文件夹")))
    {
        ui->ShareFilePB->setEnabled(true);
        ui->moveFile_pb->setEnabled(true);
        ui->DownLoadPB->setEnabled(true);
    }
    else
    {
        ui->ShareFilePB->setEnabled(false);
        ui->moveFile_pb->setEnabled(false);
        ui->DownLoadPB->setEnabled(false);
    }
}

void book::on_UploadPB_clicked()
{
    QString str = QFileDialog::getOpenFileName();
    m_strUploadFilePath = str;
    qDebug()<<str;
    if(str.isEmpty())
        return;

    int index=str.lastIndexOf('/');
    str.remove(0,index+1);
    qDebug()<<"文件名："<<str;

    QFile file(m_strUploadFilePath);
    quint32 filesize = file.size();
    QString fileInfo = str+" "+ QString::asprintf("%d",filesize);
    qDebug()<<"文件信息："<<fileInfo;

    QString strCurPath = TCPClient::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->caData,fileInfo.toStdString().c_str(),fileInfo.toStdString().size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

    m_pTimer.start(1000);


}

void book::on_DownLoadPB_clicked()
{
    QString strCurPath=TCPClient::getInstance().curPath();
    if(ui->BookListW->currentItem())
    {
        QString strDownLoadName = ui->BookListW->currentItem()->text();

        PDU *pdu=mkPDU(strCurPath.toStdString().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;

        memcpy(pdu->caData,strDownLoadName.toStdString().c_str(),strDownLoadName.toStdString().size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
        TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::information(this,"","先选择一个文件夹");
    }
}

void book::on_ShareFilePB_clicked()
{
    strShareFileName = ui->BookListW->currentItem()->text();
    QListWidget *listwidget = OpeWidget::getInstance().getFriend()->getFriendList();
    ShareFile::getInstance().updateFriend(listwidget);
    ShareFile::getInstance().show();
}

void book::on_moveFile_pb_clicked()
{
    if(ui->BookListW->currentItem())
    {
        QString strCurPath=TCPClient::getInstance().curPath();
        strMoveFileName = ui->BookListW->currentItem()->text();
        strMoveFilePath = strCurPath+"/"+strMoveFileName;
        QMessageBox::information(this,"","在主界面选择路径，然后点击确定");
        ui->moveFilePath_pb->show();
    }
    else
    {
        QMessageBox::information(this,"","先选择一个文件");
    }
}

void book::on_moveFilePath_pb_clicked()
{
    QString strDestPath=TCPClient::getInstance().curPath();
    int srclen = strMoveFilePath.toStdString().size();
    int destlen = strDestPath.toStdString().size();
    PDU *pdu = mkPDU(srclen+destlen+2);
    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    sprintf(pdu->caData,"%d %d %s",srclen,destlen,strMoveFileName.toStdString().c_str());

    memcpy(pdu->caMsg,strMoveFilePath.toStdString().c_str(),srclen);
    //pdu->caMsg是int[]先转换后偏移
    memcpy((char*)(pdu->caMsg)+srclen+1,strDestPath.toStdString().c_str(),destlen);
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

    ui->moveFilePath_pb->hide();
}
