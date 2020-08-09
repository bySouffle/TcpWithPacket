#include "tcp_core.h"
#include<QLatin1String>
#include<QTime>
int def_send_file_done_s = 0;   //  发送完成
int def_send_get_stat_s = 0; // 发送状态
//int def_response_get_stat_s = 0;    //接收状态

int ex_len = 0; // 额外长度

int def_send_file_info = 0; //  长度
int def_get_file_segment = 0;   // 和校验

int paket_or_ex = 0;

tcp_core::tcp_core(QObject *parent):QObject(parent)
{
    m_tcpServerInit();
}

void tcp_core::m_tcpServerInit()
{
    m_pTcpServer = new QTcpServer(this);

    //  绑定监听
    if(!m_pTcpServer->listen(QHostAddress(m_TcpServerIP),m_TcpServerPort))
    {
        qDebug() << __FILE__ << __LINE__  << __func__   <<  " " ;
        return;
    }
    else
    {
        qDebug()<<"server Start";
    }

    //  等待新连接
    connect(m_pTcpServer,
            &QTcpServer::newConnection,
            this,
            &tcp_core::sl_NewConnection
            );


}



void tcp_core::sl_NewConnection()
{
    m_pTcpSocket_s = m_pTcpServer->nextPendingConnection();

    //  客户端信息
    QHostAddress Cli_Addr = m_pTcpSocket_s->peerAddress();
    quint16 Cli_Port = m_pTcpSocket_s->peerPort();

    qDebug()<<"Client join====> "<<Cli_Addr.toString()<<":"<<Cli_Port;



    //  接收数据
    connect(m_pTcpSocket_s,
            &QTcpSocket::readyRead,
            this,
            &tcp_core::sl_Recv_Dealdata);


}

void tcp_core::sl_Recv_Dealdata()
{
    //  =============== to struct 1 ================
    //    QByteArray recv_buffer; //  接收的数据
    //    struct packet recv_packet;  //  数据结构
    //    QTcpSocket *m_pSenderSkt = (QTcpSocket* )this->sender();
    //    QString packet = m_pSenderSkt->readAll();
    //    if(!recv_buffer.isEmpty())
    //    {
    //        char *src = recv_buffer.data();
    //        memcpy(&recv_packet, src, sizeof (struct packet));

    //    }




    //  ============= to struct 2 ================
    QByteArray recv_buffer;
    packet st_recvData;
    packet *getData;
    //    QString str;

    QTcpSocket *m_pSenderSkt = (QTcpSocket* )this->sender();
    recv_buffer = m_pSenderSkt->readAll();
    //    if(paket_or_ex == 0){
    if(!recv_buffer.isEmpty())
    {
        memset(&st_recvData,0,sizeof(st_recvData));
        getData = (packet*)recv_buffer.data();  // 强转为结构体
    }

    //  读取同步序列
    for(unsigned int i = 0; i<sizeof (st_recvData.m_head); ++i)
    {
        st_recvData.m_head[i] = getData->m_head[i];


    }
    qDebug() << __FILE__ << __LINE__  << __func__   <<  "=====recvClient Data============= ";
    qDebug()<<(char*)(st_recvData.m_head);



    //  读取命令序列号
    st_recvData.m_seq = getData->m_seq;
    //  读取命令
    st_recvData.m_cmd = getData->m_cmd;
    //  读取para
    st_recvData.m_para = getData->m_para;
    //  读取 附加输入长度
    st_recvData.m_extern_len = getData->m_extern_len;
    //  读取 附加输入校验
    st_recvData.m_extern_sum_check = getData->m_extern_sum_check;
    //  读取 本结构前六个字段校验
    st_recvData.m_packet_sum_check = getData->m_packet_sum_check;

    // ============测试=================
    qDebug()<<"recvdata:::head: " << (char* )(st_recvData.m_head)<<"  "
           <<"seq: " << st_recvData.m_cmd<<"  "
          <<"cmd: " << st_recvData.m_para<<"  "
         <<"m_para: " << st_recvData.m_para<<"  "
        <<"m_extern_len: " << st_recvData.m_extern_len<<"  "
       <<"m_extern_sum_check: " << st_recvData.m_extern_sum_check<<"  "
      <<"m_packet_sum_check:"<<st_recvData.m_packet_sum_check<<"  ";
    //==================================

    ex_len = st_recvData.m_extern_len;  //  额外数据长度

    //  ==========process===========
    unsigned int s_recvlen = (size_t)(&(((packet*)0)->m_packet_sum_check)); //  接收到的数据长度
    qDebug() << __FILE__ << __LINE__  << __func__   <<  " " <<s_recvlen<<" "<<st_recvData.m_packet_sum_check;
    // 判断数据是否丢失或错误
    if(s_recvlen != st_recvData.m_packet_sum_check)
    {
        qDebug()<<"数据丢失"<<endl;
        m_recvstat(m_pSenderSkt, def_error_resend_stat);    //  cmd 0000重发
        ex_len = 0;
        return;
    }

    //  响应获取指令 有额外数据
    if(ex_len>0)
    {
        qDebug() << __FILE__ << __LINE__  << __func__   <<  " " << ex_len ;
        //  请求额外数据
        m_recvstat(m_pSenderSkt, def_response_get_stat);

        //  延时接收数据
        QTime dieTime = QTime::currentTime().addSecs(5);
        while (QTime::currentTime()<dieTime)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }

    }
    //  无额外数据
    if(ex_len==0)
    {
        qDebug() << __FILE__ << __LINE__  << __func__   <<  " ex_len ";
        m_recvstat(m_pSenderSkt, def_send_get_stat);
    }

    qDebug() << __FILE__ << __LINE__  << __func__   <<  " expagrecv ";

}

