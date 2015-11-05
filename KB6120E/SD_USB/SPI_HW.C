/* CH376芯片 硬件标准SPI串行连接的硬件抽象层 V1.0 */
/* 提供I/O接口子程序 */

#include	"hal.h"
extern void	delay_us ( uint32_t us );
void	mDelay0_5uS( void )  /* 至少延时0.5uS,根据单片机主频调整 */
{
	__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
}

void	CH376_PORT_INIT( void )  /* 由于使用SPI读写时序,所以进行初始化 */
{
	NVI_Init();
	INT_IRQ_Enable();
	SPI3_GPIO_Config();
/* 如果是硬件SPI接口,那么可使用mode3(CPOL=1&CPHA=1)或mode0(CPOL=0&CPHA=0),CH376在时钟上升沿采样输入,下降沿输出,数据位是高位在前 */
	CH376_END_CMD();/* 禁止SPI片选 */
}

UINT8	Spi376Exchange( UINT8 d )  /* 硬件SPI输出且输入8个位数据 */
{  
	return SPI3_RW( d );
}

void	xEndCH376Cmd( )	
{ 
	CH376_END_CMD(); 
}  /* SPI片选无效,结束CH376命令,仅用于SPI接口方式 */

void	xWriteCH376Cmd( UINT8 mCmd )  /* 向CH376写命令 */
{
	WriteCH376Cmd( mCmd );	
	mDelay0_5uS( );
	mDelay0_5uS( );
	mDelay0_5uS( );
// #ifdef	CH376_SPI_BZ
// 	UINT8	i;
// #endif
// 	CH376_SPI_SCS = 1;  /* 防止之前未通过xEndCH376Cmd禁止SPI片选 */
// /* 对于双向I/O引脚模拟SPI接口,那么必须确保已经设置SPI_SCS,SPI_SCK,SPI_SDI为输出方向,SPI_SDO为输入方向 */
// 	CH376_SPI_SCS = 0;  /* SPI片选有效 */
// 	Spi376Exchange( mCmd );  /* 发出命令码 */
// #ifdef	CH376_SPI_BZ
// 	for ( i = 30; i != 0 && CH376_SPI_BZ; -- i );  /* SPI忙状态查询,等待CH376不忙,或者下面一行的延时1.5uS代替 */
// #else
//   /* 延时1.5uS确保读写周期大于1.5uS,或者用上面一行的状态查询代替 */
// #endif
}

#ifdef	FOR_LOW_SPEED_MCU  /* 不需要延时 */
#define	xWriteCH376Data( d )	{ WriteCH376Data( d ); }  /* 向CH376写数据 */
#define	xReadCH376Data( )		(  ReadCH376Data( void ); )  /* 从CH376读数据 */
#else
void	xWriteCH376Data( UINT8 mData )  /* 向CH376写数据 */
{
	WriteCH376Data( mData );
	mDelay0_5uS( );  /* 确保读写周期大于0.6uS */
}
UINT8	xReadCH376Data( void )  /* 从CH376读数据 */
{
	mDelay0_5uS( );  /* 确保读写周期大于0.6uS */
	return( ReadCH376Data( ) );
}
#endif
/* 查询CH376中断(INT#低电平) */
UINT8	Query376Interrupt( void )
{
	return ( CH376QueryInterrupt() );
}

UINT8	mInitCH376Host( UINT8 USBSelect )  /* 初始化CH376 */
{
	UINT8	res;
	CH376_PORT_INIT( );  /* 接口硬件初始化 */
	xWriteCH376Cmd( CMD11_CHECK_EXIST );  /* 测试单片机与CH376之间的通讯接口 */
	xWriteCH376Data( 0xaa );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
	if ( res != 0x55 ) return( ERR_USB_UNKNOWN );  /* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */
	xWriteCH376Cmd( CMD11_SET_USB_MODE );  /* 设备USB工作模式 */
	xWriteCH376Data( USBSelect );  ///06  U盘 03 SD卡
	mDelayuS( 20 );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
#ifndef	CH376_INT_WIRE
#ifdef	CH376_SPI_SDO
	xWriteCH376Cmd( CMD20_SET_SDO_INT );  /* 设置SPI的SDO引脚的中断方式 */
	xWriteCH376Data( 0x16 );
	xWriteCH376Data( 0x90 );  /* SDO引脚在SCS片选无效时兼做中断请求输出 */
	xEndCH376Cmd( );
#endif
#endif
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else return( ERR_USB_UNKNOWN );  /* 设置模式错误 */
}
