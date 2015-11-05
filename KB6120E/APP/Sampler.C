/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Sampler.C
* 创 建 者: 董峰
* 描  述  : KB-6120E 采样器型号切换、显示
* 最后修改: 2014年3月18日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"

/********************************** 功能说明 ***********************************
*  根据仪器型号，显示、切换当前的采样器
*******************************************************************************/
enum  enumSamplerSelect  SamplerSelect = Q_TSP;

void	SamplerTypeShow( uint16_t yx )
{
	switch ( SamplerSelect )
	{
	case Q_TSP: Lputs( yx, "<粉尘>" );	break;
	case Q_R24: Lputs( yx, "<日均>" );	break;
	case Q_SHI:	Lputs( yx, "<时均>" );	break;
	case Q_AIR:	Lputs( yx, "<大气>" );	break;
	default:	Lputs( yx, "<未知>" );	break;
	}
}

void	SamplerTypeSwitch( void )
{
	BOOL	hasPumpTSP =  ( enumPumpNone != Configure.PumpType[PP_TSP] );
	BOOL	hasPumpR24 = (( enumPumpNone != Configure.PumpType[PP_R24_A] ) || ( enumPumpNone != Configure.PumpType[PP_R24_B] ));
	BOOL	hasPumpSHI = (( enumPumpNone != Configure.PumpType[PP_SHI_C] ) || ( enumPumpNone != Configure.PumpType[PP_SHI_D] ));
	BOOL	hasPumpAIR =  ( enumPumpNone != Configure.PumpType[PP_AIR] );

	switch ( SamplerSelect )
	{
	case Q_TSP:	if ( hasPumpR24 ){	SamplerSelect = Q_R24;	break;	}	//lint	-fallthrough
	case Q_R24:	if ( hasPumpSHI ){	SamplerSelect = Q_SHI;	break;	}	//lint	-fallthrough
	case Q_SHI:	if ( hasPumpAIR ){	SamplerSelect = Q_AIR;	break;	}	//lint	-fallthrough
	case Q_AIR:	if ( hasPumpTSP ){	SamplerSelect = Q_TSP;	break;	}	//lint	-fallthrough
	default:	break;
	}
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
