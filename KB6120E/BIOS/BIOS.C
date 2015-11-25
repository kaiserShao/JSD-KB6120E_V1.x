/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: BIOS.C
* 创 建 者: 董峰
* 描  述  : KB6120E 芯片端口访问
* 最后修改: 2014年1月14日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "Pin.H"
#include "BIOS.H"

#ifdef  UseHSE
#define	SYSCLK_FREQ_24MHz 24000000
#include "system_stm32f10x.c"
#else
#include "SystemInit_HSI.c"
#endif

// 	#define	_DINT()	__disable_irq()
// 	#define	_EINT()	__enable_irq()

#define	PinBB( _Port, _Num )	(*(__IO int32_t *)(PERIPH_BB_BASE + ((uint32_t)&(_Port) - PERIPH_BASE) * 32u + (_Num) * 4u ))

//	0：(0000B)模拟输入模式
//	4：(0100B)浮空输入模式(复位后的状态)
//	8：(1000B)上拉/下拉输入模式
//	C：(1100B)保留
//	
//	1：(0001B)通用推挽输出模式10MHz
//	2：(0010B)通用推挽输出模式2MHz
//	3：(0011B)通用推挽输出模式50MHz
//	
//	5：(0101B)通用开漏输出模式10MHz
//	6：(0110B)通用开漏输出模式2MHz
//	7：(0111B)通用开漏输出模式50MHz
//	
//	9：(1001B)复用功能推挽输出模式10MHz
//	A：(1010B)复用功能推挽输出模式2MHz
//	B：(1011B)复用功能推挽输出模式50MHz
//	
//	D：(1101B)复用功能开漏输出模式10MHz
//	E：(1110B)复用功能开漏输出模式2MHz
//	F：(1111B)复用功能开漏输出模式50MHz

/********************************** 功能说明 ***********************************
*	短延时程序，用于硬件同步，精确延时需要防止中断的干扰
*******************************************************************************/
#pragma	push
#pragma O3	Ospace
void	delay_us ( uint32_t us )
{
	while ( us-- )
	{
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
	}
}
#pragma	pop

/********************************** 功能说明 ***********************************
*	使用 GPIOB 的高8位 实现 8 位 并行IO
*******************************************************************************/
static	void	P8P_ModeInput( void )
{
	GPIOB->CRH  = 0x44444444u;	//	浮空输入方式
}

static	void	P8P_ModeOutput( void )
{
	GPIOB->CRH  = 0x33333333u;	//	推挽输出方式
}

static	uint8_t	P8P_PortRead( void )
{
	return	(uint8_t)( GPIOB->IDR >> 8 );
}

static	void	P8P_PortWrite( uint8_t OutByte )
{
	GPIOB->BSRR = ( 0xFF00uL << 16 )
              | ( 0xFF00uL & ((uint32_t)OutByte << 8 ));
}

/********************************** 功能说明 ***********************************
*	按键端口扫描
*******************************************************************************/
uint8_t	Keyboard_PortRead( void )
{
	uint8_t	PortState = 0u;

//	PortState = (( GPIOA->IDR & 0xFCu ) >> 2 ) | (( GPIOB->IDR & 0x03u ) << 6 );
//	return	(uint8_t)(~PortState);
	PortState = (( GPIOA->IDR & 0xFCu ) >> 2 ) | (( GPIOB->IDR & 0x01u ) << 6 );
	return	(uint8_t)(~PortState) & 0x7F;
}

void	Keyboard_PortInit( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	//	按键使用上拉输入模式
	GPIOA->BSRR = 0x00FCu;
	GPIOB->BSRR = 0x0003u;
	MODIFY_REG( GPIOA->CRL, 0xFFFFFF00u, 0x88888800u );
//	MODIFY_REG( GPIOB->CRL, 0x000000FFu, 0x00000088u );
	MODIFY_REG( GPIOB->CRL, 0x0000000Fu, 0x00000008u );	
}

/********************************** 功能说明 ***********************************
*	按键中断控制
*******************************************************************************/
void	Keyboard_IRQ_Enable( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );

	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PB
          //				| AFIO_EXTICR1_EXTI1_PB
                  | AFIO_EXTICR1_EXTI2_PA
                  | AFIO_EXTICR1_EXTI3_PA
                  ;
	AFIO->EXTICR[1] = AFIO_EXTICR2_EXTI4_PA
                  | AFIO_EXTICR2_EXTI5_PA
                  | AFIO_EXTICR2_EXTI6_PA
                  | AFIO_EXTICR2_EXTI7_PA
                  ;
	CLEAR_BIT( EXTI->EMR,  0x03FFu );	// no event
	CLEAR_BIT( EXTI->IMR,  0x0300u );	// 禁止中断 8#,9#
	SET_BIT(   EXTI->RTSR, 0x00FFu );	// rising edge trigger
	SET_BIT(   EXTI->FTSR, 0x00FFu );	// falling edge trigger
	WRITE_REG( EXTI->PR,   0x00FFu );	// 写1复位中断标志位
//	SET_BIT(   EXTI->IMR,  0x00FFu );	// 允许中断 0#~7#
	SET_BIT(   EXTI->IMR,  0x00FDu );	// 允许中断 0#~7#,PB1不允许中断，改接模拟信号。
}

