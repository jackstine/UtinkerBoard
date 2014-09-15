/*************************************************************
INIT_CS0_Switches_LED.h
NAME: Cukjati Jacob
Section#: 7F31
TA NAME: Kevin French
Description: The header file for switches and LED
*************************************************************/

#ifndef INIT_CS0_SWITCHES_LED_H_
#define INIT_CS0_SWITCHES_LED_H_



#define CS0_IOPORT_START 0x7000	//this is the base address location of the CS0 port
#define CS0_IOPORT_END 0x7FFF		//this is the end base address of the CS0 port
#define LED_PORT 0x7005		//just to define a LED PORT
#define SWITCH_PORT 0x7008	//just to define the SWITCH PORT
#define CS0_CTRLA 0b00010001


uint8_t static CS0_INIT=0;


#include <avr/io.h>
#include "ebi_driver.h"
#include "INIT_CS1_SRAM_LCD.h"
#include "IO.h"

//Things to note that PORTK is not used in this set up because the usage of it
// would make it invalid.  The reason is because the address does not need to
// be known since all the address in the range 0x7000-0x7FFF are assigned to the
// switches and LEDs.

// NOTE: to accept switches as input you need to set PORTJ as DIRCLR 0xFF
// NOTE: to accept LED as outputs you need to set PORT J as DIRSET 0xFF



/*************************************************************
INIT_LED_SWITCHES
NAME: Cukjati Jacob
Description: INIT FOR LED AND SWITCHES
*************************************************************/
void INIT_LED_SWITCHES(){
	CS0_INIT=1;
	if(CS1_INIT){
		PORTH_DIR=0b00110111;	// bit5 is CS1, CS0, bit0 is WE, bit1 is RE, bit2 is ALE1
		PORTH_OUT=0b00110011;	//setting the corresponding bits

		EBI_CTRL=EBI_IFMODE_3PORT_gc;	//for 3 port and address byte 0 and 1 multiplexed
	
		EBI.CS1.BASEADDR=(uint16_t) (CS1_IOPORT_START >>8);
		EBI_CS1_CTRLA=CS1_CTRLA;	//sets up for 64k and bits1:0 SRAM

		EBI.CS0.BASEADDR=(uint16_t) (CS0_IOPORT_START>>8)& 0xFF;
		EBI_CS0_CTRLA=CS0_CTRLA;

	}
	else{
		PORTH_DIR=0b00010111;	// WE (bit0) RE (bit 1) and ALE1 (bit 2) and CS0 (bit 4)
		PORTH_OUT=0b00010011;	//since RE, WE, and CS0 are active low set them
		EBI_CTRL=EBI_IFMODE_3PORT_gc;	//set bit [7:6] 00 for SDRAM which we are not using
		EBI.CS0.BASEADDR=(uint16_t) (CS0_IOPORT_START>>8)&0xFF; //set up the address of the port
		EBI_CS0_CTRLA=CS0_CTRLA;	//SETS IT UP for 4k and SRAM
	}
		PORTK_DIR=0xFF;		//set the port for address
		PORTJ_DIR=0xFF;		//set the direction of the data bus
}

/*************************************************************
LED
NAME: Cukjati Jacob
Description: displays the value on the LED
*************************************************************/
void LED(uint8_t display){
	if (!(CS0_INIT))
		INIT_LED_SWITCHES();
	IO_OUTPUT();
	__far_mem_write(LED_PORT,display);
}


/*************************************************************
SWITCHES
NAME: Cukjati Jacob
Description: returns the value read from the switch
*************************************************************/
uint8_t SWITCHES(void){
	if (!(CS0_INIT))
		INIT_LED_SWITCHES();
	IO_INPUT();
	uint8_t switchVal=__far_mem_read(SWITCH_PORT);
	return switchVal;
}


#endif /* INIT_CS0_SWITCHES_LED_H_ */
