/**************** (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: EEPROM.C
* �� �� ��: Dean
* ��  ��  : KB-6120E ������ݷ��ʣ��ڴ��ļ��ж������ĵ�ַ���֡�
* ����޸�: 2015��6��2��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include "SD_USBPort.h" 
#include "bkpdata.h"  

#define	_EE_Page_Len	128u

////////////////////////////////////////////////////////////////////////////////
BOOL	Eload( uint16_t address, void * buffer, uint16_t count )
{
	BOOL	state;

	bus_i2c_mutex_apply();
	state = EE24C512_Load( address, ( uint8_t * )buffer, count );
	bus_i2c_mutex_release();
	
	return	state;
}

BOOL	Esave( uint16_t address, void const * buffer, uint16_t count )
{
	BOOL	state;
	
	bus_i2c_mutex_apply();
	state = EE24C512_Save( address, ( uint8_t * )buffer, count );
	bus_i2c_mutex_release();

	return	state;
}

/**/
BOOL	SDLoad( const char * BUF_Name, uint32_t address, void * buffer, uint32_t count )
{																	 							 
	BOOL	state;
	state = ByteLoad( BUF_Name , address, buffer, count );			
 	return	state;
}

BOOL	SDSave( const char * BUF_Name, uint32_t address,   uint8_t * buffer, uint32_t count )
{
	BOOL	state;
	state = ByteSave( BUF_Name , buffer ,  address, count );					
	return	state;
}

///////////////////////////////////////////////////////////////////
//	EEPROM ��ַ
///////////////////////////////////////////////////////////////////
//	1.�����洢�ռ�(��ʼ��ַ��0�������С��1K)
#define	x_CalibrateRemote	0x080u	//	�궨������Զ�̣�
#define	x_CalibrateLocal 	0x100u	//	�궨���������أ�

#define	x_Configure 	0x180u	//	���ò���
#define	x_SampleSet 	0x200u	//	���ò���
//	2.��־�洢�ռ�(��ʼ��ַ��1K�������С��1K)

#define	x_PowerLogBase	100u
#define	  PowerLogSize	8u

#define	x_SumTime		0x0780u
//	3.������ַ�ռ�(��ʼ��ַ��2K�������С��2K)
#define	x_Reserve		0x0800u
//	4.�ļ��洢�ռ�(��ʼ��ַ��4K�������С������)
#define	x_File_Base		0x1000u

/********************************** ����˵�� ***********************************
*	�ļ���ȡ
*********************************** ��Ƽ�¼ ***********************************
Ӧ��ϸ����ļ�����ʼ��ַ���ȡ���ȣ�ȷ����ȡ����λ�õ��ļ�ʱ�������������ҳ����
�ۺϿ��Ǻ��趨��
TSP �ļ���ȡ��� 40��ʵ���ļ�����Լ 32
AIR �ļ���ȡ��� 24��ʵ���ļ�����Լ 16
R24 �ļ���ȡ��� 64��ʵ���ļ�����Լ 56
SHI �ļ���ȡ��� 64��ʵ���ļ�����Լ 56
�������ȡҳ�ĳ���Ϊ64����������ʱ�������������Ҫ��(ʵ��һ����128)��
�ܵ��ļ���ȡ��� 196��

�ļ��Ŵ�1��ʼ��ţ�����1#�ļ�ʵ�ʷ��ʵ���0#��ַ�������� Base - Num0Size
*******************************************************************************/
#define	FilePageSize_TSP  40u
#define	FilePageSize_AIR  32u
#define	FilePageSize_R24  64u
#define	FilePageSize_SHI  64u

void	File_Load_TSP( uint16_t FileNum, struct uFile_TSP * pFile )
{
  bool state;
	
	state = SDLoad("\\SD_TSP\\TSP.TXT", ( FilePageSize_TSP * FileNum ), pFile, sizeof(  struct uFile_TSP ));
	
	if ( ! state )
	{
		pFile->sample_begin = 0;
	}	
}

void	File_Load_AIR( uint16_t FileNum, struct uFile_AIR * pFile )
{

	bool state;
  
	state  = SDLoad("\\SD_AIR\\AIR.TXT", ( FilePageSize_AIR * FileNum ), pFile, sizeof( struct uFile_AIR ));
	
	if ( ! state )
	{
		pFile->sample_begin = 0;
	}	
}

