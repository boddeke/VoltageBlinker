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

// contains the 10 bits (start, 8-bit data, stop) to transmit
volatile unsigned short tx_shift_reg;

// send a single character
void uart_tx(char character) {
    
    // all sent? can do this because stop bit (1) is transmitted last
    while(tx_shift_reg);
    
    // setup the 10 bits (start, 8-bit data, stop)
    tx_shift_reg = ((unsigned short )character << 1) | 512;

    // enable the 9600 baud timer
    TCCR0B = (1<<CS00);
}

// send a string
void uart_tx_str(char* string) {
    
    while(*string) {
        
        uart_tx(*string++);
    }
}

// initialize
void uart_init() {
    
   // empty
    tx_shift_reg = 0;
}

// timer0 compare A match interrupt
ISR(TIMER0_COMPA_vect) {
    
    // are we done?
    if(tx_shift_reg==0) {
        
        // disable the timer
        TCCR0B = 0;
        // reset the counter
        TCNT0 = 0;
        
    } else {
        
        // hi or low?
        if(tx_shift_reg & 0x01) {
            
            PORTB |= (1<<PB1);
            
        } else {
            
            PORTB &=~ (1<<PB1);
        }
        
        // next bit
        tx_shift_reg >>= 1;
    }
}
