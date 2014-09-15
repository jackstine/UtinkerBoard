/*
 * Lab_Extras.h
 *
 * Created: 8/8/2014 6:20:07 PM
 *  Author: jacob_000
 */ 


#ifndef LAB_EXTRAS_H_
#define LAB_EXTRAS_H_

#include <avr/io.h>
#include "INIT_KEYPAD.h"
#include "inputCapture.h"
#include "INIT_CS1_SRAM_LCD.h"
#include "PezioSpeaker_INIT.h"
#include "values.h"

char LCD_REMOTE_DISPLAY[10][35]={	"MP-3:Happy by@Pharrell W./",	//0
	"BR:X-Men:Days@of Future Past/",	//1
	"BR:The LEGO@Movie/",	//2
	"NF:House of@Cards/",	//3
	"MP-3:Bohemian@Rhapsody/",	//4
	"DVD:Game of@Thrones/",	//5
	"TV:Cosmos:A@Space-Time Odyss/",	//6
	"MP-3:Dirt by@Florida GA Line/",	//7
	"TV:The Big@Bang Theory/",	//8
	"DVR:Colbert@Report/"};		//9
	

/*************************************************************
Description:plays the corresponding song from the remote
*************************************************************/
void remote_play(uint8_t buttonPressed){
	if (buttonPressed!=NO_BUTTON_PRESSED && !playingSOUND){//if button pressed
		//	clear_LCD();	//clear the LCD
		if (buttonPressed>=0x00 && buttonPressed<=0xD && buttonPressed!=6){//if the button was 0-D
			struct note playNote=create_note_from_KEYPAD(buttonPressed);//play the sound
			play_note(playNote);
			//write_string_LCD(playNote.freq_name);//write the name to LCD
		}
		else if (buttonPressed==6){
			struct musicalNote *SpaceOdyssey=create_space_odyssey();
			play_song(SpaceOdyssey,57);
		}
		else if (buttonPressed==0xE){//if button was E play Ascending order
			write_string_LCD("Ascending Order");
			struct note *queueOfNotes=create_ascending_order();
			int lengthOfQueue=8;
			play_queue_of_notes(queueOfNotes,8);
		}
		else if (buttonPressed==0xF){//if button was F play descending order
			write_string_LCD("Descending Order");
			struct note *queueOfNotes=create_descending_order();
			int lengthOfQueue=8;
			play_queue_of_notes(queueOfNotes,8);
		}
	}
}


/*************************************************************
Description:When play something is selected on the menu, we need 
to read the key value from the remote and play the pressed button
configuration
*************************************************************/
uint8_t identify_A_Key(){
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
/*	if (match!=NO_BUTTON_PRESSED){
		clear_LCD();
		write_string_Continue_LCD("A MATCH WAS MADE@ON KEY ",24);
		write_int_LCD(match);
		LCD_reset();
	}*/
	if (match!=NO_BUTTON_PRESSED){
		clear_LCD();
		write_string_LCD(LCD_REMOTE_DISPLAY[match]);
		reset_LCD();
	}
	else{
		write_string_LCD("NO MATCH");
		delay_sec(2);
	}
	identity.length=0;
	return match;
}


//reads the switches and outputs the corresponding output
void LAB2_A(){
	while(1){
		uint8_t read= SWITCHES();
		LED(read);	
	}
	
}



//frequency of 1kHZ duty cycle of 50%
void LAB2_B(){
	INIT_LED_SWITCHES();
    while(1){
		LED(0x01);
		delay_1micro();   
		LED(0x00);
		delay_1micro();
    }	
}


//this lab displays the value on the keypad
//if that value was pressed
//since this lab uses the LED we have to init it
// as well as the KEYPAD
// we poll the KEYPAD twice, this makes it work
void LAB4_A(){
	while(1){
		uint8_t volatile readVal =read_KEYPAD();
		if (readVal!=NO_BUTTON_PRESSED)
			LED(readVal);
		else
			LED(0xAA);
	}
}

