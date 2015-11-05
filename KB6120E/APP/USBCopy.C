/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: USBCopy.C
* �� �� ��: Kaiser
* ��  ��  : �ļ����� ( U �� )
* ����޸�: 2015/07/30
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "USBCopy.h"
#include "SD_USBPort.h"

BOOL	USBSave( const char * BUF_Name, uint32_t address, uint8_t *buffer, uint32_t count )
{
	BOOL	state;
 
	state = SecSave( BUF_Name ,  (uint8_t *)buffer ,  address, count );					

	return	state;
}

/***/

BOOL	USBLoad(const char * BUF_Name, uint32_t address, uint8_t * buffer, uint32_t count )
{																	 							 
	BOOL	state;
 
	state = SecLoad( BUF_Name , address, buffer, count );			
	
	return	state;
}

/**********************************************************************************/

BOOL USB_PrintInit(void)
{
  cls();
	switch( USB_Init() )
	{
		case 0:
			File_Creat();
			return TRUE;
		case 1:
			 MsgBox( "Ӳ������ !", vbOKOnly );
			SD_Init();
			return FALSE;
		case 2:
			MsgBox( "U��δ���� !", vbOKOnly );
			SD_Init();
			return FALSE;
		case 3:
			MsgBox( "�����ѶϿ� !", vbOKOnly );
			SD_Init();
			return FALSE;
		case 4:
			File_Creat();
			return TRUE;
		case 5:
			MsgBox( "δ֪���� !", vbOKOnly );
			SD_Init();
			return FALSE;
	}
	return FALSE;
}

/***************************************************************************/

BOOL USBPrint_TSP( uint16_t FileNum, struct uFile_TSP  const * pFile )
{ 
	struct	tm	t_tm;
	time_t	now;
	CHAR	sbuffer[512];
	CHAR	sbuffert[40];
	uint32_t s;
	s = ByteGetSize("\\USB_TSP\\TSP.TXT") / 512 ;	
 	memset( sbuffer, 0x00, 512 );	 
	sprintf( sbuffert, "\r\n%s��%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "�۳�������¼\r\n");
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
	sprintf( sbuffert, "�ļ�:%3u [��%2u��/��%2u��]\r\n", FileNum, pFile->run_loops, pFile->set_loops );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "��ʼʱ��:%2d��%2d�� %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò���ʱ��: %02u:%02u\r\n", pFile->set_time / 60u, pFile->set_time % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò�������: %5.1f L/m\r\n", pFile->set_flow * 0.1f );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ۼƲ���ʱ��: %02u:%02u\r\n", pFile->sum_min / 60u, pFile->sum_min % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�������(��): %7.1f L\r\n", pFile->vd  );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�������(��): %7.1f L\r\n", pFile->vnd );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );  
	sprintf( sbuffert, "ƽ������(��): %6.2f L/m\r\n", (FP32)pFile->vd  / pFile->sum_min );				
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ������(��): %6.2f L/m\r\n", (FP32)pFile->vnd / pFile->sum_min );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ����ǰ�¶�: %6.2f ��\r\n", pFile->sum_tr / pFile->sum_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ����ǰѹ��: %6.2f kPa\r\n", pFile->sum_pr / pFile->sum_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );                 
	sprintf( sbuffert, "ƽ������ѹ��: %6.2f kPa\r\n", pFile->sum_Ba / pFile->sum_min );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	now = get_Now( );(void)_localtime_r ( &now, &t_tm );	
	sprintf( sbuffert, "  == %4d/%2d/%2d %02d:%02d:%02d ==\r\n",
	t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 ); 
	delay_us(1);	
	if( !USBSave("\\USB_TSP\\TSP.TXT", s + 1, ( uint8_t *)sbuffer, ( sizeof( sbuffer ) + 511 ) / 512 ) )
	{
		SD_Init();
		return FALSE; 
	}
	SD_Init();
	MsgBox( "OK! ��γ�U�� !", vbOKOnly );
	return TRUE;
}

/************************************************************************/

