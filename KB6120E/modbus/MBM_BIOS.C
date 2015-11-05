/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: MBM_STM32_Port.C
* 创 建 者: 董峰
* 描  述  : KB6120E 芯片端口访问(MODBUS部分)
* 最后修改: 2014年5月7日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "Pin.H"

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

/********************************** 功能说明 ***********************************
*  控制 RS485 总线的方向
*******************************************************************************/
void	RS485_Direct_Transmit( void )
{
//	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
//	GPIOC->BSRR = GPIO_BSRR_BS12;
//	MODIFY_REG( GPIOC->CRH, 0x000F0000u, 0x00030000u );
}

void	RS485_Direct_Receive( void )
{
//	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
//	GPIOC->BSRR = GPIO_BSRR_BR12;
//	MODIFY_REG( GPIOC->CRH, 0x000F0000u, 0x00030000u );
}

/********************************** 功能说明 ***********************************
*  访问定时器
*******************************************************************************/
void	TIM4_Init( uint32_t u_sec )
{
	TIM_TypeDef * TIMx = TIM4;
	//	模块使能
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_TIM4EN );
	//	时基初始化
	TIMx->CR1  = 0u;									/* Counter Up, Division: 1 */
	TIMx->PSC  = ( SystemCoreClock / 1000000u ) - 1u;	/* Set the Prescaler value */
	TIMx->ARR  = u_sec;									/* Set the Autoreload value */
	TIMx->EGR  = TIM_EGR_UG;           					//	生成更新事件，立即更新 PSC
	TIMx->DIER = 0u;
}

void	TIM4_Cmd( BOOL NewState )
{
	TIM_TypeDef * TIMx = TIM4;

	if ( NewState )
	{
		/* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
		CLEAR_BIT( TIMx->SR, TIM_SR_UIF );
		SET_BIT( TIMx->DIER, TIM_DIER_UIE );
		TIMx->CNT = 0u;
		SET_BIT( TIMx->CR1, TIM_CR1_CEN );
	}
	else
	{
		/* Disable any pending timers. */
		CLEAR_BIT( TIMx->CR1, TIM_CR1_CEN );
		TIMx->CNT = 0u;
		CLEAR_BIT( TIMx->DIER, TIM_DIER_UIE );
		CLEAR_BIT( TIMx->SR, TIM_SR_UIF );
	}
}

/********************************** 功能说明 ***********************************
*  访问串口
*******************************************************************************/
void	USART3_Init( uint32_t ulBaudRate )
{
	USART_TypeDef * USARTx = USART3;
	
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_USART3EN );

	USARTx->CR1 = 0u;
	USARTx->CR2 = 0u;
	USARTx->CR3 = USART_CR3_DMAT;
	USARTx->CR1 = USART_CR1_RE | USART_CR1_TE;
	USARTx->BRR = ( SystemCoreClock / ulBaudRate );

//	switch ( ucDataBits )
//	{
//	case 7:
//		CLEAR_BIT( USARTx->CR1, USART_CR1_M );
//		break;
//	case 8:
		SET_BIT( USARTx->CR1, USART_CR1_M );
//		break;
//	}

//	switch ( eParity )
//	{
//	case MB_PAR_NONE:		/*!< No parity. */
//		CLEAR_BIT( USARTx->CR1, USART_CR1_PCE );
//		break;
//	case MB_PAR_ODD:    	/*!< Odd parity. */
//		SET_BIT( USARTx->CR1, USART_CR1_PS );
//		SET_BIT( USARTx->CR1, USART_CR1_PCE );
//		break;		
//	case MB_PAR_EVEN:   	/*!< Even parity. */
		CLEAR_BIT( USARTx->CR1, USART_CR1_PS );
		SET_BIT( USARTx->CR1, USART_CR1_PCE );
//		break;		
//	}

	SET_BIT( USARTx->CR1, USART_CR1_UE );		// Enable USARTx
}

void	USART3_PutByte( uint8_t cOutByte )
{
	USART3->DR = cOutByte;
}

uint8_t	USART3_GetByte( void )
{
	return	USART3->DR;
}












void	USART3_PortInit ( void )
{
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_AFIOEN );
	CLEAR_BIT( AFIO->MAPR, AFIO_MAPR_USART3_REMAP | AFIO_MAPR_SWJ_CFG ); 
	SET_BIT( AFIO->MAPR, AFIO_MAPR_USART3_REMAP_PARTIALREMAP );
	SET_BIT( AFIO->MAPR, AFIO_MAPR_SWJ_CFG_JTAGDISABLE );
//	GPIO_PinRemapConfig( GPIO_PartialRemap_USART3, ENABLE );

	//	PC.11 USART3 Rx, 4：(0100B)浮空输入模式(复位后的状态)
	//	PC.10 USART3 Tx, F：(1111B)复用功能开漏输出模式50MHz
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );
 	MODIFY_REG( GPIOC->CRH, 0x0000FF00u, 0x00004F00u );
}

void	USART3_RX_Cmd( BOOL NewState )
{
	USART_TypeDef * USARTx = USART3;

	if ( NewState )
	{
		SET_BIT  ( USARTx->CR1, USART_CR1_RXNEIE );
	}
	else
	{
		CLEAR_BIT( USARTx->CR1, USART_CR1_RXNEIE );
	}
}

void	USART3_TX_Cmd( BOOL NewState )
{
	USART_TypeDef * USARTx = USART3;

	if ( NewState )
	{
		SET_BIT  ( USARTx->CR1, USART_CR1_TCIE );
	}
	else
	{
		CLEAR_BIT( USARTx->CR1, USART_CR1_TCIE );
	}
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
