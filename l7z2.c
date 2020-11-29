#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>



/*2)	Klawiatura sk�ada si� z 16 klawiszy umieszczonych na skrzy�owianiach 4 linii kolumn z 4-ema liniami wierszy. 
Kolumny klawiatury do��czone s� do 4-ech wyj�ciowych linii portu B. Wiersze do��czone s� do 4-ech wej�ciowych linii portu B. 
Nale�y opracowa� program, kt�ry steruje multipleksowo kolumnami, i wy�wietla na linijce LED do��czonej do portu A stany linii 
steruj�cych kolumnami (4 starsze bity)
i stany odczytane z linii wierszy (4 m�odsze bity), jakie mia�y w chwili naci�ni�cia kt�regokolwiek klawisza.
Stan ten wy�wietlany jest tak d�ugo, jak d�ugo linia wej�ciowa w porcie sygnalizuje wci�ni�cie klawisza
Czas reakcji na przyci�ni�cie klawisza na klawiaturze nie d�u�szy ni� 150 ms. 
Czasy odmierzane timerem. Obs�uga klawiszy w obs�udze przerwania od timera. U�y� timera 0. U�y� nak�adki symulatora HAPSIM. 
W symulatorze u�y� wy�wietlaczy LED i modelu klawiatury matrycowej (wybra� jeden wiersz klawiatury).
UWAGA!! HAPSIM niepoprawnie reaguje na podci�gni�cie linii wej�ciowej do zasilania. Instrukcj� podci�gni�cia nale�y w programie "zakomentowa�".
UWAGA!! W HAPSIM po wybraniu modu�u klawiatury matrycowej w opcjach klawiatury zmieni� opis klawiszy tak, �e w lewym g�rnym rogu klawisz
odpowiada cyfrze "0", kolejny w d� w tej kolumnie cyfrze "1" itd, kolumna po kolumnie (klawisz w prawym dolnym rogu odpowiada cyfrze "F")
*/


void initTimer0()
{
    // set up timer with no prescaling
    TCCR0 |= (1 << CS00) | (1 << WGM01);
  
   

	
    OCR0 = 1;/// sets max value, after reaching -interrupt

	TIMSK |= (1 << OCIE0);// allows interrupt

	 
    TCNT0 = 0; // initializes counter
}

uint8_t currentKeyCode=0x00; //value to show on led

uint8_t currentColumn=0b0010000; 

uint8_t currentKeyTime=0; 
ISR(TIMER0_COMP_vect){

  
TCCR0 = 0x00;


PORTB = currentColumn;

uint8_t keyCode=PINB;

if ((keyCode & 0x0f)!=0 )// checks if any button is pushed
{
if(keyCode!=currentKeyCode) // if new key is pushed changes it
{
currentKeyCode=keyCode;

}
else
{

}
currentKeyTime=0;
PORTA = currentKeyCode;
}


//if time of key is above 3 it means it means after 4 interrupts button is not pushed anymore
if (currentKeyTime>3)
{
PORTA = 0x00;
}

currentKeyTime++;



 currentColumn=(currentColumn << 1); // goes to next colummn to scan
if(currentColumn==0x00)
{
currentColumn=0b0010000;// if last column is scaned , goes back to first
}



asm volatile("nop");

   initTimer0();

}




int main (void)
{

cli();//stops interrupts

DDRB=0b11110000; //sets pd2 as input and PD7-4 as output
 



DDRA=0xff; //sets pa as output
PORTA = 0x00;

  initTimer0(); 
sei();//statrs interrupts
    while(1)
    {
	asm volatile("nop");
    }
}