void tcp_core::sl_Recv_ExternData()
{

    QByteArray recv_ex_buffer;
    packet_extern_stat st_ex_recvData;
    packet_extern_stat *get_exData = nullptr;
    //    QString str;

    QTcpSocket *m_pSenderSkt = (QTcpSocket* )this->sender();
    recv_ex_buffer = m_pSenderSkt->readAll();
    if(!recv_ex_buffer.isEmpty())
    {
        memset(&st_ex_recvData,0,ex_len+4);

        get_exData = (packet_extern_stat* )recv_ex_buffer.data();  // 强转为结构体
    }

    st_ex_recvData.m_stat_1 = get_exData->m_stat_1;



    //  读取同步序列
    for(int i = 0; i<ex_len; ++i)
    {
        st_ex_recvData.rev[i] = get_exData->rev[i];

    }
    qDebug()<<(char *)(st_ex_recvData.rev);
    //    QString show_data = QString();
    //    QString m_rhead = st_recvData.m_head;
    // ============测试=================
    qDebug()<<"m_stat_1: " << st_ex_recvData.m_stat_1<<"  "
            <<"data: " << *st_ex_recvData.rev<<"  ";

}

//  =============状态返回===================
void tcp_core::m_recvstat(QTcpSocket* m_pSenderSkt, unsigned int serv_cmd)
{


    QByteArray send_buffer;
    packet st_sendData;



    //  =========sendData==============
    //      head
    QString seq_head = "talk";
    QByteArray ba_head = seq_head.toLatin1();
    for(int i=0;i<4;++i)
    {
        st_sendData.m_head[i]=ba_head[i];

    }
    qDebug()<<(char *)st_sendData.m_head;

    //    st_sendData.m_head[0]='t';
    //      seq
    st_sendData.m_seq = 17;   //    0011
    //      m_cmd
    st_sendData.m_cmd = serv_cmd; //  0021
    //      para
    st_sendData.m_para = 0;    //  0031
    //      exterm len
    st_sendData.m_extern_len = 0;
    //      exterm sumck
    st_sendData.m_extern_sum_check = 0;
    //      pack    sumck
    st_sendData.m_packet_sum_check = (size_t)(&(((packet*)0)->m_packet_sum_check));
    qDebug()<<"packst_sum_check: "<<st_sendData.m_packet_sum_check; //  0018
    //---------------------test--------------------------
    //    st_sendData.m_packet_sum_check = 24;
    //-----------------------------------------------------

    send_buffer.resize(sizeof (packet));
    memcpy(send_buffer.data(),&st_sendData,sizeof (packet));


    qDebug() << __FILE__ << __LINE__  << __func__   <<  "SERVER返回序列 " ;

    // ============测试=================
    qDebug()<<"status:return::head: " << (char*)(st_sendData.m_head)
           <<"seq: " << st_sendData.m_cmd
          <<"cmd: " << st_sendData.m_para
         <<"m_para: " << st_sendData.m_para
        <<"m_extern_len: " << st_sendData.m_extern_len
       <<"m_extern_sum_check: " << st_sendData.m_extern_sum_check
      <<"m_packet_sum_check:"<<st_sendData.m_packet_sum_check;

    //  ============================

    // 回复
    m_pSenderSkt->write(send_buffer);


    qDebug() << __FILE__ << __LINE__  << __func__   <<  " " ;

    //  释放内存

    send_buffer.clear();
    send_buffer.squeeze();
}

void tcp_core::m_recv_extdata(QTcpSocket *m_pSenderSkt)
{


    //==================================
    QByteArray recv_ex_buffer;
    packet_extern_stat st_ex_recvData;
    packet_extern_stat *get_exData = nullptr;
    //    QString str;

    //    QTcpSocket *m_pSenderSkt = (QTcpSocket* )this->sender();
    recv_ex_buffer = m_pSenderSkt->readAll();

    //  获得的数据长度不对，再请求一次
    if(recv_ex_buffer.length()!=ex_len+4)
    {
        m_recvstat(m_pSenderSkt, def_response_get_stat);

        QTime dieTime = QTime::currentTime().addSecs(5);
        while (QTime::currentTime()<dieTime)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }

        m_recv_extdata(m_pSenderSkt);
        return;
    }
    if(!recv_ex_buffer.isEmpty())
    {
        memset(&st_ex_recvData,0,ex_len+4);

        get_exData = (packet_extern_stat*)recv_ex_buffer.data();  // 强转为结构体
    }
    else
    {
        return;
    }

    st_ex_recvData.m_stat_1 = get_exData->m_stat_1;



    //  读取同步序列
    for(int i = 0; i<ex_len; ++i)
    {
        st_ex_recvData.rev[i] = get_exData->rev[i];

    }
    qDebug() << __FILE__ << __LINE__  << __func__   <<  "=========额外的数据========= ";
    qDebug()<<(char *)(st_ex_recvData.rev);

    // ============测试=================
    qDebug()<<"m_stat_1: " << st_ex_recvData.m_stat_1<<"  "
           <<"data: " << st_ex_recvData.rev<<"  ";

    m_recvstat(m_pSenderSkt, def_send_file_done);   //发送完成
    return;
}
