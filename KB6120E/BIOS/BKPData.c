#include "stm32f10x.h"
#include "bkpdata.h"
#include "BIOS.H"

void BKP_Write (uint16_t BKP_DR, uint16_t Data)
{
  __IO uint32_t tmp = 0;
  tmp = (uint32_t)BKP_BASE; 
  tmp += BKP_DR;
  *(__IO uint32_t *) tmp = Data;
}

uint16_t BKP_Read (uint16_t BKP_DR)
{
  __IO uint32_t tmp = 0;
  tmp = (uint32_t)BKP_BASE; 
  tmp += BKP_DR;
  return (*(__IO uint16_t *) tmp);
}

void BKPConfig(void)
{ 	
	RCC->APB1ENR|=1<<27; //bkp时钟
	RCC->APB1ENR|=1<<28; //后备电源时钟
	PWR->CR|=1<<8;//	
}

void Powertime_Write( uint32_t Data,uint8_t state)
{
	uint16_t DataH,DataL;
	
	DataH = Data>>16;
	DataL = Data;
	BKP_Write( BKP_DR3 , DataH );
	BKP_Write( BKP_DR4 , DataL );
	
}

uint32_t Powertime_Read( uint8_t state )
{
	uint32_t Dataout=0;
	
	Dataout  = BKP_Read(BKP_DR3);
	Dataout <<= 16;
	Dataout |= BKP_Read(BKP_DR4); 	
	
	return Dataout; 
}

/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File **********/

