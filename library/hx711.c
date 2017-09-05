#include<msp430.h>
#include "HX711.h"
//****************************************************
//延时函数
//****************************************************
void Delay_hx711_us(unsigned int t)
{
	//_nop_();
	//_nop_();
    while(t--);
}
// I/O配置

//****************************************************
//读取HX711
//****************************************************
unsigned long HX711_Read(void)	//增益128
{
	unsigned long count;
	unsigned char i;
  	P2OUT |= BIT2; //SDA
	Delay_hx711_us(5);
  	P2OUT &=~ BIT1;//SCK
  	count=0;
  	while( P2IN&BIT2 ); //SDA
  	for(i=0;i<24;i++)
	{
	 	P2OUT |= BIT1; //SCK
	 	count=count<<1;
		P2OUT &=~ BIT1;//SCK
	 	if(P2IN&BIT2)//SDA
		count++;
	}
 	P2OUT |= BIT1; //SCK
 	count=count^0x800000;
	Delay_hx711_us(5);
	P2OUT &=~ BIT1;//SCK
	return(count);
}

