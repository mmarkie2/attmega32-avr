#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>


/*/
3)	Program, jak w zadaniu 2. Dodatkowo: 
	a) przycisniêcie klawisza "Up" powoduje przeniesienie wprowadzania tekstu do górnej linii (chyba, ¿e obecnie 
	ju¿ jest ustawione wprowadzanie do górnej linii);
	kursor na pierwszej, dot¹d niezapisanej, pozycji w linii
	b) przycisniêcie klawisza "Down" powoduje przeniesienie wprowadzania tekstu do dolnej linii (chyba, ¿e obecnie 
	ju¿ jest ustawione wprowadzanie do dolnej linii);
	kursor na pierwszej, dot¹d niezapisanej, pozycji w linii
	c) przycisniêcia klawisza "left" powoduje przesuniêcie treœci obecnie aktywnej linii w lewo o 1 pozycjê
	d) przycisniêcia klawisza "right" powoduje przesuniêcie treœci obecnie aktywnej linii w prawo o 1 pozycjê
	e) na przyciœniêcie klawisza "enter" program nie reaguje
/*/


uint8_t printedCharactersCount=0;

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
	_delay_us(50);
}


void clearButton()
{
	sendDataToLCD (0x01,0);		//clears lcd
	_delay_ms(2);
	sendDataToLCD( 0b00000110, 0 );// writing left to right
	sendDataToLCD (0xC0,0);		//sets cursor to second line

	printedCharactersCount=0;

 }


void resetDisplay()
{
	sendDataToLCD (0x01,0);		//clears lcd
	_delay_ms(2);
	sendDataToLCD (0x80,0);		//resets cursor
 }






uint8_t getBinary(uint8_t keyCode) {
  uint8_t result = 0;
// returns index of char in charTable
//NOTE: function returns binary number for every button not only digits, counting from top right corner with 0 as return 
//pressing '1' returns 0 ,2 returns '1'  , 'up' returns 3 , 4 returns '4' etc

  uint8_t row = keyCode & 0x0f;
   //if checks wchich bit is set and assigns it to proper binary number
  
  if (row == 1) {
    result = result + 0;
  } else if (row == 2) {
    result = result + 4;
  } else if (row == 4) {
    result = result + 8;
  } else if (row == 8) {
    result = result + 12;
  }
  
  uint8_t col = keyCode & 0xf0;
  //if checks wchich bit is set and assigns it to proper binary number
  if (col == 16) {
    result = result + 0;
  } else if (col == 32) {
    result = result + 1;
  } else if (col == 64) {
    result = result + 2;
  } else if (col == 128) {
    result = result + 3;
  }
  
  return result;
}
char charTable[]= {'1','2','3','U',
					'4','5','6','D',
					'7','8','9','R',
					'C','0','E','L'};




void presenterSendDataToLCD( uint8_t data, uint8_t mode )
//checks input before sending to screen
{
if (data == 'C') //clear
{
clearButton();

}
else if ( data == 'E' )

{
//do nothing
}
else if (data == 'L' )
{

sendDataToLCD( 0x18, 0 );
}
else if (data == 'R' )
{
sendDataToLCD( 0x1C, 0 );
}
else if (data == 'U' )
{

}
else if (data == 'D' )
{
;

}

else
{
if(printedCharactersCount==16)
{
sendDataToLCD( 0b00000111, 0 );// makes display move to the left while writting
	

}
++printedCharactersCount;
sendDataToLCD( data, mode );
}

}

// PINPAD TIMER
void initTimer0()
{
    // set up timer with no prescaling
    TCCR0 |= (1 << CS00) | (1 << WGM01);
  
   

	
    OCR0 = 1;/// sets max value, after reaching -interrupt

	TIMSK |= (1 << OCIE0);// allows interrupt

	 
    TCNT0 = 0; // initializes counter
}



uint8_t currentKeyCode=0x00; //value to show on led

uint8_t currentColumn=0b00010000; 

uint8_t currentKeyTime=0;

 
ISR(TIMER0_COMP_vect){


  
TCCR0 = 0x00;


PORTB = currentColumn;
	asm volatile("nop");// nop added to give time to processor to read proper value
uint8_t keyCode=PINB;

if ((keyCode & 0x0f)!=0 // checks if any button is pushed
)

{
	if(keyCode!=currentKeyCode) // if new key is pushed changes it
		{
		currentKeyCode=keyCode;
		uint8_t binaryCode=getBinary(currentKeyCode);

		presenterSendDataToLCD(charTable[binaryCode],1);
		
		}
	else
		{

		}
	currentKeyTime=0;

}


//if time of key is above 3  it means after 4 interrupts button is not pushed anymore
if (currentKeyTime>3)
	{
	currentKeyCode= 0x00;

	}
else
	{
	currentKeyTime++;
	}





 currentColumn=(currentColumn << 1); // goes to next colummn to scan
if(currentColumn==0x00)
{
currentColumn=0b00010000;// if last column is scaned , goes back to first
}



asm volatile("nop");

   initTimer0();

}

// PINPAD TIMER


int main (void)
{

cli();//stops interrupts

DDRB=0b11110000; //sets pd2 as input and PD7-4 as output
 
		////initializes lcd

	DDRA = 0xFF;			//PA as output
	_delay_ms(30);			//waits for lcd to boot 
	
	sendDataToLCD(0x02,0);		//sets 4 bit mode

	sendDataToLCD(0x28,0);             	//sets 2 line, 5x8
	
resetDisplay();
///initializes lcd

  initTimer0(); 
sei();//statrs interrupts



    while(1)
    {
	asm volatile("nop");
    }
}
