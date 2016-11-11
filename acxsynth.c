/*
Logiciel de gestion du module MIDI CV/GATE ACS Polyphonique

Alain COUPEL  sept 2010  ver 2.31


PORT A :
RA0	GATE   		0   ( 0= output,  1= input )
RA1	Select DAC2 0
RA2	SDI 		0
RA3	Select DAC1 0
RA4	SCK  		0
RA5	MCLR		1	input pour reset
RA6 NC			0
RA7	LED 		0

TRISA = 00100000

PORT B :
RB0 MONOPOLY	1
RB1 FOURCHANNEL	1
RB2 MIDI Rx		1
RB3 GATE4		0
RB4 GATE3		0
RB5 NC			0
RB6 GATE2		0
RB7	GATE1		0

TRISB = 00000111


			MODE 1:			
CI1  DACa   KBD				
CI1  DACb	KBD	
CI2	 DACa	Velocity		
CI2	 DACb	Modulation	

// Configuration Mask Definitions
#define CONFIG_ADDR	0x2007
// Protection of program code 
#define PROTECT		0x1FFF
#define UNPROTECT	0x3FFF
// CCP1 Pin selection 
#define CCPRB0		0x3FFF
#define CCPRB3		0x2FFF
// In-Circuit Debugger Mode 
#define DEBUGEN		0x37FF
#define DEBUGDIS	0x3FFF
// Flash Program Memory Write Enable 
#define UNPROTECT	0x3FFF
#define WP0		0x3DFF
#define WP1		0x3BFF
#define WPA		0x39FF
// Data EE Memory Code Protection 
#define UNPROTECT	0x3FFF
#define CPD		0x3EFF
// Low Voltage Programming Enable 
#define LVPEN		0x3FFF
#define LVPDIS		0x3F7F
// Brown out detection enable 
#define BOREN		0x3FFF
#define BORDIS		0x3FBF
// Master clear reset pin function
#define MCLREN		0x3FFF
#define MCLRDIS		0x3FDF
// Power up timer enable 
#define PWRTDIS		0x3FFF
#define PWRTEN		0x3FF7
// Watchdog timer enable 
#define WDTEN		0x3FFF
#define WDTDIS		0x3FFB
// Oscillator configurations 
#define RCCLK		0x3FFF
#define RCIO		0x3FFE
#define INTCLK		0x3FFD
#define INTIO		0x3FFC
#define EC		0x3FEF
#define HS		0x3FEE
#define XT		0x3FED
#define LP		0x3FEC

#define CONFIG_ADDR2	0x2008
// Fail Clock Monitor Enable 
#define FCMEN		0x3FFF
#define FCMDIS		0x3FFE
// Internal External Switch Over 
#define IESOEN		0x3FFF
#define IESODIS		0x3FFD

*/
 #include<htc.h>		//Compilateur Hi-Tech PICC
 
__CONFIG(INTIO &WDTDIS & PWRTEN & MCLREN & UNPROTECT & BORDIS & FCMDIS & DEBUGDIS & LVPDIS );

#define		GATE RA0   //Trigger output
#define		CI2 RA1		//DAC N° 2 select
#define		SDI RA2
#define		CI1 RA3
#define		SCK RA4
#define 	LED RA7	

#define		GATE1 RB7
#define		GATE2 RB6
#define		GATE3 RB4
#define		GATE4 RB3

#define		MONOPOLY RB0
#define		FOURCHANNEL RB1

#define		DAC1 0
#define		DAC2 1
#define		DAC3 2
#define		DAC4 3

	

volatile int 
Tx_DATA;


volatile unsigned char
d, i, j, k, n,
Pitch_Wheel_VAL,
nb_MIDI_Clocks,
temp,
CI,
convert,
nb_Byte,
nb_Notes,
Rx_DATA,
Mode,
Mode_Old,
MIDI_Chnl,		// Numéro de canal MIDI
MIDI_Byte1,
MIDI_Byte2,
MIDI_Byte3,		// groupe de 3 octets MIDI
Notes_ARRAY[4],
Track[4];

