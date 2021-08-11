

#include "stm32f4xx.h"                  // Device header
void RCC_Ayar(void);
void gpio_Ayar(void);
void tim6_Ayarlar(void);

volatile uint32_t say=0;
uint16_t time2=0;

uint8_t DHT11_Read (void);
			 
void delay(uint16_t time);


uint32_t i=0;
 
//clock/(1+psc)*(1+arr); 168 000 000 / 42 000 *2 000 =  2 hz ==> 1/2 s   
//clock/(1+psc)*(1+arr); 168 000 000 / 24*7 = 10 ^6  hz ==> 1 us   
//clock/(1+psc)*(1+arr); 168 000 000 / 2400*70 = 10 ^3  hz ==> 1 ms 


			 
void delay(uint16_t time){

	
	
	 TIM6->CNT=0;
	 while(TIM6->CNT<time)
	 {
	 time2=TIM6->CNT;
	 }

}


void Set_Pin_Output(){

  
	GPIOB->MODER  = 0x00000004; //Output modu
	GPIOB->OTYPER = 0x00000000; //Push-Pull
	GPIOB->OSPEEDR =0x00000008; //Yuksek hiz

	

}

void Set_Pin_Input(){

  GPIOB->MODER  = 0x00000000; //Input modu
	GPIOB->OTYPER = 0x00000000; //Push-Pull
	GPIOB->OSPEEDR =0x00000008; //Yuksek hiz
	
}



uint8_t durum=0,Humidity=0,Temperature=0;
uint16_t tempVal=0,humVal=0;
uint8_t dhtVal[2];
uint8_t mData[40];
uint16_t mTime1 = 0, mTime2 = 0;
uint16_t mbit = 0;
uint8_t  parityVal = 0, genParity = 0;

uint8_t DHT11_Read (void){

  for(int a=0;a<40;a++) mData[a]=0;
   mTime1 = 0, mTime2 = 0, durum=0, tempVal=0, humVal=0, parityVal = 0, genParity = 0,  mbit = 0;

	
	 
   Set_Pin_Output();
	 GPIOB->BSRR =1<<17; //B1 low
	 
    delay(18000);
	
	  GPIOB->BSRR =1<<1;
	  delay(40);
   	Set_Pin_Input();
    
	  TIM6->CNT=0;
	while(GPIOB->IDR & (1<<1)) if((uint16_t)  TIM6->CNT > 500) return 0;

	 TIM6->CNT=0;
	while((GPIOB->IDR & (1<<1))==0) if((uint16_t)TIM6->CNT > 500) return 0;
	mTime1 = TIM6->CNT;

	TIM6->CNT=0;
	while(GPIOB->IDR & (1<<1)) if((uint16_t)TIM6->CNT > 500) return 0;
    mTime2 = (uint16_t)TIM6->CNT;


	if(mTime1 < 75 && mTime1 > 85 && mTime2 < 75 && mTime2 > 85)
	{

		return 0;
	}




	for(int j = 0; j < 40; j++)
	{
		
		TIM6->CNT=0;
		while((GPIOB->IDR & (1<<1))==0) if((uint16_t)TIM6->CNT > 500) return 0;
		TIM6->CNT=0;
		while((GPIOB->IDR & (1<<1))) if((uint16_t)TIM6->CNT > 500) return 0;
		mTime1 = (uint16_t)TIM6->CNT;

		
		if(mTime1 > 20 && mTime1 < 30)
		{
			mbit = 0;
		}
		else if(mTime1 > 60 && mTime1 < 80) //if pass time 70 uS set as HIGH
		{
			 mbit = 1;
		}

		//set i th data in data buffer
		mData[j] = mbit;

	}
	
	humVal=128 *mData[0]+64 *mData[1]+32 *mData[2]+16 *mData[3]+8 *mData[4]+4 *mData[5]+2 *mData[6]+ mData[7];
	tempVal=128 *mData[16]+64 *mData[17]+32 *mData[18]+16 *mData[19]+8 *mData[20]+4 *mData[21]+2 *mData[22]+ mData[23];

	
	
   /*
	//get hum value from data buffer
	for(int i = 0; i < 8; i++)
	{
		humVal += mData[i];
		humVal = humVal << 1;
	}

	//get temp value from data buffer
	for(int i = 16; i < 24; i++)
	{
		tempVal += mData[i];
		tempVal = tempVal << 1;
	}

	//get parity value from data buffer
	for(int i = 32; i < 40; i++)
	{
		parityVal += mData[i];
		parityVal = parityVal << 1;
	}

	parityVal = parityVal >> 1;
	humVal = humVal >> 1;
	tempVal = tempVal >> 1;

	genParity = humVal + tempVal;


	dhtVal[0]= tempVal;
	dhtVal[1] = humVal;
 */
	return 1;
}		 

			









