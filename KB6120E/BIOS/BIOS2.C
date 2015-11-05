/**************** (C) COPYRIGHT 2012 青岛金仕达电子科技有限公司 ****************
* 文 件 名: BIOS.C
* 创 建 者: 董峰
* 描  述  : KB6120E 芯片端口访问
* 最后修改: 2013年8月7日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "Pin.H"
#include "BIOS.H"

#include "system_stm32f10x.c"

// 	#define	DINT()	__disable_irq()
// 	#define	EINT()	__enable_irq()

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
*	使用 GPIOB 的高8位 实现 8 位 并行IO
*******************************************************************************/
#define	Port_P8P_Input()		( GPIOB->CRH  = 0x44444444u )
#define	Port_P8P_Output()		( GPIOB->CRH  = 0x33333333u )
#define	Port_P8P_Read()			((uint8_t)( GPIOB->IDR >> 8 ))

static	void	Port_P8P_Write( uint8_t OutByte )
{
	GPIOB->BSRR = 0xFF000000uL | (( OutByte << 8 ) & 0x0000FF00u );
}

/********************************** 功能说明 ***********************************
*	短延时程序，用于硬件同步，精确延时需要防止中断的干扰
*******************************************************************************/
#pragma O3
void	delay_us ( uint32_t us )
{
	while ( us-- )
	{
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
	}
}

/********************************** 功能说明 ***********************************
*	按键端口扫描
*******************************************************************************/
void	Keyboard_PortInit( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	//	按键使用上拉输入模式
	GPIOA->BSRR = 0x00FCu;
	GPIOB->BSRR = 0x0003u;
	MODIFY_REG( GPIOA->CRL, 0xFFFFFF00u, 0x88888400u );
	MODIFY_REG( GPIOB->CRL, 0x000000FFu, 0x00000088u );
}

uint8_t	Keyboard_PortRead( void )
{
	uint8_t	PortState = 0u;

	PortState = (( GPIOA->IDR & 0xFCu ) >> 2 ) | (( GPIOB->IDR & 0x03u ) << 6 );

	return	PortState;
}

/********************************** 功能说明 ***********************************
*	按键中断驱动
*******************************************************************************/
void	Keyboard_IRQ_Enable( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );

	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PB
					| AFIO_EXTICR1_EXTI1_PB
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
	SET_BIT(   EXTI->IMR,  0x00FFu );	// 允许中断 0#~7#
}

static	void	ISR_Keyboard( void )
{
	CLEAR_BIT( EXTI->IMR, 0x03FFu );	// 禁止中断,包括 8#,9#
	Keyboard_IRQ_Sever();				// 发信号给按键扫描任务
}

__irq	void	EXTI0_IRQHandler( void )
{
	ISR_Keyboard();
}

__irq	void	EXTI1_IRQHandler( void )
{
	ISR_Keyboard();
}

__irq	void	EXTI2_IRQHandler( void )
{
	ISR_Keyboard();
}

__irq	void	EXTI3_IRQHandler( void )
{
	ISR_Keyboard();
}

__irq	void	EXTI4_IRQHandler( void )
{
	ISR_Keyboard();
}

__irq	void	EXTI9_5_IRQHandler( void )
{
	ISR_Keyboard();
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

	Port_P8P_Input();
	Pin_OLED9704_DC = 0;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_RD = 0;
	delay_us( 1 );	state = Port_P8P_Read();
	Pin_OLED9704_RD = 1;
	Pin_OLED9704_CS = 1;
	Port_P8P_Output();

	return state;
}

uint8_t	OLED9704_ReadData( void  )
{
    uint8_t InData;

	Port_P8P_Input();
	Pin_OLED9704_DC = 1;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_RD = 0;
	delay_us( 1 );
	Pin_OLED9704_RD = 1;
	delay_us( 1 );
	Pin_OLED9704_RD = 0;
	delay_us( 1 );	InData = Port_P8P_Read();
	Pin_OLED9704_RD = 1;
	Pin_OLED9704_CS = 1;

    return InData;
}

void  OLED9704_WriteData( uint8_t OutData )
{
	Port_P8P_Write( OutData );
	Port_P8P_Output();
	Pin_OLED9704_DC = 1;

	Pin_OLED9704_CS = 0;
	Pin_OLED9704_WR = 0;
	delay_us( 1 );
	Pin_OLED9704_WR = 1;
	Pin_OLED9704_CS = 1;
}

void  OLED9704_WriteReg( uint8_t OutCommand )
{
	Port_P8P_Write( OutCommand );
	Port_P8P_Output();
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
 	MODIFY_REG( GPIOB->CRL, 0x0FFFFFFFu, 0x03333333u );

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
	
	Port_P8P_Input();
	Pin_TM12864_DI = 0;				// 从数据口读数据
	Pin_TM12864_RW = 1;
	Pin_TM12864_EN = 1;
	delay_us( 1 );	state = Port_P8P_Read();
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

	Port_P8P_Input();
	Pin_TM12864_DI = 1;				// 数据操作
	Pin_TM12864_RW = 1;				// 读输入
	Pin_TM12864_EN = 1;
	delay_us( 1 );
	Pin_TM12864_EN = 0;				// 从数据口读数据，虚读
	delay_us( 1 );
	Pin_TM12864_EN = 1;
	delay_us( 1 );	InData = Port_P8P_Read();
	Pin_TM12864_EN = 0;				// 从数据口读数据，实读

	return InData;
}

