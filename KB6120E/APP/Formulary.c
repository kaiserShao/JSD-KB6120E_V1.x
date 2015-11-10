/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Formulary.C
* �� �� ��: ����
* ��  ��  : KB-6120E ������ʽ����
* ����޸�: 2014��2��25��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include <math.h>
/*
	Ҫ���ļ��еĺ����Ǵ������ѧ��������ʽ������Ӧ���������йص�������Ϣ��
*/
/********************************** ����˵�� ***********************************
*	��һ��ת������������ADC������ -> ��һ����ֵ
*******************************************************************************/
__PURE	FP32	_CV_CPS120_Ba( uint16_t InValue )	
{
	return	(( InValue * ( 90.0f / 16384.0f )) + ( 30.0f ));
}

__PURE	FP32	_CV_CPS120_Temp( uint16_t InValue )
{
	return	(( InValue * ( 165.0f / 16384.0f )) - ( 40.0f ));
}

__PURE	FP32	_CV_DS18B20_Temp( int16_t InValue )
{
	return	( InValue * 0.0625f );
}

__PURE	FP32	_CV_10WD( int32_t InValue )
{
	const FP32 Gain = 0.00011920928955078125f;	//	2.5kPa / ( 32768 * 50mV * 32 / 2500 mV )
	return	Gain * InValue;
}

__PURE	FP32	_CV_005D( int32_t InValue )
{
	const FP32 Gain = 0.001621246337890625f;	//	34kPa / ( 32768 * 100mV * 16 / 2500 mV )
	return	Gain * InValue;
}

//	__PURE	FP32	_CV_FS4001_1L( int32_t InValue )
//	{
//		const FP32 Gain = 0.0000762939453125;		//	1L / ( 4096 * 4 * 80% );
//		return	Gain * InValue;
//	}

//EXCEL��ʽ	=B?/(LN(B7)+B?/(273.15+25))-273.15
__PURE	FP32	_CV_NTC10K( uint16_t ADx )
{
#define	B	3380.f
#define	K	273.15f
#define	T0	25.0f
#define	ADC_Max	4096.0f

	FP32	Tx;
	
	if ( ADx <= 100u )
	{
		Tx = -50.0f;	//	-48.2
	}
	else if ( ADx >= 4000u )
	{
		Tx = 180.0f;	//	171.2
	}
	else
	{
		FP32	RxDivR0 = (FP32)( 4096u - ADx ) / (FP32)( ADx );
		Tx = B / ( logf( RxDivR0 ) + ( B / ( K + T0 ))) - K;
	}
	
	return	Tx;
}

__PURE	FP32	_CV_CPU_Temp( uint16_t ADx )
{
	FP32	volt = ADx * ( 3.3f / 4096u );
	FP32	temp = ((( 1.43f - volt )/ 0.0043f ) + 25.0f );	//	ʽ�г����������ֲ����
	return	temp;
}

__PURE	FP32	_CV_CPU_Volt( uint16_t ADx )
{
	FP32	volt = 0.0f;

	if ( ADx )
	{
		volt = ( 1.2f * 4096u ) / ADx;
	}
	return	volt;
}

FP32	_CV_Bat_Voltage( uint16_t AD_Value )
{
	uint16_t	Ref = SensorLocal.CPU_IntVolt ;
	FP32	volt;
	const FP32	Gain = 8.0f;		//	��ѹ���� 100K:33K

	if ( Ref != 0u )
	{
		volt = AD_Value * 1.2f / Ref;
	}
	else
	{
		volt = AD_Value * ( 3.3f / 4096u );
	}
	
	return	Gain * volt;	//	���ص�ѹ����λV��
}

__PURE	FP32	_CV_Bat_Current( uint16_t AD_Value )
{
	FP32	volt = AD_Value * ( 3.3f / 4096u );
	
	//	1. ȡ������ 0.33 ������������ 0.165
	//	2. ͬ��Ŵ������Ŵ�21����
	//	3. ��ѹ���� 51K:27K
	const FP32 Gain = 1.0f / 27 * ( 51 + 27 ) / 21 / 0.165;

	return	Gain * volt;	//	���ص�������λA��
}

/*********************************** ����˵�� **********************************
*	���� ���������� ��ȡ �������
*******************************************************************************/
/************************************ ����˵�� ********************************
*	���� ������� ��ȡ ��������
*******************************************************************************/
static	const	FP32	Tr0	= 273.150f;
static	const	FP32	Pr0	= 101.325f;

static	FP32	get_Tstd( void )
{ 
	switch ( Configure.SetTstd )
	{
	default:
	case enum_273K:	return	Tr0 +  0.0f;
	case enum_293K:	return	Tr0 + 20.0f;
	case enum_298K:	return	Tr0 + 25.0f;
	}
}