void	File_Load_R24( uint16_t FileNum, struct uFile_R24 * pFile )
{

	bool state;  

	state = SDLoad("\\SD_R24\\R24.TXT", ( FilePageSize_R24 * FileNum ), pFile, sizeof( struct uFile_R24 ));
	if ( ! state )
	{
		pFile->sample_begin = 0;
	}	
}

void	File_Load_SHI( uint16_t FileNum, struct uFile_SHI * pFile )
{

	bool state;   
	state = SDLoad("\\SD_SHI\\SHI.TXT",  ( FilePageSize_SHI * FileNum ), pFile, sizeof( struct uFile_SHI ));
	
	if ( ! state )
	{
		pFile->sample_begin = 0;
	}	
}
/**/

uint8_t ss[100];

void	File_Save_TSP( uint16_t FileNum, struct uFile_TSP const * pFile )
{
	memset( ss, 0x00, 100 );
	memcpy( ss, pFile, sizeof( *pFile ));	
	assert(( FileNum >= 1 ) && ( FileNum <= FileNum_Max ));
	SDSave("\\SD_TSP\\TSP.TXT", ( FilePageSize_TSP * FileNum ), ss, FilePageSize_TSP );
}
  
void	File_Save_AIR( uint16_t FileNum, struct uFile_AIR const * pFile )
{
	memset( ss, 0x00, 100 );
	memcpy( ss, pFile, sizeof( *pFile ));
	assert(( FileNum >= 1 ) && ( FileNum <= FileNum_Max ));	
	SDSave("\\SD_AIR\\AIR.TXT", ( FilePageSize_AIR * FileNum ), ss, FilePageSize_AIR );
}
 
void	File_Save_R24( uint16_t FileNum, struct uFile_R24 const * pFile )
{
	memset( ss, 0x00, 100 );
	memcpy( ss, pFile, sizeof( *pFile ));
	assert(( FileNum >= 1 ) && ( FileNum <= FileNum_Max ));
	SDSave("\\SD_R24\\R24.TXT", ( FilePageSize_R24 * FileNum ),ss, FilePageSize_R24 );
}

void	File_Save_SHI( uint16_t FileNum, struct uFile_SHI const * pFile )
{
	memset( ss, 0x00, 100 );
	memcpy( ss, pFile, sizeof( *pFile ));
	assert(( FileNum >= 1 ) && ( FileNum <= FileNum_Max ));
	SDSave("\\SD_SHI\\SHI.TXT",  ( FilePageSize_SHI  * FileNum ), ss, FilePageSize_SHI );
}
/**/
extern	uint8_t sdinit[16];

void	File_Clean( void )
{
	Byte_CREAT_CON_DIR( "\\SD_AIR" );	//��������Ŀ¼	( ����Ѿ����ڣ�ֱ�Ӵ� )
	Byte_CREAT_WRITE_PATH( "\\SD_AIR\\AIR.TXT", sdinit, 16 );//  ���´���
	ByteFill( "\\SD_AIR\\AIR.TXT", 16, ( FilePageSize_AIR - 16 ) + FilePageSize_AIR );
	
	Byte_CREAT_CON_DIR( "\\SD_TSP" );
	Byte_CREAT_WRITE_PATH( "\\SD_TSP\\TSP.TXT", sdinit, 16 );//  ���´���
	ByteFill( "\\SD_TSP\\TSP.TXT", 16, ( FilePageSize_TSP - 16 ) + FilePageSize_TSP );
	
	Byte_CREAT_CON_DIR( "\\SD_R24" );
	Byte_CREAT_WRITE_PATH( "\\SD_R24\\R24.TXT", sdinit, 16 );//  ���´���
	ByteFill( "\\SD_R24\\R24.TXT", 16, ( FilePageSize_R24 - 16 ) + FilePageSize_R24 );
	
	Byte_CREAT_CON_DIR( "\\SD_SHI" );
	Byte_CREAT_WRITE_PATH( "\\SD_SHI\\SHI.TXT", sdinit, 16 );//  ���´���
	ByteFill( "\\SD_SHI\\SHI.TXT", 16, ( FilePageSize_SHI - 16 ) + FilePageSize_SHI );
}

