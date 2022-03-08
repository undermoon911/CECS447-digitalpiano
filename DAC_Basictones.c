

// DAC_Basictones.c
// Runs on TM4C123
// Use SysTick interrupts to implement a sine wave for each C Major notes
// Min He
// January 11, 2021

// This example shows how to generate music tones from a 3-bit DAC outputs.
// The program will first play 8 C Major notes automatically, 
// it will then allow user to play the same 8 C mojor notes manually using 
// the onboard push button SW1: press and hold SW1 will play one note;
// release SW1 will stop playing the note. The same operation can be repeated
// to play more notes in C major table in rising order and then repeat.
// System configuration: system clock is 16MHz, Systick timer is 
// used to control output rate. A Tone_Tab is used to represent 
// number of clocks for each tone frequency
// A 3-bit DAC is used to convert output from digital to analog.
// There are 16 values for each tone period.
// Port B bits 2-0 have the 3-bit DAC.
// Port F is onboard LaunchPad switches and LED.
// Port F bit 4 is negative logic switch to play sound, SW1.
// SysTick ISR: PF3 ISR heartbeat.


#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "SwitchLed.h"

// initial values for piano major tones: 
// Assume system clock is 16MHz
const unsigned long Piano[]={
30534*2,27211*2,24242*2,22923*2,
15289*2,13621*2,12135*2,11454*2,
7645*2,6810*2,6067*2,5727*2,0
};
const unsigned long Happy_Birthday[] = {
10204,4,10204,2,9091,4,10204,4,7645,4,8099,4,
  10204,2,10204,2,9091,4,10204,4,6810,4,7645,4,
	10204,2,10204,2,5102,4,6067,4,7645,4,8099,4,9091,8,
	5727,2,5727,6067,4,7645,4,6810,4,7645,8,0,0

};
const unsigned long Little_Lamb[]={
6067,4,6810,4,7645,4,6810,4,6067,4,6067,4,6067,8,
6810,4,6810,4,6810,8,6067,4,5102,4,5102,8,
6067,4,6810,4,7645,4,6810,4,6067,4,6067,4,6067,8,
6810,4,6810,4,6067,4,6067,4,6810,4,7645,8,0,0


};

const unsigned long Little_Starts[]={
7645,4,7645,4,5102,4,5102,4,4545,4,4545,4,5102,8,5727,4,5727,4,6067,4,6067,4,6810,4,6810,4,6810,8,
5102,4,5102,4,5727,4,5727,4,6067,4,6067,4,6810,8,5102,4,5102,4,5727,4,5727,4,6067,4,6067,4,6810,8,
7645,4,7645,4,5102,4,5102,4,4545,4,4545,4,5102,8,5727,4,5727,4,6067,4,6067,4,6810,4,6810,4,7645,8,0,0
	
};

//unsigned long Songselect; 
unsigned long mode,counter,key,input,previous,Pcounter,Pmode ;
unsigned char pressed;
//static unsigned long c_note;
#define NUM_VALs  64  // Assume 6-bit DAC is used, that will give 64 values for one period.
#define NUM_NOTEs 8   // number of notes to be played repeatedly




// basic functions defined at end of startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode


void PortE_Init(void);

