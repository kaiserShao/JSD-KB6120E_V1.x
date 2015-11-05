/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: CLOCK.C
* 创 建 者: 董峰
* 描  述  : 读写STM32F10x的RTC时钟
* 最后修改: 2014年5月5日
*********************************** 修订记录 ***********************************
* 版  本: V2.0
* 修订人: 董峰
* 说  明: see http://www.epochconverter.com
*******************************************************************************/
#include "BSP.H"
#include "Pin.H"
#include "BIOS.H"

/********************************** 功能说明 ***********************************
*	访问 STM32 的 ADC 模块
*******************************************************************************/
volatile	uint16_t	ADCResultBuf[16][5];

static	void	STM32_ADC_Init ( void )
{
	//	DMA1 channel1 configuration
	SET_BIT ( RCC->AHBENR,  RCC_AHBENR_DMA1EN );
	//	DMA模块禁能, 重新配置
	DMA1_Channel1->CCR   = 0u;
	DMA1_Channel1->CCR   = DMA_CCR1_PL | DMA_CCR1_MINC | DMA_CCR1_PSIZE_0 | DMA_CCR1_MSIZE_0 | DMA_CCR1_CIRC;
	//	设置DMA外设地址, 内存地址, 传输数量
	DMA1_Channel1->CPAR  = ( uint32_t ) &ADC1->DR;
	DMA1_Channel1->CMAR  = ( uint32_t ) ADCResultBuf;
	DMA1_Channel1->CNDTR = ( sizeof ( ADCResultBuf ) / sizeof ( ADCResultBuf[0][0] ) );
	SET_BIT ( DMA1_Channel1->CCR, DMA_CCR1_EN );			//	DMA通道使能

	SET_BIT ( RCC->APB2ENR, RCC_APB2ENR_ADC1EN );
	MODIFY_REG ( RCC->CFGR , RCC_CFGR_ADCPRE, RCC_CFGR_ADCPRE_DIV6 );

	// 配置控制寄存器
	ADC1->CR1 = ADC_CR1_SCAN;
	ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_TSVREFE | ADC_CR2_CONT | ADC_CR2_DMA;

	//	配置通道采样时间（通道编号从0开始）
	#define	ADC1_SMP_1p5_C		0	// 1.5 cycles
	#define	ADC1_SMP_7p5_C		1	// 7.5 cycles
	#define	ADC1_SMP_13p5_C		2	// 13.5 cycles
	#define	ADC1_SMP_28p5_C		3	// 28.5 cycles
	#define	ADC1_SMP_41p5_C		4	// 41.5 cycles
	#define	ADC1_SMP_55p5_C		5	// 55.5 cycles
	#define	ADC1_SMP_71p5_C		6	// 71.5 cycles
	#define	ADC1_SMP_239p5_C	7	// 239.5 cycles

	#define	ADC1_SMPR1_BASE		10	// R1 起始编号
	#define	ADC1_SMPR2_BASE		0	// R2 起始编号
	#define	ADC1_SMPR_WIDTH		3	// 每组设置3位

    // ...............  填入相应采样时间 ...........................................................
	ADC1->SMPR2 =	( ( ADC1_SMP_239p5_C ) << ( ADC1_SMPR_WIDTH * (  0 - ADC1_SMPR2_BASE ) ) )	//	通道0
				|	( ( ADC1_SMP_239p5_C ) << ( ADC1_SMPR_WIDTH * (  1 - ADC1_SMPR2_BASE ) ) )	//	通道1
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  2 - ADC1_SMPR2_BASE ) ) )	//	通道2
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  3 - ADC1_SMPR2_BASE ) ) )	//	通道3
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  4 - ADC1_SMPR2_BASE ) ) )	//	通道4
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  5 - ADC1_SMPR2_BASE ) ) )	//	通道5
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  6 - ADC1_SMPR2_BASE ) ) )	//	通道6
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  7 - ADC1_SMPR2_BASE ) ) )	//	通道7
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  8 - ADC1_SMPR2_BASE ) ) )	//	通道8
				|	( ( ADC1_SMP_239p5_C ) << ( ADC1_SMPR_WIDTH * (  9 - ADC1_SMPR2_BASE ) ) )	//	通道9
                ;
	ADC1->SMPR1 =	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * ( 10 - ADC1_SMPR1_BASE ) ) )	//	通道10
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * ( 11 - ADC1_SMPR1_BASE ) ) )	//	通道11
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * ( 12 - ADC1_SMPR1_BASE ) ) )	//	通道12
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * ( 13 - ADC1_SMPR1_BASE ) ) )	//	通道13
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * ( 14 - ADC1_SMPR1_BASE ) ) )	//	通道14
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * ( 15 - ADC1_SMPR1_BASE ) ) )	//	通道15
				|	( ( ADC1_SMP_41p5_C  ) << ( ADC1_SMPR_WIDTH * ( 16 - ADC1_SMPR1_BASE ) ) )	//	通道16(内部温度)
				|	( ( ADC1_SMP_41p5_C  ) << ( ADC1_SMPR_WIDTH * ( 17 - ADC1_SMPR1_BASE ) ) )	//	通道17(内部基准)
				;

	//	配置规则转换序列（序列编号从1开始）
	#define	ADC1_SQR1_BASE		13	// R1 起始编号
	#define	ADC1_SQR2_BASE		7	// R2 起始编号
	#define	ADC1_SQR3_BASE		1	// R3 起始编号
	#define	ADC1_SQR_WIDTH		5	// 每组设置5位

    // ............设置通道号，最后一行设置序列长度.................................................
	ADC1->SQR3  =	( ( 16 ) << ( ADC1_SQR_WIDTH * (  1 - ADC1_SQR3_BASE ) ) )	//	序列1
				|	( ( 17 ) << ( ADC1_SQR_WIDTH * (  2 - ADC1_SQR3_BASE ) ) )	//	序列2
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  3 - ADC1_SQR3_BASE ) ) )	//	序列3
				|	( ( 1  ) << ( ADC1_SQR_WIDTH * (  4 - ADC1_SQR3_BASE ) ) )	//	序列4
				|	( ( 9  ) << ( ADC1_SQR_WIDTH * (  5 - ADC1_SQR3_BASE ) ) )	//	序列5
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  6 - ADC1_SQR3_BASE ) ) )	//	序列6
				;
	ADC1->SQR2  =	( ( 0  ) << ( ADC1_SQR_WIDTH * (  7 - ADC1_SQR2_BASE ) ) )	//	序列7
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  8 - ADC1_SQR2_BASE ) ) )	//	序列8
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  9 - ADC1_SQR2_BASE ) ) )	//	序列9
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 10 - ADC1_SQR2_BASE ) ) )	//	序列10
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 11 - ADC1_SQR2_BASE ) ) )	//	序列11
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 12 - ADC1_SQR2_BASE ) ) )	//	序列12
				;
	ADC1->SQR1  =	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 13 - ADC1_SQR1_BASE ) ) )	//	序列13
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 14 - ADC1_SQR1_BASE ) ) )	//	序列14
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 15 - ADC1_SQR1_BASE ) ) )	//	序列15
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 16 - ADC1_SQR1_BASE ) ) )	//	序列16
				|( ( 5 - 1 ) << ( ADC1_SQR_WIDTH * ( 17 - ADC1_SQR1_BASE ) ) )	//	长度(0表示1次)
                ;

	__NOP();	//	delay_us ( 1u );
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	SET_BIT ( ADC1->CR2, ADC_CR2_ADON );						//	Enable ADC1
	SET_BIT ( ADC1->CR2, ADC_CR2_RSTCAL );						//	Reset calibration register
	while ( READ_BIT ( ADC1->CR2, ADC_CR2_RSTCAL ) ) {}			//	Check the end of ADC1 reset calibration register
	SET_BIT ( ADC1->CR2, ADC_CR2_CAL );							//	Start ADC1 calibration
	while ( READ_BIT ( ADC1->CR2, ADC_CR2_CAL ) ) {}			//	Check the end of ADC1 calibration
	SET_BIT ( ADC1->CR2, ADC_CR2_SWSTART | ADC_CR2_EXTTRIG );  	//	Start ADC1 Software Conversion

	//	 配置相应的AD输入管脚为模拟输入状态
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRL, 0x0000000Fu, 0x00000000u );	//	PA0 模拟输入模式
	MODIFY_REG( GPIOA->CRL, 0x000000F0u, 0x00000000u );	//	PA1 模拟输入模式
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	MODIFY_REG( GPIOB->CRL, 0x000000F0u, 0x00000000u );	//	PB1 模拟输入模式
}

static	uint16_t	STM32_ADC_fetch ( uint8_t Channel )
{
	const uint8_t len = sizeof ( ADCResultBuf ) / sizeof ( ADCResultBuf[0] );
	uint8_t		i;
	uint32_t	sum = 0u;

	for ( i = 0u; i < len; ++ i )
	{
		sum += ADCResultBuf[i][Channel];
	}
	return	( uint16_t ) ( sum / len );
}



void	ADC12_Init( void )
{
	STM32_ADC_Init();
}

uint16_t	ADC12_Readout( enum enumADC12_Channel ADC_Channel )
{
	return	STM32_ADC_fetch( ADC_Channel );
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
