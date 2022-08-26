//
//  board.c
//  blinker
//
//  Created by Frank Boddeke on 20220826.
//
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "board.h"
#include "uart.h"

#define blink_p 2
#define blink_w 20

volatile unsigned char blink_1;
volatile unsigned char blink_2;
volatile unsigned char blink_state;
volatile unsigned char blink_copy;
volatile unsigned char new_value_signal;


// timer1 compare A match interrupt

ISR(TIMER1_COMPA_vect) {
    
    switch(blink_state) {
            
        case 0:
            // blink_1
            PORTB |= (1<<PB4);
            blink_copy--;
            if(blink_copy==0) {
                blink_state = 1;
                blink_copy = blink_p;
            }
            break;
            
        case 1:
            // "point"
            blink_copy--;
            if(blink_copy==0) {
                if(blink_2==0) {
                    blink_state = 3;
                    blink_copy = blink_w;
                } else {
                    blink_state = 2;
                    blink_copy = blink_2;
                }
            }
            break;
            
        case 2:
            // blink_2
            PORTB |= (1<<PB4);
            blink_copy--;
            if(blink_copy==0) {
                blink_state = 3;
                blink_copy = blink_w;
            }
            break;
            
        case 3:
            // delay
            blink_copy--;
            if(blink_copy==0) {
                blink_state = 0;
                blink_copy = blink_1;
            }
            if(blink_copy==1) {
                ADCSRA |= (1<<ADSC); // Start conversion
            }
            break;
    }
}

ISR(TIMER1_COMPB_vect) {
    PORTB &= ~(1<<PB4);
}

ISR(ADC_vect) {
    
    unsigned int deci_volt = 1023*1.1*10/ADC;
    unsigned int volt = deci_volt/10;
    deci_volt -= volt * 10;
    
    blink_1 = volt;
    blink_2 = deci_volt;
    
    new_value_signal = 1;
}

void board_init(void) {
    
    // Disable interrupts
    cli();
    
    // set TX pin as output
    PORTB |= (1<<PB1);
    DDRB |= (1<<PB1);
    
    // Timer 0 for uart
    TCNT0 = 0;
    // lowering OCR0A below 150 seems to have no effect
    OCR0A = 103; // 1000000 Mhz / (1x 9600 bps ) - 1
    TCCR0A = (1<<WGM01);
    TCCR0B = 0; // will be set by uart code: TCCR0B = (1<<CS00);
    TIMSK |= (1 << OCIE0A);
    TIFR |= (1<<OCF0A);
    
    // Timer 1 for tick
    TCNT1 = 0;
    OCR1A = 5;  // start pulse/on
    OCR1B = 10; // stop pulse/off
    OCR1C = 75; // 1000000 Mhz / (4096x 10 bps ) - 1
    TCCR1 = (1<<CTC1)|(1<<CS13)|(1<<CS12)|(1<<CS10);
    TIMSK |= (1 << OCIE1A)|(1 << OCIE1B);
    TIFR |= (1<<OCF1A)|(1<<OCF1B);
    
    // LED
    PORTB |= (1<<PB4);
    DDRB |= (1<<PB4);
    
    // BLinker
    blink_state = 0;
    blink_copy = 1;
    blink_1 = 1;
    blink_2 = 1;
    new_value_signal = 0;
    
    // ADC
    ADMUX = (1<<MUX3) | (1<<MUX2); // Read 1.1V reference against AVcc
    ADCSRA = (1<<ADEN)  | (1<<ADIE) | (1<<ADPS2); // Enable, enable interrupt, /16
    ADCSRA |= (1<<ADIF); // clear interrupt
    
    // Enable interrupts
    sei();
}
