#ifndef _LIBSERIAL_H
#define _LIBSERIAL_H
int UART_Open(char* port);
void UART_Close(int fd);
int UART_Init(int fd, int speed);
int UART_Recv(int fd, char *rcv_buf,int data_len);
int UART_Send(int fd, char *send_buf,int data_len);

#endif