void	TM12864_WriteCommand( uint8_t OutCommand )
{   // 命令输出
	if ( !TM12864_isReady()){	return;	}

	Port_P8P_Write( OutCommand );	// 数据输出到数据口
	Port_P8P_Output();

	Pin_TM12864_DI = 0;				// 命令操作
	Pin_TM12864_RW = 0;				// 写输出
	Pin_TM12864_EN = 1;
	delay_us( 1 );
	Pin_TM12864_EN = 0;				// 输出到LCM
}

void	TM12864_WriteData( uint8_t OutData )
{
	if ( !TM12864_isReady()){	return;	}

	Port_P8P_Write( OutData );		// 数据输出到数据口
	Port_P8P_Output();

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

	delay_us( 10 );
	Pin_TM12864_RST = 0;
	delay_us( 10 );
	Pin_TM12864_RST = 1;
}


/********************************** 功能说明 ***********************************
*	访问DS18B20的单总线
*******************************************************************************/
#define	Pin_DS18B20_0_In	PinBB( GPIOA->IDR, 15U )
#define	Pin_DS18B20_0_Out	PinBB( GPIOA->ODR, 15U )

#define	Pin_DS18B20_1_In	PinBB( GPIOD->IDR, 2U )
#define	Pin_DS18B20_1_Out	PinBB( GPIOD->ODR, 2U )

__svc_indirect(0)	BOOL	_OW_SVC_Reset( BOOL (*fun)( void ));
__svc_indirect(0)	BOOL	_OW_SVC_Slot( BOOL (*fun)( BOOL ), BOOL );

BOOL	OW_0_Init( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRH, 0xF0000000u, 0x70000000u );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );

	Pin_DS18B20_0_Out = 1;
	delay_us( 5 );
	return	(BOOL)Pin_DS18B20_0_In;
}

static	BOOL	_OW_0_Reset( void )
{
	BOOL	acknowledge;
	// 	DINT();
	delay_us( 1 );						// 延时G(0us)
	Pin_DS18B20_0_Out = 0;				// 把总线拉为低电平
	delay_us( 480U );					// 延时H(480us)
	Pin_DS18B20_0_Out = 1;				// 释放总线		
	delay_us( 70U );					// 延时I(70us)
	acknowledge = ! Pin_DS18B20_0_In;	// 主机对总线采样, 0 表示总线上有应答, 1 表示无应答；
	// 	EINT();
	delay_us( 410U );					// 延时J(410us)
	return	acknowledge;
}

static	BOOL	_OW_0_Slot( BOOL bitOut )
{
	BOOL	bitIn;
	// 	DINT();
	Pin_DS18B20_0_Out = 0;				// 将总线拉低启动一次时隙
	delay_us( 6U );						// 延时A(6us)
	Pin_DS18B20_0_Out = bitOut;			// 输出要写入总线的位
	delay_us( 9U );						// 延时E(9us)
	bitIn = Pin_DS18B20_0_In;			// 主机对总线采样, 以读取从机上的数据
	delay_us( 45U );					// 延时C(60us)-A-E == 45us
	Pin_DS18B20_0_Out = 1;				// 时隙开始后的60us总线浮空
	// 	EINT();
	delay_us( 10U );					// 延时D(10us), 等待总线恢复

	return	bitIn;
}

BOOL	OW_0_Reset( void )
{
	return	_OW_SVC_Reset( _OW_0_Reset );
}

BOOL	OW_0_Slot( BOOL bitOut )
{
	return	_OW_SVC_Slot( _OW_0_Slot, bitOut );
}

BOOL	OW_1_Init( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPDEN );
	MODIFY_REG( GPIOD->CRL, 0x00000F00u, 0x00000700u );

	Pin_DS18B20_1_Out = 1;
	delay_us( 5 );
	return	(BOOL)Pin_DS18B20_1_In;
}

static	BOOL	_OW_1_Reset( void )
{
	BOOL	acknowledge;
	//	DINT();
	delay_us( 1 );						// 延时G(0us)
	Pin_DS18B20_1_Out = 0;				// 把总线拉为低电平
	delay_us( 480U );					// 延时H(480us)
	Pin_DS18B20_1_Out = 1;				// 释放总线		
	delay_us( 70U );					// 延时I(70us)	70
	acknowledge = ! Pin_DS18B20_1_In;	// 主机对总线采样, 0 表示总线上有应答, 1 表示无应答；
	//	EINT();
	delay_us( 410U );					// 延时J(410us)

	return	acknowledge;
}

