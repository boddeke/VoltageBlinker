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

// "formating"
#define DECIMALPOINT_WAIT           2
#define INBETWEEN_WAIT             20

volatile unsigned char volt;
volatile unsigned char deci_volt;
volatile unsigned char state;
volatile unsigned char copy;
volatile unsigned char new_value_signal;

// states
#define STATE_BLINK_VOLT            0
#define STATE_DECIMAL_POINT         1
#define STATE_BLINK_DECIVOLT        2
#define STATE_WAIT                  3

// timer1 compare A match interrupt
ISR(TIMER1_COMPA_vect) {
    
    switch(state) {
            
        case STATE_BLINK_VOLT:
            
            // turn led on (is turned off in COMPB)
            PORTB |= (1<<PB4);
            
            copy--;
            if(copy==0) {
                // done, next state
                state = STATE_DECIMAL_POINT;
                copy = DECIMALPOINT_WAIT;
            }
            
            break;
            
        case STATE_DECIMAL_POINT:
            
            // just wait
            copy--;
            if(copy==0) {
                // done, next state
                if(deci_volt==0) {
                    // exception: deci_volt = 0 --> skip that state
                    state = STATE_WAIT;
                    copy = INBETWEEN_WAIT;
                } else {
                    state = STATE_BLINK_DECIVOLT;
                    copy = deci_volt;
                }
            }
            
            break;
            
        case STATE_BLINK_DECIVOLT:
            
            // turn led on (is turned off in COMPB)
            PORTB |= (1<<PB4);
            
            copy--;
            if(copy==0) {
                // done, next state
                state = STATE_WAIT;
                copy = INBETWEEN_WAIT;
            }
            
            break;
            
        case STATE_WAIT:
            
            copy--;
            if(copy==0) {
                // done, next state
                state = STATE_BLINK_VOLT;
                copy = volt;
            }
            
            if(copy==1) {
                
                // start adc conversion somewhere during the wait, close to the end
                ADCSRA |= (1<<ADSC); // Start conversion
            }
            
            break;
    }
}

// turn off led
ISR(TIMER1_COMPB_vect) {
    
    PORTB &= ~(1<<PB4);
}

// conversion complete
ISR(ADC_vect) {
    
    // adc is setup to measure internal 1.1 reference voltage with vcc as reference
    // adc is 10-bit (0 - 1023) and lets keep 1 digit behind the decimal point
    unsigned int voltx10 = 1023*1.1*10/ADC;
    
    // volt
    volt = voltx10/10;
    // digit after the decimal point
    deci_volt = voltx10 - volt * 10;
    
    // indicate new values
    new_value_signal = 1;
}

// board and micro initialization
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
    
    // Blinker
    state = STATE_WAIT;
    copy = INBETWEEN_WAIT;
    volt = 1;
    deci_volt = 0;
    new_value_signal = 0;
    
    // ADC
    ADMUX = (1<<MUX3) | (1<<MUX2); // Read 1.1V reference against AVcc
    ADCSRA = (1<<ADEN)  | (1<<ADIE) | (1<<ADPS2); // Enable, enable interrupt, /16
    ADCSRA |= (1<<ADIF); // clear interrupt
    
    // Enable interrupts
    sei();
}

void board_dump_voltage() {
    
    char str[10];
    
    sprintf(str,"%d.%dv\r\n",(int)volt,(int)deci_volt);
    uart_tx_str(str);
}
