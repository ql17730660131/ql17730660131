#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = 0);
    ~ShareFile();
    static ShareFile& getInstance();
    void updateFriend(QListWidget *listWidget);
private slots:
    void on_cancel_pb_clicked();

    void on_ok_pb_clicked();

private:
    Ui::ShareFile *ui;
};

#endif // SHAREFILE_H
