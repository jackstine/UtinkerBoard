/*
 * I2C_SERVO.h
 *
 * Created: 8/3/2014 4:43:24 PM
 *  Author: jacob_000
 */ 


#ifndef I2C_SERVO_H_
#define I2C_SERVO_H_

#include <avr/io.h>
#include "Interrupts.h"
#include "values.h"

#define BAUD 95

struct I2C_stream8 volatile TRANS_TWI_DATA;


/*************************************************************
Description:this sets up the initialization of the I2C
*************************************************************/
void I2C_INIT(){
	//PORTD_PIN1CTRL=0b10101111;
	TWID_CTRL=0x02;
	TWID_MASTER_CTRLA=TWI_MASTER_INTLVL_HI_gc;//|TWI_MASTER_WIEN_bm;
	TWID_MASTER_CTRLB=TWI_MASTER_TIMEOUT_50US_gc;
	TWID_MASTER_BAUD=BAUD;
	set_HIGH_INT();
	TWID_MASTER_CTRLA|=TWI_MASTER_ENABLE_bm;	//enable the master
	sei();
}

/*************************************************************
Description:sets this tells the I2C what address it is
transmitting to,  for these purposes it is only 1 address
in the future it will be more  

TODO: create a array of I2C streams to add more address to
be transmitted to
*************************************************************/
void I2C_transmit_to(uint8_t addr){
	TRANS_TWI_DATA.address=addr;
}

/*************************************************************
Description:turns on the I2C transmiter, this will transmit all
the data on the TRANS_TWI_DATA if the transmit int is enabled
*************************************************************/
void set_address(){
	TWID_MASTER_ADDR=(TRANS_TWI_DATA.address<<1)&0xFE;
}

void I2C_transmit_data(){
	if (TRANS_TWI_DATA.length!=TRANS_TWI_DATA.location){
		set_address();
		TWID_MASTER_DATA=TRANS_TWI_DATA.stream[TRANS_TWI_DATA.location++];
		poll_acknowledgement();
//		poll_acknowledgement();
		//TWID_MASTER_CTRLC=TWI_MASTER_CMD_STOP_gc;
	}
}

/*************************************************************
Description:transmits a entire array of information, according
to the size of the length
*************************************************************/
void send_array_without_length_to_I2C(uint8_t array[]){
	int i=0;
	while(1){
		if (array[i]=='/')
			break;
		send_info_to_I2C(array[i]);
		i++;
	}
}

/*************************************************************
Description:this will send all the information on the array to 
TRANS_TWI_DATA, which when the I2C is activated to send
information it will send it all
*************************************************************/
void send_array_to_I2C(uint8_t array[],int length){
	for (int i=0;i<length;i++)
		send_info_to_I2C(array[i]);
}

/*************************************************************
Description:this function puts information onto the 
TRANS_TWI_DATA which will be sent to the address with the
information on TRANS_TWI_DATA
*************************************************************/
void send_info_to_I2C(uint8_t info){
	if (TRANS_TWI_DATA.length==MAX_STREAM_LENGTH)
		TRANS_TWI_DATA.length=0;
	TRANS_TWI_DATA.stream[TRANS_TWI_DATA.length++]=info;
}

void poll_acknowledgement(){
	uint8_t volatile bitMask=TWID_MASTER_STATUS&TWI_MASTER_RXACK_bm;
	while(bitMask==TWI_MASTER_RXACK_bm){
		bitMask=TWID_MASTER_STATUS&TWI_MASTER_RXACK_bm;
	}
}



void resubmit_data_I2C(){
	TRANS_TWI_DATA.location=0;
}
/*************************************************************
Description:This interrupt function continuously transmits
the data that is on the TRAS_TWI_DATA until it has transmitted
all the data
*************************************************************/
ISR(TWID_TWIM_vect){
	if (TWID_MASTER_STATUS&TWI_MASTER_WIEN_bm==TWI_MASTER_WIEN_bm){
		uint8_t volatile addressByte=(TRANS_TWI_DATA.address<<1)&0xFE;
		TWID_MASTER_ADDR=addressByte;
		poll_acknowledgement();
		while(TRANS_TWI_DATA.location!=TRANS_TWI_DATA.length){
			if (TRANS_TWI_DATA.location==MAX_STREAM_LENGTH)
				TRANS_TWI_DATA.location=0;
			TWID_MASTER_DATA=TRANS_TWI_DATA.stream[TRANS_TWI_DATA.location++];
			poll_acknowledgement();
		}
		//TWID_MASTER_CTRLA
		//TWID_MASTER_CTRLC=TWI_MASTER_CMD_STOP_gc;
	}
}



#endif /* I2C_SERVO_H_ */
