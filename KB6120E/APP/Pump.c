/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Pump.C
* �� �� ��: KB-6120E
* ��  ��  : �����ÿ��ؿ���
* ����޸�: 2014��3��17��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include <math.h>
#include <cmsis_os.h>

/********************************** ����˵�� ***********************************
*	�����õ�����������
*******************************************************************************/
static	FP32	MotorPID_fetchRunValue( enum enumPumpSelect PumpSelect )
{
	switch ( PumpSelect )
	{
	case PP_TSP:
		{
			FP32	fstd = get_fstd( PumpSelect );	
			FP32	Te   = get_Te();
			FP32	Ba   = get_Ba();
			FP32	flow = Calc_flow( fstd, Te, 0.0f, Ba );

			return	flow;
		}

	default:
		{
			FP32	fstd = get_fstd( PumpSelect );
			return	fstd;
		}

	}
}

/********************************** ����˵�� ***********************************
*	�����õ�����������
*******************************************************************************/
static	enum	enumMotorOp_State
{	//	�����ò���״̬
	eShut, eRunning, eShutReq 
} volatile  MotorOp_State[PP_Max];

struct uMotorPID_Set
{	//	�����������
	FP32	SetGain;
	FP32	Kp, Ki, Kd;
	//	hardware access interface.
	//	fetch_set, outCmd, setOutput
};
// struct  uMotorPID_Set const MotorPID_Set[PP_Max] =
// {	// SetGain   Kp       Ki      Kd 
// 	{ 0.100f, 0.0025f, 0.00015f, 0.0015f },	//	MT_TSP
// 	{ 0.001f, 0.0200f, 0.025f, 0.0500f },	//	MT_R24_A
// 	{ 0.001f, 0.0200f, 0.025f, 0.0500f },	//	MT_R24_B
// 	{ 0.100f, 0.0140f, 0.0150f, 0.0430f },	//	MT_SHI_C
// 	{ 0.100f, 0.0140f, 0.0150f, 0.0430f },	//	MT_SHI_D
// };// 	{ 0.100f, 0.0200f, 0.025f, 0.0500f },//	 
static	FP32	MotorOutValue[PP_Max];

struct  uMotorPID_Set const MotorPID_Set[PP_Max] =
{	// SetGain   Kp       Ki      Kd 
	{ 0.100f, 0.00110f, 0.000105f, 0.00009f },//MT_TSP
	{ 0.001f, 0.0190f, 0.025f, 0.0100f },	//	MT_R24_A 
	{ 0.001f, 0.0190f, 0.025f, 0.0100f },	//	MT_R24_B 
	{ 0.100f, 0.0140f, 0.0150f, 0.0430f },	//	MT_SHI_C
	{ 0.100f, 0.0140f, 0.0150f, 0.0430f },	//	MT_SHI_D 
};

