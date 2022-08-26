//
//  board.h
//  blinker
//
//  Created by Frank Boddeke on 20220826.
//
#ifndef board_h
#define board_h

void board_init(void);

extern volatile unsigned char new_value_signal;
extern volatile unsigned char blink_1;
extern volatile unsigned char blink_2;

#endif /* board_h */