void	Keyboard_IRQ_Disable( void )
{
	WRITE_REG( EXTI->PR,   0x00FFu );	// 写1复位中断标志位
	CLEAR_BIT( EXTI->IMR,  0x03FFu );	// 禁止中断,包括 8#,9#
}

/********************************** 功能说明 ***********************************
*	OLED9704 端口读写
*******************************************************************************/
#define	Pin_OLED9704_RES	PinBB( GPIOC->ODR, 0U )
#define	Pin_OLED9704_CS 	PinBB( GPIOC->ODR, 2U )
#define	Pin_OLED9704_RD 	PinBB( GPIOC->ODR, 3U )
#define	Pin_OLED9704_WR 	PinBB( GPIOC->ODR, 4U )
#define	Pin_OLED9704_DC 	PinBB( GPIOC->ODR, 5U )
#define	Pin_OLED9704_DISB	PinBB( GPIOC->ODR, 6U )

uint8_t	OLED9704_ReadState( void  )
{
	uint8_t state;

	P8P_ModeInput();
	Pin_OLED9704_DC = 0;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_RD = 0;
	delay_us( 1 );	state = P8P_PortRead();
	Pin_OLED9704_RD = 1;
	Pin_OLED9704_CS = 1;
	P8P_ModeOutput();

	return state;
}

uint8_t	OLED9704_ReadData( void )
{
  uint8_t InData;

	P8P_ModeInput();
	Pin_OLED9704_DC = 1;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_RD = 0;
	delay_us( 1 );
	Pin_OLED9704_RD = 1;
	delay_us( 1 );
	Pin_OLED9704_RD = 0;
	delay_us( 1 );	InData = P8P_PortRead();
	Pin_OLED9704_RD = 1;
	Pin_OLED9704_CS = 1;
	
  return InData;
}

void  OLED9704_WriteData( uint8_t OutData )
{
	P8P_PortWrite( OutData );
	P8P_ModeOutput();
	Pin_OLED9704_DC = 1;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_WR = 0;
	delay_us( 1 );
	Pin_OLED9704_WR = 1;
	Pin_OLED9704_CS = 1;
}

void  OLED9704_WriteReg( uint8_t OutCommand )
{
	P8P_PortWrite( OutCommand );
	P8P_ModeOutput();
	Pin_OLED9704_DC = 0;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_WR = 0;
	delay_us( 1 );
	Pin_OLED9704_WR = 1;
	Pin_OLED9704_CS = 1;
}	

void	OLED9704_DisplayDisable( void )
{
	Pin_OLED9704_DISB = 1;
}

void	OLED9704_DisplayEnable( void )
{
	Pin_OLED9704_DISB = 0;
}

void	OLED9704_PortInit( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	GPIOC->BSRR = 0xFF00u;
	MODIFY_REG( GPIOB->CRH, 0xFFFFFFFFu, 0x33333333u );

	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	GPIOC->BSRR = 0x007Fu;
 	MODIFY_REG( GPIOC->CRL, 0x0FFFFFFFu, 0x03333333u );

 	delay_us( 10u );
 	Pin_OLED9704_RES = 0;
 	delay_us( 10u );
 	Pin_OLED9704_RES = 1;
}

/********************************** 功能说明 ***********************************
*	TM12864 端口读写
*******************************************************************************/
#define	Pin_TM12864_RST 	PinBB( GPIOC->ODR, 0U )
#define	Pin_TM12864_CS2 	PinBB( GPIOC->ODR, 1U )
#define	Pin_TM12864_CS1 	PinBB( GPIOC->ODR, 2U )
#define	Pin_TM12864_EN  	PinBB( GPIOC->ODR, 3U )
#define	Pin_TM12864_RW  	PinBB( GPIOC->ODR, 4U )
#define	Pin_TM12864_DI  	PinBB( GPIOC->ODR, 5U )

uint8_t	TM12864_ReadState( void )
{
	uint8_t	state;
	
	P8P_ModeInput();
	Pin_TM12864_DI = 0;				// 从数据口读数据
	Pin_TM12864_RW = 1;
	Pin_TM12864_EN = 1;
	delay_us( 1 );	state = P8P_PortRead();
	Pin_TM12864_EN = 0;

	return	state;
}

static	BOOL	TM12864_isReady( void )
{
	uint8_t i;

	for ( i = 200U; i != 0U; --i )
	{	// waitting for Ready
		if ( ! ( TM12864_ReadState() & 0x80U ))
		{
			return TRUE;
		}
	}
	return FALSE;
}

uint8_t	TM12864_ReadData( void )
{
	uint8_t	InData;

	if ( !TM12864_isReady()){	return	0U;	}

	P8P_ModeInput();
	Pin_TM12864_DI = 1;				// 数据操作
	Pin_TM12864_RW = 1;				// 读输入
	Pin_TM12864_EN = 1;
	delay_us( 10 );
	Pin_TM12864_EN = 0;				// 从数据口读数据，虚读
	delay_us( 10 );
	Pin_TM12864_EN = 1;
	delay_us( 10 );	InData = P8P_PortRead();
	Pin_TM12864_EN = 0;				// 从数据口读数据，实读

	return InData;
}

