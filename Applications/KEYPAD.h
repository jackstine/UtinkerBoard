/*************************************************************
KEYPAD.h
NAME: Cukjati Jacob
Section#: 7F31
TA NAME: Kevin French
Description: The header file for keypad
*************************************************************/

#ifndef INIT_KEYPAD_H_
#define INIT_KEYPAD_H_

#define firstRow 0b11010000
#define secondRow 0b10110000
#define thirdRow 0b01110000
#define fourthRow 0b11100000
#define lower_half 0b000001111
//TODO make sure that this does not conflict with anything else
// that I might have used earlier to fill in some random values
//    :P      
#define NO_BUTTON_PRESSED 0xFF

uint8_t static keypadINIT=0;

#include "values.h"
#include "ebi_driver.h"

void INIT_KEYPAD();
uint8_t read_KEYPAD();
uint8_t check_KEYPAD();
uint8_t read_first_row(uint8_t);
uint8_t read_second_row(uint8_t);
uint8_t read_third_row(uint8_t);
uint8_t read_fourth_row(uint8_t);



/*************************************************************
Description: INITs the KEYPAD
*************************************************************/
void INIT_KEYPAD(){
	keypadINIT=1;			//mark that the init has happened
	PORTE_DIR=0b11110000; //first half is outputs and second is inputs
}


/*************************************************************
Description:reads the keypad and returns hex value of key pressed
*************************************************************/
uint8_t read_KEYPAD(){
	if (!(keypadINIT))
		INIT_KEYPAD();
	uint8_t result=check_KEYPAD();
	if (result==NO_BUTTON_PRESSED)
		result=check_KEYPAD();
	return result;
}

/*************************************************************
Description:reads checks the rows of the keypad for a button
*************************************************************/
uint8_t check_KEYPAD(){
		uint8_t check;
		PORTE_OUT=fourthRow;
		check=PORTE_IN;
		check= check & lower_half;
		if (!(check == lower_half))
			return read_fourth_row(check);
			
		PORTE_OUT=firstRow;
		check=PORTE_IN;
		check= check & lower_half;
		if (!(check == lower_half))
			return read_first_row(check);
		
		PORTE_OUT=secondRow;
		check=PORTE_IN;
		check= check & lower_half;
		if (!(check == lower_half))
			return read_second_row(check);
		
		PORTE_OUT=thirdRow;
		check=PORTE_IN;
		check= check & lower_half;
		if (!(check == lower_half))
			return read_third_row(check);

		return NO_BUTTON_PRESSED;
}

/*************************************************************
Description:reads the first row for a button and returns it
*************************************************************/
uint8_t read_first_row(uint8_t row){
	if (!((row & bit0) == bit0))
		return 0x1;
	if (!((row & bit1) == bit1))
		return 0x2;
	if (!((row & bit2) == bit2))
		return 0x3;
	if (!((row & bit3) == bit3))
		return 0xA;
}

/*************************************************************
Description:reads the second row for a button and returns it
*************************************************************/
uint8_t read_second_row(uint8_t row){
		if (!((row & bit0) == bit0))
			return 0x4;
		if (!((row & bit1) == bit1))
			return 0x5;
		if (!((row & bit2) == bit2))
			return 0x6;
		if (!((row & bit3) == bit3))
			return 0xB;
}

/*************************************************************
Description:reads the third row for a button and returns it
*************************************************************/
uint8_t read_third_row(uint8_t row){
	if (!((row & bit0) == bit0))
		return 0x7;
	if (!((row & bit1) == bit1))
		return 0x8;
	if (!((row & bit2) == bit2))
		return 0x9;
	if (!((row & bit3) == bit3))
		return 0xC;
}

/*************************************************************
Description:reads the fourth row for a button
and returns it
*************************************************************/
uint8_t read_fourth_row(uint8_t row){
	if (!((row & bit0) == bit0))
		return 0xE;
	if (!((row & bit1) == bit1))
		return 0x0;
	if (!((row & bit2) == bit2))
		return 0xF;
	if (!((row & bit3) == bit3))
		return 0xD;
}


#endif /* INIT_KEYPAD_H_ */
