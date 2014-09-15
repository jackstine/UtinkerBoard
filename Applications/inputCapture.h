/*************************************************************
inputCapture.h
Engineered and Programmed BY: Cukjati Jacob IV
Description:the header file for input capture
*************************************************************/


#ifndef INPUTCAPTURE_H_
#define INPUTCAPTURE_H_

#include <avr/io.h>
#include "Interrupts.h"
#include "INIT_CS1_SRAM_LCD.h"
#include "delay.h"
#include "TIMERS.h"
#include "PezioSpeaker_INIT.h"
#include "values.h"

#define GARBAGE_PULSE 20000
#define IDENTIFY_BUTTON 0x10
#define MIN_KEY_PULSE_LENGTH 40
#define PULSE_END_TIME 0x4E20		//10 ms at 1DIV
#define MINIMUM_KEY_LENGTH 20
#define NUM_OF_IR_KEYS 50
#define LARGE_PULSE 9000
#define MED_PULSE 3000
#define SMALL_PULSE 1000
#define PULSE_ERROR 0.25

//IR_key is the converted to byte size pulse widths of 20 elements
//key is the 16 bit pulses ranging to about 100 elements

struct IR_key volatile IR_KEYS[NUM_OF_IR_KEYS];
uint8_t volatile KEY_TO_READ=NO_BUTTON_PRESSED;
uint8_t volatile READING_KEY=0;
uint8_t volatile READ_COMPLETE=0;
uint8_t	volatile STARTED_READING=0;
uint8_t volatile READING_TO_IDENTIFY=0;
struct key volatile identity;
uint8_t volatile IR_KEYS_SET=0;
struct IR_key volatile key_to_match;

uint16_t volatile points[10];
uint8_t volatile countTrigger=0;
uint8_t volatile triggerInputCapture;

uint8_t compare_keys();
uint8_t identify_A_Key();

uint8_t find_pulse_length(struct key*,uint16_t);
uint8_t within_pulse_range(uint16_t,uint16_t);

/*************************************************************
Description:configures the Input Capture on Pin 0 of PORT C
*************************************************************/
void INIT_INPUT_CAPTURE(){
	PORTC_DIRCLR=0x01;	//set pin 0 for input
	PORTC_PIN0CTRL=0x00;	//for falling/rising edges	until the program starts to work
	EVSYS_CH0MUX=EVSYS_CHMUX_PORTC_PIN0_gc;
	TCC0_CTRLD=TC_EVACT_CAPT_gc|TC_EVSEL_CH0_gc;
	//TCC0_CTRLB=TC0_CCAEN_bm;	//for CCA and normal mode  if we include this the interrupt will be called constantly
	TCC0_CTRLA=TC_CLKSEL_DIV1_gc;
	set_HIGH_INT();
	TCC0_INTCTRLB=TC_CCAINTLVL_HI_gc;	//for CCAINT ME
	sei();
}

/*************************************************************
Description:turns on the input capture
*************************************************************/
void turn_input_capture_on(){
	TCC0_CTRLB=TC0_CCAEN_bm;
}
/*************************************************************
Description: the interrupt vector for the pulse of the IR
stores the pulses in memory
*************************************************************/
ISR(TCC0_CCA_vect){
	if(KEY_TO_READ!=NO_BUTTON_PRESSED){
		if (!(STARTED_READING)){
			start_Read();
			reset_IR_KEY(KEY_TO_READ);
		}
		//if reading to store key
		//TODO get rid of this????
//		else if (READING_KEY && IR_KEYS[KEY_TO_READ].length<MAX_INPUT_CAPTURE_LENGTH){
	//		read_from_stream();
		//}
		//if reading to identify a key to be read
		else if ((READING_TO_IDENTIFY || READING_KEY) && identity.length<MAX_INPUT_CAPTURE_LENGTH){
			read_identity_stream();
		}
		reset_pulse_timer();
	}
	TCC0_INTFLAGS=0x00;  //TODO  we dont need this DO WE???
}


