/****************************CH376底层接口***********************************/
#include "SPI3Init.h"
#include "BSP.H"
#include "ch376inc.h"
#ifndef	__bool_true_false_are_defined
	#include <stdbool.h>
	typedef	bool	BOOL;
#endif

extern void	delay_us ( uint32_t us );
uint8_t  CH376_Flag=0;
/*******************************************************************************
* 函  数  名      : SPI3_GPIO_Config
* 描      述      : SPI3的GPIO配置
*
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void SPI3_GPIO_Config(void)
{
	/* Initialize and enable the SSP Interface module. */
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN);SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	SET_BIT( RCC->APB1ENR,  RCC_APB1ENR_SPI3EN );
	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );
//	SET_BIT( AFIO->MAPR, AFIO_MAPR_SPI1_REMAP );
	/* SPI1_SCK, SPI1_MISO, SPI1_MOSI are SPI pins. */
	GPIOA->BSRR = ( 1 << 15 );
	MODIFY_REG( GPIOA->CRH, 0xF0000000u, 0x30000000u );	//	for CS, output
 	MODIFY_REG( GPIOB->CRL, 0x00FFF000u, 0x00B4B000u );
	/* Enable SPI in Master Mode, CPOL=1, CPHA=1. */
	SPI3->CR1 = SPI_CR1_SSI  | SPI_CR1_SSM  | SPI_CR1_SPE  | SPI_CR1_BR_0 |
							SPI_CR1_MSTR | SPI_CR1_CPHA | SPI_CR1_CPOL;
	SPI3->CR2 = 0x0000u;
}
/*******************************************************************************
* 函  数  名      : bus_SPI3xShift
* 描      述      : SPI3的读写配置
*
* 输      入      : 要写的字节
* 返      回      : 无
*******************************************************************************/
uint8_t bus_SPI3xShift( uint8_t OutByte )
{
	SPI3->DR = OutByte;	 
	while ( ! ( SPI3->SR & SPI_SR_RXNE ));
	OutByte = SPI3->DR;
	return	OutByte;
}
/******************************************************************************
* 函  数  名      : bus_SPI3xNSS
* 描      述      : SPI3的片选线
*
* 输      入      : 高低电平
* 返      回      : 无
*******************************************************************************/
void	bus_SPI3xNSS( BOOL NewOutLevel )
{
	if ( NewOutLevel )
	{
		GPIOA->BSRR = ( 1 << 15 );
	}
	else
	{
		GPIOA->BRR  = ( 1 << 15 );
	}
}
/**/
/*******************************************************************************
* 函  数  名      : CH376_END_CMD
* 描      述      : CH376失能命令
*
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void	CH376_END_CMD( void )
{	
	bus_SPI3xNSS(1);  /* 禁止SPI片选 */	
}
/*******************************************************************************
* 函  数  名      : SPI3_RW
* 描      述      : SPI3对从机的读写
*
* 输      入      : 要想从机写的数据
* 返      回      : 从机返回的数据
*******************************************************************************/
uint8_t SPI3_RW(uint8_t byte)
{
	return  bus_SPI3xShift( byte );	
}
/*******************************************************************************
* 函  数  名      : CH376_Write
* 描      述      : SPI3向CH376写一个字节
*
* 输      入      : 要向CH376写的字节
* 返      回      : 无
*******************************************************************************/
void CH376_Write(uint8_t CMD_DAT)
{	
	SPI3_RW(CMD_DAT);
}
/*******************************************************************************
* 函  数  名      : WriteCH376Cmd
* 描      述      : 向CH376写命令
*
* 输      入      : 要向376写的命令 （单字节）
* 返      回      : 无
*******************************************************************************/
void	WriteCH376Cmd( uint8_t mCmd )
{
	bus_SPI3xNSS(1);    /* 防止之前未通过xEndCH376Cmd禁止SPI片选 */
	delay_us(5);
	bus_SPI3xNSS(0);      /* SPI片选有效 */
	CH376_Write( mCmd );  /* 发出命令码 */	
	delay_us( 2 );   /* 延时1.5uS确保读写周期大于1.5uS,或者用上面一行的状态查询代替 */
}
/*******************************************************************************
* 函  数  名      : WriteCH376Data
* 描      述      : 向CH376写数据
*
* 输      入      : 要向3769写的数据
* 返      回      : 无
*******************************************************************************/
void WriteCH376Data(uint8_t dat)
{
	CH376_Write(dat);
}
/*******************************************************************************
* 函  数  名      : ReadCH376Data
* 描      述      : 从CH376读一个字节
*
* 输      入      : 0XFF
* 返      回      : CH376返回的数据
*******************************************************************************/
uint8_t ReadCH376Data(void)
{
	uint8_t d;
	d = SPI3_RW(0XFF);
	return( d );
}
/*******************************************************************************
* 函  数  名      : CH376QueryInterrupt
* 描      述      : CH376的中断标志读取函数
*
* 输      入      : 无
* 返      回      : CH376的中断标志状态
*******************************************************************************/
uint8_t CH376QueryInterrupt(void)
{
	if(CH376_Flag==1)
	{
		CH376_Flag=0;
		return 1;
	}
	else
	{
		return 0;
	}		
}
/*******************************************************************************
* 函  数  名      : NVIC_Configuration
* 描      述      : 中断向量表配置
*
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void	NVI_Init( void )
{
	// 	表7.4 抢占优先级和子优先级的表达，位数与分组位置的关系
	// 	分组位置    表达抢占优先级的位段    表达子优先级的位段
	// 		0            [7:1]                 [0:0]
	// 		1            [7:2]                 [1:0]
	// 		2            [7:3]                 [2:0]
	// 		3            [7:4]                 [3:0]
	// 		4            [7:5]                 [4:0]
	// 		5            [7:6]                 [5:0]
	// 		6            [7:7]                 [6:0]
	// 		7            无                    [7:0]（所有位）
	
	NVIC_SetPriorityGrouping(7);	//	禁止中断抢占，即没有中断嵌套
	NVIC_EnableIRQ( EXTI15_10_IRQn );
}
/*******************************************************************************
* 函  数  名      : EXTI_PC13_Config
* 描      述      : 外部中断线配置
*
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void	INT_IRQ_Enable( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
	AFIO->EXTICR[3] =  AFIO_EXTICR4_EXTI13_PC;
	CLEAR_BIT( EXTI->EMR,  0x2000u );	// no event
	CLEAR_BIT( EXTI->IMR,  0xDC00u );	// 禁止中断 
	SET_BIT(   EXTI->RTSR, 0x0000u );	// rising edge trigger
	SET_BIT(   EXTI->FTSR, 0x2000u );	// falling edge trigger
	WRITE_REG( EXTI->PR,   0x2000u );	// 写1复位中断标志位
	SET_BIT(   EXTI->IMR,  0x2000u );	// 
}
/**/
void EXTI15_10_IRQHandler(void)
{
	CH376_Flag=1; 	
	EXTI->PR=1<<13; 	
}
/**/


