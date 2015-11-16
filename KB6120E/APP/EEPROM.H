/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: EEPROM.H
* 创 建 者: 董峰
* 描  述  : KB-612E 外存数据访问，在此文件中声明外存的访问接口。
* 最后修改: 2014年4月21日
*********************************** 修订记录 ***********************************
* 版  本: V2.0
* 修订人: 董峰
* 说  明: 重载访问文件的接口，所有文件结构使用同一函数名访问。
*********************************** 设计说明 ***********************************
文件编号0..Max，共Max+1个文件，但当前文件正在进行记录，实际可查询的文件有Max个。
采样开始时间为零，表示是空文件（注：空文件也是完整文件）
*******************************************************************************/
#include <time.h>

#define	FileNum_Max			10000u
#define	PowerLogIndex_Max	10000u

////////////////////////////////////////////////////////////////////////////////
//	标、配、设置 存取
////////////////////////////////////////////////////////////////////////////////
extern	void	ConfigureSave( void );
extern	void	ConfigureLoad( void );
extern	void	ConfigureLoadDefault( void );
extern	void	CalibrateSave( void );	
extern	void	CalibrateLoad( void );
extern	void	SampleSetSave( void );
extern	void	SampleSetLoad( void );
extern	void	KB6102_CalibrateSave( void );	//	enum enumBoardSelect Board );
extern	void	KB6102_CalibrateLoad( void );	//	enum enumBoardSelect Board );


//	//	2014年2月22日	使用函数名重载
//	void	File_Load( uint16_t FileNum, struct uFile_TSP * pFile );
//	void	File_Load( uint16_t FileNum, struct uFile_AIR * pFile );
//	void	File_Load( uint16_t FileNum, struct uFile_R24 * pFile );
//	void	File_Load( uint16_t FileNum, struct uFile_SHI * pFile );
//	void	File_Save( uint16_t FileNum, struct uFile_TSP const * pFile );
//	void	File_Save( uint16_t FileNum, struct uFile_AIR const * pFile );
//	void	File_Save( uint16_t FileNum, struct uFile_R24 const * pFile );
//	void	File_Save( uint16_t FileNum, struct uFile_SHI const * pFile );

////////////////////////////////////////////////////////////////////////////////
//	泵累计时间记录
////////////////////////////////////////////////////////////////////////////////
extern	uint32_t	PumpSumTimeLoad( uint8_t Select );
extern	void		PumpSumTimeSave( uint8_t Select, uint32_t SumTime );

////////////////////////////////////////////////////////////////////////////////
//	开、关机记录到文件中的格式
////////////////////////////////////////////////////////////////////////////////
struct	uPowerLog
{
	uint32_t boot;	//	开机（上电）时间
	uint32_t shut;	//	关机（掉电）时间
};

extern	void	PowerLogSave_Boot( void );
extern	void	PowerLogSave_Shut( void );
extern	void	PowerLogLoad( uint16_t index, struct uPowerLog * pLog );
extern	void	PowerLog_Clean( void );
////////////////////////////////////////////////////////////////////////////////
//	文件存取
////////////////////////////////////////////////////////////////////////////////
//	struct	uFile
//	{
//		virtual	void	Save( uint16_t FileNum ) = 0;
//		virtual	void	Load( uint16_t FileNum ) = 0;
//	};

struct	uFile_TSP //: public uFile
{
	//	文件信息 与 采样设置
	uint32_t	sample_begin;		//	记录采样开始时间
	uint16_t	set_time;			//	设置采样时间
	uint16_t	set_flow;			//	设置采样流量
	uint8_t		set_loops; 			//	设置采样次数
	uint8_t		run_loops;			//	记录采样次数
	//	采样过程中的统计参数
	uint16_t	sum_min;
	FP32		sum_Ba;				//	大气压力
	FP32		sum_tr;				//	计前温度
	FP32		sum_pr;				//	计前温度
	FP32		max_pr;				//	计前压力最大值
	//	采样的累计值
	FP32		vd, vnd;			//	累计采样体积
	
//	public:
//		void	Save( uint16_t FileNum );
//		void	Load( uint16_t FileNum );
// 	uint32_t	DataValidMask;
};

