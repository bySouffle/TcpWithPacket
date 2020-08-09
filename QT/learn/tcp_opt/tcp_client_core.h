#ifndef TCP_CLIENT_CORE_H
#define TCP_CLIENT_CORE_H
#include<QCoreApplication>
#include<QTcpSocket>
#include<QObject>
#include"com_value.h"


class tcp_client_core:public QObject
{
    Q_OBJECT
public:
    explicit tcp_client_core(QObject *parent=0);

    //  客户端连接socket

private:
    QTcpSocket *m_pcliTcpSocket;

    QString m_conServerIP = "127.0.0.1";
    quint16 m_conServerPort = 52323;

    QString extern_data = "nihaoma???ohayo";


    // ============func===============
    void m_tcpClientInit(); //  初始化连接
    int m_tcpClientDealstat(QTcpSocket *m_pcliTcpSocket1);
    void sendData(QTcpSocket* m_pcliTcpSocket1);
    void send_extern_data(QTcpSocket* m_pcliTcpSocket1, QString extern_data);


    //==============sl==================
    void sl_Connected();
    void sl_recvmsg();


};

#endif // TCP_CLIENT_CORE_H