void	TM12864_WriteCommand( uint8_t OutCommand )
{   // 命令输出
	if ( !TM12864_isReady()){	return;	}

	P8P_PortWrite( OutCommand );	// 数据输出到数据口
	P8P_ModeOutput();

	Pin_TM12864_DI = 0;				// 命令操作
	Pin_TM12864_RW = 0;				// 写输出
	Pin_TM12864_EN = 1;
	delay_us( 1 );
	Pin_TM12864_EN = 0;				// 输出到LCM
}

void	TM12864_WriteData( uint8_t OutData )
{
	if ( !TM12864_isReady()){	return;	}

	P8P_PortWrite( OutData );		// 数据输出到数据口
	P8P_ModeOutput();

	Pin_TM12864_DI = 1;				// 数据输出
	Pin_TM12864_RW = 0;				// 写输出
	Pin_TM12864_EN = 1;
	delay_us( 1 );
	Pin_TM12864_EN = 0;				// 输出到LCM
}

void	TM12864_SelectL( void )
{
	Pin_TM12864_CS1 = 1;
	Pin_TM12864_CS2 = 0;
}

void	TM12864_SelectR( void )
{
	Pin_TM12864_CS2 = 1;
	Pin_TM12864_CS1 = 0;
}

void	TM12864_PortInit( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	GPIOC->BSRR = 0xFF00u;
	MODIFY_REG( GPIOB->CRH, 0xFFFFFFFFu, 0x33333333u );

	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	GPIOC->BSRR = 0x003Fu;
	Pin_TM12864_EN  = 0;
 	MODIFY_REG( GPIOC->CRL, 0x00FFFFFFu, 0x00333333u );

	delay_us( 10u );
	Pin_TM12864_RST = 0;
	delay_us( 10u );
	Pin_TM12864_RST = 1;
}

void	TM12864_GrayInit( void )
{
	TIM_TypeDef	* TIMx = TIM3;

	SET_BIT( TIMx->CCER, TIM_CCER_CC1E );	//	T3CH1 接LCD的背光电流控制
	SET_BIT( TIMx->CCER, TIM_CCER_CC2E );	//	T3CH2 接LCD的负压发生电路

	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	MODIFY_REG( GPIOC->CRL, 0xFF000000u, 0xBB000000u );		//	PC.6, T3CH1  PC.7  T3CH2
}

/********************************** 功能说明 ***********************************
*	访问单总线
*******************************************************************************/
#define	_OW_Pin_0_Input()		  (PinBB( GPIOA->IDR, 15U ))
#define	_OW_Pin_0_Output(_b)	(PinBB( GPIOA->ODR, 15U )=(_b))

BOOL	OW_0_Init( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );

	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRH, 0xF0000000u, 0x70000000u );

	_OW_Pin_0_Output(1);
	delay_us( 5 );
	return	_OW_Pin_0_Input();
}

static	BOOL	_OW_0_Reset( void )
{
	BOOL	acknowledge;
	// 	DINT();
	delay_us( 1 );						// 延时G(0us)
	_OW_Pin_0_Output(0);				// 把总线拉为低电平
	delay_us( 480U );					// 延时H(480us)
	_OW_Pin_0_Output(1);				// 释放总线		
	delay_us( 70U );					// 延时I(70us)
	acknowledge = ! _OW_Pin_0_Input();	// 主机对总线采样, 0 表示总线上有应答, 1 表示无应答；
	// 	EINT();
	delay_us( 410U );					// 延时J(410us)
	return	acknowledge;
}

static	BOOL	_OW_0_Slot( BOOL bitOut )
{
	BOOL	bitIn;
	// 	DINT();
	_OW_Pin_0_Output(0);				// 将总线拉低启动一次时隙
	delay_us( 6U );						// 延时A(6us)
	_OW_Pin_0_Output(bitOut);			// 输出要写入总线的位
	delay_us( 9U );						// 延时E(9us)
	bitIn = _OW_Pin_0_Input();			// 主机对总线采样, 以读取从机上的数据
	delay_us( 45U );					// 延时C(60us)-A-E == 45us
	_OW_Pin_0_Output(1);				// 时隙开始后的60us总线浮空
	// 	EINT();
	delay_us( 10U );					// 延时D(10us), 等待总线恢复

	return	bitIn;
}


#define	_OW_Pin_1_Input()		(PinBB( GPIOD->IDR,  2U ))
#define	_OW_Pin_1_Output(_b)	(PinBB( GPIOD->ODR,  2U )=(_b))

BOOL	OW_1_Init( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPDEN );
	MODIFY_REG( GPIOD->CRL, 0x00000F00u, 0x00000700u );

	_OW_Pin_1_Output(1);
	delay_us( 5 );
	return	_OW_Pin_1_Input();
}

