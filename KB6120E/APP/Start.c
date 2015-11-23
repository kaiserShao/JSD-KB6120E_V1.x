/**************** (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Start.C
* 创 建 者: 董峰
* 描  述  : KB-6120E 启动程序
* 最后修改: 2015年6月2日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"
#include <cmsis_os.h>
#include "U_INC.H"


/********************************** 功能说明 ***********************************
*	系统延时功能
*******************************************************************************/
void	delay( uint16_t ms )
{
	osDelay( ms );
}

extern	void	menu_show_env_state( void );
extern	void	show_battery( uint16_t yx );
/********************************** 功能说明 ***********************************
*  显示时间
*******************************************************************************/
void	show_std_clock( void )
{
	uint32_t	now = get_Now();

	ShowClock( 0x0600u, &now );
	show_battery( 0x0009u );
}

/********************************** 功能说明 ***********************************
*  主菜单
*******************************************************************************/
void	menu_Main( void )
{
	static	struct uMenu  const  menu[] =
	{
		{ 0x0202u, "主菜单" },
		{ 0x0203u, "设置" },	//	设置默认采样参数，原则上不需要每次都修改
		{ 0x020Au, "采样" },	//	调整每次采样都需要确认的参数，并启动采样
		{ 0x0403u, "查询" },	//	查看对应采样器的采样记录文件，及打印功能
		{ 0x040Au, "维护" }
	};

	uint8_t	item = 1u;

	for(;;)
	{
		monitor();

		cls();
		Menu_Redraw( menu );
		SamplerTypeShow( 0x000Au );

		item = Menu_SelectOnly( menu, item, show_std_clock );
		
		switch( item )
		{
		//	在主菜单下按取消键，显示大气压、恒温箱温度等环境参数
		case 1:	menu_SampleSetup();	break;
		case 2:	menu_SampleStart();	break;
		case 3:	menu_SampleQuery();	break;
		case 4:	menu_Maintenance();	break;
		case enumSelectESC:	menu_show_env_state();	break;
		case enumSelectXCH:	SamplerTypeSwitch();	break;
		}
	}
}

/********************************** 功能说明 ***********************************
*  初始化电源日志任务
*******************************************************************************/
extern	void	PowerLogSave_PowerBoot( void );
extern	void	PowerLogSave_PowerShut( void );

__task	void	_task_PowerLog( void const * p_arg )
{
	PowerLogSave_PowerBoot();

	for(;;)
	{
		PowerLogSave_PowerShut();
		delay( 30000u );			//	延时 30 秒钟
	}
}

void	PowerLog_Init( void )
{
	static	osThreadDef( _task_PowerLog, osPriorityLow, 1, 200 );

	osThreadCreate( osThread(_task_PowerLog), NULL );
}


/********************************** 功能说明 ***********************************
*	I2C 总线的互斥访问控制
*******************************************************************************/
static	osMutexId	mutexBus_I2C = NULL;

void	bus_i2c_mutex_apply( void )
{
	osMutexWait( mutexBus_I2C, osWaitForever );
}

void	bus_i2c_mutex_release( void )
{
	osMutexRelease( mutexBus_I2C );
}

static	void	bus_i2c_mutex_init( void )
{
	static	osMutexDef ( mutex_i2cbus);

	mutexBus_I2C = osMutexCreate( osMutex( mutex_i2cbus ));
}
static	osMutexId	mutexBus_SPI = NULL;

void	bus_SPI_mutex_apply( void )
{
	osMutexWait( mutexBus_SPI, osWaitForever );
}

void	bus_SPI_mutex_release( void )
{
	osMutexRelease( mutexBus_SPI );
}

static	void	bus_SPI_mutex_init( void )
{
	static	osMutexDef ( mutex_SPIbus );

	mutexBus_SPI = osMutexCreate( osMutex( mutex_SPIbus ) );
}
void	RTOS_Init( void )
{
	bus_i2c_mutex_init();
	bus_SPI_mutex_init();
}

/********************************** 功能说明 ***********************************
 *	多任务系统中的主程序，初始化转入多任务系统进行处理
*******************************************************************************/
/*
配置系统的顺序
CPU 内部、底层硬件
外部接口
外部硬件（按键、显示）
读取外部配置数据
传感器系统（RTC 时钟）
操作系统
任务
驻留任务（如：传感器读取、恒温箱温控、掉电时间记录）
采样任务
监控任务
*/
__task	int32_t	main( void )
{

	BIOS_Init();
	beep();
	RTOS_Init();		  //	尽早执行
		
	EditionSelsct();
	ConfigureLoad();	//	先确定仪器型号
	CalibrateLoad();	//	读传感器前执行,	远程存储器中的参数，应等通讯初始化后再读。
	SampleSetLoad();	//	恢复采样前执行
	
	Display_Init();
	DisplaySetGrayVolt( Configure.DisplayGray * 0.01f );
	DisplaySetLight( Configure.DisplayLight );
	DisplaySetTimeout( Configure.TimeoutLight );
	Keyboard_Init();	//	配置完背光超时时间后再初始化。

	ShowEdition();		//	版本显示 确定型号之后，显示初始化之后	
	SENSOR_Local_Init();	//	本地传感器读取以及液晶灰度调节
	
	RTC_Init();			  //	为避免启动过程中时钟失败造成的假死现象，放在显示初始化之后
	
	SD_Init();				//	SD卡读写初始化，放在开关机存取之前
	delay( 500u );
	PowerLog_Init();	//	开关机存取，时间和SD卡初始化之后
	
	delay( 2000u );		//  配合下位机初始化
	SENSOR_Remote_Init();		//	modbus通信初始化

	HCBox_Init();
	delay( 500u );
	
	Sampler_BootResume();	  //	时间配置完成之后，设置参数读入之后。
	delay( 1500u );
	
	SamplerSelect = Q_ALL;	//	初始化当前采样器为不合理的值，进行一次切换，切换到第一个合理的值。
	SamplerTypeSwitch();
	for(;;)
	{
		menu_Main();	        //	转主菜单
	}
}

/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File ****/