void LAB4_B(){
	while(1){
		for (int unsigned i=SRAM_BASE;i<=SRAM_END;i++){
			write_SRAM(i,(uint8_t) i);
		}
		LED(0xF0);
		for (int unsigned i=SRAM_BASE;SRAM_END;i++){
			uint8_t sramVal=read_SRAM(i);
			LED(sramVal);
			delay_ms(100);
		}
	}
}

void LAB5_A(){
	LED(0);
	INTERRUPTS();
	while(1){
			delay_micro(100);
			sei();
	}
}

void LAB5_B(){
	INIT_USART();
	send_string_USART("HELLO WORLD!");
	int volatile count=0;
	transmit_DATA();
	while(1){
		delay_ms(500);
		PORTQ_DIRTGL=0xF;
		if (count==18){
			send_string_without_length_USART("Sending a new round");
		}
		if (count==22)
			transmit_DATA();
		count++;
	}
}



void LAB6_A(){
	char* name="Jacob Cukjati";
	uint8_t nameLength=13;
	write_string_delayMS_LCD(name,200);
	while(1){
		delay_sec(5);
	}
}

void LAB6_B(){
	INIT_ADC();
	while(1){
			//delay_ms(500);
			ADCA_CH1_CTRL=0x81;
			ADC_LCD();
	}
}
/*************************************************************
Description: This program plays the frequency 1318.51 if
the switch is closed.
************************************************************/
void LAB7_A(){
	SPEAKER_INIT();	//INIT SPEAKER
	struct note Note;	//create tjhe note
	Note.freq=1318.51;
	Note.tone=0;
	play_speaker(Note);	//set the note
	while(1){
		uint8_t read=SWITCHES();	//read the switches
		if (read>0x00){	//if a switch is polled play the sound
			if(speakeroff)
			turn_speaker_on();	//turn speaker on if switch is polled
		}
		else
		turn_speaker_off();	//turn speaker off if no switch is polled
	}
}


/*************************************************************
Description: This program plays the corresponding frequency
if the keypad is pressed.  If E or F is pressed the program
plays ascending or descending order, in that order.
*************************************************************/

void LAB7_B(){
	while(1){
		uint8_t volatile buttonPressed=read_KEYPAD();	//poll keypad
		if (buttonPressed!=NO_BUTTON_PRESSED && !playingSOUND){//if button pressed
			clear_LCD();	//clear the LCD
			if (buttonPressed>=0x00 && buttonPressed<=0xD){//if the button was 0-D
				struct note playNote=create_note_from_KEYPAD(buttonPressed);//play the sound
				play_note(playNote);
				write_string_LCD(playNote.freq_name);//write the name to LCD
			}
			else if (buttonPressed==0xE){//if button was E play Ascending order
				write_string_LCD("Space Odyssey");
				struct musicalNote *SpaceOdyssey=create_space_odyssey();
				play_song(SpaceOdyssey,57);
			}
			else if (buttonPressed==0xF){//if button was F play descending order
				write_string_LCD("Descending Order");
				struct note *queueOfNotes=create_descending_order();
				int lengthOfQueue=8;
				play_queue_of_notes(queueOfNotes,8);
			}
		}
	}
}


void LAB7_QUIZ(){
	int time=17814/2;
	timer_ON(time);
	while (1){
	}
	
}
/*Arthor: Cukjati Jacob
 *DESCIPTION:this is Lab 8_A, it reads the glitches of environment
 *	 radiation from the IR on the board.  It does 10 captures, while
 *	 toggleing the PORTQ and then exits.
 */
/*void LAB8_A(){
		PORTC_DIRCLR=0x01;	//set pin 0 for input
		PORTC_PIN0CTRL=0x00;	//for falling/rising edges	until the program starts to work
		EVSYS_CH0MUX=EVSYS_CHMUX_PORTC_PIN0_gc;
		EVSYS_CH0CTRL=0x00;	//for 1 sample
		TCC0_CTRLD=0b00101000;	//input Capture for EVENT 0
		TCC0_CTRLB=0x10;	//for CCA and normal mode
		TCC0_CTRLA=0x01;	//for prescale 1
		set_HIGH_INT();
		TCC0_INTCTRLB=0x02;	//for CCAINT ME
		sei();
	
	while(1){
		if (countTrigger<10){
			PORTQ_DIRTGL=0x0F;
			delay_1sec();
		}
		else{
			cli();
			LED(0xFF);
		}
	}
}*/