/********************************** ����˵�� ***********************************
*	��ȡ �궨�����á����� ����
*******************************************************************************/
void	CalibrateSave( void )
{	
	Esave( x_CalibrateLocal,  &CalibrateLocal,  sizeof( CalibrateLocal  ));
	Esave( x_CalibrateRemote, &CalibrateRemote, sizeof( CalibrateRemote ));
}
void	CalibrateLoad( void )
{
	Eload( x_CalibrateLocal,  &CalibrateLocal,  sizeof( CalibrateLocal  ));

	if ( CalibrateLocal.DataValidMask != 0x5AA5u )
	{
		CalibrateLocal.origin_Ba = 8000u;			//	80.00 kPa

		CalibrateLocal.slope_Ba  = 1000u;
		
		CalibrateLocal.DataValidMask = 0x5AA5u;
	}
	
	Eload( x_CalibrateRemote, &CalibrateRemote, sizeof( CalibrateRemote ));
	
	if ( CalibrateRemote.DataValidMask != 0x5AA4u )
	{		
		CalibrateRemote.origin_Te                 = 27315u; 	//	273.15 'C
		CalibrateRemote.origin_Heater_Temp        = 27315u; 	//	273.15 'C
		CalibrateRemote.origin_HCBox_Temp         = 27315u; 	//	273.15 'C
		CalibrateRemote.slope_Te                 = 1000u;
		CalibrateRemote.slope_Heater_Temp        = 1000u;
		CalibrateRemote.slope_HCBox_Temp         = 1000u;
		
		CalibrateRemote.origin[esid_tr][PP_TSP  ] = 27315u; 	//	273.15 'C
		CalibrateRemote.origin[esid_tr][PP_R24_A] = 27315u; 	//	273.15 'C
		CalibrateRemote.origin[esid_tr][PP_R24_B] = 27315u; 	//	273.15 'C
		CalibrateRemote.origin[esid_tr][PP_SHI_C] = 27315u; 	//	273.15 'C
		CalibrateRemote.origin[esid_tr][PP_SHI_D] = 27315u; 	//	273.15 'C
		
		CalibrateRemote.slope[esid_tr][PP_TSP  ] = 1000u;
		CalibrateRemote.slope[esid_tr][PP_R24_A] = 1000u; 	
		CalibrateRemote.slope[esid_tr][PP_R24_B] = 1000u; 	
		CalibrateRemote.slope[esid_tr][PP_SHI_C] = 1000u; 	
		CalibrateRemote.slope[esid_tr][PP_SHI_D] = 1000u; 	
		
		CalibrateRemote.origin[esid_pf][PP_TSP  ] = 0x8000u;
		CalibrateRemote.origin[esid_pf][PP_R24_A] = 0x8000u;
		CalibrateRemote.origin[esid_pf][PP_R24_B] = 0x8000u;
		CalibrateRemote.origin[esid_pf][PP_SHI_C] = 0x8000u;
		CalibrateRemote.origin[esid_pf][PP_SHI_D] = 0x8000u;
		
		CalibrateRemote.origin[esid_pr][PP_TSP  ] = 0x8000u;
		CalibrateRemote.origin[esid_pr][PP_R24_A] = 0x8000u;
		CalibrateRemote.origin[esid_pr][PP_R24_B] = 0x8000u;
		CalibrateRemote.origin[esid_pr][PP_SHI_C] = 0x8000u;
		CalibrateRemote.origin[esid_pr][PP_SHI_D] = 0x8000u;
		
		

		CalibrateRemote.slope[esid_pf][PP_TSP  ] = 1000u;
		CalibrateRemote.slope[esid_pf][PP_R24_A] = 1000u;
		CalibrateRemote.slope[esid_pf][PP_R24_B] = 1000u;
		CalibrateRemote.slope[esid_pf][PP_SHI_C] = 1000u;
		CalibrateRemote.slope[esid_pf][PP_SHI_D] = 1000u;

		CalibrateRemote.slope[esid_pr][PP_TSP  ] = 1000u;
		CalibrateRemote.slope[esid_pr][PP_R24_A] = 1000u;
		CalibrateRemote.slope[esid_pr][PP_R24_B] = 1000u;
		CalibrateRemote.slope[esid_pr][PP_SHI_C] = 1000u;
		CalibrateRemote.slope[esid_pr][PP_SHI_D] = 1000u;

		CalibrateRemote.slope_flow[PP_TSP  ][0] = 1000u;
		
		CalibrateRemote.slope_flow[PP_R24_A][0] = 1000u;
				
		CalibrateRemote.slope_flow[PP_R24_B][0] = 1000u;		
	
		CalibrateRemote.slope_flow[PP_SHI_C][0] = 1000u;
		CalibrateRemote.slope_flow[PP_SHI_C][1] = 1000u;
		CalibrateRemote.slope_flow[PP_SHI_C][2] = 1000u;
		CalibrateRemote.slope_flow[PP_SHI_C][3] = 1000u;
		
		
		CalibrateRemote.slope_flow[PP_SHI_D][0] = 1000u;
		CalibrateRemote.slope_flow[PP_SHI_D][1] = 1000u;
		CalibrateRemote.slope_flow[PP_SHI_D][2] = 1000u;
		CalibrateRemote.slope_flow[PP_SHI_D][3] = 1000u;

		CalibrateRemote.DataValidMask = 0x5AA4u;
	}
}

