#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "book.h"
#include <QStackedWidget>
class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget& getInstance();
    Friend *getFriend();
    book *getBook();
signals:

public slots:
private:
    QListWidget* m_pListW;
    Friend * m_pFriend;
    book *m_pBook;
    QStackedWidget *m_pSW;//堆栈窗口
};

#endif // OPEWIDGET_H
