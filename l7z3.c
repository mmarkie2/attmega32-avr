#define F_CPU 1000000 L
#include <avr/io.h>

#include <avr/interrupt.h>

#include <avr/pgmspace.h>




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

3)	Uk�ad i metoda obs�ugi klawiatury jak wy�ej. Do obs�ugi klawiatury u�ywany jest PORTD. 
Program odczytuje znaki z klawiatury, zamienia kod klawisza na 4-bitowy kod binarny. Kody kolejnych cyfr pobieranych z klawiatury 
wpisywane s� do 16-bitowego rejestru z�o�onego z PORTB (starsza cz�� rejestru) i PORTA (m�odsza cz�� rejestru) w taki spos�b, �e 
pierwsza odczytana cyfra zajmuje 4 najm�odsze bity w PORTA, druga 4 kolejne itd. 5-ta i dalsze cyfry nie s� zapisywane.
Do PORTA i PORTB do��czone s� linijki LED wy�wietlaj�ce stan danych wprowadzonych z klawiatury.
UWAGA! Nale�y zadba�, �eby pojedyncze (a� do puszczenia klawisza) przyci�ni�cie klawisza na klawiaturze powodowa�o tylko 1-krotne wprowadzenie cyfry.
Przyci�ni�cie klawisza nie mo�e zatrzymywa� procesu skanowania klawiatury.
UWAGA!! W HAPSIM po wybraniu modu�u klawiatury matrycowej w opcjach klawiatury zmieni� opis klawiszy tak, �e w lewym g�rnym rogu klawisz
odpowiada cyfrze "0", kolejny w d� w tej kolumnie cyfrze "1" itd, kolumna po kolumnie (klawisz w prawym dolnym rogu odpowiada cyfrze "F")

*/


void initTimer0() {
  // set up timer with no prescaling
  TCCR0 |= (1 << CS00) | (1 << WGM01);

  OCR0 = 64;

  TIMSK |= (1 << OCIE0);

  // initialize counter
  TCNT0 = 0;
}


uint8_t getBinary(uint8_t keyCode) {
  uint8_t result = 0;
  keyCode = ~keyCode;

  uint8_t row = keyCode & 0x0f;
  if (row == 1) {
    result = result + 0;
  } else if (row == 2) {
    result = result + 1;
  } else if (row == 4) {
    result = result + 2;
  } else if (row == 8) {
    result = result + 3;
  }
  return result;
}

uint8_t keyCounter = 0;
uint8_t currentKeyCode = 0xff;


ISR(TIMER0_COMP_vect) {

 

  TCCR0 = 0x00;

  uint8_t keyCode = PIND;

  DDRD = 0b00001111; //sets pd2 as output and PD7-4 as  input

  asm volatile("nop");
  PORTD = 0b11110000;

  asm volatile("nop");
  keyCode |= PIND;

  if (keyCode != currentKeyCode && keyCounter<4) {
    currentKeyCode = keyCode;

    if (keyCode != 0xff) {
      switch (keyCounter) {
      case 0:
        PORTA = getBinary(currentKeyCode);
        break;
      case 1:
	  PORTA |= (getBinary(currentKeyCode) << 4);
        break;
case 2:
        PORTB = getBinary(currentKeyCode);
        break;
  case 3:
	  PORTB |= (getBinary(currentKeyCode) << 4);
        break;

      default:
        ;
      }
      keyCounter++;

    }

  }

  DDRD = 0b11110000; //sets pd2 as input and PD7-4 as output

  PORTD = 0b00001111;

  asm volatile("nop");

  initTimer0();
TIFR |= (1 << OCF0);
}



int main(void) {

  cli();

  DDRD = 0b11110000; //sets pd2 as input and PD7-4 as output

  PORTD = 0b00001111;

 DDRA = 0xff; //sets pa as output
 DDRB = 0xff; //sets pB as output

  PORTA = 0x00; 
  PORTB = 0x00;

  initTimer0();
  sei();
  while (1) {
    asm volatile("nop");
  }
}
