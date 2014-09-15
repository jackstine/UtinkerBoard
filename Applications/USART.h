/*
 * USART.h
 *
 * Created: 7/26/2014 6:57:12 PM
 *  Author: Jacob Cukjati
 */ 


#ifndef USART_H_
#define USART_H_

#define TRANSMIT_PIN 0x08
#define RECEIVE_PIN 0x04
#define BAUDSCALE -7
#define BAUDSEL 705

#include <avr/io.h>
#include "values.h"
#include "Interrupts.h"

/*************************************************************
Description:this structure allows us to keep track of the stream
the we are sending and receiving
*************************************************************/


uint8_t volatile transmiterENABLE=0;
uint8_t volatile receiverENABLE=0;
uint8_t volatile USART_INIT_ON=0;
struct stream8 volatile TRANS_STREAM;	//use location to send information out
										//use length to add strings and characters to the stream
struct stream8 volatile RECEIVE_STREAM;	//use length to receive information, use location to use info
uint8_t volatile TextReceived=0;

//NOTE that these streams can only hold 1000 bytes of data, after that they will begin to 
// rewrite the data that they have already written too


/*************************************************************
Description:INITS the USART from the USB
*************************************************************/
void INIT_USART(){
	USART_INIT_ON=1;
	turn_transmitter_on_USART();
	turn_receiver_on_USART();
	set_HIGH_INT();
	USARTC0.CTRLA=USART_RXCINTLVL_HI_gc| USART_TXCINTLVL_HI_gc;// | USART_DREINTLVL_MED_gc;
	USARTC0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc|USART_PMODE_DISABLED_gc|USART_CHSIZE_8BIT_gc;
	USARTC0.BAUDCTRLA=(uint8_t)BAUDSEL;	//this is the lower 8 bits of the BSEL
	USARTC0.BAUDCTRLB=(BAUDSCALE<<4)|(BAUDSEL>>8);	//the lower nibble is the higher 4 bits of BSEL and the higher nibble is the BSCALE	
    sei();  //Enable global interrupts
}

/*************************************************************
Description:turn on the receiver
*************************************************************/
void turn_receiver_on_USART(){
	if (!USART_INIT_ON)
		INIT_USART();
	PORTC_DIRCLR=RECEIVE_PIN;
	USARTC0_CTRLB|=USART_RXEN_bm;
	receiverENABLE=1;
}

/*************************************************************
Description:turns on the transmitter
*************************************************************/
void turn_transmitter_on_USART(){
	if (!USART_INIT_ON)
		INIT_USART();
	PORTC_DIRSET=TRANSMIT_PIN;
	USARTC0_CTRLB|=USART_TXEN_bm;
	transmiterENABLE=1;
}

/*************************************************************
Description: Turns the transmitter off
*************************************************************/
void turn_transmitter_off_USART(){
	PORTC_DIRCLR=TRANSMIT_PIN;
	USARTC0_CTRLB=USARTC0_CTRLA&(USART_TXEN_bm^0xFF);
	transmiterENABLE=0;
}

/*************************************************************
Description: turns off the receiver
*************************************************************/
void turn_receiver_off_USART(){
	PORTC_DIRSET=RECEIVE_PIN;
	USARTC0_CTRLB=USARTC0_CTRLA&(USART_RXEN_bm^0xFF);
	receiverENABLE=0;
}


/*************************************************************
Description:sends a character to the USART
*************************************************************/
void send_char_USART(char character){
	if (TRANS_STREAM.length==MAX_STREAM_LENGTH)
		TRANS_STREAM.length=0;
	TRANS_STREAM.stream[TRANS_STREAM.length++]=character;
}

/*************************************************************
Description:sends a string onto the transmitting stream
*************************************************************/
void send_string_USART(char* string){
	for (int i=0;;i++){
		if (string[i]==0)
			break;
		send_char_USART(string[i]);
	}
}

/*************************************************************
Description:sends a word of information (2 bytes) through the 

*************************************************************/
void send_word_USART(uint16_t word){
	send_char_USART((char) (word&0x0F));
	send_char_USART((char) ((word&0xF0)>>4));
}

/*************************************************************
Description:sets the transmit flag to transmit data
*************************************************************/
void transmit_DATA(){
	if (TRANS_STREAM.length!=TRANS_STREAM.location)
		USARTC0.DATA=TRANS_STREAM.stream[TRANS_STREAM.location++];
}
/*************************************************************
Description:receives information onto the stream
*************************************************************/
char recieve_char_USART(){
	if (!receiverENABLE)
		turn_receiver_on_USART();
	do 
	{
	} while (USARTC0_STATUS&USART_RXCINTLVL0_bm!=1);
	return USARTC0.DATA;
}

/*************************************************************
Description: when there is a interrupt for the receiving of information

NOTE this does not prevent the loss of data, if the length
becomes 1000, it will start back at 0 and will overwrite the data
it did before
*************************************************************/
ISR(USARTC0_RXC_vect){
	if (RECEIVE_STREAM.length==MAX_STREAM_LENGTH)
		RECEIVE_STREAM.length=0;	//WARNING: DOES not prevent overwritten data
	RECEIVE_STREAM.stream[RECEIVE_STREAM.length++]=USARTC0.DATA;
	TextReceived=1;
}

/*************************************************************
Description: when the transmitter is ready, we send information
*************************************************************/
ISR(USARTC0_TXC_vect){
	if (TRANS_STREAM.location==MAX_STREAM_LENGTH)
		TRANS_STREAM.location=0;
	if (TRANS_STREAM.length!=TRANS_STREAM.location)
		USARTC0.DATA=TRANS_STREAM.stream[TRANS_STREAM.location++];
}

/*************************************************************
Description:When the transmitter is empty we send information
*************************************************************/
ISR(USARTC0_DRE_vect){
	if (TRANS_STREAM.location==MAX_STREAM_LENGTH)
		TRANS_STREAM.location=0;
	if (TRANS_STREAM.length!=TRANS_STREAM.location)
		USARTC0.DATA=TRANS_STREAM.stream[TRANS_STREAM.location++];
}

#endif /* USART_H_ */
