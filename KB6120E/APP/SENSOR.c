/**************** (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Sensor.C
* �� �� ��: ����
* ��  ��  : KB-6120E ��ȡ ������ ����
* ����޸�: 2015��6��1��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"


//	������ԭʼ��ADC������ => ��������һ����ֵ => �궨���ݣ���㡢���ʣ�=> ����������

//	���װ壩��ѹ�������ǰ�¶ȡ���ǰѹ��������ѹ���õ��������ƴ��ģ����������
//	������������ ֱ�ӻ�����������
//	���ݱ����ͣ�ȷ��ʹ����һ����������������õ�ԭʼ�ı������������
//	�������ʵ��ȷ���Ĺ�һ�����ʣ��õ���һ���ı��������
//  ����궨���ʣ��õ�У׼��Ĺ�һ���ı��������

/**
// 2013-11-27   �޸Ĺ����������㹫ʽ
BOOL IsCalibrate = FALSE;
FP32	CalcFactFlow( FP32 fstd, FP32 Tr, FP32 Pr, FP32 Ba ){
	FP32 flow;
	if(IsCalibrate)	SetPressureOfSaturated(Tr); else PressureOfSaturated = 0.0f;
	flow = ( fstd * Pr0 * ( Tr0 + Tr )) / (( Tr0 +  AdTr ) * ( Ba - PressureOfSaturated  ));
	Pr = Pr;
	return	flow;
}
 */
 
/********************************** ����˵�� ***********************************
*	����ѹ��
*******************************************************************************/
FP32	_get_CPS120_Ba( void )
{
	uint16_t	Value  = SensorLocal.CPS120_Ba;
	FP32		Ba     = _CV_CPS120_Ba( Value );
	FP32		slope  = CalibrateLocal.slope_Ba * 0.001f;
	FP32		origin = CalibrateLocal.origin_Ba * 0.01f;

	return	((( Ba + 80.0f ) - origin ) * slope );
}

#ifdef	_use_cps120_temp
FP32	_get_CSP120_Temp( void )
{	//	ʹ�� CPS120���¶���Ϊ�����¶�
	uint16_t	Value  = SensorLocal.CPS120_Temp;
	FP32		Temp   = _CV_CPS120_Temp( Value );
	FP32		slope  = CalibrateLocal.slope_Te * 0.001f;
	FP32		origin = CalibrateLocal.origin_Te * 0.01f;

	return	(( Temp + 273.15f ) - origin ) * slope;
}
#endif

#ifdef	_use_NTC10K_Temp	
FP32	_get_NTC10K_Temp( void )
{	//	ʹ�ô��������ϵ������¶ȴ�������NTC10K�����������¶�
	uint16_t	Value = SensorRemote.te;
	FP32		Temp = _CV_NTC10K( Value );
	FP32		slope  = CalibrateRemote.slope_Te * 0.001f;
	FP32		origin = CalibrateRemote.origin_Te * 0.01f;
	
	return	(( Temp + 273.15f ) - origin ) * slope;
}
#endif

FP32	_get_DS18B20_Temp( void )
{	//	ʹ�ô��������ϵ������¶ȴ������� DS18B20�����������¶�
	uint16_t	Value = SensorRemote.Te;
	FP32		Temp = _CV_DS18B20_Temp( Value );
	FP32		slope  = CalibrateRemote.slope_Te * 0.001f;
	FP32		origin = CalibrateRemote.origin_Te * 0.01f;

	return	(( Temp + 273.15f ) - origin ) * slope;
}

/********************************** ����˵�� ***********************************
*	��ص�ѹ������
*******************************************************************************/
FP32	get_Bat_Voltage( void )
{
	FP32	slope = Configure.slope_Bat_Voltage * 0.001f;

	return	_CV_Bat_Voltage( SensorLocal.Bat_Voltage ) * slope;
}

FP32	get_Bat_Current( void )
{
	FP32	slope = Configure.slope_Bat_Current * 0.001f;

	return	_CV_Bat_Current( SensorLocal.Bat_Current ) * slope;
}

bool	get_Bat_Charging( void )
{
	FP32	threshold = Configure.threshold_Current * 0.001f;

	return	( get_Bat_Current() > threshold );		
}


/********************************** ����˵�� ***********************************
*	��ǰѹ�����װ��ѹ
*******************************************************************************/
FP32	get_Pr( enum enumPumpSelect PumpSelect )
{	//	��ǰѹ��
	int32_t 	Value  = SensorRemote.pr[PumpSelect];
	int32_t 	origin = CalibrateRemote.origin[esid_pr][PumpSelect];
	FP32		slope  = CalibrateRemote.slope[esid_pr][PumpSelect] * 0.001f;

	return	_CV_005D( origin - Value ) * slope;
}

