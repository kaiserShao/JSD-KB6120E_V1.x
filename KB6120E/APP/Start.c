/**************** (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Start.C
* �� �� ��: ����
* ��  ��  : KB-6120E ��������
* ����޸�: 2015��6��2��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include <cmsis_os.h>
#include "U_INC.H"


/********************************** ����˵�� ***********************************
*	ϵͳ��ʱ����
*******************************************************************************/
void	delay( uint16_t ms )
{
	osDelay( ms );
}

extern	void	menu_show_env_state( void );
extern	void	show_battery( uint16_t yx );
/********************************** ����˵�� ***********************************
*  ��ʾʱ��
*******************************************************************************/
static	void	show_std_clock( void )
{
	uint32_t	now = get_Now();

	ShowClock( 0x0600u, &now );
	show_battery( 0x0009u );
}

/********************************** ����˵�� ***********************************
*  ���˵�
*******************************************************************************/
void	menu_Main( void )
{
	static	struct uMenu  const  menu[] =
	{
		{ 0x0202u, "���˵�" },
		{ 0x0203u, "����" },	//	����Ĭ�ϲ���������ԭ���ϲ���Ҫÿ�ζ��޸�
		{ 0x020Au, "����" },	//	����ÿ�β�������Ҫȷ�ϵĲ���������������
		{ 0x0403u, "��ѯ" },	//	�鿴��Ӧ�������Ĳ�����¼�ļ�������ӡ����
		{ 0x040Au, "ά��" }
	};

	uint8_t	item = 1u;

	for(;;)
	{
		monitor();

		cls();
		Menu_Redraw( menu );
		SamplerTypeShow( 0x000Au );

		item = Menu_Select_Ex( menu, item, show_std_clock );
		
		switch( item )
		{
		//	�����˵��°�ȡ��������ʾ����ѹ���������¶ȵȻ�������
		case 1:	menu_SampleSetup();	break;
		case 2:	menu_SampleStart();	break;
		case 3:	menu_SampleQuery();	break;
		case 4:	menu_Maintenance();	break;
		case enumSelectESC:	menu_show_env_state();	break;
		case enumSelectXCH:	SamplerTypeSwitch();	break;
		}
	}
}

/********************************** ����˵�� ***********************************
*  ��ʼ����Դ��־����
*******************************************************************************/
extern	void	PowerLogSave_PowerBoot( void );
extern	void	PowerLogSave_PowerShut( void );

__task	void	_task_PowerLog( void const * p_arg )
{
	PowerLogSave_PowerBoot();

	for(;;)
	{
		PowerLogSave_PowerShut();
		delay( 30000u );			//	��ʱ 30 ����
	}
}

void	PowerLog_Init( void )
{
	static	osThreadDef( _task_PowerLog, osPriorityLow, 1, 200 );

	osThreadCreate( osThread(_task_PowerLog), NULL );
}


/********************************** ����˵�� ***********************************
*	I2C ���ߵĻ�����ʿ���
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

/********************************** ����˵�� ***********************************
 *	������ϵͳ�е������򣬳�ʼ��ת�������ϵͳ���д���
*******************************************************************************/
/*
����ϵͳ��˳��
CPU �ڲ����ײ�Ӳ��
�ⲿ�ӿ�
�ⲿӲ������������ʾ��
��ȡ�ⲿ��������
������ϵͳ��RTC ʱ�ӣ�
����ϵͳ
����
פ�������磺��������ȡ���������¿ء�����ʱ���¼��
��������
�������
*/
__task	int32_t	main( void )
{

	BIOS_Init();
	beep();
	RTOS_Init();		  //	����ִ��
	
	ConfigureLoad();	//	��ȷ�������ͺ�
	CalibrateLoad();	//	��������ǰִ��,	Զ�̴洢���еĲ�����Ӧ��ͨѶ��ʼ�����ٶ���
	SampleSetLoad();	//	�ָ�����ǰִ��
	
	Display_Init();
	DisplaySetGrayVolt( Configure.DisplayGray * 0.01f );
	DisplaySetLight( Configure.DisplayLight );
	DisplaySetTimeout( Configure.TimeoutLight );
	Keyboard_Init();	//	�����걳�ⳬʱʱ����ٳ�ʼ����
	
	ShowEdition();		//	ȷ���ͺ�֮����ʾ��ʼ��֮��
	RTC_Init();			  //	Ϊ��������������ʱ��ʧ����ɵļ������󣬷�����ʾ��ʼ��֮��
	SD_Init();				//	SD����д��ʼ�������ڿ��ػ���ȡ֮ǰ
	PowerLog_Init();	//	���ػ���ȡ��ʱ���ʼ��֮��
	
	delay( 1000u );		//  �����λ����ʼ��
	SENSOR_Init();		//	modbusͨ�ų�ʼ��
	
	delay( 500u );
	HCBox_Init();
	delay( 500u );

	Sampler_BootResume();	  //	ʱ���������֮�����ò�������֮��
	delay( 100u );
	
	SamplerSelect = Q_ALL;	//	��ʼ����ǰ������Ϊ�������ֵ������һ���л����л�����һ�������ֵ��
	SamplerTypeSwitch();
	delay( 1400u );
	for(;;)
	{
		menu_Main();	        //	ת���˵�
	}
}

/********  (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
