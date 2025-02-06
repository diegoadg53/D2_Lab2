/*
 * LCD_8b.h
 *
 * Created: 1/30/2025 7:09:57 PM
 *  Author: diego
 */ 


#ifndef LCD_8B_H_
#define LCD_8B_H_

#define F_CPU 16000000

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>



void LCD_8b_init(void);

void LCD_command(unsigned char command);

void LCD_Char(unsigned char data);

void LCD_String(const char *str);

void LCD_MoveToFirstLine(void);

void LCD_MoveToSecondLine(void);

void LCD_Clear(void);



#endif /* LCD_8B_H_ */