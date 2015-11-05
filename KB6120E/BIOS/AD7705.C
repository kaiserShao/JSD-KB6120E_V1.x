/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: AD7705.C
* 创 建 者: 董峰
* 描  述  : 读写AD7705程序
* 最后修改: 2014年5月5日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
/*lint	--e{750}	Info 750: local macro 'XX' not referenced	*/

#include "BSP.H"
#include "BIOS.H"

// Communications Register (RS:000)
#define DRDY	0x80		// status of the DRDY
#define RS_0	(0<<4)		// Communications Register, 8 Bits
#define RS_1	(1<<4)		// Setup Register, 8 Bits
#define RS_2	(2<<4)		// Clock Register, 8 Bits
#define RS_3	(3<<4)		// Data Register, 16 Bits
#define RS_4	(4<<4)		// Test Register, 8 Bits
#define RS_5	(5<<4)		// No Operation
#define RS_6	(6<<4)		// Offset Register, 24 Bits
#define RS_7	(7<<4)		// Gain Register, 24 Bits
#define READ	0x08		// Read/Write Select
#define STBY 	0x04		// Standby
#define CH_0	(0<<0)		// 7705: AIN1(+) AIN1(-), 7706: AIN1 COMMON
#define CH_1	(1<<0)		// 7705: AIN2(+) AIN2(-), 7706: AIN2 COMMON
#define CH_2	(2<<0)		// 7705: AIN1(-) AIN1(-), 7706: COMMON COMMON
#define CH_3	(3<<0)		// 7705: AIN1(-) AIN2(-), 7706: AIN3 COMMON
// Setup Register (RS:001)
#define FSYNC	0x01		// Filter Synchronization
#define BUF		0x02		// Buffer Control
#define	B_U		0x04		// 0: Bipolar Operation, 1: Unipolar Operation
#define BIPOLAR   0x00
#define UNIPOLAR  0x04
#define MD_0	(0<<6)		// Normal Mode
#define MD_1	(1<<6)		// Self-Calibration
#define MD_2	(2<<6)		// Zero-Scale System Calibration
#define MD_3	(3<<6)		// Full-Scale System Calibration
#define G_0		(0<<3)		// Gain = 1
#define G_1		(1<<3)		// Gain = 2
#define G_2		(2<<3)		// Gain = 4
#define G_3		(3<<3)		// Gain = 8
#define G_4		(4<<3)		// Gain = 16
#define G_5		(5<<3)		// Gain = 32
#define G_6		(6<<3)		// Gain = 64
#define G_7		(7<<3)		// Gain = 128
// Clock Register (RS:010)
#define CLKDIS	0x10		// Master Clock Disable Bit
#define CLKDIV	0x08		// Clock Divider Bit
#define CLK		0x04		// Clock Bit.
#define FS_0_0	0x00		// Code = 391, 19.98@1MHz
#define FS_0_1	0x01		// Code = 312, 25.04@1MHz
#define FS_0_2	0x02		// Code =  78, 100.2@1MHz
#define FS_0_3	0x03		// Code =  39, 200.3@1MHz
#define FS_1_0	0x04		// Code = 384, 50.0 @2.4576MHz
#define FS_1_1	0x05		// Code = 320, 60.0 @2.4576MHz
#define FS_1_2	0x06		// Code =  77, 249.4@2.4576MHz
#define FS_1_3	0x07		// Code =  38, 505.3@2.4576MHz

///////////////////////////////////////////////////////////////////
//	#define	ShiftIn()		bus_SPI1xShift( 0xFFu )
//	#define	ShiftOut(_b)	(void)bus_SPI1xShift(_b)

static	uint8_t	ShiftIn( void )
{
	return bus_SPI1xShift( 0xFFu );
}

static	void    ShiftOut( uint8_t b )
{
	bus_SPI1xShift( b );
}




BOOL	Initialize7705( )
{
	uint8_t	i;
	 
	bus_SPI1xPortInit();
	
	for( i = 7u; i != 0u; --i )
	{
		ShiftOut( 0xFFu );
	}
	
	ShiftOut( RS_4 );				// Test Register, 8 Bits
	ShiftOut( 0x00u );				// Default: 0x00
	ShiftOut( RS_2 );				// Clock Register, 8 Bits
	ShiftOut( CLKDIV + FS_1_0 );	// 50Hz @ 4.9152MHz
	
	ShiftOut( RS_1 + 0u );			// 启动指定通道的自校准转换
	ShiftOut( MD_1 + BIPOLAR  + G_5 + BUF );
	delay( 300u );
	
	ShiftOut( RS_1 + 1u );			// 启动指定通道的自校准转换
	ShiftOut( MD_1 + BIPOLAR  + G_4 + BUF );
	delay( 300u);

	return	TRUE;
}

void	Convert7705( uint8_t ChannelSelect )
{
	ShiftOut( RS_1 + ChannelSelect );		// 启动指定通道的校准转换

	if( 0u == ChannelSelect )
	{
		ShiftOut( MD_0 + BIPOLAR  + G_5 + BUF );
	}
	else
	{
		ShiftOut( MD_0 + BIPOLAR  + G_4 + BUF );
	}
	ShiftOut( RS_3 + READ + ChannelSelect );	// Reset DRDY#
	ShiftIn();
	ShiftIn();
}


uint16_t	Readout7705( uint8_t ChannelSelect )
{
	uint8_t  state, ResultH, ResultL;
	BOOL	 isReady;
	uint16_t iRetry;
	
	for ( iRetry = 20u; iRetry; --iRetry )
	{
		ShiftOut( RS_0 + READ + ChannelSelect );
		state = ShiftIn();
		isReady = ( state & DRDY ) ? FALSE : TRUE;	//	DRDY 低电平有效
		if ( isReady )
		{
			ShiftOut( RS_3 + READ + ChannelSelect );		// do Read
			ResultH = ShiftIn();
			ResultL = ShiftIn();	
			return  ( ResultL + ( ResultH * 256u ));
		}
		delay( 10u );
	}
	return 0u;	// failure !!!
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
