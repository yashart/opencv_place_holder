#ifndef UART_H
#define UART_H

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
void init_uart_fd(int fd);


#endif // UART_H
