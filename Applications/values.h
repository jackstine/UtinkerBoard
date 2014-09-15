/*************************************************************
values.h
NAME: Cukjati Jacob IV
Description: list of some useful values
*************************************************************/


#ifndef VALUES_H_
#define VALUES_H_

#define bit0 0x01
#define bit1 0x02
#define bit2 0x04
#define bit3 0x08
#define bit4 0x10
#define bit5 0x20
#define bit6 0x40
#define bit7 0x80
#define zero 0x00
#define F_CPU 2000000
#define MAX_STREAM_LENGTH 1000
#define NOTE_C 16.35
#define NOTE_D 18.35
#define NOTE_E 20.6
#define NOTE_F 21.83
#define NOTE_G 24.5
#define NOTE_A 27.5
#define NOTE_B 30.87
#define NOTE_C_SHAPE 17.32
#define NOTE_D_FLAT 17.32
#define NOTE_D_SHARP 19.45
#define NOTE_E_FLAT 19.45
#define NOTE_F_SHARP 23.12
#define NOTE_G_FLAT 23.12
#define NOTE_A_SHARP 29.14
#define NOTE_B_FLAT 29.14
#define NOTE_G_SHARP 25.96
#define NOTE_A_FLAT 25.96
#define CPU_freq 2000000
#define BEATS 64
#define MUSICAL_TIME 624	//TODO
#define MAX_KEY_LENGTH 20
#define MIN_KEY_LENGTH 10
#define MAX_INPUT_CAPTURE_LENGTH 100

/*************************************************************
Description:These are different types of streams,
each one has a 
length, which is the highest unit of information
that can be reached in the array of elements

Location: The location at which has been used up, up to
the current length, being the max location that one can 
reach
*************************************************************/
struct stream8{
	uint8_t stream[MAX_STREAM_LENGTH];
	unsigned int length;
	unsigned int location;
};

struct stream16{
	uint16_t stream[MAX_STREAM_LENGTH];
	unsigned int length;
	unsigned int location;
};


struct I2C_stream8{
	uint8_t stream[MAX_STREAM_LENGTH];
	unsigned int length;
	unsigned int location;
	uint8_t address;
};


struct IR_key{
	uint8_t stream[MAX_KEY_LENGTH];
	uint8_t length;
	uint8_t location;
};

//structure for notes
struct note{
	float freq;
	char* freq_name;
	uint8_t name_length;
	uint8_t tone;
	int time;
};

struct musicalNote{
	float freq;
	int beat;
	uint8_t tone;
};

/*************************************************************
Description:Values for the stream of the pulse widths
and the length of the pulse widths
*************************************************************/
struct key{
	uint16_t stream[MAX_INPUT_CAPTURE_LENGTH];
	uint8_t length;
	uint8_t location;
};

/*************************************************************
Description: used to convert 4 bit number to hex decimal value
*************************************************************/
char convert_to_hex(uint8_t hex){
	if (hex>=0 && hex<=9)
		return (hex+48);
	else
		return (hex+55);
}

/*************************************************************
Description: returns the CHAR value of the hex value
*************************************************************/
char get_hex_value(int hex,uint8_t unit){
	uint8_t hexMask=0x0F;
	uint8_t hexMark=4;
	uint8_t volatile mark=hex>>(unit*hexMark);
	uint8_t volatile bitMask=mark&hexMask;
	char volatile hexChar=convert_to_hex(bitMask);
	return hexChar;
}

get_frequency(uint8_t tone,float freq){
	for (int i=0;i<=(tone-1);i++)	//set the frequency for the CC
		freq=freq*2;
		
	uint16_t volatile freq_time=(uint16_t) ((CPU_freq/(freq*2))-1);
	return freq_time;
}

/*************************************************************
Description: returns a note from other information given
ex:  c 6 5000 'n' "C6"   ---> creates a C6 note for 5 sec
*************************************************************/
struct note create_note(char Note,uint8_t tone,int toneLength,char sharp_flat,char* freq_name,uint8_t name_length){
	struct note volatile newNote;
	float freq;
	//get the note
	if (Note>=0x61)
	Note=Note-0x20;
	
