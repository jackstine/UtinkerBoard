/*************************************************************
PezioSpeaker_INIT.h
NAME: Cukjati Jacob IV
Description: The header file for PezioSpeaker
*************************************************************/


#ifndef PEZIOSPEAKER_INIT_H_
#define PEZIOSPEAKER_INIT_H_

#include "values.h"
#include <avr/io.h>
#include "Interrupts.h"

uint8_t static volatile speakeroff=1;
uint8_t static volatile speakerINIT=0;
uint8_t	static volatile timerINIT=0;
uint8_t static speakerSound=0;
int speaker_timer=0;
int speaker_count=0;
uint8_t static volatile playingSOUND;


void SPEAKER_INIT();
void speaker_timer_INIT(int);
void play_note(struct note);
void speaker_timer_ON(int);
void speaker_timer_OFF();
struct note create_note(char,uint8_t,int,char,char*,uint8_t);
struct note create_note_from_KEYPAD(uint8_t);
void turn_speaker_off();
void turn_speaker_on();


/*************************************************************
Description: INITS the Speaker
*************************************************************/
void SPEAKER_INIT(){
	speakeroff=0;
	speakerINIT=1;
	PORTF_DIRSET=0x01;	//set the port for PORT F TC0B/CCB
	TCF0_CTRLA=TC_CLKSEL_DIV1_gc;
	TCF0_CTRLB=TC0_CCAEN_bm|TC_WGMODE_FRQ_gc;	//select CCB and Frequency mode
}


/*************************************************************
Description: turns off the speaker
*************************************************************/
void turn_speaker_off(){
	TCF0_CTRLA=TC_CLKSEL_OFF_gc;
	speakeroff=1;	
}

/*************************************************************

Description: turns on the speaker
*************************************************************/
void turn_speaker_on(){
		TCF0_CTRLA=TC_CLKSEL_DIV1_gc;	//Prescaler CLK 1
		speakeroff=0;
}

/*************************************************************
INPUTS:time
Description: sets a timer for time*0.1ms
*************************************************************/
void speaker_timer_INIT(int time){
	set_HIGH_INT();
	TCC1_CTRLA=0x01;	//Prescaler CLK 1
	TCC1_INTCTRLA=0x02;
	TCC1_PER=0x00C8;	//for 0.1 millisecond
	speaker_timer=time;	//set timer to time
	speaker_count=0;	//set count to 0
	speakerSound=0;
	sei();
}

/*************************************************************
INPUTS:time
Description: turns on the timer for time*0.1ms
*************************************************************/
void speaker_timer_ON(int time){
	speaker_timer_INIT(time);
}

/*************************************************************
Description: turns off the timer
*************************************************************/
void speaker_timer_OFF(){
	TCC1_CTRLA=0x00;
}


/*************************************************************
Description: INT Vector for the timer, when count==timer
the speaker is turned off and the timer is turned off
*************************************************************/
ISR(TCC1_OVF_vect){
	if (speaker_count==speaker_timer){
		turn_speaker_off();
		speaker_timer_OFF();
		speakerSound=0;
		playingSOUND=0;
		speaker_count=0;
	}
	else
		speaker_count++;
}

/*************************************************************
INPUTS: sound, *playingSound
Description: plays a note until the time on the note is 
complete the minimum time duration is 0.1 milliseconds
*************************************************************/
void play_note(struct note volatile sound){
	if (!(speakerINIT))		//if speaker is not INIT do it
		SPEAKER_INIT();
	if (speakeroff)		//if speaker is off turn it on
		turn_speaker_on();
		
	TCF0_CCA=get_frequency(sound.tone,sound.freq);
	if (sound.time!=0x00)	//set the timer if time>0
		speaker_timer_ON(sound.time);
	playingSOUND=1;	//a sound is playing
}

/*************************************************************
Description:PLays a musical song note
*************************************************************/
void play_musical_note(struct musicalNote volatile sound){
	if (!(speakerINIT))		//if speaker is not INIT do it
		SPEAKER_INIT();
	if (speakeroff)		//if speaker is off turn it on
		turn_speaker_on();

	TCF0_CCA=get_frequency(sound.tone,sound.freq);	//set CC
	if (sound.beat!=0x00)	//set the timer if time>0
		speaker_timer_ON(sound.beat);
	playingSOUND=1;	//a sound is playing
}

/*************************************************************
Description:PLays a queue of notes
*************************************************************/
void play_queue_of_notes(struct note queue[],uint8_t queue_length){
	int i=0;
	while(i<queue_length){
		if (!(playingSOUND)){
			play_note(queue[i]);
			i++;
		}
	}
}

/*************************************************************
Description: plays a song
TODO make this interrupt serviced
*************************************************************/
void play_song(struct musicalNote queue[],uint8_t queue_length){
	int i=0;
	while(i<queue_length){
		if (!(playingSOUND)){
			//all songs notes have breaks
			delay_ms(20);
			play_musical_note(queue[i]);
			i++;
		}
	}
}



/*************************************************************
Description: sets the speaker for a sound frequency
and plays it forever until interrupted
*************************************************************/
void play_speaker(struct note sound){
		if (!(speakerINIT))
			SPEAKER_INIT();
		if (speakeroff)
			turn_speaker_on();
		TCF0_CCA=get_frequency(sound.tone,sound.freq);
}

#endif /* PEZIOSPEAKER_INIT_H_ */
