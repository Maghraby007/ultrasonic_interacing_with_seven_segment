
/*
 * sensorawy.c
 *
 * Created: 5/1/2023 11:03:52 AM
 * Author : mahmo
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#define  Trigger_pin	PA7	/* Trigger pin */
#define LED_Direction DDRA		/* define LED Direction */
#define LED_PORT PORTA			/* define LED port */
#define blue PC2			/* define LED port */

int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}

int main(void)
{
	int count;
	int distance;
	DDRC=0x02;
	DDRA = 0x07;		/* Make trigger pin as output */
	LED_Direction |= 0xff;		/* define LED port direction is output */
	LED_PORT = 0xff;
	PORTD = 0xFF;		/* Turn on Pull-up */
	sei();			/* Enable global interrupt */
	TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;		/* Set all bit to zero Normal operation */
	
	char array[]={0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111,0b01011101};					/* write hex value for CA display from 0 to 9 */ 
   
    while(1)
    {
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTA |= (1 << Trigger_pin);
		_delay_us(10);
		PORTA &= (~(1 << Trigger_pin));
		
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x41;	/* Capture on rising edge, No prescaler*/
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR & (1 << ICF1)) == 0);/* Wait for rising edge */
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */

		while ((TIFR & (1 << ICF1)) == 0);/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		/* 8MHz Timer freq, sound speed =343 m/s */
		distance = (double)count / 466.47;
		if(distance<=9)
		{
			LED_PORT = array[distance]; /* write data on to the LED port */
			_delay_ms(250); /* wait for 1 second */
		}
		
		else{
			distance=10;
			LED_PORT = array[distance]; /* write data on to the LED port */
			_delay_ms(250); /* wait for 1 second */
		}
			
    }
}