static	BOOL	_OW_1_Reset( void )
{
	BOOL	acknowledge;
	//	DINT();
	delay_us( 1 );						// 延时G(0us)
	_OW_Pin_1_Output(0);				// 把总线拉为低电平
	delay_us( 480U );					// 延时H(480us)
	_OW_Pin_1_Output(1);				// 释放总线		
	delay_us( 70U );					// 延时I(70us)	70
	acknowledge = ! _OW_Pin_1_Input();	// 主机对总线采样, 0 表示总线上有应答, 1 表示无应答；
	//	EINT();
	delay_us( 410U );					// 延时J(410us)

	return	acknowledge;
}

static	BOOL	_OW_1_Slot( BOOL bitOut )
{
	BOOL	bitIn;
	// 	DINT();
	_OW_Pin_1_Output(0);				// 将总线拉低启动一次时隙
	delay_us( 6U );						// 延时A(6us)
	_OW_Pin_1_Output( bitOut );			// 输出要写入总线的位
	delay_us( 9U );						// 延时E(9us)
	bitIn = _OW_Pin_1_Input();			// 主机对总线采样, 以读取从机上的数据
	delay_us( 45U );					// 延时C(60us)-A-E == 45us
	_OW_Pin_1_Output(1);				// 时隙开始后的60us总线浮空
	// 	EINT();
	delay_us( 10U );					// 延时D(10us), 等待总线恢复

	return	bitIn;
}


__svc_indirect(0)  BOOL	_SVCCall_Reset( BOOL (*)( void ));
__svc_indirect(0)  BOOL	_SVCCall_Slot ( BOOL (*)( BOOL ), BOOL Slot );

BOOL	OW_0_Reset( void )
{
	return	_SVCCall_Reset( _OW_0_Reset );
}

BOOL	OW_0_Slot( BOOL bitOut )
{
	return	_SVCCall_Slot ( _OW_0_Slot, bitOut );
}

BOOL	OW_1_Reset( void )
{
	return	_SVCCall_Reset( _OW_1_Reset );
}

BOOL	OW_1_Slot( BOOL bitOut )
{
	return	_SVCCall_Slot ( _OW_1_Slot, bitOut );
}

/********************************** 功能说明 ***********************************
*	访问I2C总线
*******************************************************************************/
#define	Pin_I2C_SCL_In		PinBB( GPIOB->IDR, 6U )
#define	Pin_I2C_SCL_Out		PinBB( GPIOB->ODR, 6U )
#define	Pin_I2C_SDA_In		PinBB( GPIOB->IDR, 7U )
#define	Pin_I2C_SDA_Out		PinBB( GPIOB->ODR, 7U )

BOOL	bus_i2c_start ( uint8_t Address8Bit, enum I2C_DirectSet DirectSet )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
 	MODIFY_REG( GPIOB->CRL, 0xFF000000u, 0x77000000u );

	//	Verify bus available.
	Pin_I2C_SDA_Out = 1;
	Pin_I2C_SCL_Out = 1;
	delay_us( 10 );
	if( ! Pin_I2C_SDA_In ){	 return  FALSE; }
	if( ! Pin_I2C_SCL_In ){	 return  FALSE; }

	Pin_I2C_SDA_Out = 0;
	delay_us( 1 );
	Pin_I2C_SCL_Out = 0;

	if ( I2C_Write == DirectSet )
	{
		return	bus_i2c_shout( Address8Bit & 0xFEu );
	}
	else
	{
		return	bus_i2c_shout( Address8Bit | 0x01u );
	}
}

void	bus_i2c_stop ( void )
{
	Pin_I2C_SDA_Out = 0;
	delay_us( 1 );
	Pin_I2C_SCL_Out = 1;
	delay_us( 1 );
	Pin_I2C_SDA_Out = 1;
	delay_us( 1 );
}

BOOL	bus_i2c_shout ( uint8_t cOutByte )
{
	BOOL	AcknowlegeState;
	uint8_t	i;

	for( i = 8U; i != 0U; --i )
	{
		if ( cOutByte & 0x80u )
		{
			Pin_I2C_SDA_Out = 1;
		}
		else
		{
			Pin_I2C_SDA_Out = 0;
		}
		cOutByte <<= 1;

		delay_us( 1 );
		Pin_I2C_SCL_Out = 1;

		delay_us( 1 );
		Pin_I2C_SCL_Out = 0;
	}
	
	Pin_I2C_SDA_Out = 1;
	delay_us( 1 );
	Pin_I2C_SCL_Out = 1;
	delay_us( 1 );
 	AcknowlegeState	= Pin_I2C_SDA_In;
	Pin_I2C_SCL_Out = 0;

	if ( I2C_ACK != AcknowlegeState )
	{
		return	FALSE;
	}
	else
	{
		return	TRUE;
	}
}

uint8_t	bus_i2c_shin( enum I2C_AcknowlegeSet AcknowlegeSet )
{
	uint8_t		cInByte = 0U;
	uint8_t		i;

	Pin_I2C_SDA_Out = 1;		// make SDA an input
	for( i = 8U; i != 0U; --i )
	{
		delay_us( 1 );
		Pin_I2C_SCL_Out = 1;

		delay_us( 1 );
		cInByte <<= 1;
		if ( Pin_I2C_SDA_In )
		{
			cInByte |= 0x01u;
		}
		else 
		{
			cInByte &= 0xFEu;
		}

		Pin_I2C_SCL_Out = 0;
	}

	if ( I2C_ACK == AcknowlegeSet )
	{
		Pin_I2C_SDA_Out = 0;
	}
	else
	{
		Pin_I2C_SDA_Out = 1;
	}
	delay_us( 1 );
	Pin_I2C_SCL_Out = 1;
	delay_us( 1 );
	Pin_I2C_SCL_Out = 0;

	return	cInByte;
}