const unsigned char LogConvert[128]={0, 3, 3, 3, 3, 3,  3,  3,  3,  3, 4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,
5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 
12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 27, 28, 
28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42, 44, 45, 47, 48, 50, 51, 53, 54, 56, 58, 60, 61, 63, 65,
67, 69, 72, 74, 76, 79, 81, 84, 86, 89, 92, 95, 97, 101, 104, 107, 110, 114, 117, 121, 125, 128};
 
  
void delai(void){
	for(i=0;i<255;i++)
	for(j=0;j<120;j++);
}

void clignote(void){
	for(k=0;k<3;k++){
		LED = 1;
		delai();
		LED = 0;
		delai();
		}		
}	

void Init_PIC(void){	
	OSCCON = 0b01111100;		//osc  8 MHz
	TRISA = 0b00100000;			//port A  output
	PORTA = 0b00001010;			// unselect DAC 1 and DAC 2
	TRISB = 0b00000111;			// Port B  input
	ANSEL = 0;					//désactivation des convertisseurs A/N				
	OPTION = 0b00000100;		//RBPU/INTEDG/T0CS/T0SE/PSA/PS2/PS1/PS0. Prescaler 1:32	
}

void Init_Interrupt(void){
	GIE = 1;		//enables global interrupts
	PEIE = 1;		//enables all periph interrupts
	RBIE = 0;		// 0 = disables port change interrupt
	RCIE = 0;		//disable rx interrupts
	TXIE=0;			//disable tx interrupts
}			

void Init_Serial(void){		
	SPBRG=15;			//Value for 31250 bauds:((CLK/(16 * BAUD) -1))
	BRGH=1;				//High Speed
	SYNC=0;				//asynchronous
	SPEN=1;				//enable serial port pin
	CREN=1;				//enable reception
	TXEN = 0;			//Transmit disabled
	SREN=0;				//no effect
	RX9=0;				//8-bit reception
}

void Send_SPI(void){	
	for(n=0;n<16;n++){
	SCK = 0;
	Tx_DATA = Tx_DATA << 1;
	SDI = CARRY;
	SCK = 1;
		}	
}
			
void PlayNote(int DAC_nb){

int xy;
	switch(DAC_nb){
	case 0:
	CI = 0;
	xy = 0x1000;
	break;
	case 1:
	CI = 0;
	xy = 0x9000;
	break;
	case 2:
	CI = 1;
	xy = 0x1000;
	break;
	case 3:
	CI = 1;
	xy = 0x9000;
	}
		
	Tx_DATA = Notes_ARRAY[DAC_nb] - 24;		//Le do grave a la valeur 24 et pas 0
	Tx_DATA = Tx_DATA << 5;
	Tx_DATA = Tx_DATA + Pitch_Wheel_VAL - 63; // 
	Tx_DATA = Tx_DATA + xy;
	if(CI==0){
		RA3 = 0;			 
		Send_SPI();
		RA3 = 1;
		}
	else{
		RA1 = 0;			 
		Send_SPI();
		RA1 = 1;
		}	

}	
	
void PlayCTRL(int DAC_nb){
	
	if(DAC_nb == 3) DAC_nb =0x9000;
	else	DAC_nb = 0x1000;
		Tx_DATA = MIDI_Byte3 <<5; //only the first 6 bits are used (0-5V)
		Tx_DATA = Tx_DATA + DAC_nb; 
		RA1 = 0;			 
		Send_SPI();
		RA1 = 1;		
}	

void TrigON(void){
	TMR0IF = 0;			//disable interrupt flag
	TMR0 = 0;
	TMR0IE = 1;		//enable Timer0 interrupt
}

void GATEON(void){
	nb_Notes++;
	GATE1 = 1;
	GATE2 = 1;
	GATE3 = 1;
	GATE4 = 1;
	TMR0IF = 0;			//disable interrupt flag
	TMR0 = 0;
	TMR0IE = 1;		//enable Timer0 interrupt
}