static	BOOL	_OW_1_Slot( BOOL bitOut )
{
	BOOL	bitIn;
	// 	DINT();
	Pin_DS18B20_1_Out = 0;				// 将总线拉低启动一次时隙
	delay_us( 6U );						// 延时A(6us)
	Pin_DS18B20_1_Out = bitOut;			// 输出要写入总线的位
	delay_us( 9U );						// 延时E(9us)
	bitIn = Pin_DS18B20_1_In;			// 主机对总线采样, 以读取从机上的数据
	delay_us( 45U );					// 延时C(60us)-A-E == 45us
	Pin_DS18B20_1_Out = 1;				// 时隙开始后的60us总线浮空
	// 	EINT();
	delay_us( 10U );					// 延时D(10us), 等待总线恢复

	return	bitIn;
}

BOOL	OW_1_Reset( void )
{
	return	_OW_SVC_Reset( _OW_1_Reset );
}

BOOL	OW_1_Slot( BOOL bitOut )
{
	return	_OW_SVC_Slot( _OW_1_Slot, bitOut );
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
	if( ! Pin_I2C_SDA_In ){	return	FALSE;	}
	if( ! Pin_I2C_SCL_In ){	return	FALSE;	}

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


/********************************** 功能说明 ***********************************
*	访问 SPI 总线( SPI1_ )
*******************************************************************************/
// 	#define	Pin_SPI1_SCK		PinBB( GPIOA->ODR, 5U )
// 	#define	Pin_SPI1_MISO		PinBB( GPIOA->IDR, 6U )
// 	#define	Pin_SPI1_MOSI		PinBB( GPIOA->ODR, 7U )

// 	uint8_t bus_SPI1_Shift( uint8_t cIOByte )
// 	{
// 		uint8_t	i;

// 		for ( i = 8U; i != 0U; -- i ){

// 			if ( cIOByte & 0x80U )
// 			{
// 				Pin_SPI1_MOSI = 1;
// 			}
// 			else
// 			{
// 				Pin_SPI1_MOSI = 0;
// 			}
// 				
// 			delay_us( 1 );

// 			Pin_SPI1_SCK = 1;

// 			delay_us( 2 );

// 			Pin_SPI1_SCK = 0;

// 			delay_us( 1 );

// 			cIOByte <<= 1;
// 			if ( Pin_SPI1_MISO )
// 			{
// 				cIOByte |= 0x01u;
// 			}
// 			else
// 			{
// 				cIOByte &= 0xFEu;
// 			}
// 		}

// 		return	cIOByte;
// 	}

// 	void	bus_SPI1_PortInit( void )
// 	{
// 		SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
// 		MODIFY_REG( GPIOA->CRL, 0xFFF00000u, 0x34300000u );
// 		Pin_SPI1_SCK = 0;
// 	}


/********************************** 功能说明 ***********************************
*	访问 SPI 总线( SPI1x )
*******************************************************************************/
#define	Pin_SPI1xDRDY		PinBB( GPIOA->IDR, 15U )
#define	Pin_SPI1xNSS		PinBB( GPIOA->ODR, 15U )
#define	Pin_SPI1xSCK		PinBB( GPIOB->ODR, 3U )
#define	Pin_SPI1xMISO		PinBB( GPIOB->IDR, 4U )
#define	Pin_SPI1xMOSI		PinBB( GPIOB->ODR, 5U )

uint8_t bus_SPI1xShift( uint8_t OutByte )
{
	uint8_t i;
	
	for ( i = 8u; i != 0u; --i )
	{
		delay_us( 1 );
		if ( OutByte & 0x80u )
		{
			Pin_SPI1xMOSI = 1;
		}
		else
		{
			Pin_SPI1xMOSI = 0;
		}

		delay_us( 1 );
		Pin_SPI1xSCK = 0;

		delay_us( 1 );

		OutByte <<= 1;
		if ( Pin_SPI1xMISO )
		{
			OutByte |= 0x01u;
		}
		else
		{
			OutByte &= 0xFEu;
		}

		delay_us( 1 );
		Pin_SPI1xSCK = 1;
	}
	
	return	OutByte;
}


void	bus_SPI1xPortInit( void )
{
 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPBEN );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );

 	MODIFY_REG( GPIOB->CRL, 0x00FFF000u, 0x00343000u );
	Pin_SPI1xSCK = 1;
}


/********************************** 功能说明 ***********************************
*	访问 BKP 数据
*******************************************************************************/
void	BackupRegister_Write( uint16_t BKP_DR, uint16_t Data )
{
	uint32_t tmp = (uint32_t)BKP_BASE + BKP_DR;

	*(__IO uint16_t *) tmp = Data;
}

uint16_t	BackupRegister_Read( uint16_t BKP_DR )
{
	uint32_t tmp = (uint32_t)BKP_BASE + BKP_DR;

	return (*(__IO uint16_t *) tmp );
}