	switch(Note){
		case 'a':
		case 'A':
		if (sharp_flat=='s')
		freq=NOTE_A_SHARP;
		else if (sharp_flat=='f')
		freq=NOTE_A_FLAT;
		else
		freq=NOTE_A;
		break;
		
		case 'b':
		case 'B':
		if (sharp_flat=='f')
		freq=NOTE_B_FLAT;
		else
		freq=NOTE_B;
		break;
		
		case 'c':
		case 'C':
		if (sharp_flat=='s')
		freq=NOTE_C_SHAPE;
		else
		freq=NOTE_C;
		break;

		case 'd':
		case 'D':
		if (sharp_flat=='s')
		freq=NOTE_D_SHARP;
		else if (sharp_flat=='f')
		freq=NOTE_D_FLAT;
		else
		freq=NOTE_D;
		break;
		
		case 'e':
		case 'E':
		if (sharp_flat=='f')
		freq=NOTE_E_FLAT;
		else
		freq=NOTE_E;
		break;
		

		case 'f':
		case 'F':
		if (sharp_flat=='s')
		freq=NOTE_F_SHARP;
		else
		freq=NOTE_F;
		break;

		case 'g':
		case 'G':
		if (sharp_flat=='s')
		freq=NOTE_G_SHARP;
		else if (sharp_flat=='f')
		freq=NOTE_G_FLAT;
		else
		freq=NOTE_G;
		break;
	}
	
	newNote.freq_name=freq_name;
	newNote.freq=freq;
	newNote.tone=tone;
	newNote.time=toneLength;
	newNote.name_length=name_length;
	return newNote;
}


/*************************************************************
NAME: Cukjati Jacob
Description:creates musical notes occurring to the number of beats
*************************************************************/
struct musicalNote create_musical_note(char Note,uint8_t tone,int beat,char sharp_flat){
	struct musicalNote volatile newNote;
	float freq;
	int time;

	//get the note
	if (Note>=0x61)
	Note=Note-0x20;
	
	switch(Note){
		case 'a':
		case 'A':
		if (sharp_flat=='s')
		freq=NOTE_A_SHARP;
		else if (sharp_flat=='f')
		freq=NOTE_A_FLAT;
		else
		freq=NOTE_A;
		break;
		
		case 'b':
		case 'B':
		if (sharp_flat=='f')
		freq=NOTE_B_FLAT;
		else
		freq=NOTE_B;
		break;
		
		case 'c':
		case 'C':
		if (sharp_flat=='s')
		freq=NOTE_C_SHAPE;
		else
		freq=NOTE_C;
		break;

		case 'd':
		case 'D':
		if (sharp_flat=='s')
		freq=NOTE_D_SHARP;
		else if (sharp_flat=='f')
		freq=NOTE_D_FLAT;
		else
		freq=NOTE_D;
		break;
		
		case 'e':
		case 'E':
		if (sharp_flat=='f')
		freq=NOTE_E_FLAT;
		else
		freq=NOTE_E;
		break;
		

		case 'f':
		case 'F':
		if (sharp_flat=='s')
		freq=NOTE_F_SHARP;
		else
		freq=NOTE_F;
		break;

		case 'g':
		case 'G':
		if (sharp_flat=='s')
		freq=NOTE_G_SHARP;
		else if (sharp_flat=='f')
		freq=NOTE_G_FLAT;
		else
		freq=NOTE_G;
		break;
	}
	
	switch(beat){
		case 1:
		time= BEATS * (MUSICAL_TIME);
		break;
		case 2:
		time=(BEATS>>1)*MUSICAL_TIME;
		break;
		case 4:
		time=(BEATS>>2)*MUSICAL_TIME;
		break;
		case 8:
		time=(BEATS>>3)*MUSICAL_TIME;
		break;
		case 16:
		time=(BEATS>>4)*MUSICAL_TIME;
		break;
	}
	
	newNote.freq=freq;
	newNote.tone=tone;
	newNote.beat=time;
	return newNote;
}


