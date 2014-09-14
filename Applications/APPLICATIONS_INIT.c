/*************************************************************
applications_INIT_C.c
NAME: Cukjati Jacob
Description: this is Lab 8_A, it reads the glitches of environment
radiation from the IR on the board.  It does 10 captures, while 
toggling the PORTQ and then exits.
*************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ebi_driver.h"
#include "INIT_CS0_Switches_LED.h"
#include "INIT_CS1_SRAM_LCD.h"
#include "INIT_KEYPAD.h"
#include "PezioSpeaker_INIT.h"
#include "delay.h"
#include "TIMERS.h"
#include "Interrupts.h"
#include "values.h"
#include "inputCapture.h"
#include "ADC_INIT.h"
#include "USART.h"
#include "UltraSound.h"
#include "I2C_SERVO.h"
#include "MENU.h"
#include "Lab_Extras.h"
							

//LAB2_C was not included yet because it is lacking in new C material
//LAB3 is the same as LAB2  merely now because it is on PORT Addresses 0x7000


int main(void)
{
	reading_in_Keys_for_remote();
	transmit_IR_KEYS();
}

/*************************************************************
Description: so visually we want to set up the remote to
identify the buttons on the remote, so we have the LED flash
with a 3 second time to change the switches after each read
of the remote.  The LCD will display the button that we are 
reading, so no worries

to break out press 0
*************************************************************/
void reading_in_Keys_for_remote(){
	pulse_timer_INIT();
	INIT_INPUT_CAPTURE();
	int i=0;
	write_string_LCD("Switches to record@Hold 0 to send");
	while(1){
		uint8_t keypad_val=read_KEYPAD();
		//to break out of the function press 0 on the keypad
		if (keypad_val==0x00)
			break;
		LED(0xFF);
		//delay_sec(3);
		uint8_t switchVal=SWITCHES();
		if (switchVal<=NUM_OF_IR_KEYS){
			clear_LCD();
			write_string_Continue_LCD("Writing to key");
			write_number_LCD(switchVal);
			read_key_IR(switchVal);
		}
		LED(0x00);
		delay_ms(700);
	}
}

/*************************************************************
Description: so after reading all the IR_KEYS we want to transmit
to the computer to upload the values, so we can have the keys
on a file and upload that when programming the board.

we create a string of the IR_KEYS, best in string cause each
value in the key is 8 bits, so we can do that easily
*************************************************************/
void transmit_IR_KEYS(){
	INIT_USART();
	for (int i=0;i<=NUM_OF_IR_KEYS;i++){
		if(IR_KEYS[i].length>=MIN_KEY_LENGTH)
			continue;
		//first create a string
		char string[MAX_KEY_LENGTH];
		for (int j=0;j<IR_KEYS[i].length;j++){
			string[j]=IR_KEYS[i].stream[j];
		}
		send_string_USART(string);
	}
	transmit_DATA();
	write_string_LCD("TRASMITTING");
	while(1){}
}
