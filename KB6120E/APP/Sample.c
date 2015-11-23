/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Sample.C
* 创 建 者: 董峰
* 描  述  : KB-6120E 采样任务控制
* 最后修改: 2014年1月13日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"
#include <math.h>
#include <cmsis_os.h>

struct	uPumpQuery		Q_Pump[PP_Max];				/*	采样泵 状态查询数据	*/
struct	uSamplerQuery	Q_Sampler[SamplerNum_Max];	/*	采样器 状态查询数据	*/

/*	采样器 状态控制数据	*/
static	struct	
{
	BOOL	Clean;
	BOOL	Pause;
	BOOL	Fatal;
} SampleSwitch[SamplerNum_Max];

/**
	采样任务控制
*/
#include "Sample_TSP.C"
#include "Sample_R24.C"
#include "Sample_SHI.C"
#include "Sample_AIR.C"


/********************************** 功能说明 ***********************************
*	套接调用采样定时程序
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
	
	(HandlerList[SamplerID])();	//	调用采样控制程序，然后结束任务

	osThreadTerminate( osThreadGetId());
}

static	void	TaskCreate_Sampler( enum enumSamplerSelect SamplerSelect ) 
{
	static	osThreadDef( funSocket, osPriorityAboveNormal, SamplerNum_Max, 0 );

	osThreadCreate( osThread( funSocket ), (void * )SamplerSelect );
}

/********************************** 功能说明 ***********************************
*  采样任务的启动、暂停/恢复、停止控制，以及电机故障控制
*******************************************************************************/
void Sampler_R( enum enumSamplerSelect	SamplerSelect )
{
	static struct uMenu menu_TSP[] =
	{
		{ 0x0102u, " 恢复粉尘采样?" },
		{ 0x0402u, "是" }, { 0x0409u, "否" },
 	};
	static	struct uMenu menu_R24[] =
	{
		{ 0x0102u, " 恢复日均采样?" },
		{ 0x0402u, "是" }, { 0x0409u, "否" },
 	};
	static	struct uMenu menu_SHI[] =
	{
		{ 0x0102u, " 恢复时均采样?" },
		{ 0x0402u, "是" }, { 0x0409u, "否" },
 	};
	static	struct uMenu menu_AIR[] =
	{
		{ 0x0102u, " 恢复大气采样?" },
		{ 0x0402u, "是" }, { 0x0409u, "否" },
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
 
//	采样恢复：上电时，检查有无未完成的采样任务，如果有则恢复采样。
void	Sampler_BootResume( void )
{  
	//	应根据仪器型号来确定有哪些采样需要开启
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

//	启动键处理：用户启动采样
void	Sampler_Start( enum enumSamplerSelect SamplerSelect )
{
	if ( ! Sampler_isRunning( SamplerSelect ))
	{
		if (( 0u != SampleSet[SamplerSelect].sample_1 ) && ( 0u != SampleSet[SamplerSelect].set_loops ))
		{			
			SampleSwitch[SamplerSelect].Clean = FALSE;
			SampleSwitch[SamplerSelect].Pause = FALSE;
			SampleSwitch[SamplerSelect].Fatal = FALSE;
			
			SampleSet[SamplerSelect].iloop = 0u;				//	采样未初始化标志
			SampleSet[SamplerSelect].start = get_Now() / 60u;	//	记录采样启动时间
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
//	暂停键处理：用户暂停 或者 恢复 采样任务
void Sampler_PauseKey( enum enumSamplerSelect Select )
{
	if ( SampleSwitch[Select].Pause || SampleSwitch[Select].Fatal ) 
	{	//	清除采样故障，尝试恢复采样
		SampleSwitch[Select].Pause = FALSE;
		SampleSwitch[Select].Fatal = FALSE;
	}	
	else
	{
		SampleSwitch[Select].Pause = TRUE;
	}
}


//	停止键处理：用户提前中止采样任务
void	Sampler_Terminate( enum enumSamplerSelect Select )
{	
	SampleSwitch[Select].Clean = TRUE;
}

//	故障处理：通知任务任务有致命故障发生，采样任务应提前中止，不应继续执行。
void	Sampler_Fatal( enum enumSamplerSelect Select )
{

}

//	故障处理：通知任务任务有致命故障发生，采样任务应暂停，用户可选择恢复。
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

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
