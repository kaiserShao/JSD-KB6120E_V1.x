/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Keyboard.C
* 创 建 者: 董峰
* 描  述  : 按键扫描接口与访问接口
* 最后修改: 2014年5月5日
*********************************** 修订记录 ***********************************
* 版  本: V2.0 
* 修订人: 董峰
* 说  明: 使用按键变化中断与按键事件信号，降低按键处理需要的时间。
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"
#include <cmsis_os.h>

static	BOOL		KeyHited;
static	uKey		KeyDirect;
static	uKey		KeyBuffer;
/********************************** 功能说明 ***********************************
*	按键访问接口
*******************************************************************************/
osSemaphoreId semiKeyHited;		//	有新按键按下（按键处理程序发送，使用者接收）

uKey	getKey( void )
{
	uKey  KeyRead;

	if ( ! KeyHited )
	{	//	等待按键信号
		osSemaphoreWait( semiKeyHited, osWaitForever );
	}
	
	KeyRead = KeyBuffer;
	KeyHited = FALSE;
	tick();
	return	KeyRead;
}

BOOL	hitKey( uint16_t iRetry )
{
	if ( ! KeyHited )
	{	//	等待按键信号
		osSemaphoreWait( semiKeyHited, iRetry * 10u );
	}
	
	return	KeyHited;
}

BOOL	releaseKey( uKey lastKey, uint16_t iRetry )
{
	while( iRetry-- )
	{
		if ( lastKey != KeyDirect )
		{
			break;
		}
		delay( 10u );
	}
	return	( lastKey != KeyDirect ) ? TRUE : FALSE;
}

/********************************** 功能说明 ***********************************
*	按键扫描事件
*******************************************************************************/
osSemaphoreId	semiKeyChanged;	//	按键状态变化（由管脚变化中断发送，按键处理程序接收）

static	void	KeyboardStateChanged_Post( void )
{
	osSemaphoreRelease( semiKeyChanged );
}

/********************************** 功能说明 ***********************************
*	按键中断处理	使用中断检测按键的变化，可以节省大量的查询时间
*******************************************************************************/
void	Keyboard_IRQ_Server( void )
{
	Keyboard_IRQ_Disable();				//	禁止按键变化中断
	KeyboardStateChanged_Post();		//  发信号给按键扫描任务
}









volatile	uint32_t	TimeoutLight = 0u;

void		DisplaySetTimeout( uint8_t setTimeout )
{
	switch ( setTimeout )
	{
	case 0:	TimeoutLight = 0u;			break;
	case 1:	TimeoutLight = 15 * 1000u;	break;
	case 2:	TimeoutLight = 30 * 1000u;	break;
	case 3:	TimeoutLight = 60 * 1000u;	break;
	default:
	case 4:	TimeoutLight = osWaitForever;	break;
	}
}

static	void	KeyboardStateChanged_Poll( void )
{
	int32_t	result;
	
	switch ( TimeoutLight )
	{
	case 0u:
		Backlight_OFF();
		osSemaphoreWait( semiKeyChanged, osWaitForever );
		break;
	case osWaitForever:
		Backlight_ON();
		osSemaphoreWait( semiKeyChanged, osWaitForever );
		break;
	default:
		Backlight_ON();
		result = osSemaphoreWait( semiKeyChanged, TimeoutLight );

		if (  result <= 0 )
		{	//	没有等到信号量，那么应该是超时时间到了，关闭背光。
			Backlight_OFF();
			osSemaphoreWait( semiKeyChanged, osWaitForever );
		}
		break;
	}
}


/********************************** 功能说明 ***********************************
*	按键扫描接口
*******************************************************************************/
__task	void	_task_Keyboard( void const * p_arg )
{
	uKey	keyPortState, keyDebounce;

	( void )p_arg;
	delay( 100u );

	Keyboard_PortInit();
	for (;;)
	{
		Keyboard_IRQ_Enable();			//	允许按键变化触发中断
		

		delay( 20u );					//	等待按键状态稳定后再读
		keyPortState = Keyboard_PortRead();
		KeyboardStateChanged_Poll();	//	等待按键变化事件信号
		do 
    {
			keyDebounce = keyPortState;
			delay( 20u );
			keyPortState = Keyboard_PortRead();
		} while ( keyDebounce != keyPortState );

		KeyDirect = keyDebounce;		//	保存扫描结果
		
		//	新的按键入队并告知主程序有新的按键按下。
		if ( ! KeyHited )
		{
			if ( KeyBuffer < KeyDirect )
			{
				KeyHited = TRUE;
			}
			KeyBuffer = KeyDirect;

			if ( KeyHited )
			{	//	向等待按键的任务发信号
				osSemaphoreRelease( semiKeyHited );
			}
		}
	}
}

/********************************** 功能说明 ***********************************
*	按键扫描系列初始化
*******************************************************************************/
osThreadDef( _task_Keyboard, osPriorityAboveNormal, 1u, 100u );
osSemaphoreDef( KeyChanged );
osSemaphoreDef( KeyHited );

void	Keyboard_Init( void )
{
	semiKeyChanged = osSemaphoreCreate(osSemaphore( KeyChanged ), 0 );
	semiKeyHited = osSemaphoreCreate(osSemaphore( KeyHited ), 0 );
	osThreadCreate( osThread( _task_Keyboard ), NULL );
}

__irq	void	EXTI0_IRQHandler( void )
{
	Keyboard_IRQ_Server();
}

__irq	void	EXTI1_IRQHandler( void )
{
	Keyboard_IRQ_Server();
}

__irq	void	EXTI2_IRQHandler( void )
{
	Keyboard_IRQ_Server();
}

__irq	void	EXTI3_IRQHandler( void )
{
	Keyboard_IRQ_Server();
}

__irq	void	EXTI4_IRQHandler( void )
{
	Keyboard_IRQ_Server();
}

__irq	void	EXTI9_5_IRQHandler( void )
{
	Keyboard_IRQ_Server();
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