/*************************************************************
Description:This is lab8_A interrupt routine
*************************************************************/
/*ISR(TCC0_CCA_vect){
		uint8_t readValue=PORTC_IN;
		if (!readValue)
		TCC0_CNT=0x00;
		else{
			points[countTrigger++]=TCC0_CCA;
			triggerInputCapture=0x00;
			//TCC0_CTRLB=0x10;	//for CCA and normal mode
		}
	}*/

/*************************************************************
Description:After looking at the contents of the remote control
I came up with a 1 byte record of the sequences that mark off
the IR signals that I was receiving

7-6 <= BitMask type
	00=Small
	01=Med
	10=Large(High)
	11=Large(Low)
	
5-4<= Reserved
3-0<= Number in sequence  (makes 16 total) if 0 is counted as
base 1, then 16

The way we have the recorder, the information is 
low high, low high,.....low. We will record every other if
the separation between each is a low 
*************************************************************/
struct IR_key convert_IR_signal(struct key volatile *pulse){
	struct IR_key volatile convertedVal;
	if (within_pulse_range( (*pulse).stream[(*pulse).location++],LARGE_PULSE))
		convertedVal.stream[convertedVal.length++]=0b11000000;	//this is the Low that starts my remote
	while((*pulse).location!=(*pulse).length){
		if (within_pulse_range((*pulse).stream[(*pulse).location],LARGE_PULSE)){
			convertedVal.stream[convertedVal.length++]=find_pulse_length( pulse,LARGE_PULSE);
		}
		else if (within_pulse_range((*pulse).stream[(*pulse).location],MED_PULSE)){
			convertedVal.stream[convertedVal.length++]=find_pulse_length( pulse,MED_PULSE);
		}
		else if (within_pulse_range((*pulse).stream[(*pulse).location],SMALL_PULSE)){
			convertedVal.stream[convertedVal.length++]=find_pulse_length( pulse,SMALL_PULSE);
		}
		else{
			//this is not recorded yet
			//so this is a error
			continue;
		}
	}
	return convertedVal;
}


/*************************************************************
Description:Function used to count the number of Pulse widths
of a particular size given PulseVal, and keep track of the unit
that we are on
*************************************************************/
uint8_t find_pulse_length(struct key *Key,uint16_t PulseVal){
	uint8_t convert=0;
	//get initial pulse read
	if (PulseVal==LARGE_PULSE)
		convert=0x80;
	else if (PulseVal==MED_PULSE)
		convert=0x40;
	else if (PulseVal==SMALL_PULSE)
		convert=0x00;
	while(1){
		if (within_pulse_range( (*Key).stream[ (*Key).location],PulseVal)){
			(*Key).location++;
			convert++;		//add 1 to convert for a successful consecutive value
			// this will be added to the least 4 significant bits
			//there is always a low SMALL pulse after any High Pulse
			if (within_pulse_range( (*Key).stream[ (*Key).location],SMALL_PULSE))
				(*Key).location++;
		}
		else
			break;		//we are finished if there is no match
	}
	return convert;
}

/*************************************************************
Description:use to see if a pulse is within PULSE_ERROR
range of mark
*************************************************************/
uint8_t within_pulse_range(uint16_t pulse,uint16_t mark){
	if (pulse<=mark*(1+PULSE_ERROR) && pulse>=mark*(1-PULSE_ERROR))
		return 0x01;	//true
	else
		return 0x00;	//false
}


/*************************************************************
Description:if we need to record to the same IR_KEY twice
we will need to reset it
*************************************************************/
void reset_IR_KEY(uint8_t key){
	if(IR_KEYS[key].length!=0)
		IR_KEYS[key].length=0;
	identity.length=0;
}

/*************************************************************
Description:activates the knowledge that the first falling edge
has been detected and starts recording to the IR_KEYS
*************************************************************/
void start_Read(){
	STARTED_READING=1;
	TCC0_CCA=0x00;
	pulse_timer_ON();
}

