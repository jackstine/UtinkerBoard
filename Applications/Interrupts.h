/*************************************************************
Interrupts.h
NAME: Cukjati Jacob
Section#: 7F31
TA NAME: Kevin French
Description: The header file for Interrupts
*************************************************************/


#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "INIT_CS0_Switches_LED.h"

uint8_t static volatile switchEnable=1;

/*************************************************************
Description: sets the interrupts enable for high interrupts
*************************************************************/
void set_HIGH_INT(){
	PMIC.CTRL|=PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
}

/*************************************************************
Description: INITS the Interrupts to count switches
*************************************************************/
void INTERRUPTS(){
	set_HIGH_INT();
	PORTF.DIRCLR=0x02;		//set pin 2 for inputs for interrupts
	PORTF.INTCTRL=0x02;
	PORTF.INT0MASK=0x02;	//mask the interrupt on pin 2 of PORTF
	PORTF.PIN1CTRL=0x01;	//set the port for sensing rising edge detection
	sei();
}


/*************************************************************
Description: this is the INT vector for PORTF
used to count a switch activity
*************************************************************/
ISR(PORTF_INT0_vect){
	uint8_t static volatile counter=0;
		cli();
		counter++;
		LED(counter);
}

#endif /* INCFILE1_H_ */