void GATEOFF(void){
	nb_Notes--;
	if(nb_Notes >128) nb_Notes = 0;
	if(nb_Notes==0){
		GATE1 = 0;
		GATE2 = 0;
		GATE3 = 0;
		GATE4 = 0;	
		}	
}

void interrupt Timer(void){
	TMR0IE = 0;			//disable Timer0 interrupt
	TMR0IF = 0;			//disable interrupt flag

	if(!MONOPOLY){		//mode MONO
		GATE3 = 0;
		GATE4 = 0;
			}		
}

void NoteOFF(void){

//on la recherche dans le tableau

	for(i=0;i<4;i++){
	if(Notes_ARRAY[i]==MIDI_Byte2){
		Track[i] = 0;
		switch(i){
			case 0:
			GATE1 = 0;
			break;
			case 1:
			GATE2 = 0;
			break;
			case 2:
			GATE3 = 0;
			break;
			case 3:
			GATE4 = 0;
			}
		}
	}
}

void NoteON(void){
		
	for(d=0;d<4;d++){
		if(Track[d]==0){
			Notes_ARRAY[d]= MIDI_Byte2 ; 		
			Track[d] = 1;			//le DAC est maintenant occupé
			PlayNote(d);		
		switch(d){
			case 0:
			GATE1 = 1;
			break;
			case 1:
			GATE2 = 1;
			break;
			case 2:
			GATE3 = 1;
			break;
			case 3:
			GATE4 = 1 ;
			}
			d = 4;				// un DAC était libre, donc on sort de la boucle for
		}			
	}
}
		
void TestsMIDI(void){


	if(!MONOPOLY){			//MODE MONOPHONIQUE

		switch(MIDI_Byte1){
				case 0x90:           // Note On 
				if(MIDI_Byte3==0) GATEOFF();
				else {
					Notes_ARRAY[0] = MIDI_Byte2;
					Notes_ARRAY[1] = MIDI_Byte2;									
					PlayNote(DAC1);	//on joue la note sur les sorties 1 et 2
					PlayNote(DAC2);
					GATEON();
					Notes_ARRAY[3] = LogConvert[MIDI_Byte3]; //conversion exponentielle
					PlayCTRL(DAC3);										
					}
				break;  
				                                   
                case 0x80:          // Note Off 
                GATEOFF();
                break;
                                                   
                case 0xE0: 			// pitch wheel
                Pitch_Wheel_VAL = MIDI_Byte3;
                PlayNote(DAC1);
                PlayNote(DAC2);
                break;
                                 
                case 0xB0:			//Modulation
				if(MIDI_Byte2==1)	// Est ce bien la molette de modulation ?
                PlayCTRL(DAC4);     // Sortie 4                                     
       			 } 
			 }       
		else{				//  c'est ici le mode POLYPHONIQUE  
		if(FOURCHANNEL) { // mode polyphonique à 4 canaux MIDI
			
			        switch(MIDI_Byte1){
				case 0x90:           // Note On 
				if(MIDI_Byte3==0) GATE1 = 0;
				else {
					Notes_ARRAY[0] = MIDI_Byte2;									
					PlayNote(DAC1);	//on joue la note sur la sortie 1 
					GATE1 = 1;								
					}
				break;  
				                                   
                case 0x80:          // Note Off 
                GATE1 = 0;
                break;
                
 				case 0x91:           // Note On 
				if(MIDI_Byte3==0) GATE2 = 0;
				else {
					Notes_ARRAY[1] = MIDI_Byte2;									
					PlayNote(DAC2);	//on joue la note sur la sortie 2 
					GATE2 = 1;								
					}
				break;  
				                                   
                case 0x81:          // Note Off 
                GATE2 = 0;
                break;
                
               	case 0x92:           // Note On 
				if(MIDI_Byte3==0) GATE3 = 0;
				else {
					Notes_ARRAY[2] = MIDI_Byte2;									
					PlayNote(DAC3);	//on joue la note sur la sortie 3 
					GATE3 = 1;								
					}
				break;  
				                                   
                case 0x82:          // Note Off 
                GATE3 = 0;
                break;
                
                case 0x93:           // Note On 
				if(MIDI_Byte3==0) GATE4 = 0;
				else {
					Notes_ARRAY[3] = MIDI_Byte2;									
					PlayNote(DAC4);	//on joue la note sur la sortie 4 
					GATE4 = 1;								
					}
				break;  
				                                   
                case 0x83:          // Note Off 
                GATE4 = 0;
                break;                     	
			}
			

  }   //if(fourchannel   
        else{			//Mode polyphonique à 1 canal MIDI
       			  
		switch(MIDI_Byte1){
				case 0x90:          		 // Note On 
				if(MIDI_Byte3==0)	NoteOFF();
				else {
					NoteON();
					TrigON();
					}
				break;
			
				case 0xE0: 			// pitch wheel
                Pitch_Wheel_VAL = MIDI_Byte3;
                PlayNote(DAC1);
                PlayNote(DAC2);
                PlayNote(DAC3);
                PlayNote(DAC4);
                break;                                                     
                case 0x80:          // Note Off 		
                NoteOFF();
                                    
        }//switch(midi
                	        
}
	}//if else
}	
void clear_usart_errors(void){
	if (OERR){																											
	CREN=0;													
	CREN=1;																											
	}															
	if (FERR){															
	Rx_DATA=RCREG;											
	TXEN=0;													
	TXEN=1;												
	}
}
		