int main(void){ 
	unsigned char i,j,a; 
 
	
  DisableInterrupts();    
  SwitchLed_Init();       // use Port F for onboard switches and LEDs
  DAC_Init();          // Port B is DAC
  PortE_Init();           // Port E is piano input PD 0-3
	mode=0;             //mode 0 means piano mode 
	counter = 0;						// counter has different use in different mode,also keep the data
													// on how many time the PF0 is pressed
	Pmode =0;
	Pcounter =0;
	EnableInterrupts();

  while(1){

		if (mode == 0){//piano mode
		key = GPIO_PORTE_DATA_R;
		previous = 0x00;
			if(key == 0x01){
				input = GPIO_PORTE_DATA_R&0x01; // bit 0 means SW0 pressed
				if(input&&(previous==0)){ // just pressed     
						
						a = 0 + counter*4;
						Sound_Init(Piano[a]/NUM_VALs);      // Play 8 major notes
			
    }
    if(previous&&(input==0)){ // just released     
     Sound_stop();
    }
    previous = input; 
    Delay10ms();  // remove switch bounce 
		}
			
			else if(key == 0x02){
				input = GPIO_PORTE_DATA_R&0x02; // bit 1 means SW1 pressed
				if(input&&(previous==0)){ // just pressed     
						
						a = 1 + counter*4;
						Sound_Init(Piano[a]/NUM_VALs);      // Play 8 major notes
			
    }
    if(previous&&(input==0)){ // just released     
      Sound_stop();    // stop sound
    }
    previous = input; 
    Delay10ms();  // remove switch bounce
	}
			else if(key == 0x04){
				input = GPIO_PORTE_DATA_R&0x04; // bit 2 means SW2 pressed
				if(input&&(previous==0)){ // just pressed     
						
						a = 2 + counter*4;
						Sound_Init(Piano[a]/NUM_VALs);      // Play 8 major notes
			
    }
				if(previous&&(input==0)){ // just released     
						Sound_stop();   // stop sound
    }
				previous = input; 
				Delay10ms();  // remove switch bounce
		}
			
			else if(key == 0x08){
				input = GPIO_PORTE_DATA_R&0x08; // bit 3 means SW3 pressed
				if(input&&(previous==0)){ // just pressed     
						
						a = 3 + counter*4;
						Sound_Init(Piano[a]/NUM_VALs);      // Play 8 major notes
			
			
    }
    if(previous&&(input==0)){ // just released     
     Sound_stop();;    // stop sound
    }
    previous = input; 
    Delay10ms();  // remove switch bounce
	}
			
			else { // when none of the key was pressed
			a = 12;										// it's quite.
      Sound_Init(Piano[a]/NUM_VALs);}
			
			
		} 
		else if(mode ==1){//atuo-play mode
		i = 0;
		if(counter == 0 )
			{ 
				Pcounter = counter;
				Pmode = mode;
				while (Pcounter == counter && mode == 1){
						Sound_Init(Happy_Birthday[i]/NUM_VALs);
					for (j=0;j<20;j++) 
						Delay10ms();
						i = i+1;
						if (i == 55){i = 0;}
				}

		
		}
			else if(counter == 1  )
			{ 
				Pcounter = counter;
				Pmode = mode;
				while (Pcounter == counter && mode == 1){
						Sound_Init(Little_Lamb[i]/NUM_VALs);
					for (j=0;j<20;j++) 
						Delay10ms();
						i = i+1;
						if (i == 55){i = 0;}
				}

		
		}
			else if(counter == 2 )
			{ 
				Pcounter = counter;
				Pmode = mode;
				while (Pcounter == counter && mode == 1){
						Sound_Init(Little_Starts[i]/NUM_VALs);
					for (j=0;j<20;j++) 
						Delay10ms();
						i = i+1;
						if (i == 55){i = 0;}
				}


		}
}
}


}

void PortE_Init(){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;           // 1) E clock
  delay = SYSCTL_RCGC2_R;           // delay       
  GPIO_PORTE_AMSEL_R &= ~0x0F;        // 3) disable analog function
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x0F;          // 5) PE3-0  input   
  GPIO_PORTE_AFSEL_R &= ~0x0F;        // 6) no alternate function       
	GPIO_PORTE_DEN_R |= 0x0F;          // 7) enable digital pins PE3-0
	GPIO_PORTE_IS_R &= ~0x0F;
	GPIO_PORTE_IBE_R &= ~0x0F;
	GPIO_PORTE_IEV_R &= ~0x0F;
	GPIO_PORTE_ICR_R = 0x0F;
	GPIO_PORTE_IM_R |= 0x0F;
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFFFF1FFF)|0x00040000; // (g) 
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	}


void GPIOPortF_Handler(void) {
  Delay10ms();	
	Delay10ms();
	
	if(GPIO_PORTF_RIS_R&0x01){// when the PF0 is pressed
		GPIO_PORTF_ICR_R = 0x01;// the counter goes up
		if(counter ==0){				// in different mode couter has different function
			Delay10ms();					// in mode 0 which is the piano mode,counter can change the tone 
				counter = 1;				// in mode 1 which is the auto play mode, counter can change between songs.
		Delay10ms();}
		else if (counter ==1){
		Delay10ms();
		counter = 2;
		Delay10ms();}
	
	  
	else if (counter ==2){
		Delay10ms();
		counter = 0;
		Delay10ms();}
	}
	
	if(	GPIO_PORTF_RIS_R&0x10)// when the PF4 is pressed,the mode will change between 0 and 1 
	{ 												// and change between different mode.
		GPIO_PORTF_ICR_R |= 0x10;
		if(mode == 0){mode = 1;}
		else{mode =0;}
	}
}
	


           