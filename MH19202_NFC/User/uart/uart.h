
#ifndef MHSMCU_UART
#define MHSMCU_UART
#include "type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>


#define RECV_INT  (BIT0)
#define SEND_INT  (BIT1 | BIT7)


#define UART_SEND_BUF_SIZE      256
#define UART_RECV_BUF_SIZE      256

    typedef struct uart_param_s
    {
        int     tx_transmited:1;        //数据传输结束标记，0表示正在传输数据；1表示传输结束，可以开始传输下一个字节
        int     baudrate;                //串口波特率
        struct {
            int read_index;             //中断发送索引
            int write_index;            //写缓冲区索引
            //u8 send_bytes;            //发送字节数
            //u8 send_int;              //产生发送完成中断的次数
            //u32 cnt;
            volatile int bytes;                 //缓冲区中的字节数
            uint8_t buf[UART_SEND_BUF_SIZE];    //发送缓冲区
        } send;
        struct {
            int read_index;             //读缓冲区索引
            int write_index;            //中断写缓冲区索引
            volatile int bytes;                 //缓冲区中的字节数
            //u32   cnt;            //接收字节数
            //u32   error_bytes;            //偶校验出错字节数
            uint8_t buf[UART_RECV_BUF_SIZE];    //接收缓冲区
            int overflow;           //接收缓冲区溢出标记
        } recv;
    } uart_param;

    void uart_Config(uint32_t baudrate, uint32_t parity);
    void uart_RecvFlush(void);
    int32_t uart_RecvChar(void);
    int32_t uart_IsSendFinish(void);
    int32_t uart_SendChar(uint8_t ch);
    int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len);
    int32_t uart_getbytes(void);
    void uart_ReConfig(uint32_t baudrate, uint32_t parity);
    int32_t uart_SendCharPoll(uint8_t ch);

    unsigned char UART0_GetChar(void);
    void UART0_SendByte(unsigned char DAT);
    uint8_t UART0_GetString(Packet *receivePacket);

#ifdef __cplusplus
}
#endif

#endif