void RCC_Ayar()
{
	
	
	
  //	HSI OFF HSE ON
	RCC->CR &=~(1<<0);//hsi off yap
	
	RCC->CR |= 0x00010000;	// HSEON
	while(!(RCC->CR & 0x00020000));	// HSEON Ready Flag wait
	
	
	
	RCC->APB1ENR |=RCC_APB1ENR_PWREN;
	PWR->CR |=PWR_CR_VOS;
	
	

	RCC->PLLCFGR = 0x04402A04; //0040 tu..!!!!!!!!!!!!!!!!!!!!!!!!!  Eski:0x04402A04  Yeni:0x04401504
	RCC->CR |= 0x01000000;			// PLL On
	while(!(RCC->CR & (1<<25)));	// /pll bayrak kontolu.
	
	
	RCC->CFGR |= 0x00000000;		// AHB Prescaler 1   
	RCC->CFGR |= 0x00009400;		// APB1 Prescaler 4  APB2 Prescaler 2   Eski: 0x00009400  Yeni:0x00009000       
	
	
	FLASH->ACR = 0x00000605;  //5 ONEMLI
	RCC->CFGR |= 0x00000002; //Sistem PLL icin
	while(!(RCC->CFGR & (1<<1)));	// /pll bayrak kontolu.
	
	RCC->CIR |=1<<18;
	RCC->CIR |=1<<19;
	RCC->CIR |=1<<20;
	
	
}
void gpio_Ayar()
{


  //D12 Ayarlari
	RCC->AHB1ENR |=1<<3;  //D Clock Aktif.
	
	GPIOD->MODER  = 0x05000000; //Output modu
	GPIOD->OTYPER = 0x00000000; //Push-Pull
	GPIOD->OSPEEDR =0x03000000; //Yuksek hiz
	GPIOD->BSRR =1<<28; //D12 low
	
	
	//B1 Ayarlari
	RCC->AHB1ENR |=1<<1;  //B Clock Aktif.
	
	GPIOB->MODER  = 0x00000004; //Output modu
	GPIOB->OTYPER = 0x00000000; //Push-Pull
	GPIOB->OSPEEDR =0x00000008; //Yuksek hiz
	GPIOB->BSRR =1<<1; //B1 high
	GPIOB->BSRR =1<<17; //B1 low
	//deger=MS_18;
	//int a=0;
	//for(;a<40;a++) dizi[a]=0;
	
}



void tim6_Ayarlar()
{
	

	
	RCC->APB1ENR |= 1<<4;  //TIM6  Aktif.
	
	TIM6->PSC =83;  //0.5 sn icin psc:41999  ; 1us periyod icin psc=6 ;1 ms icin psc=69
	TIM6->ARR =0xffff-1;  //0.5 sn icin arr:1999 ; 1us periyod icin  arr=23;1 ms icin psc=2399
	TIM6->CR1 |=1<<0;//CEN  Aktif.
	
	
}





int main()
{
	
  RCC_Ayar();
	SystemCoreClockUpdate();
	say=SystemCoreClock;
	gpio_Ayar();
  tim6_Ayarlar();
	
	
	while(1)
	{
		
		 /*
		 durum=DHT11_Read();
     if (durum ==1)	{Temperature=tempVal; Humidity=humVal;}
     for(i=0;i<20000000;i++);
		*/
		
		 GPIOD->BSRR =1<<12;
		 for(i=0;i<100;i++)
		{
	   delay(18000);//18 ms
		}
		 GPIOD->BSRR =1<<28;
		
		 for(i=0;i<100;i++)
		{
	   delay(18000);
		}
		
	
		
	}


}