static	void	MotorPID_Task( enum enumPumpSelect PumpSelect )
{
	FP32	SetValue, RunValue, OutValue = 0.0f;
	FP32	Ek_1, Ek = 0.0f;
	FP32	Up = 0.0f, Ui = 0.0f, Ud = 0.0f;
	FP32	Upid = 0.0f;
	FP32  Initout = 0.35f;
	FP32	UpidSetGain = 0.800f;
	uint32_t  SaveTick;
	SetValue = ( Configure.SetFlow[PumpSelect] * MotorPID_Set[PumpSelect].SetGain );
	switch( PumpSelect )
	{
	case PP_TSP:
		UpidSetGain = 0.900f;
		break;
	default:
	case PP_R24_A:
	case PP_R24_B:
	case PP_SHI_C:
	case PP_SHI_D:	
		UpidSetGain = 0.800f;
		break;
	}	
	MotorOutValue[PumpSelect] = 0.0f;
	Motor_SetOutput( PumpSelect, 0u );
	Motor_OutCmd( PumpSelect, TRUE );
	//	��ʼ����ʱ
	delay( 100u );	SaveTick = osKernelSysTick( );

	while ( eRunning == MotorOp_State[PumpSelect] )
	{
		
		RunValue  = MotorPID_fetchRunValue( PumpSelect );
		//	PID����
		Ek_1 = Ek;  Ek = ( SetValue - RunValue );
		Up   = MotorPID_Set[PumpSelect].Kp * Ek;
		Ui  += MotorPID_Set[PumpSelect].Ki * Ek;
		if ( Ui < -1.0f ){  Ui = -1.0f; }
		if ( Ui > +1.0f ){  Ui = +1.0f; }
		Ud   = ( Ud * 0.667f ) + ( MotorPID_Set[PumpSelect].Kd * ( Ek - Ek_1 ) * ( 1.0f - 0.667f ));
		Upid =  ( Upid * UpidSetGain ) +( Up + Ui + Ud ) * ( 1.0f - UpidSetGain ); 
		OutValue = Upid + Initout;

		//	PID���
		if ( OutValue < 0.0f ){  OutValue = 0.0f; }
		if ( OutValue > 1.0f ){  OutValue = 1.0f; }
		MotorOutValue[PumpSelect] = OutValue;
		Motor_SetOutput( PumpSelect, 27648u * OutValue );

		//	�������ʱ
		{
			const	uint32_t	oneTick = osKernelSysTickMicroSec( 1000u );
			uint32_t	now = osKernelSysTick();
			uint32_t	lostTick, delayCount;
			//	�����Ѿ���ȥ��tick��ʱ��λ
			lostTick = (uint32_t)( now + oneTick - 1u - SaveTick ) / oneTick;
			//	����������Ҫ����ʱ����(�趨���ʱ�䣺300ms)
			if ( lostTick < 300u )
			{
				delayCount = 300u - lostTick;
			}
			else
			{	//	�Ѿ���ȥ��ʱ�䳬���趨�ļ�������¼���ʱ������������ʱ��
				SaveTick = now;
				delayCount = 30u;
			}
			delay( delayCount );
			SaveTick += delayCount * oneTick;
		}
	}
	//	PID ֹͣ
	MotorOutValue[PumpSelect] = 0.0f;
	Motor_SetOutput( PumpSelect, 0u );
	Motor_OutCmd( PumpSelect, FALSE );

	MotorOp_State[PumpSelect] = eShut;
}

/********************************** ����˵�� ***********************************
*	�׽ӵ��õ�����Ƴ���
*******************************************************************************/
static	void	MotorTask_Socket( void const * p_arg )
{	//	���ò������Ƴ���Ȼ���������
	uint32_t	arg = (uint32_t)p_arg;

	MotorPID_Task((enum enumPumpSelect)arg );

	osThreadTerminate( osThreadGetId());
}

osThreadDef( MotorTask_Socket, osPriorityHigh, ( PP_Max - 1 ), 0 );

static	void	MotorTask_Init( enum enumPumpSelect PumpSelect )
{
	osThreadCreate( osThread( MotorTask_Socket ), ( void * )PumpSelect );
}

/********************************** ����˵�� ***********************************
*  ����������ͣ����///	����ָ���Ĳ�����
*******************************************************************************/
//	#include "BIOS.H"

void  Pump_OutCmd( enum enumPumpSelect PumpSelect, BOOL NewState )
{
	if ( PumpSelect == PP_AIR )
	{
		MotorOp_State[PumpSelect] = NewState ? eRunning : eShut;
		Motor_OutCmd( PumpSelect, NewState );
		return;
	}
	
	if ( ! NewState )
	{	//	����־����������־������ֹ��	//	osThread_SetReady;			
		MotorOp_State[PumpSelect] = eShutReq;
	}
	else if ( eRunning == MotorOp_State[PumpSelect] )
	{	//	���󿪱ã����ò�û�й�?!
		;
	}
	else
	{	//	���󿪱ã�����֪���ǹص�
		MotorOp_State[PumpSelect] = eRunning;
		MotorTask_Init( PumpSelect );	
	}
// 	if( MotorOp_State[PumpSelect] == eRunning)
// 		lightopen();
// 	else
// 		lightclose();
}

///	��ȡָ���Ĳ����õĵ������[0.0 ~ 1.0]
FP32	Pump_GetOutput( enum enumPumpSelect PumpSelect )
{
	if ( PumpSelect == PP_AIR )
	{
		return	( eShutReq == MotorOp_State[PumpSelect] ) ? 0.0f : 1.0f;
	}

	if ( eShutReq == MotorOp_State[PumpSelect] )
	{
		return	0.0f;
	}
	else
	{
		return	MotorOutValue[PumpSelect];
	}
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
