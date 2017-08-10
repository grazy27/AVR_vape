/*
	* Vape.cpp
	*
	* Created: 25.07.2017 13:43:20
	* Author : ADmin
	*/ 

#pragma region defines
#define F_CPU 8000000UL
#define zero  PORTD=(PORTD|(0b11111111))&0b01110111
#define one   PORTD=(PORTD|(0b11111111))&0b00010100
#define two   PORTD=(PORTD|(0b11111111))&0b01011011
#define three PORTD=(PORTD|(0b11111111))&0b01011110
#define four  PORTD=(PORTD|(0b11111111))&0b00111100
#define five  PORTD=(PORTD|(0b11111111))&0b01101110
#define six   PORTD=(PORTD|(0b11111111))&0b01101111
#define seven PORTD=(PORTD|(0b11111111))&0b01010100
#define eight PORTD=(PORTD|(0b11111111))&0b01111111
#define nine  PORTD=(PORTD|(0b11111111))&0b01111110
#define dot  0b10000000
#define FireButton !(PINC&0b1000)
#define PlusButton !(PINC&0b100)
#define MinusButton !(PINC&0b10)
#define Power_show 0
#define VapeTime_show 1
#define Voltage_show 2
#define Number_show 3
#pragma endregion defines  
	
#pragma region includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "avr/iom8a.h"
#include <avr/eeprom.h>
#include <stdint.h> 
#pragma endregion includes
		
#pragma region prototypes
void config();
void ShowNumber();
void show_volltage();
void Measure();
void Sleep();
void Wait();
void calibrate();
#pragma endregion prototypes 
		
#pragma region variables
bool Power_changed_flag = false,Discharged = false;
uint8_t VapeTime=0,Display_mode = Power_show,Power=eeprom_read_byte((uint8_t*)0);
uint16_t V1=40,V2 = 40,showing_voltage=0,showing_number=0,Sleep_time  = 0;
float ADC1_constant = eeprom_read_float((float*)4), ADC2_constant = eeprom_read_float((float*)8);
unsigned Keypresses = 0;
#pragma endregion variables
	