uint32_t	eDataValidMask;
void	ConfigureSave( void )
{
	Esave( x_Configure, &Configure, sizeof(Configure));
}

void	ConfigureLoad( void )
{
	EditionSelsct();
	Eload( x_Configure, &Configure, sizeof(Configure));
	if ( Configure.DataValidMask != eDataValidMask )
	{
		ConfigureLoadDefault();
		Configure.DataValidMask = eDataValidMask;
	}
}

void	SampleSetSave( void )
{
	Esave( x_SampleSet, SampleSet, sizeof(SampleSet));
}

void	SampleSetLoad( void )
{
	struct uSampleSet * pSampleSet;

	Eload( x_SampleSet, SampleSet, sizeof(SampleSet));
	
	pSampleSet = &SampleSet[Q_AIR];
	if ( pSampleSet->DataValidMask != 0x56A1u ) //  0x56A1u
	{

		pSampleSet->delay1   = 1u;
		pSampleSet->sample_1  = 60u;
		pSampleSet->suspend_1 = 10u;
		pSampleSet->sample_2  = 0u;
		pSampleSet->suspend_2 = 70u;
		pSampleSet->set_loops = 1u;
		pSampleSet->FileNum = 0u;
		pSampleSet->start   = 0u;
		pSampleSet->DataValidMask = 0x56A1u;
	}	
	
	pSampleSet = &SampleSet[Q_TSP];
	if ( pSampleSet->DataValidMask != 0x56A4u ) //  0x56A4u
	{
		pSampleSet->delay1   = 1u;
		pSampleSet->sample_1  = 60u;
		pSampleSet->suspend_1 = 10u;
		pSampleSet->sample_2  = 0u;
		pSampleSet->suspend_2 = 70u;
		pSampleSet->set_loops = 1u;
		pSampleSet->FileNum = 0u;
		pSampleSet->start   = 0u;
		pSampleSet->DataValidMask = 0x56A4u;
	}

	pSampleSet = &SampleSet[Q_R24];
	if ( pSampleSet->DataValidMask != 0x56A8u ) // 0x56A8u
	{

		pSampleSet->delay1   = 1u;
		pSampleSet->sample_1  = 60u;
		pSampleSet->suspend_1 = 10u;
		pSampleSet->sample_2  = 60u;
		pSampleSet->suspend_2 = 10u;
		pSampleSet->set_loops = 1u;
		pSampleSet->FileNum = 0u;
		pSampleSet->start   = 0u;
		pSampleSet->DataValidMask = 0x56A8u;
	}

	pSampleSet = &SampleSet[Q_SHI];
	if ( pSampleSet->DataValidMask != 0x56A9u ) //  0x56A9u
	{

		pSampleSet->delay1   = 1u;
		pSampleSet->sample_1  = 60u;
		pSampleSet->suspend_1 = 10u;
		pSampleSet->sample_2  = 60u;
		pSampleSet->suspend_2 = 10u;
		pSampleSet->set_loops = 1u;
		pSampleSet->FileNum = 0u;
		pSampleSet->start   = 0u;
		pSampleSet->DataValidMask = 0x56A9u;
	}
}