/********************************** 功能说明 ***********************************
*	访问 STM32 的 RTC 模块
*******************************************************************************/
BOOL	STM32_RTC_Init( void )
{
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_BKPEN );
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_PWREN );
	SET_BIT( PWR->CR, PWR_CR_DBP );				/* Allow access to BKP Domain */

	/*	包含将时钟计数器初始化为 GMT: Sun, 1 Jan 2012 00:00:00 UTC */
	if ( BackupRegister_Read( BKP_DR10 ) != 0x5AA5u )
	{
		/* Reset Backup Domain */
		SET_BIT( RCC->BDCR, RCC_BDCR_BDRST );
 		RCC->BDCR = 0u;  									/* Reset LSEON bit */
		RCC->BDCR = 0u;  									/* Reset LSEBYP bit */
		RCC->BDCR = RCC_BDCR_LSEON;							/* Enable LSE */
		while ( ! READ_BIT( RCC->BDCR, RCC_BDCR_LSERDY ));	/* Wait till LSE is ready */
		SET_BIT( RCC->BDCR, RCC_BDCR_RTCSEL_LSE );			/* Select LSE as RTC Clock Source */
		SET_BIT( RCC->BDCR, RCC_BDCR_RTCEN );				/* Enable RTC Clock */

		/* Wait for RTC registers synchronization */
		CLEAR_BIT( RTC->CRL, RTC_CRL_RSF );
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RSF ));

		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF ));	//	RTC_WaitForLastTask();
		RTC->CRH = 0u;									//	禁止RTC中断

		/* Wait until last write operation on RTC registers has finished */
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF ));	//	RTC_WaitForLastTask();
		SET_BIT( RTC->CRL, RTC_CRL_CNF );				//  RTC_EnterConfigMode();
		/* Set Alarm to 0 */
		RTC->ALRH = 0x0000u;
		RTC->ALRL = 0x0000u;		
		/* Set RTC prescaler: set RTC period to 1sec */
		RTC->PRLH = 0x0000u;
		RTC->PRLL = 0x7FFFu;
		/*	时钟计数器初始化为 GMT: Sun, 1 Jan 2012 00:00:00 UTC, see http://www.epochconverter.com */
		RTC->CNTH = (uint16_t)( 1325376000u >> 16 );
		RTC->CNTL = (uint16_t)( 1325376000u );
		CLEAR_BIT( RTC->CRL, RTC_CRL_CNF ); 			//  RTC_ExitConfigMode();
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF ));	//	RTC_WaitForLastTask();

		BackupRegister_Write( BKP_DR10, 0x5AA5u );
	}
	return	TRUE;
}

void	STM32_RTC_Save( uint32_t timer )
{
	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF ));	//	RTC_WaitForLastTask();
	SET_BIT( RTC->CRL, RTC_CRL_CNF );				//  RTC_EnterConfigMode();
	RTC->CNTH = (uint16_t)( timer >> 16 );
	RTC->CNTL = (uint16_t)( timer );
	CLEAR_BIT( RTC->CRL, RTC_CRL_CNF ); 			//  RTC_ExitConfigMode();
	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF ));	//	RTC_WaitForLastTask();
}

uint32_t	STM32_RTC_Load( void )
{
	return (((uint32_t)RTC->CNTH << 16 ) | RTC->CNTL );
}


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
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );

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
	MODIFY_REG( GPIOA->CRH, 0x00000FF0u, 0x000004B0u );
}

/********************************** 功能说明 ***********************************
*	访问 STM32 的 ADC 模块
*******************************************************************************/
static	volatile	uint16_t	ADCResultBuf[16][4];

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
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  1 - ADC1_SMPR2_BASE ) ) )	//	通道1
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  2 - ADC1_SMPR2_BASE ) ) )	//	通道2
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  3 - ADC1_SMPR2_BASE ) ) )	//	通道3
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  4 - ADC1_SMPR2_BASE ) ) )	//	通道4
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  5 - ADC1_SMPR2_BASE ) ) )	//	通道5
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  6 - ADC1_SMPR2_BASE ) ) )	//	通道6
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  7 - ADC1_SMPR2_BASE ) ) )	//	通道7
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  8 - ADC1_SMPR2_BASE ) ) )	//	通道8
				|	( ( ADC1_SMP_1p5_C   ) << ( ADC1_SMPR_WIDTH * (  9 - ADC1_SMPR2_BASE ) ) )	//	通道9
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
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  2 - ADC1_SQR3_BASE ) ) )	//	序列2
				|	( ( 17 ) << ( ADC1_SQR_WIDTH * (  3 - ADC1_SQR3_BASE ) ) )	//	序列3
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  4 - ADC1_SQR3_BASE ) ) )	//	序列4
				|	( ( 16 ) << ( ADC1_SQR_WIDTH * (  5 - ADC1_SQR3_BASE ) ) )	//	序列5
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  6 - ADC1_SQR3_BASE ) ) )	//	序列6
				;
	ADC1->SQR2  =	( ( 17 ) << ( ADC1_SQR_WIDTH * (  7 - ADC1_SQR2_BASE ) ) )	//	序列7
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * (  8 - ADC1_SQR2_BASE ) ) )	//	序列8
				|	( ( 16 ) << ( ADC1_SQR_WIDTH * (  9 - ADC1_SQR2_BASE ) ) )	//	序列9
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 10 - ADC1_SQR2_BASE ) ) )	//	序列10
				|	( ( 17 ) << ( ADC1_SQR_WIDTH * ( 11 - ADC1_SQR2_BASE ) ) )	//	序列11
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 12 - ADC1_SQR2_BASE ) ) )	//	序列12
				;
	ADC1->SQR1  =	( ( 16 ) << ( ADC1_SQR_WIDTH * ( 13 - ADC1_SQR1_BASE ) ) )	//	序列13
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 14 - ADC1_SQR1_BASE ) ) )	//	序列14
				|	( ( 17 ) << ( ADC1_SQR_WIDTH * ( 15 - ADC1_SQR1_BASE ) ) )	//	序列15
				|	( ( 0  ) << ( ADC1_SQR_WIDTH * ( 16 - ADC1_SQR1_BASE ) ) )	//	序列16
				|(( 16 - 1 ) << ( ADC1_SQR_WIDTH * ( 17 - ADC1_SQR1_BASE ) ) )	//	长度(0表示1次)
                ;

	delay_us ( 1u );
	SET_BIT ( ADC1->CR2, ADC_CR2_ADON );						//	Enable ADC1
	SET_BIT ( ADC1->CR2, ADC_CR2_RSTCAL );						//	Reset calibration register
	while ( READ_BIT ( ADC1->CR2, ADC_CR2_RSTCAL ) ) {}			//	Check the end of ADC1 reset calibration register
	SET_BIT ( ADC1->CR2, ADC_CR2_CAL );							//	Start ADC1 calibration
	while ( READ_BIT ( ADC1->CR2, ADC_CR2_CAL ) ) {}			//	Check the end of ADC1 calibration
	SET_BIT ( ADC1->CR2, ADC_CR2_SWSTART | ADC_CR2_EXTTRIG );  	//	Start ADC1 Software Conversion

	//	 配置相应的AD输入管脚为模拟输入状态
}

