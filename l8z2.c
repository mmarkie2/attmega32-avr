
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


unsigned char text[] PROGMEM = "Good morning";


/*2)     Wyœwietlacz LCD pracuje w trybie, jak wy¿ej. Do PD.2 i PD.3 do³¹czone s¹ przyciski (statycznie 0, po przyciœniêciu 1).
 Przycisk A do PD.2, przycisk B do PD.3.
Po starcie programu wyœwielacz nic nie wyœwietla. Ka¿de wciœniêcie przycisku A wyœwietla na LCD pojedynczy znak z napisu "Good morning" 
(bez cudzys³owu). 
Po osi¹gniêciu koñca napisu, dalsze przyciœniêcia przycisku A nie powoduj¹ ¿adnego skutku.
Ka¿de wciœniêcie przycisku B czyœci LCD. Obs³uga przycisków w przerwaniach zewnêtrznych. Tekst zapisany w tablicy umieszczonej w pamiêci programu. 
Tekst zakoñczony bajtem=0x00. 
    Opcja wykonania programu:
    2c) napis rozwija siê od œrodka ekranu: najpierw "Good " w lewo od œrodka, natêpnie "morning" w prawo od œrodka */

#define F_CPU 1000000UL			
#include <avr/io.h>			
#include <util/delay.h>			




 
//LCD FUNCTIONS
void sendDataToLCD( uint8_t data, uint8_t mode )
{


// if register select==0 sends comand
	if (mode ==0)
	{
		PORTA &= 0b11111011;	
			
	}
	// if register select==1 sends character to display
	else if(mode==1)

	{
	PORTA |= 0b00000100;;
	}

uint8_t upperPart=(data & 0xF0);
	PORTA = (PORTA & 0x0F) |upperPart ; //sending upper part

	PORTA |= 0b00001000;;		//high to low pulse on enable to send data to lcd
	_delay_us(5);
	PORTA &= ~ 0b00001000;;

	_delay_us(200);

uint8_t lowerPart=(data << 4);
	PORTA = (PORTA & 0x0F) | lowerPart;  //sending lower part
	PORTA |=0b00001000;; //high to low pulse on enable to send data to lcd
	_delay_us(5);
	PORTA &= ~ (0b00001000);
	_delay_ms(5);
}




void reset()
{
	sendDataToLCD (0x01,0);		//clears lcd
	_delay_ms(2);
	sendDataToLCD (0x80,0);		//resets cursor


 }
  uint8_t textIter=0;
void setCursorToMiddle()
{

  textIter=0;
reset();

	int i=0;
	for(i=0;i<8;++i)
	{
	sendDataToLCD (0b00010100,0);//moves cursor right
	_delay_us(50);
	}
	sendDataToLCD (0b00000111,0);//sets print direction to left, cursor stays in middle


 }
 
void printCharFromSreing()

{
char data = pgm_read_byte(&(text[textIter]));
if (data=='m')
{
	sendDataToLCD (0b00000110,0);//sets print direction to right
}
if (data==0)
{
}
else
{
 sendDataToLCD( data,1);
++textIter;
}

}

//LCD FUNCTIONS



 ISR(INT0_vect){//A Button
  // interruprs are turned of inside ISR

printCharFromSreing();
PIND&= 0b11111011 ;
}

 ISR(INT1_vect){
  // interruprs are turned of inside ISR
setCursorToMiddle();
PIND&= 0b11110111 ;
}
int main()
{
		////initializes lcd

	DDRA = 0xFF;			//PA as output
	_delay_ms(30);			//waits for lcd to boot 
	
	sendDataToLCD(0x02,0);		//sets 4 bit mode
	sendDataToLCD(0x28,0);             	//sets 2 line, 5x8
	sendDataToLCD(0x0E,0); //cursor not blinking  
reset();

//initializing interrupts
cli() ;// stops interrupts

 

 MCUCR  |= (1 << ISC01 |1 << ISC00 |1 << ISC11 | 1 <<ISC10) ;//  rising edge interrupt
  GIMSK |= (1 << INT0) | (1 << INT1); // enables INT0,INT1 interrupts

  DDRD&= 0b11110011 ; //sets pd2,pd3 as input 
PIND&= 0b11110011 ;
//PORTD = 0x00001100; 
   
sei();// starts interrupts



	_delay_ms(5);

		

setCursorToMiddle();



	while(1){


asm volatile("nop");

	}
}