// /********************************** 功能说明 ***********************************
// *	访问 SPI 总线( SPIx )
// *******************************************************************************/
// #ifdef	SimulationSPI

// #define	Pin_SPIxSCK		PinBB( GPIOB->ODR,  3U )
// #define	Pin_SPIxMISO		PinBB( GPIOB->IDR,  4U )
// #define	Pin_SPIxMOSI		PinBB( GPIOB->ODR,  5U )

// uint8_t bus_SPIxShift( uint8_t OutByte )
// {
// 	uint8_t i;
// 	
// 	for ( i = 8u; i != 0u; --i )
// 	{
// 		delay_us( 1 );
// 		if ( OutByte & 0x80u )
// 		{
// 			Pin_SPIxMOSI = 1;
// 		}
// 		else
// 		{
// 			Pin_SPIxMOSI = 0;
// 		}

// 		delay_us( 1 );
// 		Pin_SPIxSCK = 0;

// 		delay_us( 1 );

// 		OutByte <<= 1;
// 		if ( Pin_SPIxMISO )
// 		{
// 			OutByte |= 0x01u;
// 		}
// 		else
// 		{
// 			OutByte &= 0xFEu;
// 		}

// 		delay_us( 1 );
// 		Pin_SPIxSCK = 1;
// 	}
// 	
// 	return	OutByte;
// }


// void	bus_SPIxPortInit( void )
// {
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
//  	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );

//  	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
// 	Pin_SPIxSCK = 1;
//  	MODIFY_REG( GPIOB->CRL, 0x00FFF000u, 0x00343000u );
// }

// #else

// uint8_t bus_SPIxShift( uint8_t IOByte )
// {
// 	SPI_TypeDef * SPIx = SPI1;

// 	while ( ! READ_BIT( SPIx->SR, SPI_SR_TXE ));	// Wait if TXE cleared, Tx FIFO is empty.
// 	SPIx->DR = IOByte;
// 	while ( ! READ_BIT( SPIx->SR, SPI_SR_RXNE ));	// Wait if RXNE cleared, Rx FIFO is empty.
// 	IOByte = SPIx->DR;

// 	return	IOByte;
// }

// void	bus_SPIxPortInit( void )
// {
// 	SPI_TypeDef * SPIx = SPI1;

// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_SPI1EN );

// 	SPIx->CR1	= SPI_CR1_MSTR
// 				| SPI_CR1_CPHA
// 				| SPI_CR1_CPOL
// 				| SPI_CR1_SSM
// 				| SPI_CR1_SSI
// 				| SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
// 				;
// 	SPIx->CR2   = 0;
// 	SET_BIT( SPIx->CR1, SPI_CR1_SPE );
// 	
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
// 	CLEAR_BIT(  AFIO->MAPR, AFIO_MAPR_SWJ_CFG ); 
// 	SET_BIT(    AFIO->MAPR, AFIO_MAPR_SPI1_REMAP );
// 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
//  	MODIFY_REG( GPIOB->CRL, 0x00FFF000u, 0x00B4B000u );
// }

// #endif

/********************************** 功能说明 ***********************************
*	访问 USART1，实现RS232，主要用于实现与微打或上位机进行通讯
*******************************************************************************/
void	UART1_Send( uint8_t OutByte )
{
	USART_TypeDef * USARTx = USART1;

	while ( ! ( READ_BIT( USARTx->SR, USART_SR_TXE )))
	{
		;
	}
	USARTx->DR = OutByte;
}

void	UART1_Init ( void )
{
	USART_TypeDef * USARTx = USART1;
	
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_USART1EN );

	// USART1 configured as follow:
	// 	- BaudRate = 9600 baud  
	// 	- Word Length = 8 Bits
	// 	- One Stop Bit
	// 	- No parity
	// 	- Hardware flow control disabled (RTS and CTS signals)
	// 	- Receive disable and transmit enabled
	USARTx->BRR = SystemCoreClock / 9600u;	/* 9600 bps							*/
	USARTx->CR1 = 0x0000u;					/* 1 start bit, 8 data bits         */
	USARTx->CR2 = 0x0000u;					/* 1 stop bit                       */
	USARTx->CR3 = 0x0000u; 					/* no flow control                  */
	SET_BIT( USARTx->CR1, USART_CR1_TE );	/* enable TX 						*/
	SET_BIT( USARTx->CR1, USART_CR1_UE );	/* Enable USARTx					*/

	/* Configure USART1 Rx (PA10) as input floating */
	/* Configure USART1 Tx (PA9) as alternate function push-pull */
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRH, 0x00000FF0u, 0x000004B0u );
}