uint16_t	STM32_ADC_fetch ( uint8_t Channel )
{
	uint8_t const len = sizeof ( ADCResultBuf ) / sizeof ( ADCResultBuf[0] );
	uint8_t		i;
	uint32_t	sum = 0u;

	for ( i = 0u; i < len; ++ i )
	{
		sum += ADCResultBuf[i][Channel];
	}
	return	( uint16_t ) ( sum / len );
}

/********************************** 功能说明 ***********************************
*	使用 TIM2 实现 可控硅斩波控制
*******************************************************************************/
static	void	TIM2_Init( void )
{
	TIM_TypeDef	* TIMx = TIM2;

	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_TIM2EN );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );

	/* TIM2_CH2 pin (PA.0, PA.1) configuration */
	MODIFY_REG( GPIOA->CRL, 0x000000FFu, 0x000000B4u );

	// 配置时基
	TIMx->CR1  = TIM_CR1_DIR;								//	向下计数
	TIMx->PSC  = (( SystemCoreClock / 1000000u ) - 1u );	//	Set the Prescaler value
	TIMx->ARR  = ( 10000u - 1u );		  					//	Set the Autoreload value: 10ms
	TIMx->EGR  = TIM_EGR_UG;           						//	生成更新事件，立即更新 PSC,

	//	T2CH1/ETR 捕捉交流电过零信号，ETRF复位定时器，清除OCxREF
	//	ETP: 0, ETR高电平或上升沿有效
	//	ETF = 0100b, 设置过滤器
	//	TS  = 111b, 外部触发输入(ETRF)
	//	SMS = 100b, 复位模式，TRGI上升沿初始化计数器
	CLEAR_REG( TIMx->SMCR );
	WRITE_REG( TIMx->SMCR, TIM_SMCR_ETF_2 | TIM_SMCR_TS );
	SET_BIT( TIMx->SMCR, TIM_SMCR_SMS_2 );

	//	T2CH2 接可控硅触发驱动: TIM_OCMode_PWM1, TIM_OCPreload_Enable, TIM_OCClear_Enable
	CLEAR_BIT( TIMx->CCER, TIM_CCER_CC2E );
	MODIFY_REG( TIMx->CCMR1, TIM_CCMR1_CC2S | TIM_CCMR1_OC2M, TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 );
	SET_BIT( TIMx->CCMR1, TIM_CCMR1_OC2CE | TIM_CCMR1_OC2PE );	
	TIMx->CCR2 = 0u;
//	SET_BIT( TIMx->CCER, TIM_CCER_CC2E );

	SET_BIT( TIMx->CR1, TIM_CR1_CEN );
}

void	TRIAC_SetOutput( uint16_t OutValue )
{	//	0~27648 转换成 10ms ~ 0ms 延时时间
	TIM_TypeDef * TIMx = TIM2;
	uint16_t	TriggerDelay;
	
	if ( OutValue >= 27648u )
	{
		OutValue = 27648u;
	}
	TriggerDelay = (uint16_t)(( OutValue * 10000uL ) / 27648u );

	//	TIMx->CCR2 = 10000u - TriggerDelay;		//	向上计数
	TIMx->CCR2 = TriggerDelay;				//	向下计数
	
	SET_BIT( TIMx->CCER, TIM_CCER_CC2E );
}