struct	uFile_SHI
{
	//	文件信息 与 采样设置
	uint32_t	sample_begin;		//	记录采样开始时间
	uint16_t	set_time[2];		//	设置采样时间
	uint16_t	set_flow[2];		//	设置采样流量
	uint8_t		set_loops; 			//	设置采样次数
	uint8_t		run_loops;			//	记录采样次数
	//	采样过程中的统计参数
	uint16_t	sum_min[2];			//	实际采样时间，每分钟一计，最大24小时
	FP32		sum_Ba;				//	大气压力
	FP32		sum_tr[2];			//	计前温度
	FP32		sum_pr[2];			//	计前压力
	FP32		max_pr[2];			//	计前压力最大值
	//	采样的累计值
	FP32		vnd[2];				//	累计的采样体积，只记录标况体积
	FP32		vd[2];				//	2015年7月23日 累计标况体积

//	public:
//		void	Save( uint16_t FileNum );
//		void	Load( uint16_t FileNum );
// 	uint32_t	DataValidMask;
};

struct	uFile_R24	//	日均与时均使用的记录结构完全相同，但分别定义，便于进行类型检查
{
	//	文件信息 与 采样设置
	uint32_t	sample_begin;		//	记录采样开始时间
	uint16_t	set_time[2];		//	设置采样时间
	uint16_t	set_flow[2];		//	设置采样流量
	uint8_t		set_loops; 			//	设置采样次数
	uint8_t		run_loops;			//	记录采样次数
	uint8_t  _r[2];
	//	采样过程中的统计参数
	uint16_t	sum_min[2];			//	实际采样时间，每分钟一计，最大24小时
	FP32		sum_Ba;				//	大气压力
	FP32		sum_tr[2];			//	计前温度
	FP32		sum_pr[2];			//	计前压力
	FP32		max_pr[2];			//	计前压力最大值
	//	采样的累计值
	FP32		vnd[2];				//	累计的采样体积，只记录标况体积
	FP32		vd[2];				//	2015年7月23日 累计标况体积

//	public:
//		void	Save( uint16_t FileNum );
//		void	Load( uint16_t FileNum );
// 	uint32_t	DataValidMask;
};

struct	uFile_AIR
{
	
	uint32_t	sample_begin;		//	记录采样开始时间
	uint16_t	set_time;			//	设置采样时间
	uint16_t	set_flow[2];			//	设置采样流量
	uint8_t		set_loops; 			//	设置采样次数
	uint8_t		run_loops;			//	记录采样次数
	//	采样过程中的统计参数	
	uint16_t	sum_min;			//	实际采样时间，每分钟一计，最大24小时
// 	FP32		vnd[2];				//	累计的采样体积，只记录标况体积
//	FP32		sum_Ba;				//	大气压力

//	public:
//		void	Save( uint16_t FileNum );
//		void	Load( uint16_t FileNum );
// 	uint32_t	DataValidMask;
};


extern	void	File_Save_TSP( uint16_t FileNum, struct uFile_TSP const * pFile );
extern	void	File_Load_TSP( uint16_t FileNum, struct uFile_TSP       * pFile );
extern	void	File_Save_R24( uint16_t FileNum, struct uFile_R24 const * pFile );
extern	void	File_Load_R24( uint16_t FileNum, struct uFile_R24       * pFile );
extern	void	File_Save_SHI( uint16_t FileNum, struct uFile_SHI const * pFile );
extern	void	File_Load_SHI( uint16_t FileNum, struct uFile_SHI       * pFile );
extern	void	File_Save_AIR( uint16_t FileNum, struct uFile_AIR const * pFile );
extern	void	File_Load_AIR( uint16_t FileNum, struct uFile_AIR       * pFile );

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