/********************************** 功能说明 ***********************************
*	访问 BKP 数据
*******************************************************************************/
void	BackupRegister_Write( uint16_t BKP_DR, uint16_t usData )
{
	uint32_t tmp = (uint32_t)BKP_BASE + BKP_DR;
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_BKPEN );	//	允许访问备份区域
	*(__IO uint16_t *) tmp = usData;
}

uint16_t	BackupRegister_Read( uint16_t BKP_DR )
{
	uint32_t tmp = (uint32_t)BKP_BASE + BKP_DR;
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_BKPEN );	//	允许访问备份区域
	return	(*(__IO uint16_t *) tmp );
}


/********************************** 功能说明 ***********************************
*	使用 TIM1 计数 ETR 脉冲，测量风扇转动圈数。
*******************************************************************************/
static	void	TIM1_Init( void )
{
	TIM_TypeDef	* TIMx = TIM1;

	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_TIM1EN );

	TIMx->SMCR	= TIM_SMCR_ETP 
              | TIM_SMCR_ETF_3
              | TIM_SMCR_ETPS_0
              ;
        
	/* Enable the TIM Counter */
	SET_BIT( TIMx->SMCR, TIM_SMCR_ECE );
 	SET_BIT( TIMx->CR1, TIM_CR1_CEN );

	// 	//	使用 TI2
	//  	CLEAR_BIT( TIMx->CCER,  TIM_CCER_CC2E );
	//  	MODIFY_REG( TIMx->CCMR1, TIM_CCMR1_CC2S | TIM_CCMR1_IC2F, TIM_CCMR1_CC2S_0 );
	//  	SET_BIT(   TIMx->CCER,  TIM_CCER_CC2P | TIM_CCER_CC2E );

	// 	/* Select the Trigger source */
	// 	MODIFY_REG( TIMx->SMCR, TIM_SMCR_TS, TIM_SMCR_TS_2 | TIM_SMCR_TS_1 );	//	TIM_TIxExternalCLK1Source_TI2
	// 	/* Select the External clock mode1 */
	// 	SET_BIT( TIMx->SMCR, TIM_SMCR_SMS );	//	TIM_SlaveMode_External1
}

/********************************** 功能说明 ***********************************
*	使用 TIM2 实现 直流PWM 电机调速
*******************************************************************************/
#define	_TIM2_PWM_Period	2400u

void	TIM2_Init( void )
{
	TIM_TypeDef	* TIMx = TIM2;

	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_TIM2EN );
	//	TIMx 时基初始化: 输入时钟频率24MHz，PWM分辨率2400，PWM频率10KHz。
	TIMx->CR1 = 0u;
	TIMx->PSC = 0u;
	TIMx->ARR = ( _TIM2_PWM_Period - 1u );
	TIMx->EGR = TIM_EGR_UG;

	//	配置过程中暂时全部关闭输出
	TIMx->CCER = 0u;
	TIMx->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1
              | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	TIMx->CCMR2 = 0u;
	TIMx->CCR1 = 0u;
	TIMx->CCR2 = 0u;
	SET_BIT( TIMx->CCER, TIM_CCER_CC2E );	//	T2CH2 接直流电机调速
	//	TIMx 使能
	SET_BIT( TIMx->CR1, TIM_CR1_CEN );

	//	配置管脚: PA.0 <-> T2CH1/ETR, PA.1 <-> T2CH2
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRL, 0x000000FFu, 0x000000BBu );
}

void	TIM2_OutCmd( BOOL NewState )
{
	TIM_TypeDef * TIMx = TIM2;

	if ( NewState )
	{
		SET_BIT( TIMx->CCER, TIM_CCER_CC2E );
	}
	else
	{
		CLEAR_BIT( TIMx->CCER, TIM_CCER_CC2E );
	}
}

void	TIM2_SetOutput( uint16_t OutValue )
{
	TIM_TypeDef * TIMx = TIM2;

	TIMx->CCR2 = (uint32_t)OutValue * _TIM2_PWM_Period / PWM_Output_Max;
}

/********************************** 功能说明 ***********************************
*	使用 TIM3 实现 PWM功能，输出范围固定：0 ~ PWM_Output_Max(通常是27648u)
*******************************************************************************/
#define	_TIM3_PWM_Period	2400u

static	uint16_t	_TIM3_PWM_Regular( uint16_t OutValue )
{
	return	(uint32_t)OutValue * _TIM3_PWM_Period / PWM_Output_Max;
}

static	void	TIM3_Init( void )
{
	TIM_TypeDef	* TIMx = TIM3;

	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_TIM3EN );
	//	TIMx 时基初始化: 输入时钟频率24MHz，PWM分辨率2400, PWM频率10KHz
	TIMx->CR1  = 0u;
	TIMx->PSC  = 0u;
	TIMx->ARR  = ( _TIM3_PWM_Period - 1u );
	TIMx->EGR  = TIM_EGR_UG;

	//	配置过程中暂时全部关闭输出
	TIMx->CCER = 0u;
	TIMx->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1
              | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	TIMx->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1
              | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
	TIMx->CCR1 = 0u;
	TIMx->CCR2 = 0u;
	TIMx->CCR3 = 0u;
	TIMx->CCR4 = 0u;

