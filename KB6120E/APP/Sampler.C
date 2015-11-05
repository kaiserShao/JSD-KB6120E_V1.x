/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Sampler.C
* �� �� ��: ����
* ��  ��  : KB-6120E �������ͺ��л�����ʾ
* ����޸�: 2014��3��18��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"

/********************************** ����˵�� ***********************************
*  ���������ͺţ���ʾ���л���ǰ�Ĳ�����
*******************************************************************************/
enum  enumSamplerSelect  SamplerSelect = Q_TSP;

void	SamplerTypeShow( uint16_t yx )
{
	switch ( SamplerSelect )
	{
	case Q_TSP: Lputs( yx, "<�۳�>" );	break;
	case Q_R24: Lputs( yx, "<�վ�>" );	break;
	case Q_SHI:	Lputs( yx, "<ʱ��>" );	break;
	case Q_AIR:	Lputs( yx, "<����>" );	break;
	default:	Lputs( yx, "<δ֪>" );	break;
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

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/