/*************************************************************
INIT_CS1_SRAM_LCD.h
NAME: Cukjati Jacob
Description: The header file for CS1 PORT: the LCD and SRAM
*************************************************************/

#ifndef INIT_CS1_SRAM_LCD_H_
#define INIT_CS1_SRAM_LCD_H_

#include "ADC_INIT.h"
#include "UltraSound.h"

#define CS1_IOPORT_START 0xAB0000
#define SRAM_PORT_START 0xAB6000
#define SRAM_PORT_END 0xABDFFF
#define SRAM_BASE 0x0000
#define SRAM_END 0x7FFF
#define CS1_CTRLA 0b00100001


#define LCD_COMMAND 0xAB5000
#define LCD_DATA 0xAB5005
#define LCD_PORT_START 0xAB4000
#define LCD_PORT_END 0xAB6FFF
#define LCD_START_ADDR 0x80
#define LCD_CLEAR 0x01
#define LCD_CURSOR_OFF 0x0E
#define LCD_SET_SECONDLINE 0xC0

#define RESET_LCD 0x02

uint8_t static CS1_INIT=0;
uint8_t static lcdINIT=0;

#include "INIT_CS0_Switches_LED.h"
#include <avr/io.h>
#include "values.h"
#include "ebi_driver.h"


/*************************************************************
Description: INIT FOR SRAM AND LCD
*************************************************************/
void INIT_SRAM_LCD(){
	CS1_INIT=1;

	//if the LED and SWITCHES are already init then we need to 
	// init both the SRAM and LED and SWITCH
	if(CS0_INIT){
		PORTH_DIR=0b00110111;	// bit5 is CS1, CS0, bit0 is WE, bit1 is RE, bit2 is ALE1
		PORTH_OUT=0b00110011;	//setting the corresponding bits

		EBI_CTRL=EBI_IFMODE_3PORT_gc;	//for 3 port and address byte 0 and 1 multiplexed
	
		EBI.CS1.BASEADDR=(uint16_t) (CS1_IOPORT_START>>8);
		EBI_CS1_CTRLA=CS1_CTRLA;	//sets up for 64k and bits1:0 SRAM

		EBI.CS0.BASEADDR=(uint16_t) (CS0_IOPORT_START>>8)& 0xFF;
		EBI_CS0_CTRLA=CS0_CTRLA;
	
		PORTK_DIR=0xFF;		//set the port for address
		PORTJ_DIR=0xFF;		//set the direction of the data bus

	}
	else{
		PORTH_DIR=0b00100111;	// bit5 is CS1, bit0 is WE, bit1 is RE, bit2 is ALE1
		PORTH_OUT=0b00100011;	//setting the corresponding bits
	
		EBI_CTRL=EBI_IFMODE_3PORT_gc;	//for 3 port and address byte 0 and 1 multiplexed
	
		EBI.CS1.BASEADDR=(uint16_t) (CS1_IOPORT_START>>8);
		EBI_CS1_CTRLA=CS1_CTRLA;	//sets up for 64k and bits1:0 SRAM
	
		PORTK_DIR=0xFF;		//set the port for address
		PORTJ_DIR=0xFF;
	}
}


/*************************************************************
INPUTS: address
RETURNS: value read from SRAM
Description: read from SRAM, INIT IF NOT ALREADY DONE
base address 0-7FFF
*************************************************************/



//TODO allow for more easy access to the SRAM so that it
//can be connected so that it organizes its own memory
uint8_t read_SRAM(int addr){
	if (!(CS1_INIT))
		INIT_SRAM_LCD();
	return __far_mem_read((SRAM_PORT_START+addr));
}

/*************************************************************
INPUTS: address, data
Description: INITS the SRAM if not done, and writes data
to the address in SRAM base 0-7FFF
*************************************************************/
void write_SRAM(int addr,uint8_t data){
	if (!(CS1_INIT))
		INIT_SRAM_LCD();
	__far_mem_write((SRAM_PORT_START+addr) ,data);
}


/*************************************************************
Description: INITS the LCD
*************************************************************/
void INIT_LCD(){
	if(!(CS1_INIT))
		INIT_SRAM_LCD();
	lcdINIT=1;
	uint8_t two_lines=0x38; //to activate the two lines
	uint8_t display_on=0x0F; //to put display on
	uint8_t clear=0x01;	//to clear the LCD
	uint8_t entry=0b00000110;
	uint8_t setting_addr=0x80;	//setting DDRAM for address 0

	__far_mem_write(LCD_COMMAND,two_lines);
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,display_on);
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,clear);
	delay_ms(2);
}

/*************************************************************
Description: Waits for the LCD busy flag to read ready
*************************************************************/

//TODO have this pice of code work
void wait_for_LCD_busy(){
	uint8_t BF;
	do {BF=__far_mem_read(LCD_COMMAND);
	} while ((BF & 0x80) == 0x80);
}

/*************************************************************
INPUTS:character
Description:writes a character to the LCD display, INITS if
not already done so
*************************************************************/
void write_char_LCD(char character){
	if(!(CS1_INIT  &&  lcdINIT)){
		INIT_SRAM_LCD();
		INIT_LCD();
	}
	
	//wait_for_LCD_busy();
	delay_micro(40);
	__far_mem_write(LCD_DATA,character);
}

