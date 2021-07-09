#include "TM4C123GH6PM.h"
#include "TM4C123.h"
#include <stdio.h>
#include <stdint.h>
/*Function prototype for Timer0A and UART module initialization */

uint32_t measureDistance(void);
void Timer_init(void);
void delayMicroSecond(int time);
void UART_init(void);
void UART5_Transmitter(unsigned char data);
void printstring(char *str);
void Delay(unsigned long counter);

/* global variables to store and display distance in cm */
uint32_t time; /*stores pulse on time */
uint32_t distance; /* stores measured distance value */
char mesg[20];  /* string format of distance value */

/* main code to take distance measurement and send data to UART terminal */
int main(void)
{
SYSCTL->RCGCGPIO |=0x20;
GPIOF->LOCK = 0x4C4F434B;
GPIOF->CR = 0x1F;
GPIOF->DEN = 0x1F;
GPIOF->DIR = 0x0E;
	
Timer_init();  /*initialize Timer0A in edge edge time */
UART_init(); /* initialize UART5 module to transmit data to computer */
	while(1)
	{
		if (distance >= 1 && distance <= 20){
			GPIOF->DATA=0x02;
		}
		else if(distance > 20 && distance <= 40){
			GPIOF->DATA=0x04;
		}
		else if(distance >	40 && distance <= 80){
			GPIOF->DATA=0x08;
		}
		else{GPIOF->DATA=0x00;}
		time = measureDistance(); /* take pulse duration measurement */ 
		distance = ((time * 10625)/10000000)-10; /* convert pulse duration into distance */
		sprintf(mesg, "\r\nDistance = %d cm", distance); /*convert float type distance data into string */
		printstring(mesg); /*transmit data to computer */
		Delay(2000);
	}
	
}

/* This function captures consecutive rising and falling edges of a periodic signal */
/* from Timer Block 0 Timer A and returns the time difference (the period of the signal). */
uint32_t measureDistance(void)
{
    int lastEdge, thisEdge;
	
	  /* Given 10us trigger pulse */
	  GPIOA->DATA &= ~(1<<4); /* make trigger  pin high */
	  delayMicroSecond(10); /*10 microseconds delay */
	  GPIOA->DATA |= (1<<4); /* make trigger  pin high */
	  delayMicroSecond(10); /*10 microseconds delay */
	  GPIOA->DATA &= ~(1<<4); /* make trigger  pin low */

 	while(1)
	{
    TIMER0->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER0->RIS & 4) == 0) ;    /* wait till captured */
	  if(GPIOB->DATA & (1<<6)) /*check if rising edge occurs */
		{
    lastEdge = TIMER0->TAR;     /* save the timestamp */
		/* detect falling edge */
    TIMER0->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER0->RIS & 4) == 0) ;    /* wait till captured */
    thisEdge = TIMER0->TAR;     /* save the timestamp */
		return (thisEdge - lastEdge); /* return the time difference */
		}
	}
}

/* Timer0A initialization function */
/* Initialize Timer0A in input-edge time mode with up-count mode */
void Timer_init(void)
{
    SYSCTL->RCGCTIMER |= 1;     /* enable clock to Timer Block 0 */
    SYSCTL->RCGCGPIO |= 2;      /* enable clock to PORTB */
    
    GPIOB->DIR &= ~0x40;        /* make PB6 an input pin */
    GPIOB->DEN |= 0x40;         /* make PB6 as digital pin */
    GPIOB->AFSEL |= 0x40;       /* use PB6 alternate function */
    GPIOB->PCTL &= ~0x0F000000;  /* configure PB6 for T0CCP0 */
    GPIOB->PCTL |= 0x07000000;
    
	  /* PB2 as a digital output signal to provide trigger signal */
	  SYSCTL->RCGCGPIO |= 1;      /* enable clock to PORTA */
	  GPIOA->DIR |=(1<<4);         /* set PB2 as a digial output pin */
	  GPIOA->DEN |=(1<<4);         /* make PB2 as digital pin */

    TIMER0->CTL &= ~1;          /* disable timer0A during setup */
    TIMER0->CFG = 4;            /* 16-bit timer mode */
    TIMER0->TAMR = 0x17;        /* up-count, edge-time, capture mode */
    TIMER0->CTL |=0x0C;        /* capture the rising edge */
    TIMER0->CTL |= (1<<0);           /* enable timer0A */
}



/* Create one microsecond second delay using Timer block 1 and sub timer A */

void delayMicroSecond(int time)
{
    int i;
    SYSCTL->RCGCTIMER |= 2;     /* enable clock to Timer Block 1 */
    TIMER1->CTL = 0;            /* disable Timer before initialization */
    TIMER1->CFG = 0x04;         /* 16-bit option */ 
    TIMER1->TAMR = 0x02;        /* periodic mode and down-counter */
    TIMER1->TAILR = 16 - 1;     /* TimerA interval load value reg */
    TIMER1->ICR = 0x1;          /* clear the TimerA timeout flag */
    TIMER1->CTL |= 0x01;        /* enable Timer A after initialization */

    for(i = 0; i < time; i++)
    {
        while ((TIMER1->RIS & 0x1) == 0) ;      /* wait for TimerA timeout flag */
        TIMER1->ICR = 0x1;      /* clear the TimerA timeout flag */
    }
}
void UART_init(void)
{
	  SYSCTL->RCGCUART |= 0x20;  /* enable clock to UART5 */
    SYSCTL->RCGCGPIO |= 0x10;  /* enable clock to PORTE for PE4/Rx and RE5/Tx */
    /* UART0 initialization */
    UART5->CTL = 0;         /* UART5 module disbable */
    UART5->IBRD = 104;      /* for 9600 baud rate, integer = 104 */
    UART5->FBRD = 11;       /* for 9600 baud rate, fractional = 11*/
    UART5->CC = 0;          //select system clock/
    UART5->LCRH = 0x60;     /* data lenght 8-bit, not parity bit, no FIFO */
    UART5->CTL = 0x301;     /* Enable UART5 module, Rx and Tx */

    /* UART5 TX5 and RX5 use PE4 and PE5. Configure them digital and enable alternate function */
    GPIOE->DEN = 0x30;      /* set PE4 and PE5 as digital */
    GPIOE->AFSEL = 0x30;    /* Use PE4,PE5 alternate function */
    GPIOE->AMSEL = 0;    /* Turn off analg function*/
    GPIOE->PCTL = 0x00110000;     /* configure PE4 and PE5 for UART */
}
void UART5_Transmitter(unsigned char data)  
{
    while((UART5->FR & (1<<5)) != 0); /* wait until Tx buffer not full */
    UART5->DR = data;                  /* before giving it another byte */
}

void printstring(char *str)
{
  while(*str)
	{
		UART5_Transmitter(*(str++));
	}
}
void Delay(unsigned long counter)
{
	unsigned long i = 0;
	
	for(i=0; i< counter*1000; i++);
}