FP32	get_pf( enum enumPumpSelect PumpSelect )
{	//	�װ��ѹ
	int32_t 	Value  = SensorRemote.pf[PumpSelect];
	int32_t 	origin = CalibrateRemote.origin[esid_pf][PumpSelect];
	FP32		slope  = CalibrateRemote.slope[esid_pf][PumpSelect] * 0.001f;

	return	_CV_10WD( Value - origin ) * slope;
}

/********************************** ����˵�� ***********************************
*	��ȡ����������
************** *****************************************************************/
FP32	get_Ba( void )		//	����ѹ��������ѹ����
{
	switch ( Configure.Mothed_Ba )
	{
	default:
	case enumUserInput:		return	Configure.set_Ba * 0.01f;
	case enumMeasureBa:		return	_get_CPS120_Ba();
	}
}

/********************************** ����˵�� ***********************************
*	�¶ȣ������������������ʹ�ô��������ĳ���¶ȣ�
*******************************************************************************/
FP32	get_Te( void )
{	//	�����¶�
	return	_get_DS18B20_Temp();	//	ʹ�ô��������ϵ������¶ȴ�������DS18B20)���������¶�
}

FP32	get_Tr( enum enumPumpSelect PumpSelect )
{	//	��ǰ�¶�
	uint16_t	Value = SensorRemote.tr[PumpSelect];
	FP32		Temp = _CV_DS18B20_Temp( Value );
	FP32		slope  = CalibrateRemote.slope[esid_tr][PumpSelect] * 0.001f;
	FP32		origin = CalibrateRemote.origin[esid_tr][PumpSelect] * 0.01f;

	return	(( Temp + 273.15f ) - origin ) * slope;
}

/********************************** ����˵�� ***********************************
*  ��ȡ��һ��֮��ģ�δУ׼�ģ�������
*******************************************************************************/

static	FP32	fetch_flow( enum enumPumpSelect PumpSelect )
{
	FP32	f_org;
	FP32	Ba, Tr, Pr;
	FP32	pf;
	//	��ȡ��������δ��һ����δУ׼�ģ�����
	switch( Configure.PumpType[PumpSelect] )
	{
	default:
	case enumPumpNone:	//	û�������� �� δ��װ
		return	0.0f;

	case enumOrifice_1:	//	1L�װ�������
	case enumOrifice_2:	//	2L�װ�������
		{
			
			Ba = get_Ba();
			Tr = get_Tr( PumpSelect );
			Pr = get_Pr( PumpSelect );
			pf = get_pf( PumpSelect );
			f_org = Calc_fstd( pf, Tr, Pr, Ba );
		}
		break;
	}

	if ( f_org < 0.001f )
	{
		return	0.0f;
	}		

	//	ʵ��ȷ����һ������
	switch ( PumpSelect )
	{
	case PP_TSP:	return	f_org * 125.0f;
	case PP_R24_A:
	case PP_R24_B:	return	f_org * 1.0f;
	case PP_SHI_C:
	case PP_SHI_D:	return	f_org * 1.0f;
	default:
	case PP_AIR:	return	0.0f;
	}
}

/********************************** ����˵�� ***********************************
*  ��ȡУ����ģ����������
*******************************************************************************/

FP32	get_fstd( enum enumPumpSelect PumpSelect )
{
	uint16_t const	* pSetSlope	= CalibrateRemote.slope_flow[PumpSelect];
	extern	FP32     const  PumpPoints[4][4];
			FP32     const	* pSetPoint;

	//	���ݱ����ͣ���ȡ��һ��֮��ģ�δУ׼�ģ�����
	FP32	f_reg	= fetch_flow( PumpSelect );

	//	���ݱ궨����������������
	switch ( PumpSelect )
	{
	case PP_TSP  :
	case PP_R24_A:
	case PP_R24_B:
		return	f_reg * ( pSetSlope[0] * 0.001f );						//	����У��

	case PP_SHI_C:
	case PP_SHI_D:
		pSetPoint = PumpPoints[Configure.PumpType[PumpSelect]];			//	������Ӧʹ�ò�ͬ�ֶ�
		return	CorrectMulitPoint( f_reg, pSetSlope, pSetPoint, 4u );	//	���У��

	default:
		return	f_reg;
	}
}

/********  (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