/*************************************************************
Description:this reads from the IR to the IR_KEYS
*************************************************************/
void read_from_stream(){
	uint16_t volatile tcc=TCC0_CCA;
	if (tcc<GARBAGE_PULSE && tcc>0)
		IR_KEYS[KEY_TO_READ].stream[IR_KEYS[KEY_TO_READ].length++]=TCC0_CCA;
	TCC0_CNT=0x00;
	pulse_timer_ON();
}

/*************************************************************
Description:This reads the identity stream
*************************************************************/
void read_identity_stream(){
	uint16_t volatile tcc=TCC0_CCA;
	if (tcc<GARBAGE_PULSE && tcc>0)
		identity.stream[identity.length++]=TCC0_CCA;
	TCC0_CNT=0x00; 		
	pulse_timer_ON();
}
/*************************************************************
Description: sets the configuration of the pulse timer
to look for the total length of silence
*************************************************************/
void pulse_timer_INIT(){
	set_HIGH_INT();
	TCD1_INTCTRLA=TC_OVFINTLVL_HI_gc;
	TCD1_PER=PULSE_END_TIME;	//10ms
	sei();

}

/*************************************************************
Description: resets the counter to the pulse_timer to 0
*************************************************************/
void reset_pulse_timer(){
	TCD1_CNT=0x00;
}

/*************************************************************
Description: when the SILENCE record has been  achieved we
need to turn off the IC and the pulse timer
*************************************************************/
ISR(TCD1_OVF_vect){
	//if we are reading a key value
	if (READING_KEY && identity.length>=MINIMUM_KEY_LENGTH){
		IR_KEYS[KEY_TO_READ]=convert_IR_signal( &identity);//convert identity
		reset_identity();
		TCC0_CCA=0x00;
		TCC0_CTRLB=0x00;	//for CCA and normal mode
		TCC0_INTFLAGS=0x00;	//clear the int flags
		pulse_timer_OFF();
		READ_COMPLETE=0x1;
		READING_KEY=0x00;
		STARTED_READING=0x00;
	}	
	//if we are identifing a key value
	else if (READING_TO_IDENTIFY && identity.length>=MINIMUM_KEY_LENGTH){
		key_to_match=convert_IR_signal( &identity);//convert identity
		reset_identity();
		TCC0_CCA=0x00;
		TCC0_CTRLB=0x00;	//for CCA and normal mode
		TCC0_INTFLAGS=0x00;	//clear the int flags
		pulse_timer_OFF();
		READING_TO_IDENTIFY=0x00;
		READ_COMPLETE=0x1;
		READING_KEY=0x00;
		STARTED_READING=0x00;
		
	}

	/*if (IR_KEYS[KEY_TO_READ].length>=MINIMUM_KEY_LENGTH || (READING_TO_IDENTIFY && identity.length>=MINIMUM_KEY_LENGTH)){
		IR_KEYS[KEY_TO_READ]=convert_IR_signal(identity);//convert identtity
		reset_identity();
		TCC0_CCA=0x00;
		TCC0_CTRLB=0x00;	//for CCA and normal mode
		TCC0_INTFLAGS=0x00;	//clear the int flags
		pulse_timer_OFF();
		READ_COMPLETE=0x1;
		READING_KEY=0x00;
		STARTED_READING=0x00;
		READING_TO_IDENTIFY=0x00;
	}*/
	
	//We have not got the right settings of minimum reads within 10ms
	else{
		STARTED_READING=0x00;
		pulse_timer_OFF();
	}
}


/*************************************************************
Description: resets the length and location of identity
*************************************************************/
void reset_identity(){
	identity.length=0;
	identity.location=0;
}

/*************************************************************
Description: turns off the pulse timer
*************************************************************/
void pulse_timer_OFF(){
	TCD1_CTRLA=TC_CLKSEL_OFF_gc;
}

