/*******************************************************
*******************************************************/
//желтый текст на синем фоне 15,15,3; 1,1,2
//желтый на коричневом 20,35,3;  5,3,1

#include <mega328p.h>
#include <spi.h>    //SPI
#include <delay.h>  //библиотека программных задержек
#include <stdio.h>  //стандартная либа ввода-вывода Си

#include <font_settings.h> //настроить используемый шрифт здесь, если стандартный планируется заменить
#include <ili9163.h>

#define HX_DATA PIND.5
#define HX_SCK  PORTD.6


// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void){

}

unsigned long result;

unsigned long sample(void){
    unsigned char i; 
    result=0;

    HX_SCK=0;
	//ждём пока закончится преобразование
	while(HX_DATA);
	
	for(i=0; i<24; i++)
	{
        //поднимаем строб
        HX_SCK=1;
        //сдвигаем значение АЦП влево, теперь значение АЦП выглядит так .......0
        result <<= 1;
        //опускаем строб
        HX_SCK=0;
        //проверяем, что на выводе data, если ноль то самый правый символ так и останется ноль .......0
        //если 1, то самый правый символ будет один .......1
        if(HX_DATA) result++;	
	}

	//выборки будем брать с канала А, с коэф.усиления 128
	//поэтому стробируем еще один раз
	HX_SCK=1;
    HX_SCK=0;
    
    HX_SCK=1;
    HX_SCK=0;
    //HX_SCK=1;
    //HX_SCK=0;
	
	return result;
}

void main(void)
{
// Input/Output Ports initialization
DDRB=(0<<DDB7) | (0<<DDB6) | (1<<DDB5) | (0<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (1<<PORTB1) | (0<<PORTB0);

DDRD.7=1;

DDRD.6=1; //HX_SCK
DDRD.5=0; //HX_DATA

HX_DATA=1;
HX_SCK=0;

// spi initialization Clock Rate: 4000,000 kHz MSB First
SPCR=(1<<SPE) | (1<<MSTR) | (1<<CPOL);
SPSR=(1<<SPI2X);

// Timer/Counter 0 initialization
TCCR0B=(0<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00);
// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

lcd_init();
lcd_fill(1,1,1);
text_color(20,25,3);
bg_color(5,3,1);
lcd_fill_str(0, 1,1,2);
lcd_text(0,0,"Уст.");
lcd_text(0,6,"рН = 6,50");

result=157;

while(1){
sample();
if (result == 0x800000) {result=0; delay_ms(500);}
if (result == 0x7FFFFF) {result=1111111; delay_ms(500);}
//lcd_fill_str(3, 1,1,2);
sprintf(string, "%lu      ", result);
lcd_type(3, 0, string);
}




// Global enable interrupts
#asm("sei")


    while(1){
    
    };

}