BOOL USBPrint_R24( uint16_t FileNum, struct uFile_R24 const * pFile )
{ 
	struct	tm	t_tm;
	time_t	now;
	uint32_t s;
	CHAR	sbuffer[1024];
	CHAR	sbuffert[40];
	s = ByteGetSize("\\USB_R24\\R24.TXT") / 512;	
 	memset( sbuffer, 0x00, 1024 );	
	sprintf( sbuffert, "\r\n%s��%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "�վ�������¼\r\n");
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ļ�:%3u [��%2u��/��%2u��]\r\n", FileNum, pFile->run_loops, pFile->set_loops );	
	strcat( sbuffer, sbuffert );memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "��ʼʱ��:%2d��%2d�� %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò���ʱ��: A %02u:%02u\r\n",	pFile->set_time[Q_PP1] / 60u, pFile->set_time[Q_PP1] % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò���ʱ��: B %02u:%02u\r\n",	pFile->set_time[Q_PP2] / 60u, pFile->set_time[Q_PP2] % 60u );	
	strcat( sbuffer, sbuffert );memset( sbuffert, 0x00, 40 );	
	sprintf( sbuffert, "���ò�������: A %5.3f L/m\r\n",	pFile->set_flow[Q_PP1] * 0.001f );								
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò�������: B %5.3f L/m\r\n",	pFile->set_flow[Q_PP2] * 0.001f );								
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ۼƲ���ʱ��: A %02u:%02u\r\n",	pFile->sum_min[Q_PP1] / 60u, pFile->sum_min[Q_PP1] % 60u );		
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ۼƲ���ʱ��: B %02u:%02u\r\n",	pFile->sum_min[Q_PP2] / 60u, pFile->sum_min[Q_PP2] % 60u );		
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�������(��): A %7.1fL\r\n",		pFile->vnd[Q_PP1] );											
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�������(��): B %7.1fL\r\n",		pFile->vnd[Q_PP2] );											
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ������(��): A %6.3f L/m\r\n",	(FP32)pFile->vnd[Q_PP1] / pFile->sum_min[Q_PP1] );			
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ������(��): B %6.3f L/m\r\n",	(FP32)pFile->vnd[Q_PP2] / pFile->sum_min[Q_PP2] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "�������(��): A %7.1fL\r\n",		pFile->vd[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "�������(��): B %7.1fL\r\n",		pFile->vd[Q_PP2] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "ƽ������(��): A %6.3f L/m\r\n",	(FP32)pFile->vd[Q_PP1] / pFile->sum_min[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "ƽ������(��): B %6.3f L/m\r\n",	(FP32)pFile->vd[Q_PP2] / pFile->sum_min[Q_PP2] ); 
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ����ǰ�¶�: A %6.2f ��\r\n",	pFile->sum_tr[Q_PP1] / pFile->sum_min[Q_PP1] );				
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ����ǰ�¶�: B %6.2f ��\r\n",	pFile->sum_tr[Q_PP2] / pFile->sum_min[Q_PP2] );					
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ����ǰѹ��: A %6.2f kPa\r\n",	pFile->sum_pr[Q_PP1] / pFile->sum_min[Q_PP1] );					
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ����ǰѹ��: B %6.2f kPa\r\n",	pFile->sum_pr[Q_PP2] / pFile->sum_min[Q_PP2] );				
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "ƽ������ѹ��: %6.2f kPa\r\n",	pFile->sum_Ba / pFile->sum_min[Q_PP1] );						
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	now = get_Now();	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffert, "  == %4d/%2d/%2d %02d:%02d:%02d == \r\n",
	t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	delay_us(1);	
	if( !USBSave("\\USB_R24\\R24.TXT", s + 1, ( uint8_t *)sbuffer, ( sizeof( sbuffer ) + 511 ) / 512 ) )
	{
		SD_Init();
		return FALSE; 
	}
	SD_Init();
	MsgBox( "OK! ��γ�U�� !", vbOKOnly );
	return TRUE;
}

/**********************************************************************/

