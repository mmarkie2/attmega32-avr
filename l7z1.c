#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



/*1)	1)	Klawiatura sk�ada si� z 4-ech klawiszy umieszczonych w jednym wierszu. Wiersz do��czony jest do pojedynczej wej�ciowej 
linii portu D b�d�cej lini� wej�ciow� 
przerwania zewnetrznego INT0. Kolumny klawiatury do��czone s� do 4-ech wyj�ciowych linii portu D (PD7-4). Opracowa� program,
 kt�ry rozpoznaje numer wci�ni�tego 
klawisza z pojedynego wiersza klawiatury.
Program w celu zidentyfkowania, kt�ry klawisz zosta� wci�ni�ty, po rozpoznaniu zdarzenia wci�ni�cia jednorazowo wykonuje operacj� 
zmiany kierunku linii u�ywanej do sygnalizacji INT0 oraz linii PD7-4, podaje na PD.2 stan 1 i czyta stan, pracuj�cych teraz w trybie wej�cia,
 linii PD7-4.
Numer klawisza liczony od lewej od 1 ( liczba w zakresie od 1 do 4) wy�wietlany na linijce LED za po�rednictwem PORTA.
W stanie statycznym wszystkie linie musz� mie� w�a�ciwe kierunki, a na liniach PD7-4 musi by� stan 1. 
Obs�uga klawiatury (WY��CZNIE!) w procedurze obs�ugi przerwania zewn�trznego.
W symulatorze u�y� wy�wietlaczy LED modelu klawiatury matrycowej (wybra� jeden wiersz klawiatury).
*/

uint8_t getBinary(uint8_t keyCode) {
  uint8_t result = 0;
  keyCode = ~keyCode;

  uint8_t row = keyCode & 0xf0;
  //if checks wchich bit is set and assigns it to proper binary number
  if (row == 16) {
    result = result + 0;
  } else if (row == 32) {
    result = result + 1;
  } else if (row == 64) {
    result = result + 2;
  } else if (row == 128) {
    result = result + 3;
  }
  return result;
}


ISR(INT0_vect){
  // interruprs are turned of inside ISR

DDRD=0b00001111; //sets pd2 as output and PD7-4 as  input

asm volatile("nop"); // no instrucions to give time for proccesor to trsd propper value
PORTD = 0b11110000;
//PIND = 0b11110000;
asm volatile("nop");
uint8_t keyCode=PIND; // reads input value


PORTA = getBinary(keyCode); //displays on led

_delay_ms(400);
PORTA = 0x00;

DDRD=0b11110000; //sets pd2 as input and PD7-4 as output
 //PIND = 0b00000100;
PORTD = 0b00000100;


asm volatile("nop");


GIFR |= (1<< INTF0); // sets INTF0 to clear interrupts queue
 

}


int main (void)
{

cli() ;// stops interrupts

 

 MCUCR  |= (1 << ISC01) ;//  INT0 falling interrupt
  GIMSK |= (1 << INT0); // enables INT0 interrupt

  DDRD=0b11110000; //sets pd2 as input and PD7-4 as output
//PIND = 0b00000100;
PORTD = 0b00000100; // sets 


DDRA=0xff; //sets pa as output
PORTA = 0x00;// clears porta diods
   
sei();// starts interrupts
    while(1)
    {
	asm volatile("nop");
    }
}