/********************************** ����˵�� ***********************************
*	��ȡ �����ػ�ʱ���¼
*******************************************************************************/
volatile	uint16_t	PowerLogIndex ;
uint8_t powertime[4];
void	PowerLogSave_PowerBoot( void )
{
	struct	uPowerLog	log;
	uint32_t	now;
	PowerLogIndex = (ByteGetSize("\\SD_POW\\POWER.TXT") -100)/8;

	log.shut =	Powertime_Read( PSHUT);//	BKP���洢ʱ�� ��Ϊ �ػ�ʱ��
	powertime[3]=(uint8_t)(log.shut>>24);
	powertime[2]=(uint8_t)(log.shut>>16);
	powertime[1]=(uint8_t)(log.shut>>8);
	powertime[0]=(uint8_t)log.shut;
	SDSave("\\SD_POW\\POWER.TXT", x_PowerLogBase + ( PowerLogSize * PowerLogIndex ) + 4 , powertime, 4);
	
	PowerLogIndex = ( PowerLogIndex % PowerLogIndex_Max ) + 1u;
	now = get_Now();
	log.boot = now;	//	��ǰʱ�� ��Ϊ ����ʱ��
	log.shut = now;
	
 	powertime[3]=(uint8_t)(log.boot>>24);
	powertime[2]=(uint8_t)(log.boot>>16);
	powertime[1]=(uint8_t)(log.boot>>8);
	powertime[0]=(uint8_t)log.boot;
	SDSave("\\SD_POW\\POWER.TXT", x_PowerLogBase + ( PowerLogSize * PowerLogIndex ) , powertime, 4);
}
 
void	PowerLogSave_PowerShut( void )
{

	uint32_t	now;
	
	now = get_Now();	
	
	Powertime_Write( now,PSHUT );

}
//	Index : 0 - ���ο���ʱ�䣬1..(Max-1) - ���Ų�����ǰ�ļ�¼��
void    PowerLogLoad( uint16_t index, struct uPowerLog * pLog )
{	
	uint16_t	i = 0;
	if( PowerLogIndex > index )
		i =  PowerLogIndex - index ;
	else
		i = 0;
  SDLoad("\\SD_POW\\POWER.TXT",x_PowerLogBase + ( PowerLogSize * i ),  pLog, 8 );
}

void    PowerLog_Clean( void )
{
	Byte_CREAT_CON_DIR( "\\SD_POW" );
	Byte_CREAT_WRITE_PATH( "\\SD_POW\\POWER.TXT", sdinit,16 );
	ByteFill( "\\SD_POW\\POWER.TXT", 16, ( x_PowerLogBase + 4 -  sizeof ( sdinit )));
	PowerLogIndex = ( ByteGetSize("\\SD_POW\\POWER.TXT") -100 ) / 8;
	SDSave( "\\SD_POW\\POWER.TXT", x_PowerLogBase + ( PowerLogSize * PowerLogIndex ) , powertime, 4 );
}

/********************************** ����˵�� ***********************************
	���ۼ�����ʱ�� ��ȡ
*******************************************************************************/
void	PumpSumTimeSave( uint8_t MotorSelect, uint32_t SumTime )
{
	Esave( x_SumTime + sizeof( SumTime ) * MotorSelect, &SumTime, sizeof(SumTime));
}

uint32_t	PumpSumTimeLoad( uint8_t MotorSelect )
{
	uint32_t	SumTime;
	Eload( x_SumTime + sizeof( SumTime ) * MotorSelect, &SumTime, sizeof(SumTime));
	return	SumTime;
}

/********************************** ����˵�� ***********************************
	���������У���ʱʹ�ñ���EEPROMģ��Զ��EEPROM���洢Զ�̴������ı궨����
*******************************************************************************/
//	void	KB6102_CalibrateSave( enum enumBoardSelect Board );
//	void	KB6102_CalibrateLoad( enum enumBoardSelect Board );
//	#define	xRemoteS_R24	( x_Reserve	)
//	#define	xRemoteS_SHI	( x_Reserve + 0x80	)

//	void	KB6102_CalibrateSave( enum enumBoardSelect BS )
//	{
//		uint16_t	address = (( BS_R24 == BS ) ? xRemoteS_R24 : xRemoteS_SHI );

//		Esave( address, CalibrateRemote + BS, sizeof(CalibrateRemote[0]));
//	}

//	void	KB6102_CalibrateLoad( enum enumBoardSelect BS )
//	{
//		uint16_t	address = (( BS_R24 == BS ) ? xRemoteS_R24 : xRemoteS_SHI );

//		Eload( address, CalibrateRemote + BS, sizeof(*CalibrateRemote));
//	}
//		
/********  (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
