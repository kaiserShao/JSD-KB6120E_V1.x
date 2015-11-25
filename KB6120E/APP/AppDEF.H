/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: AppDEF.H
* 创 建 者: 董峰
* 描  述  : KB-6120E 的应用程序头文件
* 最后修改: 2014年4月21日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "EDIT.H"

enum	enumTypeIdent
{
	type_KB6120A,
	type_KB6120AD,
	type_KB2400HL,
	type_KB6120AD2,
	type_Max,
};
enum enumExNameIdent
{
  Name_None,
  Name_JSD,
//  ...
  Name_Max,
};
extern CHAR  const * const szTypeIdent[];
extern CHAR  const * const szNameIdent[];
// #define	SysPassword1	633817UL
// #define	SysPassword2	633867UL

#define	SysPassword1a	000001UL
#define	SysPassword2a	000001UL


enum	enumSampleState
{
	state_ERROR, state_FINISH, state_SAMPLE, state_SUSPEND, state_PAUSE
};

/********************************** 功能说明 ***********************************
* 采样泵启停、调速接口（控制PID任务的运行/停止，获取PID输出值[0.0~1.0]）
*******************************************************************************/
enum	enumPumpSelect
{
	PP_TSP, PP_R24_A, PP_R24_B, PP_SHI_C, PP_SHI_D, PP_AIR,
	PP_Max
};
extern	void	Pump_OutCmd   ( enum enumPumpSelect PumpSelect, BOOL NewState );
extern	FP32	Pump_GetOutput( enum enumPumpSelect PumpSelect );

/********************************** 功能说明 ***********************************
*  电机调速控制（控制电机启动/停止，设置电机输出电压[0~27648]）
*******************************************************************************/
extern	void	Motor_OutCmd   ( enum enumPumpSelect PumpSelect, BOOL NewState );
extern	void	Motor_SetOutput( enum enumPumpSelect PumpSelect, uint16_t OutVale );

/********************************** 功能说明 ***********************************
*  采样任务的控制接口
*******************************************************************************/
enum enumSamplerSelect
{
	Q_TSP, Q_R24, Q_SHI, Q_AIR,
	SamplerNum_Max,
	Q_ALL = SamplerNum_Max,
};

enum enumCHSelect
{	// 日均、时均采样有两路
	Q_PP1, Q_PP2, Sampler2C_Max
};

extern	enum enumSamplerSelect	SamplerSelect;

extern	void	Sampler_BootResume( void );	//	开机（上电）恢复采样任务
extern	void	Sampler_Start    ( enum enumSamplerSelect SamplerSelect );
extern	void	Sampler_PauseKey ( enum enumSamplerSelect SamplerSelect );
extern	void	Sampler_Terminate( enum enumSamplerSelect SamplerSelect );
extern	BOOL	Sampler_isRunning( enum enumSamplerSelect SamplerSelect );
extern  BOOL StateRead( enum enumSamplerSelect Select );
/********************************** 功能说明 ***********************************
*  系统配置
*******************************************************************************/
enum
{	//	大气压力获取方式
	enumUserInput,		//	用户输入
	enumMeasureBa,		//	实时测量
};

enum
{	//	运行时间控制方式
	enumBySet,			//	按设置时间运行，遇停电自动扣除
	enumBySum,			//	按累计时间运行，遇停电自动补足
};

enum
{	//	启动时间延时方式
	enumByAccurate,		//	定时开机
	enumByDelay,		//	延时开机
};

enum
{	//	流量传感器的类型
	enumPumpNone, 		//	未安装
	enumOrifice_1,		//	粉尘中流量孔板（泵） 或 1L的小流量孔板（泵）
	enumOrifice_2,		//	粉尘大流量孔板（泵） 或 2L的小流量孔板（泵）
//	enumMassflow,		//	粉尘质量流量计 或 1L的小流量 或 2L的小流量
};

enum
{	//	恒温箱、加热器类型
	enumHeaterNone,		//	未安装恒温箱
	enumHCBoxOnly,		//	只有恒温箱
	enumHeaterOnly,		//	只有加热器
	enumHCBoxHeater,	//	恒温箱、加热器 两者都有
};

enum
{	//	恒温箱控制温度的方式
	MD_Shut,
	MD_Heat,
	MD_Cool,
	MD_Auto
};

enum	enumTstd
{	//	标况流量的定义温度
	enum_273K,	//	标况流量定义为：标况大气压力下，温度273K（ 0℃）时的流量
	enum_293K,	//	标况流量定义为：标况大气压力下，温度293K（20℃）时的流量
	enum_298K,	//	标况流量定义为：标况大气压力下，温度298K（25℃）时的流量
};

struct	uConfigure
{
	uint8_t		InstrumentType;			//	显示的仪器型号
	uint8_t		InstrumentName;			//	显示的仪器名称

	uint8_t		Mothed_Delay;			//	采样开始时间方式
	uint8_t		Mothed_Sample;			//	采样时间控制方式

	uint16_t	DisplayGray;		//	显示灰度设置
	uint8_t		DisplayLight;		//	显示亮度设置
	uint8_t		TimeoutLight;		//	背光延时时间：0＝关闭，255＝常亮

