//
//  main.c
//  blinker
//
//  Created by Frank Boddeke on 20220826.
//
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "board.h"
#include "uart.h"

int main(void) {
    
    char str[10];
    
    board_init();
    
    uart_init();

    uart_tx_str("\r\n\r\nvoltage blinker:\r\n");

    while(1) {

        if(new_value_signal) {
        
            new_value_signal = 0;
            
            sprintf(str,"%d.%dv\r\n",blink_1,blink_2);
            uart_tx_str(str);
        }
    }
}
