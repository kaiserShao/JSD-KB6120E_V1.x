/**************** (C) COPYRIGHT 2008 青岛金仕达电子科技有限公司 ****************
* 文 件 名: RS232.C
* 创 建 者: 
* 描  述  : 
*         : 
* 最后修改: 
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/

#include "BSP.H"
#include "BIOS.H"

//	#define ESC     0x1B
//	#define XON     0x11
//	#define XOFF    0x13

static	BOOL	(*UserBreakPrint_CB)( void );
static	BOOL	UserBreakPrint;

uint32_t	invert_dword( uint32_t U32DW )
{
	U32DW = (( U32DW & 0x0000FFFFu ) << 16 ) | (( U32DW & 0xFFFF0000u ) >> 16 );
	U32DW = (( U32DW & 0x00FF00FFu ) <<  8 ) | (( U32DW & 0xFF00FF00u ) >>  8 );
	U32DW = (( U32DW & 0x0F0F0F0Fu ) <<  4 ) | (( U32DW & 0xF0F0F0F0u ) >>  4 );
	U32DW = (( U32DW & 0x33333333u ) <<  2 ) | (( U32DW & 0xCCCCCCCCu ) >>  2 );
	U32DW = (( U32DW & 0x55555555u ) <<  1 ) | (( U32DW & 0xAAAAAAAAu ) >>  1 );

	return	U32DW;
}

uint16_t	invert_word( uint16_t U16W )
{
	return	invert_dword((uint32_t)U16W << 16 );
}

uint8_t	invert_byte( uint8_t U8B )
{
	U8B = (( U8B & 0x0Fu ) << 4 ) | (( U8B & 0xF0u ) >> 4 );
	U8B = (( U8B & 0x33u ) << 2 ) | (( U8B & 0xCCu ) >> 2 );
	U8B = (( U8B & 0x55u ) << 1 ) | (( U8B & 0xAAu ) >> 1 );

	return	U8B;
}

static	BOOL	isPrinterReady( void )
{
	return	UART1_RX_Pin_State() ? FALSE : TRUE;	//	低电平 有效
}

/**
	数据输出
*/
static	BOOL	PrinterSend_Command ( CHAR const * BufferPtr, uint8_t BufferLen )
{
	while ( BufferLen-- )
	{
		//	用户中止打印？
		if ( NULL != UserBreakPrint_CB )
		{
			if ((*UserBreakPrint_CB)())
			{
				UserBreakPrint = TRUE;
				return	FALSE;
			}
		}

		//	数据送往打印机前需要检查 Ready 信号
		if ( isPrinterReady())
		{
			UART1_Send( *BufferPtr++ );
		}
		else
		{
			delay( 10u );
		}
	}

	return	TRUE;
}

/**
	输出点阵，根据字库的方向调整输出的点阵的方向
*/
static	BOOL	PrinterSend_DotImage( uint8_t const * BufferPtr, uint8_t BufferLen )
{
	while ( BufferLen-- )
	{
		//	用户中止打印？
		if ( NULL != UserBreakPrint_CB )
		{
			if (UserBreakPrint_CB())
			{
				UserBreakPrint = TRUE;
				return	FALSE;
			}
		}

		//	数据送往打印机前需要检查 Ready 信号
		if ( isPrinterReady())
		{
			UART1_Send( invert_byte( *BufferPtr++ ));
		}
		else
		{
			delay( 10U );
		}
	}

	return	TRUE;
}

static	BOOL	PrinterPutHalf( CHAR const * sz, size_t slen, uint8_t WhichHalf )
{
	CHAR	sDat;
	CGROM	pDot;
	size_t	i;

	for ( i = 0u; i < slen; )
	{
		sDat = *sz++;
		if ( 0U == ((uint8_t)sDat & 0x80U) )
		{
			// DBC 半角英文
			pDot = DotSeekDBC( sDat );
			if ( WhichHalf )
			{
				pDot += 8U;
			}
			if ( ! PrinterSend_DotImage( pDot, 8U ))
			{
				return	FALSE;
			}
			i += 1U;
		}
		else
		{
			// SBC 全角汉字
			pDot = DotSeekSBC( sDat, *sz++ );
			if ( WhichHalf )
			{
				pDot += 16U;
			}
			if ( ! PrinterSend_DotImage( pDot, 16U ))
			{
				return  FALSE;
			}
			i += 2U;
		}
	}
	return	TRUE;
}


/*******************************************************************************
* 函数名称: 
* 功能说明: 打印通讯接口
* 输入参数: None
* 输出参数: None
* 返 回 值: 返回 是否出错
*******************************************************************************/
void	PrinterPutString( const CHAR * sz )
{
	CHAR	Command[8];
	size_t	slen;

	if ( UserBreakPrint )
	{	//	用户中止打印，再次初始化前，不再执行打印
		return;
	}

	if ( NULL == sz )
	{	//	输入空指针，打印空行
		slen = 0;
	}
	else
	{
		slen = strlen( sz );
		if ( slen > 30U )
		{	//	所用微打为240列，可以打印30个半角字符
			slen = 30U;
		}
	}

	// 设置行间距
	Command[0] = 0x1Bu;
	Command[1] = 0x31u;
	Command[2] = 0;
	// 图形打印长度
	Command[3] = 0x1Bu;
	Command[4] = 0x4Bu;
	Command[5] = ( slen * 8u ) % 256u;
	Command[6] = ( slen * 8u ) / 256u;

	// 上半行
	if ( 0u != slen )
	{	
		if ( ! PrinterSend_Command ( Command, 7u )){  return; }
		if ( ! PrinterPutHalf( sz, slen, 0u )){  return; }
	}
	PrinterSend_Command ( "\r\n", 2u );
	
	// 下半行
	if ( 0u != slen )
	{
		if ( ! PrinterSend_Command ( Command, 7u )){  return; }
		if ( ! PrinterPutHalf( sz, slen, 1u )){  return; }		
	}
	PrinterSend_Command ( "\r\n", 2u );
}

BOOL	PrinterInit( BOOL (*CB_UserTerminate)( void ))
{
	UserBreakPrint = FALSE;
	UserBreakPrint_CB = CB_UserTerminate;
	
	UART1_Init();
	return	isPrinterReady();
}

void	PrinterTestString( const CHAR * sz )
{
	while( *sz ) UART1_Send( *sz++ );	
}

/********  (C) COPYRIGHT 2008 青岛金仕达电子科技有限公司  **** End Of File ****/
