/***************2016年TI杯电子设计大赛**********
 * 			作品：G题简易电子称
 * 			苏晨珲	杨宇晴	王璞
 **********************************************/

#include <msp430.h> 
#include<stdio.h>
#include"hx711.h"

void P1_IODect();									//P1口的外部中断事件检测函数
void P13_Onclick();									//P1.3按键的中断事件处理函数
void P16_Onclick();									//P1.6按键的中断事件处理函数
void P17_Onclick();									//P1.7按键的中断事件处理函数
void GPIO_Init();									//GPIO初始化函数
void Write_String(char *string);					//字符串发送LCD函数
void Write_Char(unsigned char Data);				//字符发送函数
void End();											//LCD结束指令
void show_LCD();									//屏幕显示的参数
void P14_Onclick();

unsigned char dshi=0;								//单价全局变量个位
unsigned char dge=0;									//单价全局变量.
int chushi;
unsigned char flag=0;									//重量开始标志位
int lingdian;


void main(void)
{
     WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer



    GPIO_Init();									//初始化GPIO


    BCSCTL1 = CALBC1_1MHZ;                    // 设置主时钟
    DCOCTL = CALDCO_1MHZ;
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    _enable_interrupts();

    chushi=HX711_Read()/1000;	//处理AD读取的数据，舍弃掉后几位
    lingdian=chushi;
    chushi=chushi-8409;

    while(1)
    {
    	if(flag%2==1)
    	{
    		  show_LCD();
    	}
    	__delay_cycles(100000);

    }

}

/**************************************************************
 *
 * 功       能：屏幕显示参数
 *
 **************************************************************/
void show_LCD()
{
    char str1[20]={};
    char str2[20]={};
    char str3[20]={};


	 long i=0;

	 int j;

	End();

    i=HX711_Read()/1000;

    sprintf(str1,"t0.txt=\"%d\"",dshi);			//显示物品单价
	Write_String(str1);
	End();

    if(i<=9120)
    {
    	j=(i-lingdian)*10/72;


    	sprintf(str2,"t3.txt=\"%d\"",j);			//0~100克
    	Write_String(str2);
    	End();

        sprintf(str3,"t2.txt=\"%d\"",dshi*j);			//显示物品单价
    	Write_String(str3);
    	End();
    }
    if((i-chushi)>=9120 & (i-chushi)<=9795)
    {
    	j=(i-chushi-9120)*100/682;
    	sprintf(str2,"t3.txt=\"%d\"",j+100);			//100~200克
    	Write_String(str2);
    	End();

        sprintf(str3,"t2.txt=\"%d\"",dshi*(j+100));			//显示物品单价
    	Write_String(str3);
    	End();
    }
    if((i-chushi)>=9795 & (i-chushi)<=10441)
    {
    	j=(i-chushi-9795)*100/651;
    	sprintf(str2,"t3.txt=\"%d\"",j+200);			//200~300克
    	Write_String(str3);
    	End();

        sprintf(str3,"t2.txt=\"%d\"",dshi*(j+200));			//显示物品单价
    	Write_String(str3);
    	End();
    }

    if((i-chushi)>=10441 & (i-chushi)<=11020)
    {
    	j=(i-chushi-10441)*100/588;
    	sprintf(str2,"t3.txt=\"%d\"",j=j+300+6);			//300~400克
    	Write_String(str2);
    	End();

        sprintf(str3,"t2.txt=\"%d\"",dshi*(j+300));			//显示物品单价
    	Write_String(str3);
    	End();
    }
    if(i-chushi>11020)
    {
    	j=(i-chushi-11020)*100/574;
    	sprintf(str3,"t3.txt=\"%d\"",j+400);			//400~500克
    	Write_String(str3);
    	End();

        sprintf(str3,"t2.txt=\"%d\"",dshi*(j+400));			//显示物品单价
    	Write_String(str3);
    	End();
    }


}
/**************************************************************
 *
 * 功       能：字符串发送LCD函数
 *
 **************************************************************/
void Write_String(char *string)
{
	int i;
	for(i=0;string[i]!='\0';i++)
	{
		UCA0TXBUF=string[i];
		_delay_cycles(5000);
	}
}
/**************************************************************
 *
 * 功       能：字符发送LCD函数
 *
 **************************************************************/
void Write_Char(unsigned char Data)
{
	UCA0TXBUF=Data;
	__delay_cycles(10000);
}
/**************************************************************
 *
 * 功       能：LCD结束指令
 *
 **************************************************************/
void End()
{
	UCA0TXBUF=0xff;
	__delay_cycles(10000);
	UCA0TXBUF=0xff;
	__delay_cycles(10000);
	UCA0TXBUF=0xff;
	__delay_cycles(10000);
}
/******************************************************************************************************
 * 名       称：GPIO_Init()
 * 功       能：设定按键和LED控制IO的方向，启用按键IO的上拉电阻
 ******************************************************************************************************/