void	TRIAC_OutCmd( BOOL NewState )
{
	if ( NewState )
	{
	//	TRIAC_Init();
	}
	else
	{
		TIM_TypeDef * TIMx = TIM2;

		CLEAR_BIT( TIMx->CCER, TIM_CCER_CC2E );
	}
}


/********************************** 功能说明 ***********************************
*	使用 TIM3 实现 PWM功能，输出范围固定：0 ~ PWM_Output_Max(通常是27648u)
*******************************************************************************/
#define	_pwm_period 1000uL

void	PWM1_SetOutput( uint16_t OutValue ){ TIM3->CCR1 = OutValue * _pwm_period / PWM_Output_Max; }
void	PWM2_SetOutput( uint16_t OutValue ){ TIM3->CCR2 = OutValue * _pwm_period / PWM_Output_Max; }
void	PWM3_SetOutput( uint16_t OutValue ){ TIM3->CCR3 = OutValue * _pwm_period / PWM_Output_Max; }
void	PWM4_SetOutput( uint16_t OutValue ){ TIM3->CCR4 = OutValue * _pwm_period / PWM_Output_Max; }

static	void	TIM3_Init( void )
{
	TIM_TypeDef	* TIMx = TIM3;

	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_TIM3EN );

	//	TIM3时基初始化: 输入时钟频率1MHz，PWM周期1000个单位, PWM频率1KHz
	TIMx->CR1  = 0u;
	TIMx->PSC  = (( SystemCoreClock / 1000000u ) - 1u );
	TIMx->ARR  = ( _pwm_period - 1u );
	TIMx->EGR  = TIM_EGR_UG;

	//	T3CH2 接LCD的负压发生电路，TIM_OCMode_PWM1, TIM_OCPolarity_Low, TIM_OutputState_Enable
	CLEAR_BIT( TIMx->CCER, TIM_CCER_CC2E );
	MODIFY_REG( TIMx->CCMR1, TIM_CCMR1_CC2S | TIM_CCMR1_OC2M, TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 );
	SET_BIT( TIMx->CCER, TIM_CCER_CC2P );
	TIMx->CCR2 = 0u;
	SET_BIT( TIMx->CCER, TIM_CCER_CC2E );

	//	T3CH4 接LCD的背光电流控制，TIM_OCMode_PWM1, TIM_OCPolarity_High, TIM_OutputState_Enable
	CLEAR_BIT( TIMx->CCER, TIM_CCER_CC4E );
	MODIFY_REG( TIMx->CCMR2, TIM_CCMR2_CC4S | TIM_CCMR2_OC4M, TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 );
	CLEAR_BIT( TIMx->CCER, TIM_CCER_CC4P );
	TIMx->CCR4 = 0u;
	SET_BIT( TIMx->CCER, TIM_CCER_CC4E );

	//	TIMx 使能
	SET_BIT( TIMx->CR1, TIM_CR1_CEN );

	//	配置输出管脚
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_TIM3_REMAP, AFIO_MAPR_TIM3_REMAP_FULLREMAP );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
	MODIFY_REG( GPIOC->CRL, 0xF0000000u, 0xB0000000u );		//	PC.7, T3CH2
 	MODIFY_REG( GPIOC->CRH, 0x000000F0u, 0x000000B0u );		//	PC.9, T3CH4
}


/********************************** 功能说明 ***********************************
*	使用 EXTI3 计数 恒温箱制冷片风扇的转速脉冲，实现 风扇转速测量
*******************************************************************************/
static	uint32_t	HCBoxFan_Circles;
// 	static	uint32_t	volatile	fc_idx, fc_filter[4];


// 	__irq	void	TIM1_UP_IRQHandler( void )
// 	{
// 		uint32_t	idx = fc_idx;

// 		idx = ( idx + 1 ) % 4u;
// 		fc_filter[idx] = HCBoxFan_Circles;
// 		fc_idx = idx;
// 		
// 	}

// 	//	uint16_t	HCBoxFan_Circles;

	uint16_t	get_HCBoxFan_Circle( void ){ return	HCBoxFan_Circles; }
// 	{
// 	//	return	HCBoxFan_Circles * 15u;	//	HCBoxFan_Circles;									// PA.3 Circle.
// 		
// 		return	( fc_filter[fc_idx] );
// 	}

// 	uint16_t	get_HCBoxFanSpeed_RPM( void )
// 	{
// 		uint32_t	idx = fc_idx;
// 		
// 		return	( fc_filter[idx] - fc_filter[ ( idx + 1 ) % 4 ] ) * 15u;
// 	}

// 	__irq	void	EXTI15_10_IRQHandler( void )
// 	{
// 		if ( READ_BIT( EXTI->PR, EXTI_PR_PR12 ))		// interrupt pending
// 		{
// 			WRITE_REG( EXTI->PR, EXTI_PR_PR12 );		// 写1复位中断标志位

// 			//	中断处理
// 			++HCBoxFan_Circles;
// 		}
// 	}

