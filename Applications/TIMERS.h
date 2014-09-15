/*************************************************************
TIMERS.h
NAME: Cukjati Jacob
Description: The header file for timers
*************************************************************/


#ifndef TIMERS_H_
#define TIMERS_H_

#include <avr/io.h>

int unsigned static count;
int unsigned static timer;
uint8_t static timerON;
uint8_t led_display=0x00;

/*************************************************************
INPUTS:time
Description: sets a timer for time*0.1ms
*************************************************************/
void timer_INIT(int time){
	set_HIGH_INT();
	TCC1_CTRLA=0x01;	//Prescaler CLK 1
	TCC1_INTCTRLA=0x02;
	TCC1_PER=0x00C8;	//for 0.1 millisecond
	timer=time;	//set timer to time
	count=0;	//set count to 0
	timerON=1;
	LED(led_display);
	sei();
}

/*************************************************************
INPUTS:time
Description: turns on the timer for time*0.1ms
*************************************************************/
void timer_ON(int time){
	timer_INIT(time);
}

/*************************************************************
Description: turns off the timer
*************************************************************/
void timer_OFF(){
	TCC1_CTRLA=0x00;
}


/*************************************************************
Description: INT Vector for the timer, when count==timer
the speaker is turned off and the timer is turned off
************************************************************
ISR(TCC1_OVF_vect){
	if (count==timer){
		PORTQ_DIRTGL=0x0F;
		led_display= ~led_display;
		LED(led_display);
		count=0;
		timerON=0;
	}
	else
	count++;
}*/


#endif /* TIMERS_H_ */
