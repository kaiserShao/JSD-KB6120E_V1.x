/**************** (C) COPYRIGHT 2008 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: RS232.C
* �� �� ��: 
* ��  ��  : 
*         : 
* ����޸�: 
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
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
	return	UART1_RX_Pin_State() ? FALSE : TRUE;	//	�͵�ƽ ��Ч
}

/**
	�������
*/
static	BOOL	PrinterSend_Command ( CHAR const * BufferPtr, uint8_t BufferLen )
{
	while ( BufferLen-- )
	{
		//	�û���ֹ��ӡ��
		if ( NULL != UserBreakPrint_CB )
		{
			if ((*UserBreakPrint_CB)())
			{
				UserBreakPrint = TRUE;
				return	FALSE;
			}
		}

		//	����������ӡ��ǰ��Ҫ��� Ready �ź�
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
	������󣬸����ֿ�ķ����������ĵ���ķ���
*/
static	BOOL	PrinterSend_DotImage( uint8_t const * BufferPtr, uint8_t BufferLen )
{
	while ( BufferLen-- )
	{
		//	�û���ֹ��ӡ��
		if ( NULL != UserBreakPrint_CB )
		{
			if (UserBreakPrint_CB())
			{
				UserBreakPrint = TRUE;
				return	FALSE;
			}
		}

		//	����������ӡ��ǰ��Ҫ��� Ready �ź�
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
			// DBC ���Ӣ��
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
			// SBC ȫ�Ǻ���
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
* ��������: 
* ����˵��: ��ӡͨѶ�ӿ�
* �������: None
* �������: None
* �� �� ֵ: ���� �Ƿ����
*******************************************************************************/
void	PrinterPutString( const CHAR * sz )
{
	CHAR	Command[8];
	size_t	slen;

	if ( UserBreakPrint )
	{	//	�û���ֹ��ӡ���ٴγ�ʼ��ǰ������ִ�д�ӡ
		return;
	}

	if ( NULL == sz )
	{	//	�����ָ�룬��ӡ����
		slen = 0;
	}
	else
	{
		slen = strlen( sz );
		if ( slen > 30U )
		{	//	����΢��Ϊ240�У����Դ�ӡ30������ַ�
			slen = 30U;
		}
	}

	// �����м��
	Command[0] = 0x1Bu;
	Command[1] = 0x31u;
	Command[2] = 0;
	// ͼ�δ�ӡ����
	Command[3] = 0x1Bu;
	Command[4] = 0x4Bu;
	Command[5] = ( slen * 8u ) % 256u;
	Command[6] = ( slen * 8u ) / 256u;

	// �ϰ���
	if ( 0u != slen )
	{	
		if ( ! PrinterSend_Command ( Command, 7u )){  return; }
		if ( ! PrinterPutHalf( sz, slen, 0u )){  return; }
	}
	PrinterSend_Command ( "\r\n", 2u );
	
	// �°���
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

/********  (C) COPYRIGHT 2008 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/