/**************** (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司 ****************
* 文 件 名: SENSOR.H
* 创 建 者: 董峰
* 描  述  : KB-6120E 传感器部分的头文件
* 最后修改: 2015年6月1日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/

/********************************** 功能说明 ***********************************
*	传感器读数
*******************************************************************************/
#define	_sys_get_Ba		_get_CPS120_Ba
#define	_sys_get_Te		_get_DS18B20_Temp

extern	FP32	_get_CPS120_Ba( void );		//	环境压力
extern	FP32	_get_DS18B20_Temp( void );	//	环境温度

extern	FP32	get_Ba( void );		//	环境压力
extern	FP32	get_Te( void );		//	环境温度
extern	FP32	get_Tr( enum enumPumpSelect PumpSelect );	//	计前压力
extern	FP32	get_Pr( enum enumPumpSelect PumpSelect );	//	计前温度
extern	FP32	get_pf( enum enumPumpSelect PumpSelect );	//	孔板差压

extern	FP32	get_fstd( enum enumPumpSelect PumpSelect );	//	标况流量

/********************************** 功能说明 ***********************************
*	归一化转换，传感器（ADC）读数 -> 归一化量值
*******************************************************************************/
#define	__PURE	__pure

extern	__PURE	FP32	_CV_CPS120_Ba( uint16_t InValue );
extern	__PURE	FP32	_CV_CPS120_Temp( uint16_t InValue );
extern	__PURE	FP32	_CV_DS18B20_Temp( int16_t InValue );
extern	__PURE	FP32	_CV_10WD( int32_t InValue );
extern	__PURE	FP32	_CV_005D( int32_t InValue );
//	extern	__PURE	FP32	_CV_FS4001_1L( int32_t InValue );
extern	__PURE	FP32	_CV_NTC10K( uint16_t ADx );
extern	__PURE	FP32	_CV_CPU_Temp( uint16_t ADx );
extern	__PURE	FP32	_CV_CPU_Volt( uint16_t ADx );
extern			FP32	_CV_Bat_Voltage( uint16_t AD_Value );
extern	__PURE	FP32	_CV_Bat_Current( uint16_t AD_Value );

extern	__PURE	FP32	Calc_fstd( FP32 pf, FP32 Tr, FP32 Pr, FP32 Ba );

extern	__PURE	FP32	Calc_flow( FP32 fstd, FP32 Tr, FP32 Pr, FP32 Ba );

extern	__PURE	FP32	CorrectMulitPoint( FP32 inValue, uint16_t const SlopeList[], FP32 const PointList[], uint8_t PointCount );

extern  __PURE  FP32  Calc_Pbv( FP32 T );
/********************************** 功能说明 ***********************************
*	本地传感器读数及标定数据访问
*******************************************************************************/
//	enum 
//	{
//	//	sid_Ba, sid_Te, sid_tr, sid_pr, sid_pf,
//	//	sid_Max
//	};

struct	uSensorLocal
{	//	本地传感器原始读数
	uint16_t	CPS120_Ba, CPS120_Temp;

	uint16_t	CPU_IntTemp;
	uint16_t	CPU_IntVolt;
//	uint16_t	NTC10K_Temp;
	uint16_t	LCD_Voltage;
	uint16_t	Bat_Voltage;
	uint16_t	Bat_Current;
};

struct	uCalibrateLocal
{
	uint16_t	origin_Ba;		//	本地传感器 零点
	uint16_t	slope_Ba;		//	本地传感器 倍率

	uint16_t	DataValidMask;
};

//	本地数据
extern	struct	uSensorLocal		SensorLocal;
extern	struct	uCalibrateLocal		CalibrateLocal;			

/********************************** 功能说明 ***********************************
*  远程传感器读数及标定数据访问
*******************************************************************************/
//	enum	enumPumpSelect
//	{
//		PP_TSP, PP_R24_A, PP_R24_B, PP_SHI_C, PP_SHI_D, PP_AIR,
//		PP_Max
//	};

enum
{
	esid_pf, esid_tr, esid_pr,
	esid_Max
};

struct	uSensorRemote
{
	uint16_t	iCounter;
	bool	isExist[PP_Max];
	
	uint16_t	Ba;	//	大气压力
	int16_t 	Te;	//	环境温度
	int16_t 	Tm;	//	电压温度
	int16_t 	tr[PP_Max];
	uint16_t	pr[PP_Max];
	uint16_t	pf[PP_Max];
	
	bool	has_HCBox;	//	恒温箱
	int16_t		HCBoxRunTemp;
	int16_t		HCBoxOutValue;
	uint16_t	HCBoxFanSpeed;
	
	bool	has_Heater;	//	加热器
	int16_t 	HeaterRunTemp;
	int16_t		HeaterOutValue;
};

struct	uCalibrateRemote
{	//	远程传感器标定数据
	uint16_t	origin[esid_Max][5];   //点
	uint16_t	slope [esid_Max][5];   //斜
	uint16_t	slope_flow[5][4];	//	最多四个校正点

	uint16_t	origin_Te;
	uint16_t	slope_Te;

	uint16_t	origin_Heater_Temp;
	uint16_t	slope_Heater_Temp;

	uint16_t	origin_HCBox_Temp;
	uint16_t	slope_HCBox_Temp;

	uint16_t	DataValidMask;	
};

//	远程数据
extern	struct	uSensorRemote		SensorRemote;		//	[BS_Max][BC_Max];
extern	struct	uCalibrateRemote	CalibrateRemote;	//	[BS_Max][BC_Max];

/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File ****/