BOOL USBPrint_SHI( uint16_t FileNum, struct uFile_SHI const * pFile )
{ 
	struct	tm	t_tm;
	time_t	now;
	CHAR	sbuffer[1024];
	CHAR	sbuffert[40];
	uint32_t s;
	s = ByteGetSize("\\USB_SHI\\SHI.TXT") / 512;
 	memset( sbuffer, 0x00, 1024 );	
	sprintf( sbuffert, "\r\n%s��%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
  if( Configure.InstrumentType == type_KB2400HL )
  {
    sprintf( sbuffert, "������¼\r\n"); 
    strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  }
  else		
  {
    sprintf( sbuffert, "ʱ��������¼\r\n"); 
    strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  }
  sprintf( sbuffert, "�ļ�:%3u [��%2u��/��%2u��]\r\n", FileNum, pFile->run_loops, pFile->set_loops );	
  strcat( sbuffer, sbuffert );memset( sbuffert, 0x00, 40 );
  (void)_localtime_r ( &pFile->sample_begin, &t_tm );
  sprintf( sbuffert, "��ʼʱ��:%2d��%2d�� %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "���ò���ʱ��: C %02u:%02u\r\n",	pFile->set_time[Q_PP1] / 60u, pFile->set_time[Q_PP1] % 60u );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "���ò���ʱ��: D %02u:%02u\r\n",	pFile->set_time[Q_PP2] / 60u, pFile->set_time[Q_PP2] % 60u );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "���ò�������: C %5.1f L/m\r\n",	pFile->set_flow[Q_PP1] * 0.1f );							
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "���ò�������: D %5.1f L/m\r\n",	pFile->set_flow[Q_PP2] * 0.1f );					
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "�ۼƲ���ʱ��: C %02u:%02u\r\n",	pFile->sum_min[Q_PP1] / 60u, pFile->sum_min[Q_PP1] % 60u );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "�ۼƲ���ʱ��: D %02u:%02u\r\n",	pFile->sum_min[Q_PP2] / 60u, pFile->sum_min[Q_PP2] % 60u );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "�������(��): C %7.1fL\r\n",		pFile->vnd[Q_PP1] );												
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "�������(��): D %7.1fL\r\n",		pFile->vnd[Q_PP2] );											
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ������(��): C %6.1f L/m\r\n",	(FP32)pFile->vnd[Q_PP1] / pFile->sum_min[Q_PP1] );			
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ������(��): D %6.1f L/m\r\n",	(FP32)pFile->vnd[Q_PP2] / pFile->sum_min[Q_PP2] );			
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "�������(��): C %7.1fL\r\n",		pFile->vd[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "�������(��): D %7.1fL\r\n",		pFile->vd[Q_PP2] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "ƽ������(��): C %6.1f L/m\r\n",	(FP32)pFile->vd[Q_PP1] / pFile->sum_min[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "ƽ������(��): D %6.1f L/m\r\n",	(FP32)pFile->vd[Q_PP2] / pFile->sum_min[Q_PP2] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ����ǰ�¶�: C %6.2f ��\r\n",	pFile->sum_tr[Q_PP1] / pFile->sum_min[Q_PP1] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ����ǰ�¶�: D %6.2f ��\r\n",	pFile->sum_tr[Q_PP2] / pFile->sum_min[Q_PP2] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ����ǰѹ��: C %6.2f kPa\r\n",	pFile->sum_pr[Q_PP1] / pFile->sum_min[Q_PP1] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ����ǰѹ��: D %6.2f kPa\r\n",	pFile->sum_pr[Q_PP2] / pFile->sum_min[Q_PP2] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "ƽ������ѹ��: %6.2f kPa\r\n",	pFile->sum_Ba / pFile->sum_min[Q_PP1] );							
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
	now = get_Now();	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffert, "  == %4d/%2d/%2d %02d:%02d:%02d ==\r\n",
		t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	delay_us(1);	
	if( !USBSave("\\USB_SHI\\SHI.TXT", s + 1, ( uint8_t *)sbuffer, ( sizeof( sbuffer ) + 511 ) / 512 ) )
	{
		SD_Init();
		return FALSE; 
	}
	SD_Init();
	MsgBox( "OK! ��γ�U�� !", vbOKOnly );
	return TRUE;
}

/********************************************************************/

BOOL USBPrint_AIR( uint16_t FileNum, struct uFile_AIR const * pFile )
{
  struct	tm	t_tm;
	time_t	now;
	CHAR	sbuffer[512];
	CHAR	sbuffert[40];
	uint32_t s;
	s = ByteGetSize("\\USB_AIR\\AIR.TXT") / 512;		
 	memset( sbuffer, 0x00, 512 );	
	sprintf( sbuffert, "\r\n%s��%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "����������¼\r\n");
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
	sprintf( sbuffert, "�ļ�:%3u [��%2u��/��%2u��]\r\n", FileNum, pFile->run_loops, pFile->set_loops );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "��ʼʱ��:%2d��%2d�� %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò���ʱ��: %02u:%02u\r\n", pFile->set_time / 60u, pFile->set_time % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ۼƲ���ʱ��: %02u:%02u\r\n", pFile->sum_min / 60u, pFile->sum_min % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò���������: %5.1fL/m\r\n", pFile->set_flow[Q_PP1] * 0.1f );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ۼƲ��������: %5.1fL\r\n", pFile->sum_min * Configure.AIRSetFlow[Q_PP1] * 0.1f);		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "���ò���������: %5.1fL/m\r\n", pFile->set_flow[Q_PP2] * 0.1f );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "�ۼƲ��������: %5.1fL\r\n", pFile->sum_min * Configure.AIRSetFlow[Q_PP2] * 0.1f);		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	
	now = get_Now( );(void)_localtime_r ( &now, &t_tm );	
	sprintf( sbuffert, "  == %4d/%2d/%2d %02d:%02d:%02d ==\r\n",
	t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 ); 
	delay_us(1);	
	if( !USBSave("\\USB_AIR\\AIR.TXT", s + 1, ( uint8_t *)sbuffer, ( sizeof( sbuffer ) + 511 ) / 512 ) )
	{
		SD_Init();
		return FALSE; 
	}
	SD_Init();
	MsgBox( "OK! ��γ�U�� !", vbOKOnly );
	return TRUE;
}


/********  (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾  **** End Of File **********/