	uint8_t		HeaterType;				//	恒温箱/加热器的选配情况
	uint8_t		HCBox_SetMode;			//	恒温箱控制模式
	uint16_t	HCBox_SetTemp;			//	恒温箱恒温温度
	uint16_t	Heater_SetTemp;			//	加热器恒温温度
	uint8_t		Heater_SW;          //	加热器开关

	uint8_t		SetTstd;				//	标况流量的定义温度 
	uint8_t		Mothed_Ba;				//	大气压力获取方式
	uint16_t	set_Ba;					//	保存用户输入气压

	uint16_t	SetFlow[PP_Max];		//	采样流量
	uint16_t	AIRSetFlow[Sampler2C_Max];	//大气泵采样流量
	uint8_t		PumpType[PP_Max];		//	时均采样的流量计、泵的型号，影响采样流量的设置范围，标定点的选择。

	uint16_t	TSP_Pr_Portect;			//	电机保护：计前压力设置
	
	uint16_t	slope_Bat_Voltage;
	uint16_t	slope_Bat_Current;
	uint16_t	threshold_Current;
	
	uint8_t   Battery_SW;
	uint8_t		shouldCalcPbv;		//	饱和水汽压是否参与计算。
  

	uint16_t	DataValidMask;	
	uint32_t  ExNum;						//	出厂编号
  uint8_t   ExName;						//	厂家名称
	uint32_t	Password;					//	维护密码
};

extern	struct	uConfigure	Configure;					//	系统配置

/********************************** 功能说明 ***********************************
*  采样设置
*******************************************************************************/
struct	uSampleSet
{
	uint16_t	delay1;			//	延时（或定时）开机时间
	uint16_t	sample_1;		//	采样时间
	uint16_t	suspend_1;		//	间隔时间
	uint16_t	sample_2;		//	（第二路泵的）采样时间
	uint16_t	suspend_2;		//	（第二路泵的）间隔时间
	uint16_t	set_loops;		//	采样循环次数

	uint16_t	FileNum;		//	文件编号
	uint32_t	start;			//	起始时刻
	uint16_t	iloop;			//	循环次数

	uint16_t	DataValidMask;
};

extern	struct	uSampleSet	SampleSet[SamplerNum_Max];	//	采样设置


/*	采样泵 状态查询数据	*/
struct	uPumpQuery
{
	BOOL	xp_state;	//	开、关状态
	FP32	 vd, vnd;
	uint16_t sum_time;
};

extern	struct	uPumpQuery	Q_Pump[PP_Max];

/*	采样器 状态查询数据	*/
struct	uSamplerQuery
{
	enum enumSampleState state;
	uint16_t	timer;
	uint8_t     loops;
};

extern	struct	uSamplerQuery	Q_Sampler[SamplerNum_Max];

/********************************** 功能说明 ***********************************
*	菜单项处理
*******************************************************************************/
extern	void	CalibrateZero_TSP( void );
extern	void	CalibrateZero_R24( void );
extern	void	CalibrateZero_SHI( void );

extern	void	HCBox_Init( void );
extern	void	SENSOR_Remote_Init( void );
extern	void	SENSOR_Local_Init ( void );

extern	void	ShowEdition( void );
extern	void	ShowEdition_Inner( void );
extern	void	Configure_ShowNameIdent( void );

extern	void	SamplerTypeShow( uint16_t yx );
extern	void	SamplerTypeSwitch( void );

extern	void	menu_QueryLog( void );
extern	void	menu_FactoryDebug( void );
extern	void	menu_Calibrate( void );

//	RTOS.C
extern	void	bus_modbus_mutex_apply( void );
extern	void	bus_modbus_mutex_release( void );

extern	void	bus_i2c_mutex_apply( void );
extern	void	bus_i2c_mutex_release( void );

///////////////////////////////////////////////////////////////////
//	Query.C
extern	void	Query_File( enum enumSamplerSelect SamplerSelect );
extern	void	PowerLog_Query( void );
extern	void	File_Clean( void );

extern	void	menu_SampleSetup( void );
extern	void	menu_SampleStart( void );
extern	void	menu_SampleQuery( void );
extern	void	menu_Maintenance( void );
extern	void	monitor( void );

//	HCBox.C
extern	void		set_HCBoxTemp( FP32 TempSet, uint8_t ModeSet );
extern	FP32		get_HCBoxTemp( void );
extern	FP32		get_HCBoxOutput( void );
extern	uint16_t	get_HCBoxFanSpeed( void );

extern	void		set_HeaterTemp( FP32 SetTemp );
extern	FP32		get_HeaterTemp( void );
extern	FP32		get_HeaterOutput( void );

extern	void	PWM2_SetOutput( uint16_t );

extern	BOOL	EditI32U( uint16_t yx, uint32_t * pNUM, uint16_t fmt );

extern	uint32_t	eDataValidMask;
extern	void	EditionSelsct( void );
extern	void	show_std_clock( void );

//	SampleSet.c
extern	BOOL SampleFinishFState[SamplerNum_Max];
#include "SENSOR.H"
#include "EEPROM.H"

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