/*Author: Cukjati Jacob
 *DESCIPTION:this displays a menu to record a input capture from a
 * 40kHZ remote and then allows you to play the corresponding key
 * on the remote from the play something menu option
 * CODE is all on the INPUT CAPTURE header file
 */
void LAB8_B(){
	beginning_MENU();
}

/*****************************************************
Activates the ultrasound and displays inches and meters
from the measurements taken from the ultrasound.
*****************************************************/
void extraLAB(){
	ultraSound_INIT();
	int unsigned volatile count=0;
	while(1){
		uint8_t volatile mess=PORTD_IN;
		LED(mess);
		if (count==5)
			activate_ultra_sound();
		count++;
	}
}

/***************************************************
TODO
***************************************************/
void extraLAB2(){
	I2C_INIT();
	I2C_transmit_to(0x02);
	uint8_t volatile count=0;
	send_info_to_I2C(1);
	send_info_to_I2C(2);
	set_address();
	while(1){
		resubmit_data_I2C();
		I2C_transmit_data();
		I2C_transmit_data();
		LED(count);
		count++;
	}
}

/**************************************
This program sends information through serial
communication, from a external computer. if the
serial information is textToFind it will display
the letters until the entire string is finished
and will finish with flashing LED and the completed
message,  if there is a character that does not correspond
to the string, then it will clear the LCD and start over
***************************************/
void Exam(){
	INIT_USART();
	int volatile i=0;
	uint8_t volatile textToFindLength=5;
	char textToFind[5]="Hello";
	INIT_SRAM_LCD();
	clear_LCD();
	LED(0);
	while(1){
		if (TextReceived){
			if (RECEIVE_STREAM.location!=RECEIVE_STREAM.length-1)
				RECEIVE_STREAM.location=RECEIVE_STREAM.length-1;
			if (RECEIVE_STREAM.stream[RECEIVE_STREAM.location++]==textToFind[i]){
				write_char_LCD(textToFind[i]);
				i++;
				if (i==textToFindLength)
					break;
			}
			else{
				//RECEIVE_STREAM.location++;
				i=0;
				clear_LCD();
			}
			TextReceived=0;
		}
	}
	write_string_Continue_LCD(" 10% of@exam 2 done!");
	while(1){
		LED(0xFF);
		delay_ms(370);
		LED(0x00);
		delay_ms(370);
	}
}


/*************************************************************
Description: Beginning menu to 1 recored a key
or 2 play a key
*************************************************************/
void beginning_MENU(){
	pulse_timer_INIT();
	INIT_INPUT_CAPTURE();
	int i=0;
	write_string_LCD("1)Record a Key@2)Play Something");
	while(1){
		uint8_t readKeypad=read_KEYPAD();
/*		if (IR_KEYS_SET){
			uint8_t key=identify_A_Key();
			if (key!=NO_BUTTON_PRESSED){
				readKeypad=key;
			}
		}*/
		if (readKeypad==0x1)
			recording_A_Key_MENU();
		else if (readKeypad==0x2){
			uint8_t key= identify_A_Key();
			remote_play(key);
		}
	}
	
}

/*************************************************************
Description:this is Menu that follows the recording of a key
*************************************************************/
void recording_A_Key_MENU(){
	char* string="Press a key from@0-50 to record it";
	write_string_LCD(string);
	delay_ms(250);//delay to stop the keypad
	while(1){
		uint8_t readKeypad=read_KEYPAD();
		if(readKeypad<=0x9 && readKeypad>=0x00){
			read_key_IR(readKeypad);
			break;
		}
	}
}

#endif /* LAB_EXTRAS_H_ */
