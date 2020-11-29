#define F_CPU 1000000 L
#include <avr/io.h>

#include <avr/interrupt.h>

#include <avr/pgmspace.h>




/*2)	Klawiatura sk³ada siê z 16 klawiszy umieszczonych na skrzy¿owianiach 4 linii kolumn z 4-ema liniami wierszy. 
Kolumny klawiatury do³¹czone s¹ do 4-ech wyjœciowych linii portu B. Wiersze do³¹czone s¹ do 4-ech wejœciowych linii portu B. 
Nale¿y opracowaæ program, który steruje multipleksowo kolumnami, i wyœwietla na linijce LED do³¹czonej do portu A stany linii 
steruj¹cych kolumnami (4 starsze bity)
i stany odczytane z linii wierszy (4 m³odsze bity), jakie mia³y w chwili naciœniêcia któregokolwiek klawisza.
Stan ten wyœwietlany jest tak d³ugo, jak d³ugo linia wejœciowa w porcie sygnalizuje wciœniêcie klawisza
Czas reakcji na przyciœniêcie klawisza na klawiaturze nie d³u¿szy ni¿ 150 ms. 
Czasy odmierzane timerem. Obs³uga klawiszy w obs³udze przerwania od timera. U¿yæ timera 0. U¿yæ nak³adki symulatora HAPSIM. 
W symulatorze u¿yæ wyœwietlaczy LED i modelu klawiatury matrycowej (wybraæ jeden wiersz klawiatury).
UWAGA!! HAPSIM niepoprawnie reaguje na podci¹gniêcie linii wejœciowej do zasilania. Instrukcjê podci¹gniêcia nale¿y w programie "zakomentowaæ".
UWAGA!! W HAPSIM po wybraniu modu³u klawiatury matrycowej w opcjach klawiatury zmieniæ opis klawiszy tak, ¿e w lewym górnym rogu klawisz
odpowiada cyfrze "0", kolejny w dó³ w tej kolumnie cyfrze "1" itd, kolumna po kolumnie (klawisz w prawym dolnym rogu odpowiada cyfrze "F")

3)	Uk³ad i metoda obs³ugi klawiatury jak wy¿ej. Do obs³ugi klawiatury u¿ywany jest PORTD. 
Program odczytuje znaki z klawiatury, zamienia kod klawisza na 4-bitowy kod binarny. Kody kolejnych cyfr pobieranych z klawiatury 
wpisywane s¹ do 16-bitowego rejestru z³o¿onego z PORTB (starsza czêœæ rejestru) i PORTA (m³odsza czêœæ rejestru) w taki sposób, ¿e 
pierwsza odczytana cyfra zajmuje 4 najm³odsze bity w PORTA, druga 4 kolejne itd. 5-ta i dalsze cyfry nie s¹ zapisywane.
Do PORTA i PORTB do³¹czone s¹ linijki LED wyœwietlaj¹ce stan danych wprowadzonych z klawiatury.
UWAGA! Nale¿y zadbaæ, ¿eby pojedyncze (a¿ do puszczenia klawisza) przyciœniêcie klawisza na klawiaturze powodowa³o tylko 1-krotne wprowadzenie cyfry.
Przyciœniêcie klawisza nie mo¿e zatrzymywaæ procesu skanowania klawiatury.
UWAGA!! W HAPSIM po wybraniu modu³u klawiatury matrycowej w opcjach klawiatury zmieniæ opis klawiszy tak, ¿e w lewym górnym rogu klawisz
odpowiada cyfrze "0", kolejny w dó³ w tej kolumnie cyfrze "1" itd, kolumna po kolumnie (klawisz w prawym dolnym rogu odpowiada cyfrze "F")

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
