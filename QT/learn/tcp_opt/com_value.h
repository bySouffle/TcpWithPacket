#ifndef COM_VALUE_H
#define COM_VALUE_H

typedef struct packet                                                           //                   |
{                                                                       //      sss             |
    unsigned char m_head [4]    ; // 同步序列 ‘t’ ‘a’ ‘l’ ‘k’            //                   |
    unsigned int  m_seq         ; // 命令序号 可以不管 一直填写0           //                   |
    unsigned int  m_cmd         ; // 命令 --------------------------------------<----------<--
    unsigned int  m_para        ; // 可以不用管
    unsigned int  m_extern_len          ;  // 无附加输入则为0  // 有附加输入则为附加数据长度
    unsigned int  m_extern_sum_check    ;  // 附加数据的校验            【方式 和校验 】
    unsigned int  m_packet_sum_check    ;  // 本结构体前六个字段的校验   【方式 和校验 】
}st_packet;

typedef struct packet_extern_stat
{
    // 0
    unsigned int  m_stat_1 ;
    // 4
    unsigned char rev [ 1024  - 4 ]    ;

}st_extern_stat;

#define def_send_get_stat     0x00000001    //  无附加数据   客户端关闭
#define def_response_get_stat 0x80000001    //  有附加数据   客户端发送附加数据

#define def_error_resend_stat 0x00000000    //  数据错误重新发送
#define def_send_file_done    0x00000010    //  文件传输完成



//#define def_send_file_info     // 发送 文件信息   发送   packet_extern_file_info    结构   ?   |
//#define def_get_file_segment   // 发送 文件段的   发送   packet_extern_file_segment 结构   ?    |
//#define def_send_file_done     // 发送 文件传输完成 无附加数据                                  |
//#define def_send_get_stat      // 发送 获取 状态 命令                                          |
//#define def_response_get_stat  // 响应 获取 状态 命令

#endif // COM_VALUE_H
