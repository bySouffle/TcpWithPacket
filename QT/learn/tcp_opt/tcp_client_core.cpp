#include "tcp_client_core.h"
int def_send_file_done_c = 0;   //  发送完成
int def_send_get_stat_c = 0; // 发送状态
int def_response_get_stat_c = 0;    //接收状态


tcp_client_core::tcp_client_core(QObject *parent):QObject(parent)
{

    m_tcpClientInit();
}

void tcp_client_core::m_tcpClientInit()
{

    m_pcliTcpSocket = new QTcpSocket(this);
    //  连接
    m_pcliTcpSocket->connectToHost(m_conServerIP,m_conServerPort);

    qDebug() << __FILE__ << __LINE__  << __func__   <<  " " ;
    connect(m_pcliTcpSocket,
            &QTcpSocket::connected,
            this,
            &tcp_client_core::sl_Connected);

}



void tcp_client_core::sl_Connected()
{
    QByteArray c_send_buffer;
    packet c_st_sendData;



    //  =========sendData==============
    //      head
    QString seq_head = "talk";
//    char data[1024];
//    QByteArray ba_head = seq_head.toLatin1();
//    char *s_temp = ba_head.data();
//    memcpy(data,s_temp,ba_head.length());


    qDebug() << __FILE__ << __LINE__  << __func__   <<  "=======Client发送的数据======= ";

    QByteArray ba_head = "talk";
    for(int i=0;i<4;++i)
    {
        c_st_sendData.m_head[i]=ba_head[i];

    }
    qDebug()<<(char *)(c_st_sendData.m_head);




//    st_sendData.m_head[0]='t';
    //      seq
    c_st_sendData.m_seq = 17;   //    0011
    //      m_cmd
    //  ===========
    c_st_sendData.m_cmd = 33; //  0021
    //      para
    c_st_sendData.m_para = 0;    //  0031
    //      exterm len
    c_st_sendData.m_extern_len = 15;         //===========test============
    //      exterm sumck
    c_st_sendData.m_extern_sum_check = 0;
    //      pack    sumck
    c_st_sendData.m_packet_sum_check = (size_t)(&(((packet*)0)->m_packet_sum_check));
    qDebug()<<"packst_sum_check: "<<c_st_sendData.m_packet_sum_check; //  0018
    //---------------------test--------------------------
//    st_sendData.m_packet_sum_check = 24;
    //-----------------------------------------------------

    c_send_buffer.resize(sizeof (packet));
    memcpy(c_send_buffer.data(),&c_st_sendData,sizeof (packet));



    qDebug() << __FILE__ << __LINE__  << __func__   <<  " " ;

    // ============测试=================
    qDebug()<<"client_send_data===========head:" << ( char* )(c_st_sendData.m_head)<<" "
    <<"seq: " << c_st_sendData.m_cmd<<" "
    <<"cmd: " << c_st_sendData.m_para<<" "
    <<"m_para: " << c_st_sendData.m_para<<" "
    <<"m_extern_len: " << c_st_sendData.m_extern_len<<" "
    <<"m_extern_sum_check: " << c_st_sendData.m_extern_sum_check<<" "
    <<"m_packet_sum_check:"<<c_st_sendData.m_packet_sum_check;

    //  ============================

    // 发送
    m_pcliTcpSocket->write(c_send_buffer);
    //  发送完成状态符
    def_send_file_done_c = 1;

    c_send_buffer.clear();
    c_send_buffer.squeeze();

//=====================接收================================

    connect(m_pcliTcpSocket,
            &QTcpSocket::readyRead,
            this,
            &tcp_client_core::sl_recvmsg);
}

void tcp_client_core::sl_recvmsg()
{
    //  获取发送状态
    QTcpSocket *mc_pSenderSkt = (QTcpSocket* )this->sender();
    unsigned int sendstatus = m_tcpClientDealstat(m_pcliTcpSocket);

    //发送失败
    if(sendstatus == def_error_resend_stat)
    {
        sendData(mc_pSenderSkt);
    }

    //无附加请求  断开连接？
    if(sendstatus == def_send_get_stat)
    {
        m_pcliTcpSocket->disconnectFromHost();
    }

    //获得额外数据请求
    if(sendstatus == def_response_get_stat )
    {
        send_extern_data(m_pcliTcpSocket, extern_data);
    }

}


