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
    
    // initialize board stuff
    board_init();
    
    // initialize uart
    uart_init();

    // say hello
    uart_tx_str("\r\n\r\nvoltage blinker:\r\n");

    // forever
    while(1) {

        // new voltage measurement
        if(new_value_signal) {
        
            new_value_signal = 0;
            
            // dump value over uart
            board_dump_voltage();
        }
    }
}
