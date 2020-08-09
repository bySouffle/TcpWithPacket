#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLineEdit>
#include<QBoxLayout>
#include<QLabel>
#include<QPaintEvent>
#include<QSpinBox>
#include<QPainter>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    QBoxLayout* m_pDstAddrLayout;
    //  4位IP
    QLineEdit * m_pLineIP_1;
    QLineEdit * m_pLineIP_2;
    QLineEdit * m_pLineIP_3;
    QLineEdit * m_pLineIP_4;

    QLabel* m_pDot1;
    QLabel* m_pDot2;
    QLabel* m_pDot3;

    //  port
    QLineEdit * m_pLinePort;


    //  Ip 输入指针
    QLineEdit* m_pNextEdit(QLineEdit* m_pCurEdit);

    //  判断文本输入
    bool isEdit(QObject* object);

    //  返回的IP
    QString get_ip();

    //
    void setText(const QString& text);


    //  初始化目的地址输入框
    void initDstAddr();

    //  初始化IP输入框
    void initIPEdit(QLineEdit* edit);

    //  事件
    void paintEvent(QPaintEvent* event);

    //  事件过滤
    bool eventFilter(QObject* object, QEvent* event);



    //===========Sl=====================

    void sl_ipEdit(const QString& text);


    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