void GPIO_Init()
{
	//-----设定P1.0和P1.6的输出初始值-----
	P1DIR |= BIT0+BIT6;							//设定P1.0为输出
	P1OUT &= ~BIT0;						//设定P1.0初值
	//-----配合机械按键，启用内部上拉电阻-----
	P1REN |= BIT3;				        //启用P1.3内部上下拉电阻
	P1OUT |= BIT3;			            //将电阻设置为上拉
	//-----配置P1.3中断参数-----
	P1DIR &= ~BIT3;                    // P1.3设为输入(可省略)
	P1IES |= BIT3;                          // P1.3设为下降沿中断
	P1IE  |= BIT3 ;                          // 允许P1.3中断

	P1DIR &= ~BIT6;                    // P1.3设为输入(可省略)
	P1IES |= BIT6;                          // P1.3设为下降沿中断
	P1IE  |= BIT6 ;                          // 允许P1.3中断

	P1DIR &= ~BIT7;                    // P1.3设为输入(可省略)
	P1IES |= BIT7;                          // P1.3设为下降沿中断
	P1IE  |= BIT7 ;                          // 允许P1.3中断

	P1DIR &= ~BIT4;                    // P1.3设为输入(可省略)
	P1IES |= BIT4;                          // P1.3设为下降沿中断
	P1IE  |= BIT4 ;                          // 允许P1.3中断

    //P2SEL &=~ (BIT1+BIT2);
    P2DIR &=~ BIT2;//SDA
    P2DIR |=  BIT1;//SCK
}
/******************************************************************************************************
 * 名       称：PORT1_ISR()
 * 功       能：响应P1口的外部中断服务
 * 说       明：P1.0~P1.8共用了PORT1中断，所以在PORT1_ISR()中必须查询标志位P1IFG才能知道
 * 					 具体是哪个IO引发了外部中断。P1IFG必须手动清除，否则将持续引发PORT1中断。
 ******************************************************************************************************/
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
	//-----启用Port1事件检测函数-----
	P1_IODect();								//检测通过，则会调用事件处理函数
   	P1IFG=0;                            			//退出中断前必须手动清除IO口中断标志
}
/******************************************************************************************************
 * 名       称：P1_IODect()
 * 功       能：判断具体引发中断的IO，并调用相应IO的中断事件处理函数
 * 说       明：该函数兼容所有8个IO的检测，请根据实际输入IO激活“检测代码”。
 * 					 本例中，仅有P1.3被用作输入IO，所以其他7个IO的“检测代码”没有被“激活”。
 ******************************************************************************************************/
void P1_IODect()
{
	unsigned int Push_Key=0;
	//-----排除输出IO的干扰后，锁定唯一被触发的中断标志位-----
	Push_Key=P1IFG&(~P1DIR);
	//-----延时一段时间，避开机械抖动区域-----
	__delay_cycles(10000);							//消抖延时
	//----判断按键状态是否与延时前一致-----
	if((P1IN&Push_Key)==0) 	                	//如果该次按键确实有效
   {
	 //----判断具体哪个IO被按下，调用该IO的事件处理函数-----
	  switch(Push_Key)
	  {
	   case BIT3: 	P13_Onclick();		break;
	   case BIT4: 	P14_Onclick();		break;
	   case BIT6: 	P16_Onclick();		break;
	   case BIT7: 	P17_Onclick();		break;
	   default:							break;		//任何情况下均加上default
	  }
   }
}
/******************************************************************************************************
 * 名    称：P13_Onclick()
 * 功    能：P1.3的中断事件处理函数，即P1.3键被按下
 * 说    明：使用事件处理函数的形式，可以增强代码的移植性和可读性
 ******************************************************************************************************/
void P13_Onclick()
{
	dshi++;
}
/******************************************************************************************************
 * 名    称：P16_Onclick()
 * 功    能：P1.3的中断事件处理函数，即P1.6键被按下
 * 说    明：使用事件处理函数的形式，可以增强代码的移植性和可读性
 ******************************************************************************************************/
void P16_Onclick()
{
	dshi--;
}
/******************************************************************************************************
 * 名    称：P17_Onclick()
 * 功    能：P1.3的中断事件处理函数，即当P1.7
 * 说    明：使用事件处理函数的形式，可以增强代码的移植性和可读性
 ******************************************************************************************************/
void P17_Onclick()
{
	flag++;
	P1OUT^=BIT0;
}
/******************************************************************************************************
 * 名    称：P14_Onclick()
 * 功    能：去皮
 ******************************************************************************************************/
void P14_Onclick()
{
    char str4[20]={};
	long i=0;
	int j;
	End();
    i=HX711_Read()/1000;

	j=(i-lingdian)*10/72;


	sprintf(str4,"t1.txt=\"%d\"",j);			//0~100克
	Write_String(str4);
	End();

}


