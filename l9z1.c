#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>


/*/
1) 	Wyœwietlacz LCD skonfigurowany jest do wyœwietlania 2 linii, format znaku 5x8. Interfejs 4-bitowy. Program przyjmuje z klawiatury matrycowej
 liczbê sk³adaj¹ca 
siê z max 16 cyfr dziesiêtnych (blokada pobierania, przy próbie wprowadzenia wiêkszej liczby cyfr). Wyœwietla liczbê, w miarê wprowadzania
 kolejnych cyfr, 
na wyœwietlaczu LCD. Ponadto, program reaguje na przyciœniêcie klawisza "Clear" wyczyszczeniem wyœwietlacza (kursor do lewego rogu). 
Przycisniêcia pozosta³ych klawiszy klawiatury s¹ ignorowane. 
Czas reakcji na przyciœniêcie klawisza na klawiaturze nie d³u¿szy ni¿ 40 ms. Czas odmierzany timerem. Odczyt klawiatury realizowany
 multipleksowo w obs³udze 
przerwania od timera. Klawiatura do³¹czona do portu B. 
/*/




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
	_delay_us(5);
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


uint8_t  printedCharactersCount=0;

void presenterSendDataToLCD( uint8_t data, uint8_t mode )
//checks input before sending to screen
{
if (data == 'C') //clear
{
resetDisplay();
printedCharactersCount=0;
}
else if (data == 'U' || data == 'D' || data == 'R' || data == 'L' || data == 'E' )

{
//do nothing
}

else
{
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
&& printedCharactersCount<16)// checks if  there is space for characters 

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
