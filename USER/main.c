#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include  "delay.h"
#include "gpio.h"
#include "stdio.h"

#define LED PBout(11)

extern u32 sysClk;

u16 msCnt=0;

void uart_test(void);

void LEDON(){ LED = 0;}
void LEDOFF() { LED = 1;}

void Delay(u32 count)
 {
  u32 i=0;
  for(;i<count;i++);

 }

 int main(void)
 {	
  initGPIO(); 
  delay_init();

  uart_test();
//  printf("start\n"); 
  while(1)
	{
		if(msCnt > 1000 ){
			msCnt = 0;
			//LED = ~LED;
			do8266();
		}
	}
 }