// 	void	EXIT12_Init( void )
// 	{
// 		SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
// 		SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
// 		MODIFY_REG( AFIO->EXTICR[3], AFIO_EXTICR4_EXTI12, AFIO_EXTICR4_EXTI12_PA );	/* set PA to use */
// 		MODIFY_REG( GPIOA->CRH, 0x000F0000u, 0x00080000u );			/* PA.12 上拉/下拉输入模式  */
// 		GPIOA->BSRR = ( 1 << 12 );

// 		NVIC_EnableIRQ( EXTI15_10_IRQn );								/* enable IRQ EXTI 3 */

// 		SET_BIT(   EXTI->IMR,  EXTI_IMR_MR12 );						/* unmask interrupt			*/
// 		CLEAR_BIT( EXTI->EMR,  EXTI_EMR_MR12 );						/* no event					*/
// 		CLEAR_BIT( EXTI->RTSR, EXTI_RTSR_TR12 );						/* no rising edge trigger   */
// 		SET_BIT(   EXTI->FTSR, EXTI_FTSR_TR12 );						/* set falling edge trigger */
// 	}

/********************************** 功能说明 ***********************************
*	访问 GPIO
*******************************************************************************/
#define	Pin_Buzzer			PinBB( GPIOB->ODR, 2U )
#define	Pin_P_air_1_DR		PinBB( GPIOC->ODR, 8U )

#define	Pin_HCBoxDR_Cool	PinBB( GPIOA->ODR, 11U )
#define	Pin_HCBoxDR_Heat	PinBB( GPIOA->ODR, 12U )
#define	Pin_HCBoxDR_Fan		PinBB( GPIOA->ODR, 2U )
#define	HCBox_DR_ON			1
#define	HCBox_DR_OFF		0

void	beep( void )
{
	Pin_Buzzer = 0;//1;
	delay( 200u );
	Pin_Buzzer = 0;
}

void	tick( void )
{
	Pin_Buzzer = 0;//1;
	delay( 20u );
	Pin_Buzzer = 0;
}

void	HCBoxFan_ON( void )   {  Pin_HCBoxDR_Fan  = HCBox_DR_ON;  }
void	HCBoxFan_OFF( void )  {  Pin_HCBoxDR_Fan  = HCBox_DR_OFF; }
void	HCBoxHeat_ON( void )  {  Pin_HCBoxDR_Heat = HCBox_DR_ON;  }
void	HCBoxHeat_OFF( void ) {  Pin_HCBoxDR_Heat = HCBox_DR_OFF; }
void	HCBoxCool_ON( void )  {  Pin_HCBoxDR_Cool = HCBox_DR_ON;  }
void	HCBoxCool_OFF( void ) {  Pin_HCBoxDR_Cool = HCBox_DR_OFF; }
void	PP_AIR_ON( void )		{  Pin_P_air_1_DR = 1; }
void	PP_AIR_OFF( void )		{  Pin_P_air_1_DR = 0; }


BOOL	bus_SPI1xDRDY( void ){	return	( Pin_SPI1xDRDY ) ? TRUE : FALSE; }
BOOL	UART1_RX_Pin_State( void )
{
	if ( READ_BIT( GPIOA->IDR, GPIO_IDR_IDR10 ))
	{
		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}

#define	Pin_485_Direct		PinBB( GPIOC->ODR, 12u )
void	MB_485_Direct_Transmit( void )	{  Pin_485_Direct = 1;  }
void	MB_485_Direct_Receive( void )	{  Pin_485_Direct = 0;  }

void	GPIO_Init ( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );

	// PC.6 for Buzzer.
	Pin_Buzzer = 0;
 	MODIFY_REG( GPIOB->CRL, 0x0F00u, 0x0300u );

	// PA.2, PA.11, PA.12 for HCBox.
	HCBoxFan_OFF();
	HCBoxHeat_OFF();
	HCBoxCool_OFF();
	MODIFY_REG( GPIOA->CRL, 0x00000F00u, 0x00000300u );
 	MODIFY_REG( GPIOA->CRH, 0x000FF000u, 0x00033000u );

	// PC.8 for Pump1
	MODIFY_REG( GPIOC->CRH, 0x0000000Fu, 0x00000003u );

	// mdf_REG( GPIOA->CRH, 0xF0000000u, 0x30000000u );	//	for CS, output
	// 	SPI1xNSS = 1;
	MODIFY_REG( GPIOA->CRH, 0xF0000000u, 0x40000000u );	//	for DRDY, input
}

