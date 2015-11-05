/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: HCBox.C
* 创 建 者: 董峰
* 描  述  : KB-6120E 恒温箱温度控制
* 最后修改: 2014年4月21日
*********************************** 修订记录 ***********************************
* 版  本: V1.1
* 修订人: 董峰
* 说  明: 增加显示接口，显示恒温箱状态
*******************************************************************************/
#include	"AppDEF.H"
//#include	"BIOS.H"
#include <cmsis_os.h>


static	struct	uHCBox
{
	uint8_t	SetMode;		//	设定的控制方式：禁止、加热、制冷、自动 四种方式
	FP32	SetTemp;		//	设定的控制温度：
	FP32	RunTemp;		//	实测的运行温度：
	FP32	OutValue;		//	控制信号输出值[-1000,0,+1000]，正数表示加热，负数表示制冷。
} HCBox;

/********************************** 功能说明 ***********************************
*	测量风扇转速
*******************************************************************************/
#define	fanCountListLen	(4u+(1u+2u))
static	uint16_t	fanCountList[fanCountListLen];
static	uint8_t		fanCountList_index = 0;

uint16_t	FanSpeed_fetch( void )
{
	/*	固定间隔1s记录风扇转动圈数到缓冲区，
	 *	依次计算增量并滤波的结果即风扇转速。
	 */
	uint8_t 	ii, index = fanCountList_index;
	uint16_t	sum = 0u;
	uint16_t	max = 0u;
	uint16_t	min = 0xFFFFu;
	uint16_t	x0, x1, speed;

	x1 = fanCountList[index];
	for ( ii = fanCountListLen - 1u; ii != 0; --ii )
	{
		//	依次求增量得到速度
		x0 = x1;
		if ( ++index >= fanCountListLen ){  index = 0u; }
		x1 = fanCountList[index];
		speed = ( x1 - x0 );
		//	对多个数据进行滤波
		if ( speed > max ) {  max = speed; }
		if ( speed < min ) {  min = speed; }
		sum += speed;
	}

	speed = (uint16_t)( sum - max - min ) / ( fanCountListLen - (1u+2u));
	
	return	speed  * 30u;
}

static	uint16_t	fan_shut_delay = 0u;

static	void	HCBoxFan_Update( void )
{	//	定间隔记录转动圈数
	fanCountList[ fanCountList_index] = HCBoxFan_Circle_Read();
	if ( ++fanCountList_index >= fanCountListLen )
	{
		fanCountList_index = 0u;
	}

	//	风扇开关单稳态控制
	if ( --fan_shut_delay == 0u )
	{
		HCBoxFan_OutCmd( FALSE );
	}
}

void	set_HCBoxTemp( FP32 TempSet, uint8_t ModeSet )
{
	HCBox.SetTemp = TempSet;
	HCBox.SetMode = ModeSet;
}

FP32	get_HCBoxTemp( void )
{
	return	HCBox.RunTemp;
}

FP32	get_HCBoxOutput( void )
{
	return	HCBox.OutValue;
}

uint16_t	get_HCBoxFanSpeed( void )
{
	return	FanSpeed_fetch();
}

/********************************** 功能说明 ***********************************
*  输出控制（隐含循环定时功能）
*******************************************************************************/
static	uint32_t	SaveTick = 0u;