//  处理状态
int tcp_client_core::m_tcpClientDealstat(QTcpSocket *m_pcliTcpSocket)
{
    QByteArray c_recv_buffer;
    packet c_st_recvData;
    packet *c_getData;
//    QString str;

    c_recv_buffer = m_pcliTcpSocket->readAll();
    if(!c_recv_buffer.isEmpty())
    {
        memset(&c_st_recvData,0,sizeof(c_st_recvData));

        c_getData = (packet*)c_recv_buffer.data();  // 强转为结构体
    }

    //  读取同步序列
    for(unsigned int i = 0; i<sizeof (c_st_recvData.m_head); ++i)
    {
        c_st_recvData.m_head[i] = c_getData->m_head[i];

    }

    qDebug() << __FILE__ << __LINE__  << __func__   <<  " ==========客户端读取返回序列==============";

    qDebug()<<(char*)(c_st_recvData.m_head);
    //  读取命令序列号
    c_st_recvData.m_seq = c_getData->m_seq;
    //  读取命令
    c_st_recvData.m_cmd = c_getData->m_cmd;
    //  读取para
    c_st_recvData.m_para = c_getData->m_para;
    //  读取 附加输入长度
    c_st_recvData.m_extern_len = c_getData->m_extern_len;
    //  读取 附加输入校验
    c_st_recvData.m_extern_sum_check = c_getData->m_extern_sum_check;
    //  读取 本结构前六个字段校验
    c_st_recvData.m_packet_sum_check = c_getData->m_packet_sum_check;


//    QString show_data = QString();
//    QString m_rhead = st_recvData.m_head;
    // ============测试=================
    qDebug()<<"head: " << (char *)(c_st_recvData.m_head)<<"  "
    <<"seq: " << c_st_recvData.m_cmd<<"  "
    <<"cmd: " << c_st_recvData.m_para<<"  "
    <<"m_para: " << c_st_recvData.m_para<<"  "
    <<"m_extern_len: " << c_st_recvData.m_extern_len<<"  "
    <<"m_extern_sum_check: " << c_st_recvData.m_extern_sum_check<<"  "
    <<"m_packet_sum_check:"<<c_st_recvData.m_packet_sum_check<<"  ";


    //  释放内存
    free(c_getData);
    c_recv_buffer.clear();
    c_recv_buffer.squeeze();

    //  返回命令值
    return c_st_recvData.m_cmd;
}



void tcp_client_core::sendData(QTcpSocket* m_pcliTcpSocket)
{
    QByteArray c_send_buffer;
    packet c_st_sendData;



    //  =========sendData==============
    //      head
    QString seq_head = "talk";
//    QByteArray ba_head = seq_head.toLatin1();
    QByteArray ba_head = "talk";
    for(int i=0;i<4;++i)
    {
        c_st_sendData.m_head[i]=ba_head[i];

    }
    qDebug() << __FILE__ << __LINE__  << __func__   <<  "======Client重发序列====== ";
    qDebug()<<(char*)c_st_sendData.m_head;

\
//    st_sendData.m_head[0]='t';
    //      seq
    c_st_sendData.m_seq = 17;   //    0011
    //      m_cmd
    c_st_sendData.m_cmd = 33; //  0021
    //      para
    c_st_sendData.m_para = 49;    //  0031
    //      exterm len
    c_st_sendData.m_extern_len = 15;
    //      exterm sumck
    c_st_sendData.m_extern_sum_check = 0;
    //      pack    sumck
    c_st_sendData.m_packet_sum_check = (size_t)(&(((packet*)0)->m_packet_sum_check));
    qDebug()<<"packst_sum_check: "<<c_st_sendData.m_packet_sum_check; //  0018
    //---------------------test--------------------------
//    st_sendData.m_packet_sum_check = 24;
    //-----------------------------------------------------

    c_send_buffer.resize(sizeof (packet));
    memcpy(c_send_buffer.data(),&c_st_sendData,sizeof (packet));



    qDebug() << __FILE__ << __LINE__  << __func__   <<  " " ;

    // ============测试=================
    qDebug()<<"resend    head: " << (char*)(c_st_sendData.m_head)
    <<"seq: " << c_st_sendData.m_cmd
    <<"cmd: " << c_st_sendData.m_para
    <<"m_para: " << c_st_sendData.m_para
    <<"m_extern_len: " << c_st_sendData.m_extern_len
    <<"m_extern_sum_check: " << c_st_sendData.m_extern_sum_check
    <<"m_packet_sum_check:"<<c_st_sendData.m_packet_sum_check;

    //  ============================

    // 发送
    m_pcliTcpSocket->write(c_send_buffer);
    //  发送完成状态符
    def_send_file_done_c = 1;

    c_send_buffer.clear();
    c_send_buffer.squeeze();

}

//  发送额外数据
void tcp_client_core::send_extern_data(QTcpSocket *m_pcliTcpSocket, QString extern_data)
{
    QByteArray c_send_buffer;
    packet_extern_stat c_ex_sendData;



    //  额外数据状态
    c_ex_sendData.m_stat_1=1;
    //  =========sendData==============
    //      head

    QByteArray ext_rev = extern_data.toLatin1();
    char *temp = ext_rev.data();
    memcpy(c_ex_sendData.rev,temp,ext_rev.length());


    //      exterm sumck
    //---------------------test--------------------------
//    st_sendData.m_packet_sum_check = 24;
    //-----------------------------------------------------

    c_send_buffer.resize(ext_rev.length()+4);
    memcpy(c_send_buffer.data(),&c_ex_sendData,ext_rev.length()+4);




    qDebug() << __FILE__ << __LINE__  << __func__   <<  "======发送的额外数据======== ";
    //  结构体转换为QByteA
    // ============测试=================
    qDebug()<<" CLIENT expm_stat: " << c_ex_sendData.m_stat_1
            <<"msg: "<< *c_ex_sendData.rev;

    //  ============================

    // 发送
    m_pcliTcpSocket->write(c_send_buffer);
    //  发送完成状态符
    def_send_file_done_c = 1;

    c_send_buffer.clear();
    c_send_buffer.squeeze();
}