/*************************************************************
Description: sets the correct note from the keypad button
and returns the note
*************************************************************/
struct note create_note_from_KEYPAD(uint8_t key){
	struct note created_note;
	switch(key){
		case 1:
		created_note=create_note('c',6,2000,'n',"C6",2);
		break;
		case 2:
		created_note=create_note('c',6,2000,'s',"C#6/Db6",7);
		break;
		case 3:
		created_note=create_note('d',6,2000,'n',"D6",2);
		break;
		case 4:
		created_note=create_note('d',6,2000,'s',"D#6/Eb6",7);
		break;
		case 5:
		created_note=create_note('e',6,2000,'n',"E6",2);
		break;
		case 6:
		created_note=create_note('f',6,2000,'n',"F6",2);
		break;
		case 7:
		created_note=create_note('f',6,2000,'s',"F#6/Gb6",7);
		break;
		case 8:
		created_note=create_note('g',6,2000,'n',"G6",2);
		break;
		case 9:
		created_note=create_note('g',6,2000,'s',"G#6/Ab6",7);
		break;
		case 0:
		created_note=create_note('a',6,2000,'n',"A6",2);
		break;
		case 10:
		created_note=create_note('a',6,2000,'s',"A#6/Bb6",7);
		break;
		case 11:
		created_note=create_note('b',6,2000,'n',"B6",2);
		break;
		case 12:
		created_note=create_note('c',7,2000,'n',"C7",2);
		break;
		case 13:
		created_note=create_note('c',7,2000,'s',"C#7/Db7",7);
		break;
	}
	return created_note;
}


struct musicalNote* create_space_odyssey(){
			struct musicalNote space[]={
					create_musical_note('f',4,'2','n'),
					create_musical_note('c',5,2,'n'),
					create_musical_note('f',5,1,'n'),
					create_musical_note('c',5,16,'n'),
					create_musical_note('f',5,1,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),

					create_musical_note('f',1,'2','n'),
					create_musical_note('c',5,2,'n'),
					create_musical_note('f',5,1,'n'),
					create_musical_note('c',5,16,'n'),
					create_musical_note('f',5,1,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),

					create_musical_note('f',4,'2','n'),
					create_musical_note('c',5,2,'n'),
					create_musical_note('f',5,1,'n'),
					create_musical_note('c',5,16,'n'),
					create_musical_note('f',5,1,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),
					create_musical_note('c',5,8,'n'),
					create_musical_note('g',5,8,'n'),


					create_musical_note('d',5,16,'n'),
					create_musical_note('e',5,16,'n'),
					create_musical_note('f',5,8,'n'),
					create_musical_note('f',5,2,'n'),
					create_musical_note('g',5,4,'n'),
					create_musical_note('d',5,8,'n'),
					create_musical_note('e',5,8,'n'),
					create_musical_note('f',5,2,'n'),
					create_musical_note('f',5,8,'n'),
					create_musical_note('f',5,8,'n'),
					create_musical_note('d',5,16,'n'),
					create_musical_note('e',5,16,'n'),

					create_musical_note('f',5,2,'n'),
					create_musical_note('g',5,4,'n'),
					create_musical_note('a',6,4,'n'),
					create_musical_note('b',6,4,'n'),

					create_musical_note('b',6,2,'n'),
				create_musical_note('e',5,2,'n')};
				
			return space;
}

struct note* create_descending_order(){
				struct note c7=create_note('c',7,4744,'n',"c7",2);
				struct note c6=create_note('c',6,4744,'n',"c6",2);
				struct note d6=create_note('d',6,4744,'n',"d6",2);
				struct note e6=create_note('e',6,4744,'n',"e6",2);
				struct note f6=create_note('f',6,4744,'n',"f6",2);
				struct note g6=create_note('g',6,4744,'n',"g6",2);
				struct note a6=create_note('a',6,4744,'n',"g6",2);
				struct note b6=create_note('b',6,4744,'n',"g6",2);
				struct note queueOfNotes[]={c7,b6,a6,g6,f6,e6,d6,c6};
	return queueOfNotes;
}

struct note* create_ascending_order(){
				struct note c6=create_note('c',6,4744,'n',"c6",2);
				struct note d6=create_note('d',6,4744,'n',"d6",2);
				struct note e6=create_note('e',6,4744,'n',"e6",2);
				struct note f6=create_note('f',6,4744,'n',"f6",2);
				struct note g6=create_note('g',6,4744,'n',"g6",2);
				struct note a6=create_note('a',6,4744,'n',"g6",2);
				struct note b6=create_note('b',6,4744,'n',"g6",2);
				struct note c7=create_note('c',7,4744,'n',"c7",2);
				struct note queueOfNotes[]={c6,d6,e6,f6,g6,a6,b6,c7};
				return queueOfNotes;
}
#endif /* VALUES_H_ */