void	HCBox_Output( FP32 OutValue )
{
	//	更新输出状态
	HCBox.OutValue = OutValue;

	if      ( OutValue < 0.0f )
	{
		//	关闭加热
		HCBoxHeat_OutCmd( FALSE );
		//	开启制冷
		if      ( OutValue < -0.990f )
		{
			HCBoxCool_OutCmd( TRUE );
			//	delay( 1000u );
		}
		else if ( OutValue > -0.010f )
		{
			HCBoxCool_OutCmd( FALSE );
			//	delay( 1000u );
		}
		else
		{	
			uint16_t	delayCount = (((( -1000 * OutValue ) + 5 ) / 10 ) * 10 );
			
			HCBoxCool_OutCmd( TRUE );
			delay( delayCount );
			HCBoxCool_OutCmd( FALSE );
			//	delay( 1000u - delayCount );
		}
	}
	else if ( OutValue > 0.0f )
	{
		//	关闭制冷
		HCBoxCool_OutCmd( FALSE );
		//	开启加热
		if      ( OutValue > +0.990f )
		{
			HCBoxHeat_OutCmd( TRUE );
			//	delay( 1000u );
		}
		else if ( OutValue < +0.010f )
		{
			HCBoxHeat_OutCmd( FALSE );
			//	delay( 1000u );
		}
		else
		{
			uint16_t	delayCount = (((( +1000 * OutValue ) + 5 ) / 10 ) * 10 );
			
			HCBoxHeat_OutCmd( TRUE );	
			delay( delayCount );			//	加热延时
			HCBoxHeat_OutCmd( FALSE );
			//	delay( 1000u - delayCount );	//	加热暂停
		}
	}
	else
	{
		//	关闭加热
		HCBoxHeat_OutCmd( FALSE );
		//	关闭制冷
		HCBoxCool_OutCmd( FALSE );

		//	delay( 1000u );
	}

	//	定间隔延时
	{
		//	使用SysTick同步方法，要点在于delay的单位是固定的1毫秒，osKernelSysTickMicroSec( 1000 )即1毫秒。
		const	uint32_t  Tick1ms = osKernelSysTickMicroSec( 1000u );
		uint16_t delayCount = 1000u - (( osKernelSysTick() - SaveTick ) / Tick1ms );

		if ( delayCount < 1000u )
		{
			delay( delayCount );
		}
		
		SaveTick += ( Tick1ms * 1000u );
	}

	//	延时后立即执行
	HCBoxFan_Update();			//	测量风扇转速
}

/********************************** 功能说明 ***********************************
*	加热器恒温箱共用一个温度信号，两者不能同时使用。
*******************************************************************************/
void	HCBoxTemp_Update( void )
{
	int16_t	T16S;

	while ( ! DS18B20_Read( &DS18B20_HCBox, &T16S ))
	{
		HCBox_Output( 0.0f );	//	注意与等待状态的输出保持一致
	}
	HCBox.RunTemp = _CV_DS18B20_Temp( T16S );
}

/********************************** 功能说明 ***********************************
*  等待状态，禁止加热、制冷
*******************************************************************************/
static	void	HCBox_Wait( void )
{
	//	设置自动模式即无法确定实际工作模式可暂时进入等待状态
	do {
		HCBoxTemp_Update();

		HCBox_Output( 0.0f );	//	等待状态输出（隐含循环定时功能）

	} while ( MD_Shut == HCBox.SetMode );
}
/********************************** 功能说明 ***********************************
*  电机温度NTC
*******************************************************************************/
static	void	MotorTemp( void )
{
	
}
/********************************** 功能说明 ***********************************
*  制冷状态：制冷方式工作
*******************************************************************************/
static	void	HCBox_Cool( void )
{
	const	FP32	Kp = 0.1171875f;
	const	FP32	Ki = ( Kp / 240.0f );
	const	FP32	Kd = ( Kp *  80.0f );

	FP32	TempRun, TempSet;
	FP32	Ek_1, Ek = 0.0f;
	FP32	Up = 0.0f, Ui = 0.0f, Ud = 0.0f;
	FP32	Upid = 0.0f;

	uint16_t	shut_delay_count = 0u;
	BOOL		en_cool = TRUE;

		HCBoxTemp_Update();		//	实时读取温度;  if ( 失败 ) 转入待机状态
		
		//	计算PID输出，输出量值归一化到[-1.0至 0.0]范围
		TempRun = HCBox.RunTemp;
		TempSet = HCBox.SetTemp;
		Ek_1 = Ek;
		Ek = ( TempSet - TempRun );

	while ( en_cool )
	{
		HCBoxTemp_Update();		//	实时读取温度;  if ( 失败 ) 转入待机状态
		
		//	计算PID输出，输出量值归一化到[-1.0至 0.0]范围
		TempRun = HCBox.RunTemp;
		TempSet = HCBox.SetTemp;
		Ek_1 = Ek;
		Ek = ( TempSet - TempRun );
		Up = Kp * Ek;
		Ui += Ki * Ek;
		if ( Ui < -0.50f ){  Ui = -0.50f; }
		if ( Ui > +0.50f ){  Ui = +0.50f; }
		Ud = ( Ud * 0.8f ) + (( Kd * ( Ek - Ek_1 )) * 0.2f );
		Upid = Up + Ui + Ud;;
		if ( Upid >  0.0f ){  Upid =  0.0f; }
		if ( Upid < -1.0f ){  Upid = -1.0f; }

		//	风扇输出控制（制冷方式下开启风扇，暂不调速－2014年1月15日）
		if ( Upid < 0.0f )
		{
			fan_shut_delay = 60u;
			HCBoxFan_OutCmd( TRUE );
		}
		
		//	输出
		if ( FanSpeed_fetch() < 100u )
		{	//	风扇不转，禁止制冷片工作
			HCBox_Output( 0.0f );	//	注意与等待状态的输出保持一致
		}
		else
		{
			HCBox_Output( Upid );	//	制冷状态输出（隐含循环定时功能）
		}

		switch ( HCBox.SetMode )
		{
		case MD_Auto:
			//	如果温度偏差超过2'C且维持一段时间（30min）, 切换工作方式
			if ( Ek > -2.0f )
			{
				shut_delay_count = 0u;
			}
			else if ( shut_delay_count < ( 60u * 30u ))
			{
				++shut_delay_count;
			}
			else
			{
				en_cool = FALSE;
			}
			break;
		case MD_Cool:	en_cool = TRUE; 	break;
		case MD_Heat:	en_cool = FALSE;	break;
		default:
		case MD_Shut:	en_cool = FALSE;	break;
		}
	}
}

