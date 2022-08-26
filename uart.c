//
//  uart.c
//  blinker
//
//  Created by Frank Boddeke on 20220826.
//
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

volatile unsigned short tx_shift_reg;

void uart_tx(char character) {
    
    while(tx_shift_reg);
    
    tx_shift_reg = ((unsigned short )character << 1) | 512;

   TCCR0B = (1<<CS00);
}

void uart_tx_str(char* string) {
    
    while( *string ) {
        
        uart_tx( *string++ );
    }
}

void uart_init() {
    
   // empty
    tx_shift_reg = 0;
}

// timer0 compare A match interrupt

ISR(TIMER0_COMPA_vect) {
    
    if(tx_shift_reg==0) {
       TCCR0B = 0;
       TCNT0 = 0;
    } else {
       if(tx_shift_reg & 0x01) {
           PORTB |= (1<<PB1);
       } else {
           PORTB &=~ (1<<PB1);
       }
       tx_shift_reg >>= 1;
    }
}