//	SET_BIT( TIMx->CCER, TIM_CCER_CC3E );	//	T3CH3 接大气采样泵
//	SET_BIT( TIMx->CCER, TIM_CCER_CC4E );	//	T3CH4 接制冷箱风扇
	//	TIMx 使能
	SET_BIT( TIMx->CR1, TIM_CR1_CEN );

	//	配置输出管脚
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_TIM3_REMAP, AFIO_MAPR_TIM3_REMAP_FULLREMAP );
//	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
//	MODIFY_REG( GPIOC->CRL, 0x0F000000u, 0x0B000000u );		//	PC.6, T3CH1
//	MODIFY_REG( GPIOC->CRL, 0xF0000000u, 0xB0000000u );		//	PC.7, T3CH2
// 	MODIFY_REG( GPIOC->CRH, 0x0000000Fu, 0x0000000Bu );		//	PC.8, T3CH3
// 	MODIFY_REG( GPIOC->CRH, 0x000000F0u, 0x000000B0u );		//	PC.9, T3CH4
}

void	PWM1_SetOutput( uint16_t OutValue )
{
	TIM_TypeDef * TIMx = TIM3;
	
	TIMx->CCR1 = _TIM3_PWM_Regular( OutValue );
}

void	PWM2_SetOutput( uint16_t OutValue )
{
	TIM_TypeDef * TIMx = TIM3;
	
	TIMx->CCR2 = _TIM3_PWM_Regular( OutValue );
}

void	PWM3_SetOutput( uint16_t OutValue )
{
	TIM_TypeDef * TIMx = TIM3;
	
	TIMx->CCR3 = _TIM3_PWM_Regular( OutValue );
}

void	PWM4_SetOutput( uint16_t OutValue )
{
	TIM_TypeDef * TIMx = TIM3;
	
	TIMx->CCR4 = _TIM3_PWM_Regular( OutValue );
}

/********************************** 功能说明 ***********************************
*	测量风扇转速（此处实际是测量风扇转的圈数，实际转速需要根据时间进一步确定）
*******************************************************************************/
void	HCBoxFan_Circle_PortInit( void )
{
	TIM1_Init();

	// PA.12 上拉输入模式
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	GPIOA->BSRR = GPIO_BSRR_BS12;
	MODIFY_REG( GPIOA->CRH, 0x000F0000u, 0x00080000u );
}

uint16_t	HCBoxFan_Circle_Read( void )
{
	return	TIM1->CNT;
}

/********************************** 功能说明 ***********************************
*	访问 GPIO  
*******************************************************************************/
void	MeasureBattery_OutCmd( BOOL NewState )
{	//	PC8 高电平有效
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	GPIOC->BSRR = NewState ? GPIO_BSRR_BS8 : GPIO_BSRR_BR8;
 	MODIFY_REG( GPIOC->CRH, 0x0000000Fu, 0x00000003u );
}
void	AIRLightOutCmd( BOOL NewState )
{	//	PC9 高电平有效
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	GPIOC->BSRR = NewState ? GPIO_BSRR_BS9 : GPIO_BSRR_BR9;
 	MODIFY_REG( GPIOC->CRH, 0x000000F0u, 0x00000030u );
}
// void	HCBoxHeat_OutCmd( BOOL NewState )
// {	//	PA.8
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
// 	GPIOA->BSRR = NewState ? GPIO_BSRR_BS8 : GPIO_BSRR_BR8;
//  	MODIFY_REG( GPIOA->CRH, 0x0000000Fu, 0x00000003u );
// }

// void	HCBoxCool_OutCmd( BOOL NewState )
// {	//	PA.11
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
// 	GPIOA->BSRR = NewState ? GPIO_BSRR_BS11 : GPIO_BSRR_BR11;
// 	MODIFY_REG( GPIOA->CRH, 0x0000F000u, 0x00003000u );
// }

// void	HCBoxFan_OutCmd( BOOL NewState )
// {	//	PC.9, on-off mode
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
// 	GPIOC->BSRR = NewState ? GPIO_BSRR_BS9 : GPIO_BSRR_BR9;
// 	MODIFY_REG( GPIOC->CRH, 0x000000F0u, 0x00000030u );
// }

void	Speaker_OutCmd( BOOL NewState )
{	//	PB.2(BOOT1), 上电复位时外部下拉到GND.
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	GPIOB->BSRR = NewState ? GPIO_BSRR_BS2 : GPIO_BSRR_BR2;
 	MODIFY_REG( GPIOB->CRL, 0x00000F00u, 0x00000300u );
}

void	beep( void )
{
// 	Speaker_OutCmd( TRUE );
	delay( 200u );
	Speaker_OutCmd( FALSE );
}

void	tick( void )
{
// 	Speaker_OutCmd( TRUE );
	delay( 20u );
	Speaker_OutCmd( FALSE );
}

void	PumpAIR_OutCmd( BOOL NewState )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	GPIOB->BSRR = NewState ? GPIO_BSRR_BS8 : GPIO_BSRR_BR8;
	MODIFY_REG( GPIOB->CRH, 0x0000000Fu, 0x00000003u );
}

