/*************************************************************
UltraSound.h
Engineered and Programmed BY: Cukjati Jacob IV
Description:the header file for ultraSound
*************************************************************/

#ifndef ULTRASOUND_H_
#define ULTRASOUND_H_

#include "Interrupts.h"
#include "values.h"

#define AIR_TEMP 19
#define INCHES_IN_METERS 39.37

uint8_t volatile ULTRASOUND_INIT=0;
uint8_t volatile ultraSoundTime=0;
uint8_t volatile messageSent=0;
uint16_t volatile timeForDistance;
float volatile distance;

/*************************************************************
Description:configures the UltraSound and Initializes it
*************************************************************/
void ultraSound_INIT(){
	ULTRASOUND_INIT=1;
	PORTD_DIRCLR=bit2;
	//ok so we might want to trigger on the falling edge, and once the ultrasound
	//has been activated we can then reset the Pin CTRL to trigger both rising and falling edges.
	PORTD_PIN2CTRL=0x07;  // disable the port sensing, this is because we will allow
							//activate ultrasound to control the sensing of the PORT pin
	set_HIGH_INT();
	PORTD_INTCTRL=0x04;	//set the Mask 1 int on
	PORTD_INT1MASK=bit2;
	ultraSound_Timer_INIT();
}

/*************************************************************
Description:configures the Timer for the ultrasound
*************************************************************/
void ultraSound_Timer_INIT(){
	set_HIGH_INT();
	TCC1_CTRLA=0x01;	//Prescaler CLK 1
	TCC1_CTRLB=0x00;	//normal mode
	TCC1_CTRLC=0x00;	//no CMP for frequency only waveform
	TCC1_CTRLD=0x00;	//NO event capture
	TCC1_CTRLE=0x00;	//Normal mode
	TCC1_INTCTRLA=0x02;
	TCC1_INTCTRLB=0x00;
	TCC1_PER=0xFFFF;	//for about 32 milliseconds
	sei();
}

/*************************************************************
Description:turns off the ultraSound timer
*************************************************************/
void turn_ultraSound_Timer_off(){
	TCC1_CTRLA=0x00;
}

/*************************************************************
Description:turns on the ultraSound timer
*************************************************************/
void turn_ultraSound_Timer_on(){
	TCC1_CTRLA=0x01;	//Prescaler CLK 1
	TCC1_CNT=0;
}

/*************************************************************
Description: Sends a bit to the UltraSound and activates it.
in the meantime we wait for the information to be sent from
the ultrasound.
************************************************************/
void activate_ultra_sound(){
	if (!ULTRASOUND_INIT)
		ultraSound_INIT();
	messageSent=1;
	PORTD_DIRSET=bit2;
	PORTD_OUT=bit2;
	PORTD_DIRCLR=bit2;
	//NOTE:  there is a conflicting issue here because of the OUT put to the pin to activate the ultrasound
	PORTD_PIN2CTRL=0x01;	//then we tell the pin to find that pulse width
}

/*************************************************************
Description:ISR for the ultrasound to start the timer and 
to end the timer and record the distance
*************************************************************/
ISR(PORTD_INT1_vect){
	if (messageSent){
		turn_ultraSound_Timer_on();
		messageSent=0;
		PORTD_PIN2CTRL=0x02;
	}
	else{
		timeForDistance=TCC1_CNT;
		turn_ultraSound_Timer_off();
		float volatile floating=timeForDistance*((float)1/F_CPU);
		distance=(331.5+(0.6*AIR_TEMP))*floating; //  m/s * ( unit * 1/cpu 1/s)
		LCD_get_distance();
	}
}


#endif /* ULTRASOUND_H_ */
