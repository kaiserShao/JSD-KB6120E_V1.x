/**************** (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Sensor.C
* 创 建 者: 董峰
* 描  述  : KB-6120E 读取 传感器 结论
* 最后修改: 2015年6月1日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"


//	传感器原始（ADC）读数 => 传感器归一化量值 => 标定数据（零点、倍率）=> 传感器读数

//	（孔板）差压（代入计前温度、计前压力、大气压）得到（流量计处的）标况流量。
//	（质量）流量 直接换算出标况流量
//	根据泵类型，确定使用哪一个标况流量读数，得到原始的标况流量读数。
//	代入根据实验确定的归一化倍率，得到归一化的标况流量。
//  代入标定倍率，得到校准后的归一化的标况流量。

/**
// 2013-11-27   修改工况流量计算公式
BOOL IsCalibrate = FALSE;
FP32	CalcFactFlow( FP32 fstd, FP32 Tr, FP32 Pr, FP32 Ba ){
	FP32 flow;
	if(IsCalibrate)	SetPressureOfSaturated(Tr); else PressureOfSaturated = 0.0f;
	flow = ( fstd * Pr0 * ( Tr0 + Tr )) / (( Tr0 +  AdTr ) * ( Ba - PressureOfSaturated  ));
	Pr = Pr;
	return	flow;
}
 */
 
/********************************** 功能说明 ***********************************
*	大气压力
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
{	//	使用 CPS120的温度作为环境温度
	uint16_t	Value  = SensorLocal.CPS120_Temp;
	FP32		Temp   = _CV_CPS120_Temp( Value );
	FP32		slope  = CalibrateLocal.slope_Te * 0.001f;
	FP32		origin = CalibrateLocal.origin_Te * 0.01f;

	return	(( Temp + 273.15f ) - origin ) * slope;
}
#endif

#ifdef	_use_NTC10K_Temp	
FP32	_get_NTC10K_Temp( void )
{	//	使用传感器板上单独的温度传感器（NTC10K）测量环境温度
	uint16_t	Value = SensorRemote.te;
	FP32		Temp = _CV_NTC10K( Value );
	FP32		slope  = CalibrateRemote.slope_Te * 0.001f;
	FP32		origin = CalibrateRemote.origin_Te * 0.01f;
	
	return	(( Temp + 273.15f ) - origin ) * slope;
}
#endif

FP32	_get_DS18B20_Temp( void )
{	//	使用传感器板上单独的温度传感器（ DS18B20）测量环境温度
	uint16_t	Value = SensorRemote.Te;
	FP32		Temp = _CV_DS18B20_Temp( Value );
	FP32		slope  = CalibrateRemote.slope_Te * 0.001f;
	FP32		origin = CalibrateRemote.origin_Te * 0.01f;

	return	(( Temp + 273.15f ) - origin ) * slope;
}

/********************************** 功能说明 ***********************************
*	电池电压、电流
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


/********************************** 功能说明 ***********************************
*	计前压力、孔板差压
*******************************************************************************/
FP32	get_Pr( enum enumPumpSelect PumpSelect )
{	//	计前压力
	int32_t 	Value  = SensorRemote.pr[PumpSelect];
	int32_t 	origin = CalibrateRemote.origin[esid_pr][PumpSelect];
	FP32		slope  = CalibrateRemote.slope[esid_pr][PumpSelect] * 0.001f;

	return	_CV_005D( origin - Value ) * slope;
}

FP32	get_pf( enum enumPumpSelect PumpSelect )
{	//	孔板差压
	int32_t 	Value  = SensorRemote.pf[PumpSelect];
	int32_t 	origin = CalibrateRemote.origin[esid_pf][PumpSelect];
	FP32		slope  = CalibrateRemote.slope[esid_pf][PumpSelect] * 0.001f;

	return	_CV_10WD( Value - origin ) * slope;
}

/********************************** 功能说明 ***********************************
*	读取传感器结论
************** *****************************************************************/
FP32	get_Ba( void )		//	大气压力（环境压力）
{
	switch ( Configure.Mothed_Ba )
	{
	default:
	case enumUserInput:		return	Configure.set_Ba * 0.01f;
	case enumMeasureBa:		return	_get_CPS120_Ba();
	}
}

/********************************** 功能说明 ***********************************
*	温度（根据配置情况，决定使用传感器板的某个温度）
*******************************************************************************/
FP32	get_Te( void )
{	//	环境温度
	return	_get_DS18B20_Temp();	//	使用传感器板上单独的温度传感器（DS18B20)测量环境温度
}

FP32	get_Tr( enum enumPumpSelect PumpSelect )
{	//	计前温度
	uint16_t	Value = SensorRemote.tr[PumpSelect];
	FP32		Temp = _CV_DS18B20_Temp( Value );
	FP32		slope  = CalibrateRemote.slope[esid_tr][PumpSelect] * 0.001f;
	FP32		origin = CalibrateRemote.origin[esid_tr][PumpSelect] * 0.01f;

	return	(( Temp + 273.15f ) - origin ) * slope;
}

/********************************** 功能说明 ***********************************
*  读取归一化之后的（未校准的）的流量
*******************************************************************************/

static	FP32	fetch_flow( enum enumPumpSelect PumpSelect )
{
	FP32	f_org;
	FP32	Ba, Tr, Pr;
	FP32	pf;
	//	读取传感器（未归一化、未校准的）流量
	switch( Configure.PumpType[PumpSelect] )
	{
	default:
	case enumPumpNone:	//	没有流量计 或 未安装
		return	0.0f;

	case enumOrifice_1:	//	1L孔板流量计
	case enumOrifice_2:	//	2L孔板流量计
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

	//	实验确定归一化倍率
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

/********************************** 功能说明 ***********************************
*  读取校正后的（标况）流量
*******************************************************************************/

FP32	get_fstd( enum enumPumpSelect PumpSelect )
{
	uint16_t const	* pSetSlope	= CalibrateRemote.slope_flow[PumpSelect];
	extern	FP32     const  PumpPoints[4][4];
			FP32     const	* pSetPoint;

	//	根据泵类型，读取归一化之后的（未校准的）流量
	FP32	f_reg	= fetch_flow( PumpSelect );

	//	根据标定参数进行数据修正
	switch ( PumpSelect )
	{
	case PP_TSP  :
	case PP_R24_A:
	case PP_R24_B:
		return	f_reg * ( pSetSlope[0] * 0.001f );						//	单点校正

	case PP_SHI_C:
	case PP_SHI_D:
		pSetPoint = PumpPoints[Configure.PumpType[PumpSelect]];			//	泵类型应使用不同分段
		return	CorrectMulitPoint( f_reg, pSetSlope, pSetPoint, 4u );	//	多点校正

	default:
		return	f_reg;
	}
}

/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File ****/