BOOL	PumpAIR_GetOutState( void )
{
	return	READ_BIT( GPIOB->IDR, GPIO_IDR_IDR8 ) ? TRUE : FALSE;
}

BOOL	UART1_RX_Pin_State( void )
{
	//	PA.10 for USART1 RX.
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRH, 0x00000F00u, 0x00000400u );
	
	return	READ_BIT( GPIOA->IDR, GPIO_IDR_IDR10 ) ? TRUE : FALSE;
}

/********************************** 功能说明 ***********************************
*	读取 JTAG 上的两个跳线，如果插着跳线，返回1，否则返回0。
*******************************************************************************/
uint8_t	get_Jumper( void )
{
	uint8_t		JumperState;
	uint16_t	portState, prevState;
	uint8_t 	i, iRetryMax = 100u;

	//	PA.13 - JTMS/SWDIO, 默认内部上拉，但插跳线时与VCC相连
	//	PA.14 - JTCK/SWCLK, 默认内部下拉，但插跳线时与GND相连

	//	暂时关闭调试功能
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_DISABLE );		/*!< JTAG-DP Disabled and SW-DP Disabled */
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	GPIOA->BSRR = GPIO_BSRR_BR13;	//	改为内部下拉
	GPIOA->BSRR = GPIO_BSRR_BS14;	//	改为内部上拉
	MODIFY_REG( GPIOA->CRH, 0x0FF00000u, 0x08800000u );

	//	读取跳线状态，持续30ms状态不变，才确认读取结果有效。
	portState = 0u;
	i = 0u;
	prevState = portState;
	do {
		delay_us( 300u );
		portState = READ_BIT( GPIOA->IDR, GPIO_IDR_IDR13 | GPIO_IDR_IDR14 );
		if ( prevState != portState )
		{
			i = 0u;
			prevState = portState;
		}
	} while ( ++i < iRetryMax );
	
	JumperState = ((( portState >> 13 ) & 0x03u ) ^ 0x02u );	//	不插跳线结果为0，插跳线结果为1。

	//	重新打开调试功能
	GPIOA->BSRR = GPIO_BSRR_BS13;		//	改回默认上拉
	GPIOA->BSRR = GPIO_BSRR_BR14;		//	改回默认下拉
	CLEAR_BIT( AFIO->MAPR, AFIO_MAPR_SWJ_CFG );
	SET_BIT( AFIO->MAPR, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );	/*!< JTAG-DP Disabled and SW-DP Enabled */

	return	JumperState;
}

/********************************** 功能说明 ***********************************
 *　Brief:	Enters SLEEP mode.
*******************************************************************************/
void	NVIC_Sleep( void )
{
	CLEAR_BIT( SCB->SCR, SCB_SCR_SLEEPONEXIT_Msk | SCB_SCR_SLEEPDEEP_Msk );
	__WFE();
}

/********************************** 功能说明 ***********************************
*	NVIC 配置
*******************************************************************************/
void	NVIC_IRQ_Cmd( IRQn_Type IRQn, BOOL NewState )
{	//	使用 SVC 访问 NVIC，实现中断的开关控制。
	__svc_indirect(0) void _SVC_Call( void (*)( IRQn_Type IRQn ), IRQn_Type IRQn );

	if ( NewState )
	{
		_SVC_Call( NVIC_EnableIRQ, IRQn );
	}
	else
	{
		_SVC_Call( NVIC_DisableIRQ, IRQn );
	}
}

static	void	NVIC_Init( void )
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
	NVIC_SetPriorityGrouping( 7 );	//	禁止中断抢占，即没有中断嵌套

	//	for Keyboard.
	NVIC_EnableIRQ( EXTI0_IRQn );
	NVIC_EnableIRQ( EXTI1_IRQn );
	NVIC_EnableIRQ( EXTI2_IRQn );
	NVIC_EnableIRQ( EXTI3_IRQn );
	NVIC_EnableIRQ( EXTI4_IRQn );
	NVIC_EnableIRQ( EXTI9_5_IRQn );

	//	for MODBUS.
	NVIC_EnableIRQ( TIM4_IRQn );
	NVIC_EnableIRQ( USART3_IRQn );

	//	__set_CONTROL( 0x03u );		//	切换到PSP, 并转入非特权模式
}

/********************************** 功能说明 ***********************************
*	BIOS 初始化
*******************************************************************************/
void	BIOS_Init( void )
{
	NVIC_Init();		//	在特权模式下执行低层配置，配置完成后(可选)切换到非特权模式。
//	GPIO_Init();		//	主要对未用的端口进行配置，让系统更加稳定一些。
//	TIM1_Init();		//	for HCBox Fan Speed.
	TIM2_Init();		//	for 100L Motor PWMx1.
	TIM3_Init();		//	for PWMx4。
}


/********************************** 功能说明 ***********************************
 *	for PC-Lint
*******************************************************************************/
//	#ifdef	_lint
//	BOOL	_OW_SVC_Reset( BOOL (*Handler)( void )            ) {  return Handler(      ); }
//	BOOL	_OW_SVC_Slot ( BOOL (*Handler)( BOOL ), BOOL Slot ) {  return Handler( Slot ); }
//	#endif

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
