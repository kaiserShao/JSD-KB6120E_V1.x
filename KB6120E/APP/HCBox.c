/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: HCBox.C
* �� �� ��: ����
* ��  ��  : KB-6120E �������¶ȿ���
* ����޸�: 2014��4��21��
*********************************** �޶���¼ ***********************************
* ��  ��: V1.1
* �޶���: ����
* ˵  ��: ������ʾ�ӿڣ���ʾ������״̬
*******************************************************************************/
#include	"AppDEF.H"
//#include	"BIOS.H"
#include <cmsis_os.h>


static	struct	uHCBox
{
	uint8_t	SetMode;		//	�趨�Ŀ��Ʒ�ʽ����ֹ�����ȡ����䡢�Զ� ���ַ�ʽ
	FP32	SetTemp;		//	�趨�Ŀ����¶ȣ�
	FP32	RunTemp;		//	ʵ��������¶ȣ�
	FP32	OutValue;		//	�����ź����ֵ[-1000,0,+1000]��������ʾ���ȣ�������ʾ���䡣
} HCBox;

/********************************** ����˵�� ***********************************
*	��������ת��
*******************************************************************************/
#define	fanCountListLen	(4u+(1u+2u))
static	uint16_t	fanCountList[fanCountListLen];
static	uint8_t		fanCountList_index = 0;

uint16_t	FanSpeed_fetch( void )
{
	/*	�̶����1s��¼����ת��Ȧ������������
	 *	���μ����������˲��Ľ��������ת�١�
	 */
	uint8_t 	ii, index = fanCountList_index;
	uint16_t	sum = 0u;
	uint16_t	max = 0u;
	uint16_t	min = 0xFFFFu;
	uint16_t	x0, x1, speed;

	x1 = fanCountList[index];
	for ( ii = fanCountListLen - 1u; ii != 0; --ii )
	{
		//	�����������õ��ٶ�
		x0 = x1;
		if ( ++index >= fanCountListLen ){  index = 0u; }
		x1 = fanCountList[index];
		speed = ( x1 - x0 );
		//	�Զ�����ݽ����˲�
		if ( speed > max ) {  max = speed; }
		if ( speed < min ) {  min = speed; }
		sum += speed;
	}

	speed = (uint16_t)( sum - max - min ) / ( fanCountListLen - (1u+2u));
	
	return	speed  * 30u;
}

static	uint16_t	fan_shut_delay = 0u;

static	void	HCBoxFan_Update( void )
{	//	�������¼ת��Ȧ��
	fanCountList[ fanCountList_index] = HCBoxFan_Circle_Read();
	if ( ++fanCountList_index >= fanCountListLen )
	{
		fanCountList_index = 0u;
	}

	//	���ȿ��ص���̬����
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

/********************************** ����˵�� ***********************************
*  ������ƣ�����ѭ����ʱ���ܣ�
*******************************************************************************/
static	uint32_t	SaveTick = 0u;

void	HCBox_Output( FP32 OutValue )
{
	//	�������״̬
	HCBox.OutValue = OutValue;

	if      ( OutValue < 0.0f )
	{
		//	�رռ���
		HCBoxHeat_OutCmd( FALSE );
		//	��������
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
		//	�ر�����
		HCBoxCool_OutCmd( FALSE );
		//	��������
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
			delay( delayCount );			//	������ʱ
			HCBoxHeat_OutCmd( FALSE );
			//	delay( 1000u - delayCount );	//	������ͣ
		}
	}
	else
	{
		//	�رռ���
		HCBoxHeat_OutCmd( FALSE );
		//	�ر�����
		HCBoxCool_OutCmd( FALSE );

		//	delay( 1000u );
	}

	//	�������ʱ
	{
		//	ʹ��SysTickͬ��������Ҫ������delay�ĵ�λ�ǹ̶���1���룬osKernelSysTickMicroSec( 1000 )��1���롣
		const	uint32_t  Tick1ms = osKernelSysTickMicroSec( 1000u );
		uint16_t delayCount = 1000u - (( osKernelSysTick() - SaveTick ) / Tick1ms );

		if ( delayCount < 1000u )
		{
			delay( delayCount );
		}
		
		SaveTick += ( Tick1ms * 1000u );
	}

	//	��ʱ������ִ��
	HCBoxFan_Update();			//	��������ת��
}

/********************************** ����˵�� ***********************************
*	�����������乲��һ���¶��źţ����߲���ͬʱʹ�á�
*******************************************************************************/
void	HCBoxTemp_Update( void )
{
	int16_t	T16S;

	while ( ! DS18B20_Read( &DS18B20_HCBox, &T16S ))
	{
		HCBox_Output( 0.0f );	//	ע����ȴ�״̬���������һ��
	}
	HCBox.RunTemp = _CV_DS18B20_Temp( T16S );
}

