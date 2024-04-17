/*
 * Lab_4.c
 *
 * Created: 09/04/2024 01:20:33 p. m.
 * Author : Albert V H
 */ 
// LIBRERIAS
# define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
// FUNCIONES
void setup(void);
void timmer0(void);
void mover_pines(void);
void initadc(void);
//VARIABLES
int contador_b;
int contador_c;
int sumar;
int restar;
int transistores;
int contador;
int valores_display_unidades;
int valores_display_decenas;
int alarma;
int leer;
 const int valores_display[16]={0xF9, 0x21, 0xBA, 0xAB, 0x63, 0xCB, 0xDB,0xA1, 0xFB, 0xEB, 0xF3, 0x5B, 0xD8, 0x3B, 0xDA, 0xD2};
// LOOP PRINCIPAL
int main(void)
{
	cli();
	setup();
	timmer0();
	initadc();
	sei();
	sumar=0;
	restar=0;
	contador_c=0;
    while (1) 
    {
	if(leer==10){
		leer=0;
		ADCSRA |= (1<<ADSC); //INICIAR EL ADC LA SECUENCIA
	}
	if((sumar==1) ){
		sumar=0;
		contador++;
		mover_pines();
    }else if((restar==1) ){
		restar=0;
		contador--;
		mover_pines();
	}
	if(((valores_display_decenas<<4)|(valores_display_unidades))==contador){
		alarma=1;
		if (transistores==0)
		{
			PORTD|=(1<<PIND2); //|valores_display_unidades;
		}else if (transistores==1)
		{
			PORTD|=(1<<PIND2); //|valores_display_decenas;
		}
		}else{
		PORTD &=~(1<<PIND2);
		alarma=0;
	}
	
	
}
}
// FUNCIONES E INTERUPCIONES 
void setup(void){
	
	// ACTIVAR LOS PUERTOS D COMO SALIDAS
 	DDRD = 0xFF;
	PORTD =0x00;
	// ACTIVAR LOS PUERTOS B COMO SALIDAS
	DDRB= 0xFF;
	PORTB= 0x00;
	//ACTIVAR LOS PUERTOS 0-3 COMO SALIDAS Y 4- 7 COMO ENTRADAS
	DDRC= 0x0F;
	PORTC=0xF0;
	//ACTIVAR  LAS INTERUPCCIONES DEL PUERTO C
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<< PCINT13)|(1<<PCINT12);
	//DESACTIVAR EL RX Y TX
	UCSR0B = 0x00;
}
void mover_pines(void){
	contador_c = (contador>>4)& 0b1111;
	contador_b=contador<<2;
	if (transistores==0){
		PORTB= contador_b|0x01;
	}else if (transistores==1){
		PORTB=contador_b|0x02;
	}
	PORTC= (contador_c)|0xF0;
}
void timmer0(void){
	TCCR0B |= (1<<CS02)|(1<<CS00);
	TCNT0 = 100;
	TIMSK0 |= (1<<TOV0);
}
void initadc(void){
	// VOLTAJE DE REF ES 5V
	ADMUX = 0,
	ADMUX |= (1<<REFS0);
	ADMUX |= (1<<ADLAR);
	// Entrada 6 de MUX
	ADMUX |=(1<<MUX1)|(1<<MUX2)|(1<<ADLAR);
	//SETEAR LA VIANA DEL ADC
	ADCSRA =0;
	ADCSRA |= (1<<ADEN); //PRENDER EL ADC
	ADCSRA |= (1<<ADIE); // INTERUPCIONES 
	ADCSRA |= (1<<ADPS2) |(1<<ADPS0)|(1<<ADPS1);// PRESCALER DE 128
	
}
//INTERUPCIONES 
ISR(PCINT1_vect){
	if(!(PINC &(1<<PINC4))){  //CUANDO ESTÉ APACHADO FUNCIONA 
		sumar=1;
		restar=0;
		
	}else if(!(PINC &(1<<PINC5))){
		sumar=0;
		restar=1;
	}
}
ISR(TIMER0_OVF_vect){
	TCNT0=100;
	TIFR0 |= (1<<TOV0);
	leer++;
	if (transistores==0){
		transistores++;
		PORTB &=~(1<<PORTB1);
		PORTB |= (1<< PORTB0);
		if (alarma==1)
		{
			PORTD=valores_display[valores_display_unidades]|(1<<PIND2);
			}else{
			PORTD=valores_display[valores_display_unidades];
		}
		
		}else if(transistores==1){
		transistores=0;
		PORTB &=~(1<<PORTB0);
		PORTB |= (1<< PORTB1);
		if (alarma==1)
		{
			PORTD=valores_display[valores_display_decenas]|(1<<PIND2);
			}else{
			PORTD=valores_display[valores_display_decenas];
		}
	}
	}
	
ISR(ADC_vect){
	valores_display_unidades=(ADCH)& 0b1111;
	valores_display_decenas= (ADCH>>4)&0b1111;
	ADCSRA |= (1<<ADIF);
}