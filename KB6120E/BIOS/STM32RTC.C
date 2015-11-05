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
 *	访问 STM32 的 RTC 模块
 *	包含将时钟计数器初始化为 GMT: Sun, 1 Jan 2012 00:00:00 UTC
*******************************************************************************/
BOOL	STM32_RTC_Init( void )
{
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_BKPEN );	//	允许访问备份区域
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_PWREN );
	SET_BIT( PWR->CR, PWR_CR_DBP );				//	Allow access to BKP Domain

	if ( ! READ_BIT( RCC->BDCR, RCC_BDCR_RTCEN ))
	{
		/* Reset Backup Domain */
		SET_BIT( RCC->BDCR, RCC_BDCR_BDRST );
 		RCC->BDCR = 0u;  									/* Reset LSEON bit */
		RCC->BDCR = 0u;  									/* Reset LSEBYP bit */
		RCC->BDCR = RCC_BDCR_LSEON;							/* Enable LSE */
		while ( ! READ_BIT( RCC->BDCR, RCC_BDCR_LSERDY )){}	/* Wait till LSE is ready */
		SET_BIT( RCC->BDCR, RCC_BDCR_RTCSEL_LSE );			/* Select LSE as RTC Clock Source */
		SET_BIT( RCC->BDCR, RCC_BDCR_RTCEN );				/* Enable RTC Clock */

		/* Wait for RTC registers synchronization */
		CLEAR_BIT( RTC->CRL, RTC_CRL_RSF );
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RSF )){}		//	RTC_WaitForSynchro();

		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
		RTC->CRH = 0u;										//	禁止RTC中断

		/* Wait until last write operation on RTC registers has finished */
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
		SET_BIT( RTC->CRL, RTC_CRL_CNF );					//  RTC_EnterConfigMode();
		/* Set Alarm to 0 */
		RTC->ALRH = 0u;
		RTC->ALRL = 0u;
		/* Set RTC prescaler: set RTC period to 1sec */
		RTC->PRLH = 0x0000u;
		RTC->PRLL = 0x7FFFu;
		/*	时钟计数器初始化为 GMT: Sun, 1 Jan 2012 00:00:00 UTC, see http://www.epochconverter.com */
		RTC->CNTH = (uint16_t)( 1325376000u >> 16 );
		RTC->CNTL = (uint16_t)( 1325376000u );
		CLEAR_BIT( RTC->CRL, RTC_CRL_CNF ); 				//  RTC_ExitConfigMode();
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
	}
	return	TRUE;
// 	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_BKPEN );	//	允许访问备份区域
// 	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_PWREN );
// 	SET_BIT( PWR->CR, PWR_CR_DBP );				//	Allow access to BKP Domain
// 	SET_BIT( RCC->BDCR, RCC_BDCR_RTCSEL_LSI );
// 	SET_BIT( RCC->CSR, RCC_CSR_LSION );
// 	SET_BIT( RCC->CSR, RCC_CSR_LSIRDY );
// 	if ( ! READ_BIT( RCC->BDCR, RCC_BDCR_RTCEN ))
// 	{
// 		/* Reset Backup Domain */
// // 		SET_BIT( RCC->BDCR, RCC_BDCR_BDRST );
// //  		RCC->BDCR = 0u;  									/* Reset LSEON bit */
// // 		RCC->BDCR = 0u;  									/* Reset LSEBYP bit */
// 	SET_BIT( RCC->BDCR, RCC_BDCR_RTCSEL_LSI );
// 	SET_BIT( RCC->CSR, RCC_CSR_LSION );
// 	SET_BIT( RCC->CSR, RCC_CSR_LSIRDY );

// // 		RCC->BDCR = RCC_BDCR_LSEON;							/* Enable LSE */		

// // 		while ( ! READ_BIT( RCC->BDCR, RCC_BDCR_LSERDY )){}	/* Wait till LSE is ready */
// // 		SET_BIT( RCC->BDCR, RCC_BDCR_RTCSEL_LSE );			/* Select LSE as RTC Clock Source */
// 		SET_BIT( RCC->BDCR, RCC_BDCR_RTCEN );				/* Enable RTC Clock */

// 		/* Wait for RTC registers synchronization */
// 		CLEAR_BIT( RTC->CRL, RTC_CRL_RSF );
// 		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RSF )){}		//	RTC_WaitForSynchro();

// 		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
// 		RTC->CRH = 0u;										//	禁止RTC中断