/**
	�װ��������㹫ʽ
	  ���� �װ��ѹ����ǰ�¶ȡ���ǰѹ��������ѹ����
	  ���� �װ崦�ı������
	����������Ķ����¶ȸ������ò���ȷ����
*/
__PURE	FP32	Calc_fstd( FP32 pf, FP32 Tr, FP32 Pr, FP32 Ba )
{
	FP32	Tstd = get_Tstd();

	FP32	fstd;

	pf *= (( Ba + Pr ) *  Tstd ) / (( Tr0 + Tr ) * Pr0 );

	if ( pf < 0.001f )
	{
		fstd = 0.0f;
	}
	else
	{
		fstd = sqrtf( pf );
	}

	return	fstd;
}

/**
	�����������㹫ʽ
	  ���� ��֪����������������㴦���¶ȣ����������㴦�ģ����ѹ��������ѹ����
	  ���� �����������㴦�ģ���������
	����������Ķ����¶ȸ������ò���ȷ����
*/
__PURE	FP32	Calc_flow( FP32 fstd, FP32 Tx, FP32 Px, FP32 Ba, enum enumSamplerSelect SamplerSelect  )
{
	FP32	Pbv = 0.0f;	//	����ˮ��ѹ
	FP32	Tstd;

	FP32	flow;
	switch( SamplerSelect )
	{
	case Q_TSP:
	Tstd = Tr0;
	break;
	case Q_R24:
	case Q_SHI:
	case Q_AIR:
	default:
	Tstd = get_Tstd();
	break;
	}
	if ( Ba < 0.001f )
	{
		flow = fstd;
	}
	else
	{
		Pbv = 0.0f;
		if ( Configure.shouldCalcPbv )
		{
			Pbv = Calc_Pbv( Tx );
		}
		flow = (( fstd * (( Tr0 + Tx ) * Pr0 )) / ((( Ba + Px ) - Pbv ) * Tstd ));
	}

	return	flow;
}

/********************************** ����˵�� ***********************************
* �����趨���У׼�����ߣ���������������һ���������õ�һ����������������
* ���������
	inValue  ����Ҫ�����ı���
	SlopeList�����������εı���
	PointList�����������εķֵ�
	PointCount �������εķֶ���
*******************************************************************************/
__PURE	FP32	CorrectMulitPoint( FP32 inValue, uint16_t const SlopeList[], FP32 const PointList[], uint8_t PointCount )
{
	FP32	y1, y2;
	FP32	x1, x2;
	uint8_t i = 0u;

	y2 = 0.0f;
	x2 = 0.0f;

	do {
		FP32	slope = ((FP32)SlopeList[i] * 0.001f );

		y1 = y2;
		x1 = x2;

		y2 = PointList[i];
		x2 = y2 / slope;
		
	} while (( x2 < inValue ) && ( ++i < PointCount ));

	return	(( inValue - x1 ) * (( y2 - y1 ) / ( x2 - x1 )) + y1 );
}


/**
	��֪��sum[( Xi - Xmean )^2] = sum( Xi ^ 2 ) - { [sum(Xi)]^2 } / n
 */
#include <math.h>
uint16_t	RMS( const uint16_t * pSrc, uint16_t iCount )
{
	uint32_t	sumXiXi = 0;
	uint32_t	sumXi   = 0;
	uint16_t	Xi;
	uint16_t	Result;
	uint16_t	i = iCount;

	do {
		Xi = *pSrc++;

		sumXi   += Xi;

		sumXiXi += Xi * Xi;

	} while( --i );
	
	Result = rint( sqrtf( sumXiXi - (((int64_t)sumXi * sumXi) / iCount )));
	
	return	Result;
}
/* 
   ����ˮ��ѹ 
   ˮƽ�棺 Ln^ew = (10.286T-2148.4909)/(T-35.85)
    ���� :  Ln^ew = 12.5633-2670.59/T
*/
__PURE  FP32  Calc_Pbv( FP32 T )
{
// 	#define To 273.15
// 	FP32   Pbv;
// 	T += To;
// 	if( T >= To)
// 		Pbv = exp( ( 10.286 * T - 2148.4909 ) /( T - 35.85 ) );
// 	else
// 		Pbv = exp( 12.5633 - 2670.59 / T );
// 	return Pbv;
	static const FP32 C[7] =
	{
		6.107799961E-01f,
		4.436518521E-02f,
		1.428945805E-03f,
		2.650648471E-05f,
		3.031240396E-07f,
		2.034080948E-09f,
		6.136820929E-12f
	};
	return ((((((((((((C[6] * T) + C[5]) * T) + C[4]) * T) + C[3]) * T) + C[2]) * T) + C[1]) * T) + C[0]);
}
/**/

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
