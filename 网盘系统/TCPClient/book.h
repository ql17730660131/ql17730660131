#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include "protocol.h"
#include <QListWidgetItem>
#include <QTimer>
namespace Ui {
class book;
}

class book : public QWidget
{
    Q_OBJECT

public:
    explicit book(QWidget *parent = 0);
    ~book();

    void flushFile();
    void clearEnterDir();
    QString returnEnterDir();

    void updateFileList(const PDU *pdu);

    QString getShareFileName();

public slots:
    void uploadFileData();
private slots:
    void on_CreateDirPB_clicked();

    void on_FlushFilePB_clicked();

    void on_DelDirPB_clicked();

    void on_RenamePB_clicked();

    void on_BookListW_doubleClicked(const QModelIndex &index);

    void on_ReturnPB_clicked();

    void on_BookListW_itemClicked(QListWidgetItem *item);

    void on_UploadPB_clicked();

    void on_DownLoadPB_clicked();

    void on_ShareFilePB_clicked();

    void on_moveFile_pb_clicked();

    void on_moveFilePath_pb_clicked();

private:
    Ui::book *ui;

    QString m_strEnterDir;
    QString m_strUploadFilePath;

    QTimer m_pTimer;

    QString strShareFileName;
    QString strMoveFilePath;
    QString strMoveFileName;
};

#endif // BOOK_H
