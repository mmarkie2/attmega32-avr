


/*3)    Program jak powy¿ej z tym, ¿e w pamiêci danych umieszczono 5-bajtow¹ tablicê zawieraj¹c¹ nastêpuj¹c¹ sekwencjê bajtów: 02 F1 38 BA 97
Program zamienia ka¿dy pó³bajt z tej tablicy na odpowiadaj¹cy kod ASCII i wyœwietla na LCD. Do zamiany nale¿y u¿yæ 
tablicy transkoduj¹cej z kodu szestnastkowego na 
kod ASCII (cyfry 0-F, cyfry symbolizowane literami uzywaj¹ wielkich liter). Tablica transkoduj¹ca w pamiêci programu.
 Bajty tablicy maj¹ byæ wyœwietlone tak, 
¿eby pomiêdzy parami cyfr kolejnych bajtów wystêpowa³y spacje, a uk³ad napisu zgodny by³ z powy¿ej umieszczonym zapisem zawartoœci tablicy bajtów.
    Opcja wykonania programu:
    3b) napis pojawia siê z prawej strony wyœwietlacza*/



//maciej markiewicz





#define F_CPU 1000000UL			
#include <avr/io.h>			
#include <util/delay.h>			
#include <avr/pgmspace.h>

unsigned char   dict[] PROGMEM = //0-F
{
   48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70
    
};

 

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
	int i;;
for(i=0;i<16;++i)
	{
	sendDataToLCD (0b00010100,0);//moves cursor right
	_delay_us(50);
	}

 }
int main()
{
		////initializes lcd

	DDRA = 0xFF;			//PA as output
	_delay_ms(30);			//waits for lcd to boot 
	
	sendDataToLCD(0x02,0);		//sets 4 bit mode
	sendDataToLCD(0x28,0);             	//sets 2 line, 5x8
	sendDataToLCD(0x0E,0); //cursor not blinking 
sendDataToLCD (0b00000111,0);//sets print direction to left, cursor stays


reset();
	_delay_ms(5);

		


 uint8_t bytes[] ={
  0x02, 0xF1 , 0x38 , 0xBA , 0x97
    
};

	int i;
	uint8_t byteHigh;
uint8_t byteLow;
	for	(i=0; i<sizeof(bytes);++i)
	{
	//reads low and high byte
byteHigh= (bytes[i]>>4) & 0x0f;
byteLow= (bytes[i]) & 0x0f;

	char data = pgm_read_byte(&(dict[byteHigh])); //gets ascii code from array, eg dict[0]='0'
sendDataToLCD(data,1);

data = pgm_read_byte(&(dict[byteLow]));
sendDataToLCD(data,1);

sendDataToLCD(' ',1);//space
	_delay_ms(50);
	}



	while(1){}

	return 0;
}