/*************************************************************
Description: turns on the pulse timer
*************************************************************/
void pulse_timer_ON(){
	TCD1_CTRLA=TC_CLKSEL_DIV1_gc;
}

/*************************************************************
Description:this reads the key and displays the key being read
*************************************************************/
void read_key_IR(uint8_t key){
	display_reading_key(key);
	//********* set condition variables***********
	KEY_TO_READ=key;
	READING_KEY=1;
	//****************************************
	turn_input_capture_on();
	//*****************read******************
	while(!READ_COMPLETE){
		//stall
	}
	READ_COMPLETE=0x00;
	KEY_TO_READ=NO_BUTTON_PRESSED;
	check_Key_Set();
	//********************play sound to confirm*************
	display_confirm_read(key);
	delay_1sec();//delay for screen to go to beginning menu
}

/*************************************************************
TODO: create a only remote menu after all  the keys have
been recorded
*************************************************************/
void check_Key_Set(){
	if (IR_KEYS[1].length>MIN_KEY_PULSE_LENGTH && IR_KEYS[2].length>MIN_KEY_PULSE_LENGTH)
		IR_KEYS_SET=0x1;
}


/*************************************************************
Description:When play something is selected on the menu, we need 
to read the key value from the remote and play the pressed button
configuration
*************************************************************/
/*uint8_t identify_A_Key(){
	write_string_LCD("Reading for key");
	READING_TO_IDENTIFY=1;
	KEY_TO_READ=IDENTIFY_BUTTON;
	turn_input_capture_on();
	while(!READ_COMPLETE){
		//stall the clock
	}
	READ_COMPLETE=0x00;
	KEY_TO_READ=NO_BUTTON_PRESSED;
	uint8_t match=compare_keys(key_to_match);
//	if (match!=NO_BUTTON_PRESSED){
//		clear_LCD();
//		write_string_Continue_LCD("A MATCH WAS MADE@ON KEY ",24);
//		write_int_LCD(match);
//		LCD_reset();
//	}
	if (match!=NO_BUTTON_PRESSED){
		clear_LCD();
		write_string_LCD(LCD_REMOTE_DISPLAY[match]);
		reset_LCD();
	}
	else{
		write_string_LCD("NO MATCH");
		delay_sec(2);
	}
	key_to_match.length=0;
	return match;
}*/

/*************************************************************
Description:this function compares the identity key with the
IR_KEYS, if there is a match that key is sent on to do its job
*************************************************************/
uint8_t compare_keys(struct IR_key matchKey){
	uint8_t match=0x00;
	uint8_t key=NO_BUTTON_PRESSED;
	for (int i=0;i<10;i++){
		//if the lengths are equal go on, else continue with next key
		if (matchKey.length==IR_KEYS[i].length){
			for (int j=0;j<IR_KEYS[i].length;j++){
				if (IR_KEYS[i].stream[j]==matchKey.stream[j]){
					match=0x01;
					continue;
				}
				else{
					match=0x00;
					break;
				}
			}
			if (match){
				key=i;
				break;
			}
		}
	}
	return key;
}



/*************************************************************
Description:This function displays the key that is being read
in the read key menu option
*************************************************************/
void display_reading_key(uint8_t key){
		//************ display to LCD************
	char* string="Reading key ";
	clear_LCD();
	write_string_Continue_LCD(string);
	write_number_LCD(key);
}


/*************************************************************
Description:this function displays the confirmation that a key
was read and saved in memory
*************************************************************/
void display_confirm_read(uint8_t key){
	//******************  display confirmation***************
	char* recorded_string="The Key ";
	char* wasRecorded="@was recorded!";
	clear_LCD();
	write_string_Continue_LCD(recorded_string);
	write_number_LCD(key);
	write_string_Continue_LCD(wasRecorded);
}





#endif /* INPUTCAPTURE_H_ */