int main(void)
	{
		config();
		show_volltage();
		if(MinusButton&&PlusButton)
		calibrate();
		bool StillPressed = false;
			
		
		while (1)
		{

			if(MinusButton)
			{
			Sleep_time = 0;
				_delay_ms(75);
				if(MinusButton&&PlusButton)
				{
					
					while(MinusButton&&PlusButton)
					{
						show_volltage();
					}
				}
				else
				if(MinusButton)
				if(Power>1)
					Power--;
						
					
		}
			
			
			if(PlusButton)
			{
				Sleep_time = 0;
				_delay_ms(75);
				if(MinusButton&&PlusButton)
				{
					while(MinusButton&&PlusButton)
					{
							
						show_volltage();
					}
				}
					else
				if(PlusButton)
				if(Power<100) 
					Power++;
					
			}
				
						
					if(FireButton&&!StillPressed)
					{
					Measure();
						while(1)
						{
							if (Keypresses==5)
							{
								Sleep();
								Keypresses = 0;
							}
							if (FireButton)
							{
								for(int i = 0;i<150;i++)
								{
									    
									if(!FireButton) break;
									_delay_ms(1); 
								}
							}
							if (FireButton) break;
								
							if(!FireButton)
							{
								Keypresses++;
								for(int i = 0;i<150;i++)
								{
									if(FireButton) break;
									_delay_ms(1);
								}
							}
							if(!FireButton) break;
						}
				
		Keypresses = 0;
		Display_mode = VapeTime_show;
			TCCR2=0b1101010;  //enable PWM
			OCR2=((double)Power)*2.55;
			while(FireButton)
			{

				_delay_ms(90); // 1 second = 900 ms =)
				VapeTime++;
				Sleep_time = 0;
				if(VapeTime>=100)
				{
					StillPressed=true;
					break;
				}
			}
			}
			TCCR2=0b0101010;   //Disable PWM
			OCR2=0;//Disable PWM
			PORTB&=~(1<<PORTB3);//Surely disable PWM
			VapeTime = 0;
			Display_mode = Power_show;
				
			
		if (!FireButton&&StillPressed) StillPressed = false; 

	}
}
void config()
{
DDRD=0xFF;
DDRB=0xFF;
DDRC = 0xF0;
PORTD=0x00;
PORTB=0x7;
PORTC = 0x0E;

TCCR1B = (0<<CS12)|(1<<CS11)|(0<<CS10); //xtall/1
TIMSK |= (1<<TOIE1);

ASSR=0x00;//synhro/unsynhro
TCCR2=0b101001; //clock divider
TCNT2=0x00;//clock statement
OCR2=0;//Duty%
ADMUX|=(1<<REFS1)|(1<<REFS0)|(1<<ADLAR);
ADCSRA|=(1<<ADEN)|(0<<ADIE)|(1<<ADPS2);
sei();
}
ISR( TIMER1_OVF_vect )
{
Sleep_time++;
if(Sleep_time>=3000) //about 12 seconds delay
Wait();

TCNT1 = 60000;
ShowNumber();
}
void ShowNumber()
	{
		unsigned Number = 0;
		switch (Display_mode)
		{
				
			case Power_show:
			{
				uint8_t power = Power;
				for(short i = 2;i>=0;i--)
				{
						
					Number = power%10;

					power/=10;
					if(Number==0&&power==0) break;
					PORTB|=0b111;
					PORTB=(~(1<<i));
					switch (Number)
					{
						case 0:{zero; break;}
						case 1:{one; break;}
						case 2:{two; break;}
						case 3:{three; break;}
						case 4:{four; break;}
						case 5:{five; break;}
						case 6:{six; break;}
						case 7:{seven; break;}
						case 8:{eight; break;}
						case 9:{nine; break;}
					}
					_delay_us(400);
					PORTD=0x00;
						
				}
				break;
			}
			case Voltage_show:
				{
					uint16_t V = showing_voltage;
					for(short i = 2;i>=0;i--)
					{
							
						Number = V%10;

						V/=10;
						PORTB|=0b111;
						PORTB=(~(1<<i));
						if (i==0)
						switch (Number)
						{
							case 0:{zero|dot; break;}
							case 1:{one|dot; break;}
							case 2:{two|dot; break;}
							case 3:{three|dot; break;}
							case 4:{four|dot; break;}
							case 5:{five|dot; break;}
							case 6:{six|dot; break;}
							case 7:{seven|dot; break;}
							case 8:{eight|dot; break;}
							case 9:{nine|dot; break;}
						}
						else
						{ 
							switch (Number)
							{
								case 0:{zero; break;}
								case 1:{one; break;}
								case 2:{two; break;}
								case 3:{three; break;}
								case 4:{four; break;}
								case 5:{five; break;}
								case 6:{six; break;}
								case 7:{seven; break;}
								case 8:{eight; break;}
								case 9:{nine; break;}
							}

						}




						_delay_us(400);
						PORTD=0x00;
						}
					break;

			}
			case VapeTime_show:
				{
					uint8_t T = VapeTime;
					for(short i = 2;i>=1;i--)
					{
						    
						Number = T%10;

						T/=10;
						PORTB|=0b111;
						PORTB=(~(1<<i));
						if (i==1)
							switch (Number)
							{
								case 0:{zero|dot; break;}
								case 1:{one|dot; break;}
								case 2:{two|dot; break;}
								case 3:{three|dot; break;}
								case 4:{four|dot; break;}
								case 5:{five|dot; break;}
								case 6:{six|dot; break;}
								case 7:{seven|dot; break;}
								case 8:{eight|dot; break;}
								case 9:{nine|dot; break;}
							}
						else
						{
							switch (Number)
							{
								case 0:{zero; break;}
								case 1:{one; break;}
								case 2:{two; break;}
								case 3:{three; break;}
								case 4:{four; break;}
								case 5:{five; break;}
								case 6:{six; break;}
								case 7:{seven; break;}
								case 8:{eight; break;}
								case 9:{nine; break;}
							}

						}

						_delay_us(400);
						PORTD=0x00;
						    
				}
				break;
		} 
			case Number_show:
				{
					uint8_t N = showing_number;
					for(short i = 2;i>=0;i--)
					{
							
						Number = N%10;

						N/=10;
						if(Number==0&&N==0) break;
						PORTB|=0b111;
						PORTB=(~(1<<i));
						switch (Number)
						{
							case 0:{zero; break;}
							case 1:{one; break;}
							case 2:{two; break;}
							case 3:{three; break;}
							case 4:{four; break;}
							case 5:{five; break;}
							case 6:{six; break;}
							case 7:{seven; break;}
							case 8:{eight; break;}
							case 9:{nine; break;}
						}
						_delay_us(400);
						PORTD=0x00;
							
					}
					break;
				}

		}
	}
