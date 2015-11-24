#ifndef COM_UART_H
#define COM_UART_H
/********define in uart_cfg.c  and ComUart.c   ***************/

extern	void PrintRegVal(unsigned int* Addr);
extern	void uart_printf(char *fmt,...);
extern	int SendString(unsigned char *s);
extern	int ComUart_Init(void);

extern void COM_UART_Send(unsigned char *pt);
extern unsigned int power(unsigned int n);
extern void COM_UART_Send1(unsigned char * string,unsigned int num);
extern void COM_UART_Send2(unsigned char * string,unsigned int first_num,unsigned int second_num);
#endif
