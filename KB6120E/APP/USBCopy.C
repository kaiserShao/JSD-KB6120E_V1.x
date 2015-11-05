/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: USBCopy.C
* 创 建 者: Kaiser
* 描  述  : 文件拷贝 ( U 盘 )
* 最后修改: 2015/07/30
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
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
			 MsgBox( "硬件故障 !", vbOKOnly );
			SD_Init();
			return FALSE;
		case 2:
			MsgBox( "U盘未连接 !", vbOKOnly );
			SD_Init();
			return FALSE;
		case 3:
			MsgBox( "连接已断开 !", vbOKOnly );
			SD_Init();
			return FALSE;
		case 4:
			File_Creat();
			return TRUE;
		case 5:
			MsgBox( "未知故障 !", vbOKOnly );
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
	sprintf( sbuffert, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "粉尘采样记录\r\n");
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
	sprintf( sbuffert, "文件:%3u [第%2u次/共%2u次]\r\n", FileNum, pFile->run_loops, pFile->set_loops );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "开始时间:%2d月%2d日 %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样时间: %02u:%02u\r\n", pFile->set_time / 60u, pFile->set_time % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样流量: %5.1f L/m\r\n", pFile->set_flow * 0.1f );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样时间: %02u:%02u\r\n", pFile->sum_min / 60u, pFile->sum_min % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(工): %7.1f L\r\n", pFile->vd  );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(标): %7.1f L\r\n", pFile->vnd );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );  
	sprintf( sbuffert, "平均流量(工): %6.2f L/m\r\n", (FP32)pFile->vd  / pFile->sum_min );				
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均流量(标): %6.2f L/m\r\n", (FP32)pFile->vnd / pFile->sum_min );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前温度: %6.2f ℃\r\n", pFile->sum_tr / pFile->sum_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前压力: %6.2f kPa\r\n", pFile->sum_pr / pFile->sum_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );                 
	sprintf( sbuffert, "平均大气压力: %6.2f kPa\r\n", pFile->sum_Ba / pFile->sum_min );	
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
	MsgBox( "OK! 请拔出U盘 !", vbOKOnly );
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
	sprintf( sbuffert, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "日均采样记录\r\n");
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "文件:%3u [第%2u次/共%2u次]\r\n", FileNum, pFile->run_loops, pFile->set_loops );	
	strcat( sbuffer, sbuffert );memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "开始时间:%2d月%2d日 %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样时间: A %02u:%02u\r\n",	pFile->set_time[Q_PP1] / 60u, pFile->set_time[Q_PP1] % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样时间: B %02u:%02u\r\n",	pFile->set_time[Q_PP2] / 60u, pFile->set_time[Q_PP2] % 60u );	
	strcat( sbuffer, sbuffert );memset( sbuffert, 0x00, 40 );	
	sprintf( sbuffert, "设置采样流量: A %5.3f L/m\r\n",	pFile->set_flow[Q_PP1] * 0.001f );								
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样流量: B %5.3f L/m\r\n",	pFile->set_flow[Q_PP2] * 0.001f );								
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样时间: A %02u:%02u\r\n",	pFile->sum_min[Q_PP1] / 60u, pFile->sum_min[Q_PP1] % 60u );		
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样时间: B %02u:%02u\r\n",	pFile->sum_min[Q_PP2] / 60u, pFile->sum_min[Q_PP2] % 60u );		
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(标): A %7.1fL\r\n",		pFile->vnd[Q_PP1] );											
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(标): B %7.1fL\r\n",		pFile->vnd[Q_PP2] );											
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均流量(标): A %6.3f L/m\r\n",	(FP32)pFile->vnd[Q_PP1] / pFile->sum_min[Q_PP1] );			
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均流量(标): B %6.3f L/m\r\n",	(FP32)pFile->vnd[Q_PP2] / pFile->sum_min[Q_PP2] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "采样体积(工): A %7.1fL\r\n",		pFile->vd[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "采样体积(工): B %7.1fL\r\n",		pFile->vd[Q_PP2] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "平均流量(工): A %6.3f L/m\r\n",	(FP32)pFile->vd[Q_PP1] / pFile->sum_min[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "平均流量(工): B %6.3f L/m\r\n",	(FP32)pFile->vd[Q_PP2] / pFile->sum_min[Q_PP2] ); 
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前温度: A %6.2f ℃\r\n",	pFile->sum_tr[Q_PP1] / pFile->sum_min[Q_PP1] );				
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前温度: B %6.2f ℃\r\n",	pFile->sum_tr[Q_PP2] / pFile->sum_min[Q_PP2] );					
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前压力: A %6.2f kPa\r\n",	pFile->sum_pr[Q_PP1] / pFile->sum_min[Q_PP1] );					
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前压力: B %6.2f kPa\r\n",	pFile->sum_pr[Q_PP2] / pFile->sum_min[Q_PP2] );				
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均大气压力: %6.2f kPa\r\n",	pFile->sum_Ba / pFile->sum_min[Q_PP1] );						
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
	MsgBox( "OK! 请拔出U盘 !", vbOKOnly );
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
	sprintf( sbuffert, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
  if( Configure.InstrumentType == type_KB2400HL )
  {
    sprintf( sbuffert, "采样记录\r\n"); 
    strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  }
  else		
  {
    sprintf( sbuffert, "时均采样记录\r\n"); 
    strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  }
  sprintf( sbuffert, "文件:%3u [第%2u次/共%2u次]\r\n", FileNum, pFile->run_loops, pFile->set_loops );	
  strcat( sbuffer, sbuffert );memset( sbuffert, 0x00, 40 );
  (void)_localtime_r ( &pFile->sample_begin, &t_tm );
  sprintf( sbuffert, "开始时间:%2d月%2d日 %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "设置采样时间: C %02u:%02u\r\n",	pFile->set_time[Q_PP1] / 60u, pFile->set_time[Q_PP1] % 60u );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "设置采样时间: D %02u:%02u\r\n",	pFile->set_time[Q_PP2] / 60u, pFile->set_time[Q_PP2] % 60u );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "设置采样流量: C %5.1f L/m\r\n",	pFile->set_flow[Q_PP1] * 0.1f );							
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "设置采样流量: D %5.1f L/m\r\n",	pFile->set_flow[Q_PP2] * 0.1f );					
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "累计采样时间: C %02u:%02u\r\n",	pFile->sum_min[Q_PP1] / 60u, pFile->sum_min[Q_PP1] % 60u );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "累计采样时间: D %02u:%02u\r\n",	pFile->sum_min[Q_PP2] / 60u, pFile->sum_min[Q_PP2] % 60u );	
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "采样体积(标): C %7.1fL\r\n",		pFile->vnd[Q_PP1] );												
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "采样体积(标): D %7.1fL\r\n",		pFile->vnd[Q_PP2] );											
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均流量(标): C %6.1f L/m\r\n",	(FP32)pFile->vnd[Q_PP1] / pFile->sum_min[Q_PP1] );			
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均流量(标): D %6.1f L/m\r\n",	(FP32)pFile->vnd[Q_PP2] / pFile->sum_min[Q_PP2] );			
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "采样体积(工): C %7.1fL\r\n",		pFile->vd[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "采样体积(工): D %7.1fL\r\n",		pFile->vd[Q_PP2] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "平均流量(工): C %6.1f L/m\r\n",	(FP32)pFile->vd[Q_PP1] / pFile->sum_min[Q_PP1] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
// 	sprintf( sbuffert, "平均流量(工): D %6.1f L/m\r\n",	(FP32)pFile->vd[Q_PP2] / pFile->sum_min[Q_PP2] );
// 	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均计前温度: C %6.2f ℃\r\n",	pFile->sum_tr[Q_PP1] / pFile->sum_min[Q_PP1] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均计前温度: D %6.2f ℃\r\n",	pFile->sum_tr[Q_PP2] / pFile->sum_min[Q_PP2] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均计前压力: C %6.2f kPa\r\n",	pFile->sum_pr[Q_PP1] / pFile->sum_min[Q_PP1] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均计前压力: D %6.2f kPa\r\n",	pFile->sum_pr[Q_PP2] / pFile->sum_min[Q_PP2] );				
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "平均大气压力: %6.2f kPa\r\n",	pFile->sum_Ba / pFile->sum_min[Q_PP1] );							
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
	MsgBox( "OK! 请拔出U盘 !", vbOKOnly );
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
	sprintf( sbuffert, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
  sprintf( sbuffert, "大气采样记录\r\n");
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );	
	sprintf( sbuffert, "文件:%3u [第%2u次/共%2u次]\r\n", FileNum, pFile->run_loops, pFile->set_loops );
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "开始时间:%2d月%2d日 %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样时间: %02u:%02u\r\n", pFile->set_time / 60u, pFile->set_time % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样时间: %02u:%02u\r\n", pFile->sum_min / 60u, pFile->sum_min % 60u );
	strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样流量Ⅰ: %5.1fL/m\r\n", pFile->set_flow[Q_PP1] * 0.1f );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样体积Ⅰ: %5.1fL\r\n", pFile->sum_min * Configure.AIRSetFlow[Q_PP1] * 0.1f);		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样流量Ⅱ: %5.1fL/m\r\n", pFile->set_flow[Q_PP2] * 0.1f );		
  strcat( sbuffer, sbuffert );	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样体积Ⅱ: %5.1fL\r\n", pFile->sum_min * Configure.AIRSetFlow[Q_PP2] * 0.1f);		
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
	MsgBox( "OK! 请拔出U盘 !", vbOKOnly );
	return TRUE;
}


/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File **********/