/********************************** 功能说明 ***********************************
*	读取 JTAG 上的两个跳线，如果插着跳线，相应位返回1，否则返回0.
*******************************************************************************/
static	uint8_t	get_Jumper( void )
{
 	uint32_t	JumperState;
	uint32_t	newState;
	uint_fast8_t	i;
	
	//	PA.13 - JTMS/SWDIO, 默认内部上拉，但插跳线时与+3V相连
	//	PA.14 - JTCK/SWCLK, 默认内部下拉，但插跳线时与GND相连

	//	暂时关闭调试功能
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	GPIOA->BSRR = GPIO_BSRR_BR13;	//	改为内部下拉
	GPIOA->BSRR = GPIO_BSRR_BS14;	//	改为内部上拉
	MODIFY_REG( GPIOA->CRH, 0x0FF00000u, 0x08800000u );
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
 	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_DISABLE );		/*!< JTAG-DP Disabled and SW-DP Disabled */
	
	//	读取跳线状态，持续10ms状态不变，才确认读取结果有效。
	newState = ( GPIOA->IDR & ( GPIO_IDR_IDR13 | GPIO_IDR_IDR14 ));

	for ( i = 0u; i < 100u; ++i )
	{
		JumperState = newState;
		delay_us( 100u );
		newState = ( GPIOA->IDR & ( GPIO_IDR_IDR13 | GPIO_IDR_IDR14 ));
		
		if ( JumperState != newState )
		{
			i = 0u;
		}
	}
	
	//	重新打开调试功能
	GPIOA->BSRR = GPIO_BSRR_BS13;	//	改回默认上拉
	GPIOA->BSRR = GPIO_BSRR_BR14;	//	改回默认下拉
	MODIFY_REG( AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );	/*!< JTAG-DP Disabled and SW-DP Enabled */
	
	return	( JumperState ^ 0x02u );	//	不插跳线结果为0，插跳线结果为1。
}

/********************************** 功能说明 ***********************************
*	BIOS 初始化
*******************************************************************************/
void	NVIC_Init( void )
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
//	NVIC_SetPriorityGrouping( 7 );
//	NVIC_EnableIRQ( EXTI3_IRQn );

	// for Keyboard.
	NVIC_EnableIRQ( EXTI0_IRQn );
	NVIC_EnableIRQ( EXTI1_IRQn );
	NVIC_EnableIRQ( EXTI2_IRQn );
	NVIC_EnableIRQ( EXTI3_IRQn );
	NVIC_EnableIRQ( EXTI4_IRQn );
	NVIC_EnableIRQ( EXTI9_5_IRQn );
	
//	__set_CONTROL( 0x03u );	//	使用 PSP, 并转入非特权模式
}

uint32_t	BIOS_Init( void )
{
	uint8_t	Jumper = get_Jumper();
	
	NVIC_Init();
	GPIO_Init();
//	TIM1_Init();	//	for HCBox Output
	TIM2_Init();	//	中（大）流量电机调速
	TIM3_Init();	//	for PWM.
	
//	EXIT3_Init();	//	for Fan Speed.
//	STM32_RTC_Init();	推迟到显示初始化完成再进行
	STM32_ADC_Init();
	
	return	Jumper;
}




/********************************** 功能说明 ***********************************
*	不使用操作系统时，需要自定义SVC的套接程序
*********************************** 设计说明 ***********************************
用户触发SVC中断，如果不能立即响应，会上访成硬件故障，所以，触发时要格外小心。
触发SVC后，可能会发生别的中断抢占，所以入口参数不能直接用，应使用堆栈中的值。
在用户级线程模式下，入口参数使用PSP栈，所以取参数时需要判断使用的是哪个堆栈。
*******************************************************************************/
// 	__asm void SVC_Handler (void)
// 	{
// 		// 判断入口参数保存在哪个栈中，并取回入口参数
// 		TST    	LR, #0x4 				;// 测试EXC_RETURN的第2位
// 		ITE   	EQ                    	;
// 		MRSEQ	R0, MSP  				;// 0: 使用主堆栈，故把MSP的值取出
// 		MRSNE	R0, PSP 				;// 1: 使用进程栈，故把PSP的值取出

// 	// 	LDR		R1, [R0,#24] 			;// 从栈中读取PC的值
// 	// 	LDRB	R0, [R1,#-2]			;// 从SVC指令中读取立即数放到R0
// 		LDM     R0, { R0-R3, R12 }      ;// 从栈中取得入口参数
// 		
// 		//	执行用户程序
// 		PUSH	{ LR }
// 		BLX     R12                     ; Call SVC Function 
// 		POP		{ LR }

// 		//	判断入口参数保存在哪个栈中，并回写返回值
// 		TST     LR, #2					;// 测试EXC_RETURN的第2位
// 		ITE   	EQ                    	;
// 		MRSEQ   R12, MSP                ;// 0: 使用主堆栈(MSP)
// 		MRSNE   R12, PSP                ;// 1: 使用进程栈(PSP)
// 		STM     R12,{ R0-R3 }			;// 回写函数返回值

// 		//	中断返回
// 		BX      LR
// 	}
/********************************** 功能说明 ***********************************
*	使用 SVC 写 NVIC，实现中断的开关控制。
*******************************************************************************/
__svc_indirect(0)	void	_Sys_SVC_CALL( void ( *fun)( IRQn_Type IRQn ), uint8_t IRQn );
	
void	sys_IRQ_Cmd( uint8_t IRQn, BOOL NewState )
{
	if ( NewState )
	{
		_Sys_SVC_CALL( NVIC_EnableIRQ, IRQn );
	}
	else
	{
		_Sys_SVC_CALL( NVIC_DisableIRQ, IRQn );
	}
}

/********  (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司  **** End Of File ****/