/********************************** 功能说明 ***********************************
*  加热状态：加热方式工作
*******************************************************************************/
static	void	HCBox_Heat( void )
{
	const	FP32	Kp = 0.0390625f;
	const	FP32	Ki = ( Kp / 160.0f );
	const	FP32	Kd = ( Kp *  80.0f );

//	const	FP32	Kp = 0.2;
//	const	FP32	Ki = ( Kp / 100.0f );
//	const	FP32	Kd = ( Kp *  10.0f );

	FP32	TempRun, TempSet;
	FP32	Ek_1, Ek = 0.0f;
	FP32	Up = 0.0f, Ui = 0.0f, Ud = 0.0f;
	FP32	Upid = 0.0f;

	uint16_t	shut_delay_count = 0u;
	BOOL		en_heat = TRUE;

	while ( en_heat )
	{
		HCBoxTemp_Update();

		//	计算PID输出，输出量值归一化到[0.0 至+1.0]范围
		TempRun = HCBox.RunTemp;
		TempSet = HCBox.SetTemp;
		Ek_1 = Ek;
		Ek = ( TempSet - TempRun );
		Up = Kp * Ek;
		Ui += Ki * Ek;
		if ( Ui < -0.25f ){  Ui = -0.25f; }
		if ( Ui > +0.25f ){  Ui = +0.25f; }
		Ud = ( Ud * 0.8f ) + (( Kd * ( Ek - Ek_1 )) * 0.2f );
		Upid = ( Up + Ui + Ud );
		if ( Upid <  0.0f ){  Upid = 0.0f; }
		if ( Upid > +1.0f ){  Upid = 1.0f; }

		HCBox_Output( Upid );	//	加热状态输出（隐含循环定时功能）

		switch ( HCBox.SetMode )
		{
		case MD_Auto:
			//	如果温度偏差超过2'C且维持一段时间（30min）, 切换工作方式
			if ( Ek < +2.0f )
			{
				shut_delay_count = 0u;
			}
			else if ( shut_delay_count < ( 60u * 30u ))
			{
				++shut_delay_count;
			}
			else
			{
				en_heat = FALSE;
			}
			break;
		case MD_Heat:	en_heat = TRUE; 	break;
		case MD_Cool:	en_heat = FALSE;	break;
		default:
		case MD_Shut:	en_heat = FALSE;	break;
		}
	}
}