main(){
	Init_PIC();
	Init_Serial();
	Init_Interrupt();
	nb_Byte=0;
	nb_Notes = 0;
	GATEOFF();
	for(i=0;i<4;i++)Track[i]= 0;
	clignote();		
	Pitch_Wheel_VAL = 63;



while(MIDI_Byte1!=0x90){				//Autolearn MIDI Channel

      if(RCIF){	        
		clear_usart_errors();
        Rx_DATA=RCREG;
		MIDI_Byte1 = Rx_DATA&0xF0;
		MIDI_Chnl = Rx_DATA&0x0F;
		}
}

for (;;){			// boucle infinie : on peut jouer tant qu'on veut !!
	
	Mode = MONOPOLY;
	Mode = Mode <<1;
	Mode = Mode + FOURCHANNEL;
	if(Mode != Mode_Old){
		GATEOFF();
		nb_Byte = 0;
		Mode_Old = Mode;
	}	
   
     	if(RCIF){
		        
		clear_usart_errors();
        Rx_DATA=RCREG;

	if(Rx_DATA < 0xF0){		// Note

		switch(nb_Byte){
            case 0:
			if(!(Rx_DATA&0x80)){		//Test de Running Status
				nb_Byte = 2;
				MIDI_Byte2 = Rx_DATA;
				}
			else{
			    MIDI_Byte1=Rx_DATA - MIDI_Chnl ;
                nb_Byte = 1;
                
			}//if running status
           	break;
            case 1:           
            MIDI_Byte2=Rx_DATA;
            nb_Byte = 2;
         
          	break;
            case 2:
            MIDI_Byte3=Rx_DATA;
            nb_Byte = 0;
            TestsMIDI();                                                                    
			}//switch(nb_Byte)
		}//if(rx_data
		
	if(Rx_DATA == 0xF8){	// MIDI Clock				 			  
		if(nb_MIDI_Clocks == 0){
			GATE = !GATE;
			LED = GATE;	
			nb_MIDI_Clocks = 12;
			} 
				 nb_MIDI_Clocks--;	
	}
	
	if(Rx_DATA == 0xFA){		//Start. We must reset before starting
			GATE = 1;
			LED = 1;
			nb_MIDI_Clocks = 12;
				} 
	if(Rx_DATA == 0xFB)		//continue
			
	if(Rx_DATA == 0xFC){			//stop
		GATE = 0;
		LED = 0;
		GATE1 = 0;
		GATE2 = 0;
		GATE3 = 0;
		GATE4 = 0;
		nb_MIDI_Clocks = 0;
		}
			                  
	}//if(RCIF)

}//for
	
}//main

