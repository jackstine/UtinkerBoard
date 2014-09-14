/*
 * ADC_INIT.h
 *
 * Created: 7/25/2014 5:55:25 PM
 *  Author: jacob_000
 */ 


#ifndef ADC_INIT_H_
#define ADC_INIT_H_

#include <avr/io.h>
#include "Interrupts.h"

#define ADC_TOP 4095
#define VREF 5

uint16_t volatile ADC_RES;
float volatile ADC_VOLT=0;

/*************************************************************
Description: INITS the ADC voltage converter
*************************************************************/
void INIT_ADC(){
	PORTA_DIRCLR=0xFF;
	ADCA_CH1_CTRL=ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA_REFCTRL=ADC_REFSEL_INTVCC_gc;	//for internal Vcc/1.6
	ADCA_CTRLB=ADC_RESOLUTION_12BIT_gc;//|ADC_FREERUN_bm;
	//ADCA_EVCTRL=ADC_SWEEP_01_gc|ADC_EVACT_SWEEP_gc;
	ADCA_CH1_MUXCTRL=ADC_CH_MUXPOS_PIN1_gc;
	ADCA_CTRLA=ADC_ENABLE_bm;
	ADCA_CH1_CTRL=ADC_CH_START_bm;
	set_HIGH_INT();
	ADCA_CH1_INTCTRL=ADC_CH_INTLVL_HI_gc;
	sei();	
}

/*************************************************************
Description: The interrupt to read the voltage and calculate it
*************************************************************/
ISR(ADCA_CH1_vect){
	ADC_RES=ADCA_CH1_RES;
	float volatile floating=ADC_RES*VREF;
	ADC_VOLT=(floating)/((ADC_TOP)+1);
}

#endif /* ADC_INIT_H_ */