/********************************** 功能说明 ***********************************
*	恒温箱温度控制
*******************************************************************************/
__task	void	_task_HCBox( void const * p_arg )
{
	HCBoxFan_Circle_PortInit();
	
	SaveTick = osKernelSysTick();                      // get start value of the Kernel system tick

	HCBox_Wait();	//	dummy read, skip 0x0550
	HCBox_Wait();	//	强制停留在等待状态一段时间

	for(;;)
	{
		switch ( HCBox.SetMode )
		{
		case MD_Auto:
			if      (( HCBox.SetTemp - HCBox.RunTemp ) < -1.0f )
			{
				HCBox_Cool();
			}
			else if (( HCBox.SetTemp - HCBox.RunTemp ) > +1.0f )
			{
				HCBox_Heat();
			}
			else
			{	//	设置自动模式即无法确定实际工作模式可暂时进入等待状态
				HCBox_Wait();
			}
			break;
		case MD_Cool:	HCBox_Cool();	break;
		case MD_Heat:	HCBox_Heat();	break;
		default:
		case MD_Shut:	HCBox_Wait();	break;
		}
	}
}

/********************************** 功能说明 ***********************************
*	加热器温度控制（与恒温箱的区别：控制参数，加热器的控制量Kp无穷大）
*******************************************************************************/
static	struct	uHeater
{
	FP32	SetTemp;
	FP32	RunTemp;
	BOOL	OutState;
} Heater;

static	void	HeaterHeat_OutCmd( BOOL NewState )
{
	HCBoxHeat_OutCmd( NewState );
}

__task  void	_task_Heater( void const * p_arg )
{
	int16_t	T16S;

	( void )DS18B20_Read( &DS18B20_HCBox, &T16S );

	for(;;)
	{
		delay ( 1000u );	//	任务每1000ms循环一次

		if ( ! DS18B20_Read( &DS18B20_HCBox, &T16S ))
		{
			Heater.OutState = FALSE;
		}
		else
		{
			Heater.RunTemp = _CV_DS18B20_Temp( T16S );
			Heater.OutState = ( Heater.RunTemp < Heater.SetTemp );
		}

		HeaterHeat_OutCmd( Heater.OutState );
	}
}

void	set_HeaterTemp( FP32 SetTemp )
{
	Heater.SetTemp = SetTemp;
}

FP32	get_HeaterTemp( void )
{
	return	Heater.RunTemp;
}

BOOL	get_HeaterOutput( void )
{
	return	Heater.OutState;
}

/********************************** 功能说明 ***********************************
*  初始化恒温控制任务
*******************************************************************************/
osThreadDef( _task_HCBox, osPriorityBelowNormal, 1, 200 );

