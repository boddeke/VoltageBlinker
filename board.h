//
//  board.h
//  blinker
//
//  Created by Frank Boddeke on 20220826.
//
#ifndef board_h
#define board_h

// Pinout ATtiny25/45/85
//
//                1 - PB5 PCINT5/nRESET/ADC0/dW
//                2 - PB3 PCINT3/XTAL1/nOC1B/ADC3
//            LED 3 - PB4 PCINT4/XTAL2/CLKO/OC1B/ADC2
//            GND 4 - GND
//
//                5 - PB0 MOSI/DI/SDA/AIN0/OC0A/OC1A/AREF/PCINT0
//             TX 6 - PB1 MISO/DO/AIN1/OC0B/OC1A/PCINT1
//                7 - PB2 SCK/USCK/SCL/ADC1/T0/INT0/PCINT2
//            VCC 8 - VCC

void board_init(void);
void board_dump_voltage(void);

extern volatile unsigned char new_value_signal;

#endif /* board_h */