void Measure()
{
cli();



	ADMUX&=0xF8;
	_delay_ms(10);
	ADCSRA|=1<<ADSC;
	while(!(ADCSRA&(1<<ADIF)));
	V1= (int)(ADCH*2*ADC1_constant); 
		

	ADMUX|=0x07;
	_delay_ms(10);
	ADCSRA|=1<<ADSC;
	while(!(ADCSRA&(1<<ADIF)));
	V2=(int)(((ADCH*3.3)*ADC2_constant)-V1);
		
	_delay_ms(10);	
	
	if(V1<300||V2<300)
	PORTC|=1<<PORTC4;
	else PORTC&=~(1<<PORTC4);
		
	sei();
}
void show_volltage()
{
	
Measure();

showing_number = 1;
Display_mode = Number_show;
_delay_ms(300);

	
showing_voltage = V1;
Display_mode=Voltage_show;
_delay_ms(1500);

showing_number = 2;
Display_mode = Number_show;
_delay_ms(300);

showing_voltage = V2;
Display_mode=Voltage_show;
_delay_ms(1500);

Display_mode = Power_show;
}
void Sleep()
{
eeprom_write_byte ((uint8_t*) 0, Power);
cli();
Keypresses = 0;
while(1)
{
	while(1)
	{
		if (Keypresses==5)
		{
			sei();
			return;
		}
		if (FireButton)
		{
			for(int i = 0;i<150;i++)
			{
					
				if(!FireButton) break;
				_delay_ms(1);
			}
		}
		if (FireButton) break;
			
		if(!FireButton)
		{
			Keypresses++;
			for(int i = 0;i<150;i++)
			{
				if(FireButton) break;
				_delay_ms(1);
			}
		}
		if(!FireButton) break;
	}
	Keypresses = 0;
}

}
void Wait()
{
cli();
while(!MinusButton&&!PlusButton&&!FireButton);
sei();
Sleep_time = 0; 
}
void calibrate()
{
Measure();
ADC1_constant = 1;
ADC2_constant = 1;
Display_mode = Voltage_show;
while(!FireButton)
{
	Measure();
	showing_voltage = V1;
	if(MinusButton)
	{
		Sleep_time = 0;
		_delay_ms(40);
		if(MinusButton)
		ADC1_constant-=0.001;
	}

	if(PlusButton)
	{
		Sleep_time = 0;
		_delay_ms(40);
		if(PlusButton)
		ADC1_constant+=0.001;
	}
}
_delay_ms(500);
while(!FireButton)
{
	Measure();
	showing_voltage = V2;
	if(MinusButton)
	{
		Sleep_time = 0;
		_delay_ms(40);
		if(MinusButton)
		ADC2_constant-=0.001;
	}

	if(PlusButton)
	{
		Sleep_time = 0;
		_delay_ms(40);
		if(PlusButton)
		ADC2_constant+=0.001;
	}
}
eeprom_write_float((float*)4,ADC1_constant);
eeprom_write_float((float*)8,ADC2_constant);
show_volltage();
}

