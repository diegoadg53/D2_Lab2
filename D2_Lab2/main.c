/*
 * D2_Lab2.c
 *
 * Created: 1/30/2025 6:07:19 PM
 * Author : diego
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include "LCD_8b/LCD_8b.h"

volatile char bufferRX;
volatile uint8_t bandera_lectura = 0;
volatile uint8_t valor_pot0 = 0;
volatile uint8_t valor_pot1 = 0;
uint8_t valor_contador = 1;
volatile uint8_t alternador_pots = 0;

void setup(void);
void mapear5V(int valor, char *buffer, size_t tamano);
void int_to_string(int valor, char *buffer);

int main(void)
{
    /* Replace with your application code */
	
	setup();
	
	LCD_String(" S1:   S2:  S3:");
	
    while (1) 
    {
		if (bandera_lectura == 1)
		{
			bandera_lectura = 0;
			if (bufferRX == '+')
			{
				if (valor_contador < 20)
				{
					valor_contador += 1;
				}
			} else if (bufferRX == '-')
			{
				if (valor_contador > 0)
				{
					valor_contador -= 1;
				}
			}
		}
		char s1[5];
		mapear5V(valor_pot0, s1, sizeof(s1));
		char s2[5];
		mapear5V(valor_pot1, s2, sizeof(s2));
		char s3[3];
		int_to_string(valor_contador, s3);
		char salida[15];
		char V[] = "V ";
		strcpy(salida, s1);
		strcat(salida, V);
		strcat(salida, s2);
		strcat(salida, V);
		strcat(salida, s3);
		//snprintf(salida, sizeof(salida), "%s%s%s%s%s", s1, V, s2, V, s3);
		LCD_MoveToSecondLine();
		LCD_String(salida);
    }
}

void init_ADC(void){
	ADMUX = 0;
	//Vref = AVCC = 5V
	ADMUX |= (1<<REFS0);
	// Justificado a la izquierda
	ADMUX |= (1<<ADLAR);
	
	ADCSRA = 0;
	// Habilitar ADC
	ADCSRA |= (1<<ADEN);
	// Máscara de interrupción del ADC
	ADCSRA |= (1<<ADIE);
	// Prescaler del ADC a 128
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	ADCSRB = 0;
	
	// El ADC está configurado para correr en modo Single Conversion
	//Iniciar primera conversión
	ADCSRA |= (1<<ADSC);
}

void init_UART57600(void){
	
	// Configurar pines TX y RX
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	//Configurar A modo Fast U2X0 = 1
	UCSR0A = 0;
	UCSR0A |= (1<<U2X0);
	
	//Configurar B Habilitar ISR RX
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0);
	
	// Paso 4: Configurar C Frame: 8 bits datos, no paridad, 1 bit stop
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	
	// Baudrate = 57600
	UBRR0 = 34;
	
}

void setup(void){
	cli();
	DDRC = 0;
	init_UART57600();
	init_ADC();
	LCD_8b_init();
	sei();
}

void float_a_string(float numero, char *buffer) {
	int parte_entera = (int)numero;  // Obtener la parte entera
	int parte_decimal = (int)((numero - parte_entera) * 100);  // Obtener 2 decimales
	
	// Convertir a string manualmente
	int i = 0;
	buffer[i++] = '0' + parte_entera;

	buffer[i++] = '.';  // Agregar punto decimal

	// Convertir parte decimal a string (dos dígitos fijos)
	buffer[i++] = '0' + (parte_decimal / 10);
	buffer[i++] = '0' + (parte_decimal % 10);

	buffer[i] = '\0';  // Terminar string
}


void mapear5V(int valor, char *buffer, size_t tamano){
	float resultado = (valor / 255.0) * 5.0;
	//snprintf(buffer, tamano, "%.2f", resultado);
	//dtostrf(resultado, 4, 2, buffer);
	float_a_string(resultado, buffer);
}

void int_to_string(int valor, char *buffer){
	if (valor < 10){
		buffer[0] = '0';
		buffer[1] = '0' + valor;
		buffer[2] = '\0';
	} else {
		buffer[0] = '0' + (valor / 10);
		buffer[1] = '0' + (valor % 10);
		buffer[2] = '\0';
	}
}

ISR(ADC_vect){
	ADCSRA &= ~(1<<ADEN);
	if (alternador_pots==0)
	{
		
		ADMUX |= (1<<MUX0);
		valor_pot0 = ADCH;
		alternador_pots += 1;
	}
	else if (alternador_pots == 1)
	{
		
		ADMUX &= ~(1<<MUX0);
		valor_pot1 = ADCH;
		alternador_pots = 0;
	}
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	ADCSRA |= (1<<ADIF);
}

ISR(USART_RX_vect){
	bandera_lectura = 1;
	bufferRX = UDR0;
}