// 		/* Wait until last write operation on RTC registers has finished */
// 		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
// 		SET_BIT( RTC->CRL, RTC_CRL_CNF );					//  RTC_EnterConfigMode();
// 		/* Set Alarm to 0 */
// 		RTC->ALRH = 0u;
// 		RTC->ALRL = 0u;
// 		/* Set RTC prescaler: set RTC period to 1sec */
// 		RTC->PRLH = 0x0000u;
// 		RTC->PRLL = 0x7FFFu;
// 		/*	时钟计数器初始化为 GMT: Sun, 1 Jan 2012 00:00:00 UTC, see http://www.epochconverter.com */
// 		RTC->CNTH = (uint16_t)( 1325376000u >> 16 );
// 		RTC->CNTL = (uint16_t)( 1325376000u );
// 		CLEAR_BIT( RTC->CRL, RTC_CRL_CNF ); 				//  RTC_ExitConfigMode();
// 		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
// 	}
// 	return	TRUE;
}

static	void	STM32_RTC_Save( uint32_t timer )
{
	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
	SET_BIT( RTC->CRL, RTC_CRL_CNF );					//  RTC_EnterConfigMode();
	RTC->CNTH = (uint16_t)( timer >> 16 );
	RTC->CNTL = (uint16_t)( timer );
	CLEAR_BIT( RTC->CRL, RTC_CRL_CNF ); 				//  RTC_ExitConfigMode();
	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
}

static	uint32_t	STM32_RTC_Load( void )
{
	return (((uint32_t)RTC->CNTH << 16 ) | RTC->CNTL );
}

/********************************** 功能说明 ***********************************
 *	访问 STM32 的 RTC 接口
*******************************************************************************/
BOOL	RTC_Init ( void )
{
	return	STM32_RTC_Init();
}

void	RTC_Save ( const uClock * pClock )
{
	STM32_RTC_Save( * pClock );
}

void	RTC_Load (       uClock * pClock )
{
	* pClock = STM32_RTC_Load();
}

/********************************** 功能说明 ***********************************
 *	读取当前时间
*******************************************************************************/
uint32_t	get_Now( void )
{
	return	STM32_RTC_Load();
}

/********************************** 功能说明 ***********************************
 *	测量 HSI
*******************************************************************************/
#define 	MCLK_Len	(4u+(1u))
uint16_t	MCLK_List[MCLK_Len];
uint8_t   MCLK_Index =  0u;

uint32_t CalcMCLK( void )
{
	uint8_t 	i, index = MCLK_Index;
	uint32_t	sum = 0u;
	uint16_t	x0, x1, speed;

	x1 = MCLK_List[index];
	for ( i = MCLK_Len - 1u; i != 0; --i )
	{
		//	依次求增量得到速度
		x0 = x1;
		if ( ++index >= MCLK_Len ){  index = 0u; }
		x1 = MCLK_List[index];
		speed = (uint16_t)( x1 - x0 );
		sum += speed;
	}
	speed = sum / ( MCLK_Len - (1u));
	
	return	speed * 500u;
}

void	RTC_IRQHandler( void )
{
	TIM_TypeDef * TIMx = TIM5;

	if ( READ_BIT( RTC->CRL, RTC_CRL_SECF ))
	{
		CLEAR_BIT( RTC->CRL, RTC_CRL_SECF );
	
		MCLK_List[MCLK_Index] = TIMx->CNT;
		if ( ++MCLK_Index >= MCLK_Len ) { MCLK_Index = 0u; }
		
		while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();		
	}
}

void	RTC_IRQ_Init( void )
{
	TIM_TypeDef * TIMx = TIM5;
	
	SET_BIT( RCC->APB1ENR, RCC_APB1ENR_TIM5EN );
	
	TIMx->CR1  = 0u;
	TIMx->PSC  = 500u - 1u;
	TIMx->ARR  = 65535u;
	TIMx->EGR  = TIM_EGR_UG;
	SET_BIT( TIMx->CR1, TIM_CR1_CEN );
	
	CLEAR_BIT( RTC->CRL, RTC_CRL_RSF );
	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RSF )){}		//	RTC_WaitForSynchro();

	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();

	SET_BIT( RTC->CRH, RTC_CRH_SECIE );					//  允许RTC秒中断

	while ( ! READ_BIT( RTC->CRL, RTC_CRL_RTOFF )){}	//	RTC_WaitForLastTask();
		
	NVIC_EnableIRQ( RTC_IRQn );
}

uint16_t	Read_HSITrim( void )
{
	return	( RCC->CR & 0xFFFFu );
}

void	HSITRIM_UP( void )
{
	uint8_t	trim;
	
	trim = ( RCC->CR & RCC_CR_HSITRIM ) & 0xFFu;
	trim += 8u;
	MODIFY_REG( RCC->CR, RCC_CR_HSITRIM, trim );
}

void	HSITRIM_DOWN( void )
{
	uint8_t	trim;
	
	trim = ( RCC->CR & RCC_CR_HSITRIM ) & 0xFFu;
	trim -= 8u;
	MODIFY_REG( RCC->CR, RCC_CR_HSITRIM, trim );
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