/*************************************************************
INPUTS: hex, units
Description:writes the hexadecimal value of the value hex, with
the number of units in units
ex:  hex= 129 units=3
OUTPUT: 0x081
*************************************************************/
void write_hex_LCD(int hex,int units){
	write_string_Continue_LCD("0x",2);
	for (int i=units-1;i>=0;i--)
		write_char_LCD(get_hex_value(hex,i));
}


/*************************************************************
INPUTS: num, precision
Description:writes the floating point number to
the precision of precision
ex:  num=3.41232  precision=2
output: 3.4   NOTE: ONLY WORKS FOR num less than 10
*************************************************************/
void write_float_LCD(float num,uint8_t precision){
	int temp;
	uint8_t first=1;
	for (uint8_t i=0;i<precision;i++){
		temp=num;
		write_single_digit_LCD(temp);
		num=num-temp;
		num=num*10;
		if (first){
			write_char_LCD('.');
			first=0;
		}
	}
	
}

/*************************************************************
INPUTS: integer
Description:writes the integer value to the LCD
*************************************************************/
void write_single_digit_LCD(uint8_t integer){
	char character=(char)(((int) '0')+integer);
	if(!(CS1_INIT  &&  lcdINIT)){
		INIT_SRAM_LCD();
		INIT_LCD();
	}
	
	//wait_for_LCD_busy();
	delay_micro(40);
	__far_mem_write(LCD_DATA,character);
}
/*************************************************************
INPUTS:string, length
Description: first clears the LCD, then writes a string
to the LCD, inits if not already done so and resets the LCD

FORMATING:  @ starts a new line
/ the end of the string
*************************************************************/
void write_string_LCD(char* string){
	if(!(CS1_INIT  &&  lcdINIT)){
		INIT_SRAM_LCD();
		INIT_LCD();
	}
	clear_LCD();
	delay_micro(40);
	int i=0;
	while(1){
		//if the '@' character is 
		//in the string we start a new line
		if (string[i]==0)
			break;
		else if (string[i]=='@'){
			set_nextLine_LCD();
		}	
		else
			write_char_LCD(string[i]);
		i++;
	}
}

/*************************************************************
INPUTS:string, length
Description: writes a string to the LCD, inits if not already
done so and does not reset the LCD.
But allows you to continue writing to the LCD after use, where
you last left off in the LCD addresses
*************************************************************/
void write_string_Continue_LCD(char* string){
	if(!(CS1_INIT  &&  lcdINIT)){
		INIT_SRAM_LCD();
		INIT_LCD();
	}
	delay_micro(40);
	int i=0;
	while(1){
		if (string[i]==0)
			break;
		else if (string[i]=='@'){
			set_nextLine_LCD();
		}	
		else
			write_char_LCD(string[i]);
		i++;
	}
}

/*************************************************************
INPUTS:string, length
Description: sets the LCD to the nextLine
*************************************************************/
void set_nextLine_LCD(){
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,LCD_SET_SECONDLINE);
	delay_micro(40);
}

/*************************************************************
INPUTS:string, length,time
Description: writes a string to the LCD, in typing style
inits if not already done so and resets the LCD

FORMATTING:  @ is used to start on the second line
*************************************************************/
void write_string_delayMS_LCD(char* string,uint8_t time){
		if(!(CS1_INIT  &&  lcdINIT)){
		INIT_SRAM_LCD();
		INIT_LCD();
	}
	int i=0;
	while(1){
		if (string[i]==0)
			break;
		else if (string[i]=='@')
			set_nextLine_LCD();
		else{
			write_char_LCD(string[i]);
			delay_ms(time);
		}
		i++;
	}
}

/*************************************************************
Description: resets the LCD, starts
address on LCD to 0x00, and turns off cursor
*************************************************************/
void reset_LCD(){
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,RESET_LCD);
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,LCD_START_ADDR);
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,LCD_CURSOR_OFF);
}

/*************************************************************
Description: clears the LCD screen
*************************************************************/
void clear_LCD(){
	reset_LCD();
	delay_micro(40);
	__far_mem_write(LCD_COMMAND,LCD_CLEAR);
	delay_ms(2);
}

/*************************************************************
Description: writes a number to the LCD

TODO: do a better job getting thing faster
*************************************************************/
void write_number_LCD(int num){
	int temp=num;
	int unsigned multiple=1;
	while(temp!=0){
		temp/=10;
		multiple*=10;
	}
	temp=num;
	multiple/=10;
	while(multiple!=0){
		int space=temp/multiple;
		write_single_digit_LCD(space);
		temp-=space*multiple;
		multiple/=10;
	}
}

/*************************************************************
Description:LCD gets the distance from the ultrasound,
displays the meters and inches
*************************************************************/
void LCD_get_distance(){
	clear_LCD();
	write_float_LCD(distance,3);
	write_string_Continue_LCD(" meters@");
	int inches=distance*INCHES_IN_METERS;
	write_number_LCD(inches);
	write_string_Continue_LCD(" inches");
}

/*************************************************************
Description:writes the ADC voltage to the LCD
*************************************************************/
void ADC_LCD(){
	clear_LCD();
	write_float_LCD(ADC_VOLT,3);
	write_string_Continue_LCD(" V (");
	write_hex_LCD(ADC_RES,3);
	write_char_LCD(')');
}

#endif /* INIT_CS1_SRAM_LCD_H_ */
