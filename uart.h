//
//  uart.h
//  blinker
//
//  Created by Frank Boddeke on 20220826.
//
#ifndef uart_h
#define uart_h

void uart_init(void);
void uart_tx_str(char* string);
void uart_tx(char character);

#endif /* uart_h */
