/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Sample.C
* �� �� ��: ����
* ��  ��  : KB-6120E �����������
* ����޸�: 2014��1��13��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include <math.h>
#include <cmsis_os.h>

struct	uPumpQuery		Q_Pump[PP_Max];				/*	������ ״̬��ѯ����	*/
struct	uSamplerQuery	Q_Sampler[SamplerNum_Max];	/*	������ ״̬��ѯ����	*/

/*	������ ״̬��������	*/
static	struct	
{
	BOOL	Clean;
	BOOL	Pause;
	BOOL	Fatal;
} SampleSwitch[SamplerNum_Max];

/**
	�����������
*/
#include "Sample_TSP.C"
#include "Sample_R24.C"
#include "Sample_SHI.C"
#include "Sample_AIR.C"


/********************************** ����˵�� ***********************************
*	�׽ӵ��ò�����ʱ����
*******************************************************************************/
static	__task	void	funSocket( void const * p_arg )
{
	//	typedef	void ( * FunHandler )( void );
	//	static	const	FunHandler	HandlerList[SamplerNum_Max] =
	static	void ( * const HandlerList[SamplerNum_Max])( void ) =
	{
		_task_Sample_TSP,
		_task_Sample_R24,
		_task_Sample_SHI,
		_task_Sample_AIR,
	};

	uint32_t SamplerID = (uint32_t)p_arg;
	
	(HandlerList[SamplerID])();	//	���ò������Ƴ���Ȼ���������

	osThreadTerminate( osThreadGetId());
}

static	void	TaskCreate_Sampler( enum enumSamplerSelect SamplerSelect ) 
{
	static	osThreadDef( funSocket, osPriorityAboveNormal, SamplerNum_Max, 0 );

	osThreadCreate( osThread( funSocket ), (void * )SamplerSelect );
}

/********************************** ����˵�� ***********************************
*  �����������������ͣ/�ָ���ֹͣ���ƣ��Լ�������Ͽ���
*******************************************************************************/
void Sampler_R( enum enumSamplerSelect	SamplerSelect )
{
	static struct uMenu menu_TSP[] =
	{
		{ 0x0102u, " �ָ��۳�����?" },
		{ 0x0402u, "��" }, { 0x0409u, "��" },
 	};
	static	struct uMenu menu_R24[] =
	{
		{ 0x0102u, " �ָ��վ�����?" },
		{ 0x0402u, "��" }, { 0x0409u, "��" },
 	};
	static	struct uMenu menu_SHI[] =
	{
		{ 0x0102u, " �ָ�ʱ������?" },
		{ 0x0402u, "��" }, { 0x0409u, "��" },
 	};
	static	struct uMenu menu_AIR[] =
	{
		{ 0x0102u, " �ָ���������?" },
		{ 0x0402u, "��" }, { 0x0409u, "��" },
 	};
	static	struct	uMenu	* menux[SamplerNum_Max] =
	{
		menu_TSP,
		menu_R24,
		menu_SHI,
		menu_AIR,
	};
	uint8_t item = 1u; 
	cls();
	TaskCreate_Sampler( SamplerSelect );
	Menu_Redraw( menux[SamplerSelect] );	
	do {
		item = Menu_Select3( menux[SamplerSelect], item, 10 );		
		switch ( item )
		{
		case 1:			
			item = enumSelectESC; 
			break;
		default:
			SampleSwitch[SamplerSelect].Clean = TRUE;
			item = enumSelectESC;
			break;
		}
	} while( enumSelectESC != item );
	
}
 
//	�����ָ����ϵ�ʱ���������δ��ɵĲ��������������ָ�������
void	Sampler_BootResume( void )
{  
	//	Ӧ���������ͺ���ȷ������Щ������Ҫ����
  uint32_t Rsec = get_Now();
			
	if( Configure.PumpType[PP_TSP] ==  enumOrifice_1 )
		if ( SampleSet[Q_TSP].start != 0 )
		   Sampler_R( Q_TSP );
	if( ( Configure.PumpType[PP_R24_A] ==  enumOrifice_1 )
	 && ( Configure.PumpType[PP_R24_B] ==  enumOrifice_1 ) )	
		if ( SampleSet[Q_R24].start != 0 )
			Sampler_R( Q_R24 );
	if( ( Configure.PumpType[PP_SHI_C] ==  enumOrifice_1 )
	 && ( Configure.PumpType[PP_SHI_D] ==  enumOrifice_1 ) )
		if ( SampleSet[Q_SHI].start != 0 )
			Sampler_R( Q_SHI );
	if( Configure.PumpType[PP_AIR] ==  enumOrifice_1 )	
		if ( SampleSet[Q_AIR].start != 0 )
			Sampler_R( Q_AIR );
}

//	�����������û���������
void	Sampler_Start( enum enumSamplerSelect SamplerSelect )
{
	if ( ! Sampler_isRunning( SamplerSelect ))
	{
		if (( 0u != SampleSet[SamplerSelect].sample_1 ) && ( 0u != SampleSet[SamplerSelect].set_loops ))
		{			
			SampleSwitch[SamplerSelect].Clean = FALSE;
			SampleSwitch[SamplerSelect].Pause = FALSE;
			SampleSwitch[SamplerSelect].Fatal = FALSE;
			
			SampleSet[SamplerSelect].iloop = 0u;				//	����δ��ʼ����־
			SampleSet[SamplerSelect].start = get_Now() / 60u;	//	��¼��������ʱ��
			SampleSetSave();

			TaskCreate_Sampler( SamplerSelect );
		}
	}
}
BOOL StateRead( enum enumSamplerSelect Select )
{
	if ( SampleSwitch[Select].Pause || SampleSwitch[Select].Fatal ) 
		return TRUE;
	else
		return FALSE;
}
//	��ͣ�������û���ͣ ���� �ָ� ��������
void Sampler_PauseKey( enum enumSamplerSelect Select )
{
	if ( SampleSwitch[Select].Pause || SampleSwitch[Select].Fatal ) 
	{	//	����������ϣ����Իָ�����
		SampleSwitch[Select].Pause = FALSE;
		SampleSwitch[Select].Fatal = FALSE;
	}	
	else
	{
		SampleSwitch[Select].Pause = TRUE;
	}
}


//	ֹͣ�������û���ǰ��ֹ��������
void	Sampler_Terminate( enum enumSamplerSelect Select )
{	
	SampleSwitch[Select].Clean = TRUE;
}

//	���ϴ���֪ͨ�����������������Ϸ�������������Ӧ��ǰ��ֹ����Ӧ����ִ�С�
void	Sampler_Fatal( enum enumSamplerSelect Select )
{

}

//	���ϴ���֪ͨ�����������������Ϸ�������������Ӧ��ͣ���û���ѡ��ָ���
void	Sampler_Error( enum enumSamplerSelect Select )
{

}


BOOL	Sampler_isRunning( enum enumSamplerSelect SamplerSelect )
{
	switch ( SamplerSelect )
	{
	case Q_TSP:
	case Q_R24:
	case Q_SHI:
	case Q_AIR:
		return	( 0u != SampleSet[SamplerSelect].start ) ? TRUE : FALSE;
	default:
	case Q_ALL:
		if ( 0u != SampleSet[Q_TSP].start ){ return TRUE; }
		if ( 0u != SampleSet[Q_R24].start ){ return TRUE; }
		if ( 0u != SampleSet[Q_SHI].start ){ return TRUE; }
		if ( 0u != SampleSet[Q_AIR].start ){ return TRUE; }
		return	FALSE;
	}
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