void	HCBox_Init( void )
{
//	switch ( Configure.HeaterType )
//	{
//	default:
//	case enumHeaterNone:	MsgBox( "未安装恒温箱", vbOKOnly );	break;
//	case enumHCBoxOnly:		Configure_HCBox();	break;	
//	case enumHeaterOnly:	Configure_Heater();	break;
//	case enumHCBoxHeater:	MsgBox( "硬件不能支持", vbOKOnly );	break;
//	}
	set_HCBoxTemp( Configure.HCBox_SetTemp * 0.1f, Configure.HCBox_SetMode );
	osThreadCreate( osThread( _task_HCBox ), NULL );
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/


#if 0

// 使用TIMx的CH1的捕获功能，用DMA记录脉冲的周期.
#define	CMR_Len	10
static	uint16_t	CMRA[CMR_Len];

void	CMR1( void )
{
	DMA_Channel_TypeDef	* DMA1_Channelx = DMA1_Channel6;
	TIM_TypeDef * TIMx = TIM16;
	
	//	DMA1 channel1 configuration
	SET_BIT ( RCC->AHBENR,  RCC_AHBENR_DMA1EN );
	//	DMA模块禁能, 重新配置
	DMA1_Channelx->CCR  = 0u;
	DMA1_Channelx->CCR  = DMA_CCR6_PL_0						//	通道优先级：01 中等
						| DMA_CCR6_PSIZE_0					//	内存数据位：01 16位
						| DMA_CCR6_MSIZE_0					//	外设数据位：01 16位
						| DMA_CCR6_MINC						//	增量模式：内存增量
						| DMA_CCR6_CIRC						//	循环传输：使能循环
					//	| DMA_CCR6_DIR						//	传送方向：从外设读
						;
	DMA1_Channelx->CPAR  = (uint32_t) &TIM16->CCR1;			//	设置DMA外设地址
	DMA1_Channelx->CMAR  = (uint32_t) CMRA;					//	内存地址
	DMA1_Channelx->CNDTR = CMR_Len;							//	传输数量
	SET_BIT ( DMA1_Channelx->CCR, DMA_CCR1_EN );			//	使能DMA通道

	//	配置TIMx 进行输入捕获。
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_TIM16EN );
	TIMx->CR1   = 0u;
	TIMx->CR2   = 0u;
	TIMx->CCER  = 0u;
	TIMx->CCMR1 = 0u;
	TIMx->CCMR2 = 0u;
	//	TIMx 时基初始化: 输入时钟频率24MHz，分频成1MHz的输入。
	//	时基决定可以测量的最低速度与最高速度。
	TIMx->PSC = 240u - 1;	//	10us @ 24MHz
	TIMx->ARR = 0xFFFFu;
	TIMx->EGR = TIM_EGR_UG;
	
	TIMx->CCMR1 = TIM_CCMR1_CC1S_0					//	CC1S  : 01b   IC1 映射到IT1上。
				| TIM_CCMR1_IC1F_1|TIM_CCMR1_IC1F_0	//	IC1F  : 0011b 配置输入滤波器，8个定时器时钟周期滤波
				| TIM_CCMR1_IC2PSC_1				//	IC1PSC: 01b   配置输入分频，每隔2次事件发生一次捕获
				;
	TIMx->CCER  = TIM_CCER_CC1E						//	允许 CCR1 执行捕获
				| TIM_CCER_CC1P						//	负边沿CCR1捕获信号周期。
				;
	TIMx->DIER  = TIM_DIER_CC1DE;

	TIMx->CR1   = TIM_CR1_CEN;						//	使能定时器

	//	配置管脚：PA.6 浮空输入
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRL, 0x0F000000u, 0x04000000u );
}

uint16_t	fetchSpeed( void )
{	//	取 DMA 计数 或 内存地址指针，并连续向前增量两次。
	//	如果DMA计数 或 内存指针都不可用，取N次的差值，并丢弃最大值和最小值。
	
	/*	固定间隔1s记录风扇转动圈数到缓冲区，
	 *	依次计算增量并滤波的结果即风扇转速。
	 */
	DMA_Channel_TypeDef	* DMA1_Channelx = DMA1_Channel6;
	uint8_t 	ii, index;
	uint16_t	sum = 0u;
//	uint16_t	max = 0u;
//	uint16_t	min = 0xFFFFu;
	uint16_t	x0, x1, period;

	index = ( DMA1_Channelx->CMAR - ( uint32_t ) CMRA ) / sizeof( uint16_t);	//	内存地址
	if ( ++index >= CMR_Len ){  index = 0u; }
	if ( ++index >= CMR_Len ){  index = 0u; }
	
	x1 = CMRA[index];
	for ( ii = CMR_Len - 2u; ii != 0; --ii )
	{
		//	依次求增量得到速度
		x0 = x1;
		if ( ++index >= CMR_Len ){  index = 0u; }
		x1 = CMRA[index];
		period = (uint16_t)( x1 - x0 );
		//	对多个数据进行滤波
//		if ( period > max ) {  max = period; }
//		if ( period < min ) {  min = period; }
//		sum += period;
	}
	period = sum / ( CMR_Len - 2u );
//	period = (uint16_t)( sum - max - min ) / ( CMR_Len - (1u+2u));

	if ( period == 0u )
	{
		return	0xFFFFu;
	}
	else
	{	//	每分钟的计数周期 / 每个脉冲的计数时间 => 每分钟的转速
		return	(( 60u * 100000u ) / period );
	}
}

#endif