/********************************** ����˵�� ***********************************
*  �ȴ�״̬����ֹ���ȡ�����
*******************************************************************************/
static	void	HCBox_Wait( void )
{
	//	�����Զ�ģʽ���޷�ȷ��ʵ�ʹ���ģʽ����ʱ����ȴ�״̬
	do {
		HCBoxTemp_Update();

		HCBox_Output( 0.0f );	//	�ȴ�״̬���������ѭ����ʱ���ܣ�

	} while ( MD_Shut == HCBox.SetMode );
}
/********************************** ����˵�� ***********************************
*  ����¶�NTC
*******************************************************************************/
static	void	MotorTemp( void )
{
	
}
/********************************** ����˵�� ***********************************
*  ����״̬�����䷽ʽ����
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

		HCBoxTemp_Update();		//	ʵʱ��ȡ�¶�;  if ( ʧ�� ) ת�����״̬
		
		//	����PID����������ֵ��һ����[-1.0�� 0.0]��Χ
		TempRun = HCBox.RunTemp;
		TempSet = HCBox.SetTemp;
		Ek_1 = Ek;
		Ek = ( TempSet - TempRun );

	while ( en_cool )
	{
		HCBoxTemp_Update();		//	ʵʱ��ȡ�¶�;  if ( ʧ�� ) ת�����״̬
		
		//	����PID����������ֵ��һ����[-1.0�� 0.0]��Χ
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

		//	����������ƣ����䷽ʽ�¿������ȣ��ݲ����٣�2014��1��15�գ�
		if ( Upid < 0.0f )
		{
			fan_shut_delay = 60u;
			HCBoxFan_OutCmd( TRUE );
		}
		
		//	���
		if ( FanSpeed_fetch() < 100u )
		{	//	���Ȳ�ת����ֹ����Ƭ����
			HCBox_Output( 0.0f );	//	ע����ȴ�״̬���������һ��
		}
		else
		{
			HCBox_Output( Upid );	//	����״̬���������ѭ����ʱ���ܣ�
		}

		switch ( HCBox.SetMode )
		{
		case MD_Auto:
			//	����¶�ƫ���2'C��ά��һ��ʱ�䣨30min��, �л�������ʽ
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

/********************************** ����˵�� ***********************************
*  ����״̬�����ȷ�ʽ����
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

		//	����PID����������ֵ��һ����[0.0 ��+1.0]��Χ
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

		HCBox_Output( Upid );	//	����״̬���������ѭ����ʱ���ܣ�

		switch ( HCBox.SetMode )
		{
		case MD_Auto:
			//	����¶�ƫ���2'C��ά��һ��ʱ�䣨30min��, �л�������ʽ
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



/********************************** ����˵�� ***********************************
*	�������¶ȿ���
*******************************************************************************/
__task	void	_task_HCBox( void const * p_arg )
{
	HCBoxFan_Circle_PortInit();
	
	SaveTick = osKernelSysTick();                      // get start value of the Kernel system tick

	HCBox_Wait();	//	dummy read, skip 0x0550
	HCBox_Wait();	//	ǿ��ͣ���ڵȴ�״̬һ��ʱ��

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
			{	//	�����Զ�ģʽ���޷�ȷ��ʵ�ʹ���ģʽ����ʱ����ȴ�״̬
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

/********************************** ����˵�� ***********************************
*	�������¶ȿ��ƣ������������𣺿��Ʋ������������Ŀ�����Kp�����
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
		delay ( 1000u );	//	����ÿ1000msѭ��һ��

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

/********************************** ����˵�� ***********************************
*  ��ʼ�����¿�������
*******************************************************************************/
osThreadDef( _task_HCBox, osPriorityBelowNormal, 1, 200 );

void	HCBox_Init( void )
{
//	switch ( Configure.HeaterType )
//	{
//	default:
//	case enumHeaterNone:	MsgBox( "δ��װ������", vbOKOnly );	break;
//	case enumHCBoxOnly:		Configure_HCBox();	break;	
//	case enumHeaterOnly:	Configure_Heater();	break;
//	case enumHCBoxHeater:	MsgBox( "Ӳ������֧��", vbOKOnly );	break;
//	}
	set_HCBoxTemp( Configure.HCBox_SetTemp * 0.1f, Configure.HCBox_SetMode );
	osThreadCreate( osThread( _task_HCBox ), NULL );
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/


#if 0

// ʹ��TIMx��CH1�Ĳ����ܣ���DMA��¼���������.
#define	CMR_Len	10
static	uint16_t	CMRA[CMR_Len];

void	CMR1( void )
{
	DMA_Channel_TypeDef	* DMA1_Channelx = DMA1_Channel6;
	TIM_TypeDef * TIMx = TIM16;
	
	//	DMA1 channel1 configuration
	SET_BIT ( RCC->AHBENR,  RCC_AHBENR_DMA1EN );
	//	DMAģ�����, ��������
	DMA1_Channelx->CCR  = 0u;
	DMA1_Channelx->CCR  = DMA_CCR6_PL_0						//	ͨ�����ȼ���01 �е�
						| DMA_CCR6_PSIZE_0					//	�ڴ�����λ��01 16λ
						| DMA_CCR6_MSIZE_0					//	��������λ��01 16λ
						| DMA_CCR6_MINC						//	����ģʽ���ڴ�����
						| DMA_CCR6_CIRC						//	ѭ�����䣺ʹ��ѭ��
					//	| DMA_CCR6_DIR						//	���ͷ��򣺴������
						;
	DMA1_Channelx->CPAR  = (uint32_t) &TIM16->CCR1;			//	����DMA�����ַ
	DMA1_Channelx->CMAR  = (uint32_t) CMRA;					//	�ڴ��ַ
	DMA1_Channelx->CNDTR = CMR_Len;							//	��������
	SET_BIT ( DMA1_Channelx->CCR, DMA_CCR1_EN );			//	ʹ��DMAͨ��

	//	����TIMx �������벶��
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_TIM16EN );
	TIMx->CR1   = 0u;
	TIMx->CR2   = 0u;
	TIMx->CCER  = 0u;
	TIMx->CCMR1 = 0u;
	TIMx->CCMR2 = 0u;
	//	TIMx ʱ����ʼ��: ����ʱ��Ƶ��24MHz����Ƶ��1MHz�����롣
	//	ʱ���������Բ���������ٶ�������ٶȡ�
	TIMx->PSC = 240u - 1;	//	10us @ 24MHz
	TIMx->ARR = 0xFFFFu;
	TIMx->EGR = TIM_EGR_UG;
	
	TIMx->CCMR1 = TIM_CCMR1_CC1S_0					//	CC1S  : 01b   IC1 ӳ�䵽IT1�ϡ�
				| TIM_CCMR1_IC1F_1|TIM_CCMR1_IC1F_0	//	IC1F  : 0011b ���������˲�����8����ʱ��ʱ�������˲�
				| TIM_CCMR1_IC2PSC_1				//	IC1PSC: 01b   ���������Ƶ��ÿ��2���¼�����һ�β���
				;
	TIMx->CCER  = TIM_CCER_CC1E						//	���� CCR1 ִ�в���
				| TIM_CCER_CC1P						//	������CCR1�����ź����ڡ�
				;
	TIMx->DIER  = TIM_DIER_CC1DE;

	TIMx->CR1   = TIM_CR1_CEN;						//	ʹ�ܶ�ʱ��

	//	���ùܽţ�PA.6 ��������
	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
	MODIFY_REG( GPIOA->CRL, 0x0F000000u, 0x04000000u );
}

uint16_t	fetchSpeed( void )
{	//	ȡ DMA ���� �� �ڴ��ַָ�룬��������ǰ�������Ρ�
	//	���DMA���� �� �ڴ�ָ�붼�����ã�ȡN�εĲ�ֵ�����������ֵ����Сֵ��
	
	/*	�̶����1s��¼����ת��Ȧ������������
	 *	���μ����������˲��Ľ��������ת�١�
	 */
	DMA_Channel_TypeDef	* DMA1_Channelx = DMA1_Channel6;
	uint8_t 	ii, index;
	uint16_t	sum = 0u;
//	uint16_t	max = 0u;
//	uint16_t	min = 0xFFFFu;
	uint16_t	x0, x1, period;

	index = ( DMA1_Channelx->CMAR - ( uint32_t ) CMRA ) / sizeof( uint16_t);	//	�ڴ��ַ
	if ( ++index >= CMR_Len ){  index = 0u; }
	if ( ++index >= CMR_Len ){  index = 0u; }
	
	x1 = CMRA[index];
	for ( ii = CMR_Len - 2u; ii != 0; --ii )
	{
		//	�����������õ��ٶ�
		x0 = x1;
		if ( ++index >= CMR_Len ){  index = 0u; }
		x1 = CMRA[index];
		period = (uint16_t)( x1 - x0 );
		//	�Զ�����ݽ����˲�
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
	{	//	ÿ���ӵļ������� / ÿ������ļ���ʱ�� => ÿ���ӵ�ת��
		return	(( 60u * 100000u ) / period );
	}
}

#endif

