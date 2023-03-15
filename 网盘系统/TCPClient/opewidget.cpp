#include "opewidget.h"
#include "tcpclient.h"
#include <QFont>
OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent)
{
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");
    QFont font;
    font.setPointSize(14);
    m_pListW->setFont(font);
    m_pListW->setMaximumWidth(150);

    m_pFriend = new Friend;
    m_pBook = new book;

    m_pSW=new QStackedWidget(this);
    m_pSW->addWidget(m_pFriend);//默认显示第一个
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain=new QHBoxLayout(this);
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(m_pListW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
    this->setWindowTitle(TCPClient::getInstance().loginName());
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;

}

Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}

book *OpeWidget::getBook()
{
    return m_pBook;
}
