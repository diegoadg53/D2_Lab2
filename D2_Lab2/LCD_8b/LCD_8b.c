/*
 * LCD_8b.c
 *
 * Created: 1/30/2025 7:10:08 PM
 *  Author: diego
 */ 

#include "LCD_8b.h"

#define RS PD2
#define EN PD3

void LCD_command(unsigned char command){
	PORTD &= ~(1 << RS); // RS = 0 (modo comando)
	PORTD = (PORTD & 0x0F) | ((command << 4) & 0xF0);
	PORTB = (PORTB & 0xF0) | ((command >> 4) & 0x0F);
	PORTD |= (1 << EN); // EN = 1
	_delay_us(4);
	PORTD &= ~(1 << EN);
	_delay_us(100);
}

void LCD_8b_init(void){
	DDRD |= 0xFC;
	DDRB |= 0x0F;
	_delay_ms(20);
	
	LCD_command(0x38); // b'0011_1000 Bus de 8 bits, Dos líneas, Caracter de 5x8
	LCD_command(0x0C); // b'0000_1100 Activar visualizador
	LCD_command(0x06); // b'0000_0110 Modo de entrada, incremento por 1, desplazamiento desactivado
	LCD_command(0x01); // b'0000_0001 Borrar display
	_delay_ms(2);
}

void LCD_Char(unsigned char data){
	PORTD |= (1 << RS); // RS = 1 modo dato
	PORTD = (PORTD & 0x0F) | ((data << 4) & 0xF0);
	PORTB = (PORTB & 0xF0) | ((data >> 4) & 0x0F);
	PORTD |= (1 << EN);
	_delay_us(4);
	PORTD &= ~(1 << EN);
	_delay_us(100);
}

void LCD_String(const char *str){
	while (*str) {
		LCD_Char(*str++);
	}
}

void LCD_MoveToFirstLine(void){
	LCD_command(0x80);
}

void LCD_MoveToSecondLine(void){
	LCD_command(0xC0);
}

void LCD_Clear(void){
	LCD_command(0x01);
}