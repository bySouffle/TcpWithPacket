#ifndef TCP_CORE_H
#define TCP_CORE_H
#include<QTcpSocket>
#include<QTcpServer>
#include<QObject>
#include<QCoreApplication>
#include"com_value.h"

//---------------------------------------------------------------------------------------------
//#define def_send_file_info     // 发送 文件信息   发送   packet_extern_file_info    结构       |
//#define def_get_file_segment   // 发送 文件段的   发送   packet_extern_file_segment 结构       |
//#define def_send_file_done     // 发送 文件传输完成 无附加数据                                  |
//#define def_send_get_stat      // 发送 获取 状态 命令                                          |
//#define def_response_get_stat  // 响应 获取 状态 命令                                          |
//---------------------------------------------------------------------------------------------


class tcp_core:public QObject

{
    Q_OBJECT
public:
    explicit tcp_core(QObject *parent=0);

    QString m_TcpServerIP = "127.0.0.1";
    quint16 m_TcpServerPort = 52323;

    QTcpSocket *m_pTcpSocket_s; // 服务器socket
    QTcpSocket *m_pTcpSocket_c; // 客户端socket
    QTcpServer *m_pTcpServer;



private:
    //==============FUNC=============
    void m_tcpServerInit(); //  初始化连接

    void m_recvstat(QTcpSocket* m_pSenderSkt, unsigned int serv_cmd);  //  接收数据后返回状态
    void m_recv_extdata(QTcpSocket* m_pSenderSkt);


    QString m_reqServIP = "127.0.0.1";
    quint16 m_reqServPort = 52323;




    //==============SL===============
    void sl_NewConnection();
    void sl_Recv_Dealdata();
    void sl_Recv_ExternData();


};

#endif // TCP_CORE_H
