/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: MBM_Port.C
* 创 建 者: 董峰
* 描  述  : MODBUS 主节点移植
* 最后修改: 2014年5月7日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "Pin.H"
#include "MBM.H"
#include "MBM_Port.H"

void	RS485_Direct_Receive( void );
void	RS485_Direct_Transmit( void );

void	TIM4_Init( uint32_t u_sec );
void	TIM4_Cmd( BOOL NewState );

void	USART3_PortInit( void );
void	USART3_Init( uint32_t ulBaudRate );
void	USART3_PutByte( uint8_t cOutByte );
uint8_t	USART3_GetByte( void );
void	USART3_TX_Cmd( BOOL NewState );
void	USART3_RX_Cmd( BOOL NewState );


uint8_t ucSerialPDU[256];

/********************************** 功能说明 ***********************************
*  定时器中断
*******************************************************************************/
__irq	void	TIM4_IRQHandler( void )
{
	TIM_TypeDef * TIMx = TIM4;

	if ( READ_BIT( TIMx->SR, TIM_SR_UIF ))
	{
		/* Clear TIM4 Capture Compare1 interrupt pending bit*/
		CLEAR_BIT( TIMx->SR, TIM_SR_UIF );
		vMBM_RTU_T35_ISR();
	}
}

/********************************** 功能说明 ***********************************
*  串口中断
*******************************************************************************/
__irq	void	USART3_IRQHandler( void )
{
	USART_TypeDef * USARTx = USART3;
	
	if ( READ_BIT( USARTx->CR1, USART_CR1_RXNEIE ))
	{
		if ( READ_BIT( USARTx->SR, USART_SR_RXNE ))
		{
			//	接收数据处理
			vMBM_RTU_Receive_ISR( );
		}
	}

	if ( READ_BIT( USARTx->CR1, USART_CR1_TCIE ))
 	{
 		if ( READ_BIT( USARTx->SR, USART_SR_TC ))
 		{
 			//	发送数据处理
			vMBM_RTU_Send_ISR( );
 		}
 	}
}

/********************************** 功能说明 ***********************************
*  MBus工具－错误日志
*******************************************************************************/
volatile eMBErrorCode	lastError;

void	vMBusLogError( eMBErrorCode	eStatus )
{
	lastError = eStatus;
}

void	vMBusLogQuery( eMBErrorCode * peStatus )
{
	* peStatus = lastError;
}

/********************************** 功能说明 ***********************************
*  MBus工具－内存复制，计数以16位字为单位进行。
*******************************************************************************/
void	vMBus_Memory_Put( uint8_t * pucMBusPDU, const uint16_t * pRegistersVal, uint8_t usRegCount )
{		
	uint8_t i;
	for ( i = 0u; i < usRegCount; ++i )
	{
		*pucMBusPDU++ = HIBYTE( pRegistersVal[i] );
		*pucMBusPDU++ = LOBYTE( pRegistersVal[i] );
	}
}

void	vMBus_Memory_Get( uint16_t * pInputRegisters, const uint8_t * pucMBusPDU, uint8_t usRegCount )
{
	uint8_t i;

	for ( i = 0u; i < usRegCount; ++i )
	{
		pInputRegisters[i] = *pucMBusPDU++;
		pInputRegisters[i] <<= 8;
		pInputRegisters[i] |= *pucMBusPDU++;
	}
}

/********************************** 功能说明 ***********************************
*  移植 定时器
*******************************************************************************/
void	vMBM_Timers_Init( uint16_t u_sec )
{
	TIM4_Init( u_sec );
}

void	vMBM_Timers_Cmd( BOOL NewState )
{
	TIM4_Cmd( NewState );
}

/********************************** 功能说明 ***********************************
*  移植串口
*******************************************************************************/
void	vMBM_Serial_Init( uint32_t ulBaudRate, uint8_t ucDataBits, eMBParity eParity )
{
	USART3_Init( ulBaudRate );
	USART3_PortInit( );
}

void	vMBM_SerialPutByte( uint8_t ucByte )
{
	USART3_PutByte( ucByte );
}

BOOL	xMBM_SerialGetByte( uint8_t * pucByte )
{
	* pucByte = USART3_GetByte();
	return	TRUE;	//	忽略接收错误
}

void	vMBM_SerialGet_Cmd( BOOL xRxEnable )
{
    /* If xRxEnable enable serial receive interrupts.  */
	if ( xRxEnable )
	{
		RS485_Direct_Receive();
	}
	USART3_RX_Cmd( xRxEnable );
}

void	vMBM_SerialPut_Cmd( BOOL xTxEnable )
{
    /* If xTxENable enable transmitter empty interrupts. */
	if ( xTxEnable )
	{
		RS485_Direct_Transmit();
	}
	USART3_TX_Cmd( xTxEnable );
}

/********************************** 功能说明 ***********************************
*  收发事件管理
*******************************************************************************/
#include "cmsis_os.h"

static	osSemaphoreId  semaphore_TX;
static	osSemaphoreId  semaphore_RX;

void	vMBM_EventPut_Post( void )
{	//	设置发送完成标志
	osSemaphoreRelease( semaphore_TX );
}

BOOL	xMBM_EventPut_Poll( void )
{	//	等待发送完成标志
	return	( osSemaphoreWait( semaphore_TX, 300u ) > 0 );
}

void	vMBM_EventGet_Post( void )
{	//	设置接收完成标志
	osSemaphoreRelease( semaphore_RX );
}

BOOL	xMBM_EventGet_Poll( void )
{	//	等待接收完成标志
	return	( osSemaphoreWait( semaphore_RX, 500u ) > 0 );
}

void	vMBM_Event_Init( void )
{
	static	osSemaphoreDef ( semaphore_Send );
	static	osSemaphoreDef ( semaphore_Receive );

	semaphore_TX = osSemaphoreCreate(osSemaphore(semaphore_Send), 0 );
	semaphore_RX = osSemaphoreCreate(osSemaphore(semaphore_Receive), 0 );

	assert(( NULL != semaphore_TX ) && ( NULL != semaphore_RX ));
}

/********************************** 功能说明 ***********************************
*  MODBUS 总线的互斥访问控制
*******************************************************************************/
osMutexId	mutexBus_MBM;
osMutexDef ( mutex_MBus );

void	vMBus_apply( void )
{
	osMutexWait( mutexBus_MBM, osWaitForever );
}

void	vMBus_release( void )
{
	osMutexRelease( mutexBus_MBM );
}

void	vMBus_Mutex_Init( void )
{

	mutexBus_MBM = osMutexCreate ( osMutex ( mutex_MBus ));

	assert( NULL != mutexBus_MBM );
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
