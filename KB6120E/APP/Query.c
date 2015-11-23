/**************** (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Query.C
* 创 建 者: Dean
* 描  述  : KB6120C 采样数据 记录/查询/打印
* 最后修改: 2013年6月5日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "USBCopy.h"
#include <stdio.h>
/**
	文件打印
*/
static	BOOL	PromptTest( void ){	return	FALSE; }

static	void	Print_File_TSP( uint16_t FileNum, struct uFile_TSP const * pFile )
{
	CHAR	sbuffer[40];
	struct	tm	t_tm;
	time_t	now;

	while( ! PrinterInit( PromptTest ))
	{
		if( vbCancel == MsgBox( "打印机未联机 !", vbRetryCancel ))
		{
			return;
		}
	}
  sprintf( sbuffer, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	PrinterPutString( sbuffer );
	PrinterPutString( "粉尘采样记录" );
	sprintf( sbuffer, "文件:%3u [第%2u次/共%2u次]", FileNum, pFile->run_loops, pFile->set_loops );	PrinterPutString( sbuffer );

	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffer, "开始时间:%2d月%2d日 %02d:%02d", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	PrinterPutString( sbuffer );

	sprintf( sbuffer, "设置采样时间: %02u:%02u", pFile->set_time / 60u, pFile->set_time % 60u );	PrinterPutString( sbuffer );
	sprintf( sbuffer, "设置采样流量: %5.1f L/m", pFile->set_flow * 0.1f );							PrinterPutString( sbuffer );

	sprintf( sbuffer, "累计采样时间: %02u:%02u", pFile->sum_min / 60u, pFile->sum_min % 60u ); 		PrinterPutString( sbuffer );
	sprintf( sbuffer, "采样体积(工): %7.1f L", pFile->vd  );										PrinterPutString( sbuffer );
	sprintf( sbuffer, "采样体积(标): %7.1f L", pFile->vnd );										PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均流量(工): %6.2f L/m", (FP32)pFile->vd  / pFile->sum_min );				PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均流量(标): %6.2f L/m", (FP32)pFile->vnd / pFile->sum_min );				PrinterPutString( sbuffer );

	sprintf( sbuffer, "平均计前温度: %6.2f ℃ ", pFile->sum_tr / pFile->sum_min );					PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均计前压力: %6.2f kPa", pFile->sum_pr / pFile->sum_min );					PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均大气压力: %6.2f kPa", pFile->sum_Ba / pFile->sum_min );					PrinterPutString( sbuffer );

	now = get_Now();
	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffer, "  == %4d/%2d/%2d %02d:%02d:%02d ==  ",
	t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	PrinterPutString( sbuffer );

	PrinterPutString( " " );	PrinterPutString( " " );
}

static	void	Print_File_R24( uint16_t FileNum, struct uFile_R24 const * pFile )
{
	CHAR	sbuffer[40];
	struct	tm	t_tm;
	time_t	now;

	while( ! PrinterInit( PromptTest ))
	{
		if( vbCancel == MsgBox( "打印机未联机 !", vbRetryCancel ))
		{
			return;
		}
	}

	sprintf( sbuffer, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	PrinterPutString( sbuffer );
	PrinterPutString( "日均采样记录" );
	sprintf( sbuffer, "文件:%3u [第%2u次/共%2u次]", FileNum, pFile->run_loops, pFile->set_loops );	PrinterPutString( sbuffer );

	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffer, "开始时间:%2d月%2d日 %02d:%02d", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	PrinterPutString( sbuffer );

	sprintf( sbuffer, "设置采样时间: A %02u:%02u",	pFile->set_time[Q_PP1] / 60u, pFile->set_time[Q_PP1] % 60u );	PrinterPutString( sbuffer );
	sprintf( sbuffer, "设置采样时间: B %02u:%02u",	pFile->set_time[Q_PP2] / 60u, pFile->set_time[Q_PP2] % 60u );	PrinterPutString( sbuffer );
	sprintf( sbuffer, "设置采样流量: A %5.3f L/m",	pFile->set_flow[Q_PP1] * 0.001f );								PrinterPutString( sbuffer );
	sprintf( sbuffer, "设置采样流量: B %5.3f L/m",	pFile->set_flow[Q_PP2] * 0.001f );								PrinterPutString( sbuffer );
	sprintf( sbuffer, "累计采样时间: A %02u:%02u",	pFile->sum_min[Q_PP1] / 60u, pFile->sum_min[Q_PP1] % 60u );		PrinterPutString( sbuffer );
	sprintf( sbuffer, "累计采样时间: B %02u:%02u",	pFile->sum_min[Q_PP2] / 60u, pFile->sum_min[Q_PP2] % 60u );		PrinterPutString( sbuffer );
	sprintf( sbuffer, "采样体积(标): A %7.1fL",		pFile->vnd[Q_PP1] );											PrinterPutString( sbuffer );
	sprintf( sbuffer, "采样体积(标): B %7.1fL",		pFile->vnd[Q_PP2] );											PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均流量(标): A %6.3f L/m",	(FP32)pFile->vnd[Q_PP1] / pFile->sum_min[Q_PP1] );				PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均流量(标): B %6.3f L/m",	(FP32)pFile->vnd[Q_PP2] / pFile->sum_min[Q_PP2] );				PrinterPutString( sbuffer );
	sprintf( sbuffer, "采样体积(工): A %7.1fL",		pFile->vd[Q_PP1] );											PrinterPutString( sbuffer );
	sprintf( sbuffer, "采样体积(工): B %7.1fL",		pFile->vd[Q_PP2] );											PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均流量(工): A %6.3f L/m",	(FP32)pFile->vd[Q_PP1] / pFile->sum_min[Q_PP1] );				PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均流量(工): B %6.3f L/m",	(FP32)pFile->vd[Q_PP2] / pFile->sum_min[Q_PP2] );				PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均计前温度: A %6.2f ℃",	pFile->sum_tr[Q_PP1] / pFile->sum_min[Q_PP1] );					PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均计前温度: B %6.2f ℃ ",	pFile->sum_tr[Q_PP2] / pFile->sum_min[Q_PP2] );					PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均计前压力: A %6.2f kPa",	pFile->sum_pr[Q_PP1] / pFile->sum_min[Q_PP1] );					PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均计前压力: B %6.2f kPa",	pFile->sum_pr[Q_PP2] / pFile->sum_min[Q_PP2] );					PrinterPutString( sbuffer );
	sprintf( sbuffer, "平均大气压力: %6.2f kPa",	pFile->sum_Ba / pFile->sum_min[Q_PP1] );						PrinterPutString( sbuffer );


	now = get_Now();	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffer, "  == %4d/%2d/%2d %02d:%02d:%02d ==  ",
		t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	PrinterPutString( sbuffer );

	PrinterPutString( " " );	PrinterPutString( " " );
}

static	void	Print_File_SHI( uint16_t FileNum, struct uFile_SHI const * pFile )
{
	CHAR	sbuffer[40];
	struct	tm	t_tm;
	time_t	now;

	while( ! PrinterInit( PromptTest ))
	{
		if( vbCancel == MsgBox( "打印机未联机 !", vbRetryCancel ))
		{
			return;
		}
	}
  sprintf( sbuffer, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
  PrinterPutString( sbuffer );
  if(  Configure.InstrumentType == type_KB2400HL )
  {
    PrinterPutString( "采样记录" );		
  }
  else
  {
    PrinterPutString( "时均采样记录" );
  }
  sprintf( sbuffer, "文件:%3u [第%2u次/共%2u次]", FileNum, pFile->run_loops, pFile->set_loops );	PrinterPutString( sbuffer );
  (void)_localtime_r ( &pFile->sample_begin, &t_tm );
  sprintf( sbuffer, "开始时间:%2d月%2d日 %02d:%02d", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
  PrinterPutString( sbuffer );

  sprintf( sbuffer, "设置采样时间: C %02u:%02u",	pFile->set_time[Q_PP1] / 60u, pFile->set_time[Q_PP1] % 60u );		PrinterPutString( sbuffer );
  sprintf( sbuffer, "设置采样时间: D %02u:%02u",	pFile->set_time[Q_PP2] / 60u, pFile->set_time[Q_PP2] % 60u );		PrinterPutString( sbuffer );
  sprintf( sbuffer, "设置采样流量: C %5.1f L/m",	pFile->set_flow[Q_PP1] * 0.1f );									PrinterPutString( sbuffer );
  sprintf( sbuffer, "设置采样流量: D %5.1f L/m",	pFile->set_flow[Q_PP2] * 0.1f );									PrinterPutString( sbuffer );
  sprintf( sbuffer, "累计采样时间: C %02u:%02u",	pFile->sum_min[Q_PP1] / 60u, pFile->sum_min[Q_PP1] % 60u );			PrinterPutString( sbuffer );
  sprintf( sbuffer, "累计采样时间: D %02u:%02u",	pFile->sum_min[Q_PP2] / 60u, pFile->sum_min[Q_PP2] % 60u );			PrinterPutString( sbuffer );
  sprintf( sbuffer, "采样体积(标): C %7.1fL",		pFile->vnd[Q_PP1] );												PrinterPutString( sbuffer );
  sprintf( sbuffer, "采样体积(标): D %7.1fL",		pFile->vnd[Q_PP2] );												PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均流量(标): C %6.1f L/m",	(FP32)pFile->vnd[Q_PP1] / pFile->sum_min[Q_PP1] );					PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均流量(标): D %6.1f L/m",	(FP32)pFile->vnd[Q_PP2] / pFile->sum_min[Q_PP2] );					PrinterPutString( sbuffer );
  sprintf( sbuffer, "采样体积(工): C %7.1fL",		pFile->vd[Q_PP1] );												PrinterPutString( sbuffer );
  sprintf( sbuffer, "采样体积(工): D %7.1fL",		pFile->vd[Q_PP2] );												PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均流量(工): C %6.1f L/m",	(FP32)pFile->vd[Q_PP1] / pFile->sum_min[Q_PP1] );					PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均流量(工): D %6.1f L/m",	(FP32)pFile->vd[Q_PP2] / pFile->sum_min[Q_PP2] );					PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均计前温度: C %6.2f ℃",	pFile->sum_tr[Q_PP1] / pFile->sum_min[Q_PP1] );						PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均计前温度: D %6.2f ℃ ",	pFile->sum_tr[Q_PP2] / pFile->sum_min[Q_PP2] );						PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均计前压力: C %6.2f kPa",	pFile->sum_pr[Q_PP1] / pFile->sum_min[Q_PP1] );						PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均计前压力: D %6.2f kPa",	pFile->sum_pr[Q_PP2] / pFile->sum_min[Q_PP2] );						PrinterPutString( sbuffer );
  sprintf( sbuffer, "平均大气压力: %6.2f kPa",	pFile->sum_Ba / pFile->sum_min[Q_PP1] );							PrinterPutString( sbuffer );
	now = get_Now();	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffer, "  == %4d/%2d/%2d %02d:%02d:%02d ==  ",
		t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );						PrinterPutString( sbuffer );
	PrinterPutString( " " );	PrinterPutString( " " );
}

/***/
static	void	Print_File_AIR( uint16_t FileNum, struct uFile_AIR const * pFile )
{
	CHAR	sbuffer[40];
	struct	tm	t_tm;
	time_t	now;

	while( ! PrinterInit( PromptTest ))
	{
		if( vbCancel == MsgBox( "打印机未联机 !", vbRetryCancel ))
		{
			return;
		}
	}
	sprintf( sbuffer, "\r\n%s型%s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	PrinterPutString( sbuffer );
	PrinterPutString( "大气采样记录" );
	sprintf( sbuffer, "文件:%3u [第%2u次/共%2u次]", FileNum, pFile->run_loops, pFile->set_loops );	PrinterPutString( sbuffer );

	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffer, "开始时间:%2d月%2d日 %02d:%02d", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	PrinterPutString( sbuffer );

	sprintf( sbuffer, "设置采样时间: %02u:%02u", pFile->set_time / 60u, pFile->set_time % 60u );	PrinterPutString( sbuffer );
	sprintf( sbuffer, "设置采样流量Ⅰ: %5.1f L/m", pFile->set_flow[Q_PP1] * 0.1f );							PrinterPutString( sbuffer );
	sprintf( sbuffer, "累计采样体积Ⅰ: %5.1f L", pFile->sum_min * Configure.AIRSetFlow[Q_PP1] * 0.1f );							PrinterPutString( sbuffer );
	sprintf( sbuffer, "设置采样流量Ⅱ: %5.1f L/m", pFile->set_flow[Q_PP2] * 0.1f );							PrinterPutString( sbuffer );
	sprintf( sbuffer, "累计采样体积Ⅱ: %5.1f L", pFile->sum_min * Configure.AIRSetFlow[Q_PP2] * 0.1f );							PrinterPutString( sbuffer );
	sprintf( sbuffer, "累计采样时间: %02u:%02u", pFile->sum_min / 60u, pFile->sum_min % 60u ); 		PrinterPutString( sbuffer );
	now = get_Now();
	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffer, "  == %4d/%2d/%2d %02d:%02d:%02d ==  ",
	t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	PrinterPutString( sbuffer );

	PrinterPutString( " " );	PrinterPutString( " " );
}


/************************************************************/
static void FPrintf_TSP(uint16_t FileNum, struct uFile_TSP  const * pFile)
{
	  static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "文件输出" },
		{ 0x0301u, "打印机" },	
		{ 0x0601u, " U 盘 " },
	};

	uint8_t	item = 1u;
  BOOL state = FALSE;
	do
	{
		cls();
		Menu_Redraw( menu );
		SamplerTypeShow( 0x000Au );
     
		item = Menu_Select( menu, item );
		
		switch( item )
		{
		//	在主菜单下按取消键，显示大气压、恒温箱温度等环境参数
		case 1:
			Print_File_TSP( FileNum, pFile );
			state = TRUE;	
			break;
		case 2:		
			if( USB_PrintInit() )
			{
        USBPrint_TSP( FileNum, pFile );
				state = TRUE;			
			}
			break;
		case enumSelectESC: 
			state = TRUE;
			break;
		case enumSelectXCH: 
			break;
		}
	}while( state != TRUE );
}

/*********************************************************************/
static void FPrintf_R24(uint16_t FileNum, struct uFile_R24  const * pFile)
{
	  static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "文件输出" },
		{ 0x0301u, "打印机" },	
		{ 0x0601u, " U 盘 " },
	};

	uint8_t	item = 1u;
  BOOL state = FALSE;
	do
	{
		cls();
		Menu_Redraw( menu );
		SamplerTypeShow( 0x000Au );
     
		item = Menu_Select( menu, item );
		
		switch( item )
		{
		//	在主菜单下按取消键，显示大气压、恒温箱温度等环境参数
		case 1:
			Print_File_R24( FileNum, pFile );
			state = TRUE;	
			break;
		case 2:		
			if( USB_PrintInit() )
			{
        USBPrint_R24( FileNum, pFile );
				state = TRUE;			
			}
			break;
		case enumSelectESC: 
			state = TRUE;
			break;
		case enumSelectXCH: 
			break;
		}
	}while( state != TRUE );
}

/***********************************************************************/
static void FPrintf_SHI(uint16_t FileNum, struct uFile_SHI  const * pFile)
{
	  static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "文件输出" },
		{ 0x0301u, "打印机" },	
		{ 0x0601u, " U 盘 " },
	};

	uint8_t	item = 1u;
  BOOL state = FALSE;
	do
	{
		cls();
		Menu_Redraw( menu );
		SamplerTypeShow( 0x000Au );
     
		item = Menu_Select( menu, item );
		
		switch( item )
		{
		//	在主菜单下按取消键，显示大气压、恒温箱温度等环境参数
		case 1:
			Print_File_SHI( FileNum, pFile );
			state = TRUE;	
			break;
		case 2:		
			if( USB_PrintInit() )
			{
        USBPrint_SHI( FileNum, pFile );
				state = TRUE;			
			}
			break;
		case enumSelectESC: 
			state = TRUE;
			break;
		case enumSelectXCH: 
			break;
		}
	}while( state != TRUE );
}
/************************************************************************/
static void FPrintf_AIR(uint16_t FileNum, struct uFile_AIR  const * pFile)
{
	  static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "文件输出" },
		{ 0x0301u, "打印机" },	
		{ 0x0601u, " U 盘 " },
	};

	uint8_t	item = 1u;
  BOOL state = FALSE;
	do
	{
		cls();
		Menu_Redraw( menu );
		SamplerTypeShow( 0x000Au );
     
		item = Menu_Select( menu, item );
		
		switch( item )
		{
		//	在主菜单下按取消键，显示大气压、恒温箱温度等环境参数
		case 1:
			Print_File_AIR( FileNum, pFile );
			state = TRUE;	
			break;
		case 2:		
			if( USB_PrintInit() )
			{
        USBPrint_AIR( FileNum, pFile );
        state = TRUE;
      }
			break;
		case enumSelectESC: 
			state = TRUE;
			break;
		case enumSelectXCH: 
			break;
		}
	}while( state != TRUE );
}

/**/
/**
	文件查询
*/
void	Query_File_TSP( void )
{
	uint16_t	FileNum;
	struct	uFile_TSP File;
	enum 
	{
		opt_exit,
		opt_tsp_1,   opt_tsp_2,   opt_tsp_3,   opt_tsp_4,
		opt_max, opt_min = opt_tsp_1
	};
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint8_t	option = opt_min;
	FileNum = SampleSet[Q_TSP].FileNum;

	if (( FileNum < 1 ) || ( FileNum > FileNum_Max ))
	{	//	文件全空！主要指 FileNum 是 0 的情况。
		beep();
		MsgBox( "粉尘文件:[空]", vbOKOnly );
		return;	//	中止 查询
	}
  
	File_Load_TSP ( FileNum, &File );
	
	do {
		cls();	

		if ( 0u == File.sample_begin )
		{
			Lputs( 0x0000u, "粉尘文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, "[空]" );
		}
		else
		{
			Lputs( 0x0000u, "粉尘文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, NULL );
			switch ( option )
			{
			default:
			case opt_tsp_1:
				Lputs( 0x000Bu, "[1/4]" );
				Lputs( 0x0200u, "开始:" );	    ShowDATE( 0x0205u, File.sample_begin );
				Lputs( 0x0400u, "采样次数:" );	ShowI16U( 0x0409u, File.run_loops,   0x0200u, NULL );
				Lputs( 0x0600u, "设置次数:" );	ShowI16U( 0x0609u, File.set_loops,   0x0200u, NULL );
				break;
			case opt_tsp_2:
				Lputs( 0x000Bu, "[2/4]" );
				Lputs( 0x0200u, "温  度:" );	ShowFP32( 0x0207u, File.sum_tr / File.sum_min, 0x0602u, "℃" );
				Lputs( 0x0400u, "压  力:" );	ShowFP32( 0x0407u, File.sum_pr / File.sum_min, 0x0602u, "kPa" );
				Lputs( 0x0600u, "大气压:" );	ShowFP32( 0x0607u, File.sum_Ba / File.sum_min, 0x0602u, "kPa" );
			//	Lputs( 0x0200u, "环境温度:" );	ShowFP32( 0x0207u, File.sum_tr / File.sum_min, 0x0602u, "℃" );
				break;
			case opt_tsp_3:
				Lputs( 0x000Bu, "[3/4]" );
				Lputs( 0x0200u, "工况:" );		ShowFP32( 0x0205u, File.vd     / File.sum_min, 0x0701u, "L/m" );
				Lputs( 0x0400u, "标况:" );		ShowFP32( 0x0405u, File.vnd    / File.sum_min, 0x0701u, "L/m" );
				Lputs( 0x0600U, "设置:" );		ShowI16U( 0x0605u, File.set_flow,              0x0701u, "L/m" );
				break;
			case opt_tsp_4:
				Lputs( 0x000Bu, "[4/4]" );
				Lputs( 0x0200u, "工况体积:" );	ShowFP32( 0x0209u, File.vd,                    0x0600u, "L" );
				Lputs( 0x0400u, "标况体积:" );	ShowFP32( 0x0409u, File.vnd,                   0x0600u, "L" );
				Lputs( 0x0600u, "采样时间:" );	ShowTIME( 0x060Bu, File.sum_min  );
			//	Lputs( 0x0600u, "设置时间:" );	ShowTIME( 0x0609u, File.set_time );
				break;
			}
		}		
		switch ( getKey() )
		{
		case K_UP:		//	向前（时间较早的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == 1u )
			{
				fname = 1;
				beep();
				MsgBox( "无更多文件!", vbOKOnly );
			}
			else
			{
				fname = FileNum - 1u;			
			}

			File_Load_TSP ( fname, &File );
			FileNum = fname;
			if ( 0u == File.sample_begin )
			{
				beep();	//	此文件可能无效！
			}
		}	break;

		case K_DOWN:	//	向后（时间较新的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == FileNum_Max )
			{
				fname = FileNum_Max;
				beep();
				MsgBox( "文件满！", vbOKOnly );
				return;					
			}
			else
			{
				fname = FileNum + 1u;
			}

			File_Load_TSP ( fname, &File );
			if( fname > SampleSet[Q_TSP].FileNum )
			{
				beep();//	到达最后，"后"面无有效数据
				File_Load_TSP ( FileNum, &File );
			}
			else	
			{
				if ( 0u == File.sample_begin )
					beep();
				FileNum = fname;
			}
		}	break;
  
		case K_OK:
			if ( 0u != File.sample_begin )
			{
				FPrintf_TSP( FileNum, &File );
			}
			
			break;

		case K_RIGHT:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;

		case K_LEFT:
			if ( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;

		case K_ESC:
			option = opt_exit;
			break;
			
		case K_OK_UP:	
			if ( gray < 2050u )
			{
				++gray;
			}
			if( ! releaseKey( K_OK_UP,100 ))
			{
				while( ! releaseKey( K_OK_UP, 3 ))
				{
					++gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;
		case K_OK_DOWN:
			if ( gray >  200u )
			{
				--gray;
			}
			if( ! releaseKey( K_OK_DOWN,100 ))
			{
				while( ! releaseKey( K_OK_DOWN, 1 ))
				{
					--gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;

		case K_OK_RIGHT:
			if ( gray < ( 2000u - 50u ))
			{ 
				gray += 100u;
			}
			graychanged = true;
			break;
		case K_OK_LEFT:	
			if ( gray > ( 200 + 20u ))
			{
				gray -= 20u;
			}
			graychanged = true;
			break;
		default:
			break;
		}
		if( graychanged == true )
		{
			DisplaySetGrayVolt( gray * 0.01f );
			Configure.DisplayGray = gray;
			ConfigureSave();
			graychanged = FALSE;
		}	
		
	} while ( opt_exit != option );
}
/************************************************************/
void	Query_File_R24( void )
{
	struct	uFile_R24	File;
	uint16_t	FileNum;
	enum 
	{
		opt_exit,
		opt_A2X_1,
		opt_A2X_A1,   opt_A2X_A2,   opt_A2X_A3,opt_A2X_A4,
		opt_A2X_B1,   opt_A2X_B2,   opt_A2X_B3,opt_A2X_B4,
		opt_max, opt_min = 1u
	};
	uint8_t	option = opt_min;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	

	FileNum = SampleSet[Q_R24].FileNum;

	if (( FileNum < 1 ) || ( FileNum > FileNum_Max ))
	{
		beep();
		MsgBox( "日均文件:[空]", vbOKOnly );
		return;	//	中止 查询
	}

	File_Load_R24 ( FileNum, &File );
	
	do {
		cls();

		if ( 0u == File.sample_begin )
		{
			Lputs( 0x0000u, "日均文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, "[空]" );
		} 
		else
		{
			Lputs( 0x0000u, "日均文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, NULL );
			switch ( option )
			{
			default:
			case opt_A2X_1:
				Lputs( 0x000Bu, "[1/9]" );
				Lputs( 0x0200u, "开始:" );	    ShowDATE( 0x0205u, File.sample_begin );
				Lputs( 0x0400u, "采样次数:" );	ShowI16U( 0x0409u, File.run_loops,   0x0200u, NULL );
				Lputs( 0x0600u, "设置次数:" );	ShowI16U( 0x0609u, File.set_loops,   0x0200u, NULL );
			
				break;
			case opt_A2X_A1:
				Lputs( 0x000Bu, "[2/9]" );
				Lputs( 0x0200u, "温 度A:" );	ShowFP32( 0x0207u, File.sum_tr[Q_PP1] / File.sum_min[Q_PP1], 0x0602u, "℃" );
				Lputs( 0x0400u, "压 力A:" );	ShowFP32( 0x0407u, File.sum_pr[Q_PP1] / File.sum_min[Q_PP1], 0x0602u, "kPa" );
				Lputs( 0x0600u, "大气压:" );	ShowFP32( 0x0607u, File.sum_Ba        / File.sum_min[Q_PP1], 0x0602u, "kPa" );
				break;
			
			case opt_A2X_A2:
				Lputs( 0x000Bu, "[3/9]" );
				Lputs( 0x0300U, "采样流量A:" );
				Lputs( 0x0600U, "标况:" );		ShowFP32( 0x0605u, File.vnd[Q_PP1]   / File.sum_min[Q_PP1], 0x0703u, "L/m" );
				break;

			case opt_A2X_A3:
				Lputs( 0x000Bu, "[4/9]" );
				Lputs( 0x0300u, "采样体积A:" );	
				Lputs( 0x0600u, "标况:" );		ShowFP32( 0x0605u, File.vnd[Q_PP1],  0x0902u, "L " );
				break;
				
			case opt_A2X_A4:
				Lputs( 0x000Bu, "[5/9]" );
				Lputs( 0x0200u, "采样时间A:" );
				Lputs( 0x0400u, "累计时间:" );	ShowTIME( 0x040Bu, File.sum_min[Q_PP1]  );
				Lputs( 0x0600u, "设置时间:" );	ShowTIME( 0x060Bu, File.set_time[Q_PP1] );
				break;

			case opt_A2X_B1:
				Lputs( 0x000Bu, "[6/9]" );
				Lputs( 0x0200u, "温 度B:" );	ShowFP32( 0x0207u, File.sum_tr[Q_PP2] / File.sum_min[Q_PP2], 0x0602u, "℃" );
				Lputs( 0x0400u, "压 力B:" );	ShowFP32( 0x0407u, File.sum_pr[Q_PP2] / File.sum_min[Q_PP2], 0x0602u, "kPa" );
				Lputs( 0x0600u, "大气压:" );	ShowFP32( 0x0607u, File.sum_Ba         / File.sum_min[Q_PP1], 0x0602u, "kPa" );
				break;
			
			case opt_A2X_B2:
				Lputs( 0x000Bu, "[7/9]" );
				Lputs( 0x0300U, "采样流量B:" );
				Lputs( 0x0600U, "标况:" );		ShowFP32( 0x0605u, File.vnd[Q_PP2]   / File.sum_min[Q_PP2], 0x0703u, "L/m" );
				break;

			case opt_A2X_B3:
				Lputs( 0x000Bu, "[8/9]" );
				Lputs( 0x0300u, "采样体积B:" );	
				Lputs( 0x0600u, "标况:" );		ShowFP32( 0x0605u, File.vnd[Q_PP2],  0x0902u, "L " );
				break;
				
			case opt_A2X_B4:

				Lputs( 0x000Bu, "[9/9]" );
				Lputs( 0x0200u, "采样时间B:" );
				Lputs( 0x0400u, "累计时间:" );	ShowTIME( 0x040Bu, File.sum_min[Q_PP2]  );
				Lputs( 0x0600u, "设置时间:" );	ShowTIME( 0x060Bu, File.set_time[Q_PP2] );
				break;
			}
		}
		
		switch ( getKey())
		{
		case K_UP:		//	向前（时间较早的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == 1u )
			{
				fname = 1;
				beep();
				MsgBox( "无更多文件!", vbOKOnly );
			}
			else
			{
				fname = FileNum - 1u;			
			}

			File_Load_R24 ( fname, &File );
			FileNum = fname;
			if ( 0u == File.sample_begin )
			{
				beep();	//	此文件可能无效！
			}
		}	break;

		case K_DOWN:	//	向后（时间较新的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == FileNum_Max )
			{
				fname = FileNum_Max;
				beep();
				MsgBox( "文件满！", vbOKOnly );
				return;					
			}
			else
			{
				fname = FileNum + 1u;
			}

			File_Load_R24 ( fname, &File );
			if( fname > SampleSet[Q_R24].FileNum )
			{
				beep();//	到达最后，"后"面无有效数据
				File_Load_R24 ( FileNum, &File );
			}
			else	
			{
				if ( 0u == File.sample_begin )
					beep();
				FileNum = fname;
			}
		}	break;

		case K_OK:
			if ( 0u != File.sample_begin )
			{
				FPrintf_R24( FileNum, &File );
			}
			break;

		case K_RIGHT:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;

		case K_LEFT:
			if ( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;

		case K_ESC:
			option = opt_exit;
			break;
			
		case K_OK_UP:	
			if ( gray < 2050u )
			{
				++gray;
			}
			if( ! releaseKey( K_OK_UP,100 ))
			{
				while( ! releaseKey( K_OK_UP, 3 ))
				{
					++gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;
		case K_OK_DOWN:
			if ( gray >  200u )
			{
				--gray;
			}
			if( ! releaseKey( K_OK_DOWN,100 ))
			{
				while( ! releaseKey( K_OK_DOWN, 1 ))
				{
					--gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;

		case K_OK_RIGHT:
			if ( gray < ( 2000u - 50u ))
			{ 
				gray += 100u;
			}
			graychanged = true;
			break;
		case K_OK_LEFT:	
			if ( gray > ( 200 + 20u ))
			{
				gray -= 20u;
			}
			graychanged = true;
			break;
		default:
			break;
		}
		if( graychanged == true )
		{
			DisplaySetGrayVolt( gray * 0.01f );
			Configure.DisplayGray = gray;
			ConfigureSave();
			graychanged = FALSE;
		}	
		
	} while ( opt_exit != option );
}
/*************************************************/
void	Query_File_SHI( void )
{
	struct	uFile_SHI	File;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint16_t	FileNum;
	enum 
	{
		opt_exit,
		opt_A2X_1,
		opt_A2X_A1,   opt_A2X_A2,   opt_A2X_A3,
		opt_A2X_B1,   opt_A2X_B2,   opt_A2X_B3,
		opt_max, opt_min = 1u
	};
	uint8_t	option = opt_min;

	FileNum = SampleSet[Q_SHI].FileNum;
	
	if (( FileNum < 1 ) || ( FileNum > FileNum_Max ))
	{
		beep();
		MsgBox( "时均文件:[空]", vbOKOnly );
		return;	//	中止 查询
	}
	
	File_Load_SHI ( FileNum, &File );

	do {
		
		cls();
		if ( 0u == File.sample_begin )
		{
			if( Configure.InstrumentType == type_KB2400HL )
			{
				Lputs( 0x0000u, "文件号" );	ShowI16U( 0x0008u, FileNum, 0x0300u, "[空]" );
			}
      else
			{
				Lputs( 0x0000u, "时均文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, "[空]" );
			}
		} 
		else
		{
			if( Configure.InstrumentType == type_KB2400HL )
			{
				Lputs( 0x0000u, "文件号" );	ShowI16U( 0x0008u, FileNum, 0x0300u, NULL );
			}
			else
			{
				Lputs( 0x0000u, "时均文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, NULL );
			}
      switch ( option )
      {
      default:
      case opt_A2X_1:
        Lputs( 0x000Bu, "[1/7]" );
        Lputs( 0x0200u, "开始:" );	    ShowDATE( 0x0205u, File.sample_begin );
        Lputs( 0x0400u, "采样次数:" );	ShowI16U( 0x0409u, File.run_loops,   0x0200u, NULL );
        Lputs( 0x0600u, "设置次数:" );	ShowI16U( 0x0609u, File.set_loops,   0x0200u, NULL );
      
        break;
      case opt_A2X_A1:
        Lputs( 0x000Bu, "[2/7]" );
        {
          Lputs( 0x0200u, "温 度C:" );	ShowFP32( 0x0207u, File.sum_tr[Q_PP1] / File.sum_min[Q_PP1], 0x0602u, "℃" );
          Lputs( 0x0400u, "压 力C:" );	ShowFP32( 0x0407u, File.sum_pr[Q_PP1] / File.sum_min[Q_PP1], 0x0602u, "kPa" );
          Lputs( 0x0600u, "大气压:" );	ShowFP32( 0x0607u, File.sum_Ba         / File.sum_min[Q_PP1], 0x0602u, "kPa" );
        }
        break;
      
      case opt_A2X_A2:
        Lputs( 0x000Bu, "[3/7]" );
        Lputs( 0x0300U, "标况C:" );		ShowFP32( 0x0307u, File.vnd [Q_PP1]   / File.sum_min[Q_PP1], 0x0503u, "L/m" );
        Lputs( 0x0600U, "设置C:" );		ShowI16U( 0x0607u, File.set_flow[Q_PP1] * 100,               0x0503u, "L/m" );
        break;

      case opt_A2X_A3:
        Lputs( 0x000Bu, "[4/7]" );
        Lputs( 0x0200u, "体积C:" );		ShowFP32( 0x0206u, File.vnd[Q_PP1],                           0x0802u, "L" );
        Lputs( 0x0400u, "采样时间:" );	ShowTIME( 0x0409u, File.sum_min[Q_PP1]  );
        Lputs( 0x0600u, "设置时间:" );	ShowTIME( 0x0609u, File.set_time[Q_PP1] );
        break;

      case opt_A2X_B1:
        Lputs( 0x000Bu, "[5/7]" );
        {
          Lputs( 0x0200u, "温 度D:" );	ShowFP32( 0x0207u, File.sum_tr[Q_PP2] / File.sum_min[Q_PP2], 0x0602u, "℃" );
          Lputs( 0x0400u, "压 力D:" );	ShowFP32( 0x0407u, File.sum_pr[Q_PP2] / File.sum_min[Q_PP2], 0x0602u, "kPa" );
          Lputs( 0x0600u, "大气压:" );	ShowFP32( 0x0607u, File.sum_Ba         / File.sum_min[Q_PP1], 0x0602u, "kPa" );
        }
        break;
      
      case opt_A2X_B2:
        Lputs( 0x000Bu, "[6/7]" );
        Lputs( 0x0300U, "标况D:" );		ShowFP32( 0x0307u, File.vnd [Q_PP2]   / File.sum_min[Q_PP2], 0x0503u, "L/m" );
        Lputs( 0x0600U, "设置D:" );		ShowI16U( 0x0607u, File.set_flow[Q_PP2] * 100,               0x0503u, "L/m" );
        break;

      case opt_A2X_B3:
        Lputs( 0x000Bu, "[7/7]" );
        Lputs( 0x0200u, "体积D:" );		ShowFP32( 0x0206u, File.vnd[Q_PP2],                           0x0802u, "L" );
        Lputs( 0x0400u, "采样时间:" );	ShowTIME( 0x0409u, File.sum_min[Q_PP2]  );
        Lputs( 0x0600u, "设置时间:" );	ShowTIME( 0x0609u, File.set_time[Q_PP2] );
        break;
      }		
		}		
		switch ( getKey())
		{
		case K_UP:		//	向前（时间较早的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == 1u )
			{
				fname = 1;
				beep();
				MsgBox( "无更多文件!", vbOKOnly );
			}
			else
			{
				fname = FileNum - 1u;			
			}

			File_Load_SHI ( fname, &File );
			FileNum = fname;
			if ( 0u == File.sample_begin )
			{
				beep();	//	此文件可能无效！
			}
		}	break;

		case K_DOWN:	//	向后（时间较新的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == FileNum_Max )
			{
				fname = FileNum_Max;
				beep();
				MsgBox( "文件满！", vbOKOnly );
				return;				
			}
			else
			{
				fname = FileNum + 1u;
			}

			File_Load_SHI ( fname, &File );
			if( fname > SampleSet[Q_SHI].FileNum )
			{
				beep();//	到达最后，"后"面无有效数据
				File_Load_SHI ( FileNum, &File );
			}
			else	
			{
				if ( 0u == File.sample_begin )
					beep();
				FileNum = fname;
			}
		}	break;

		case K_OK:
			if ( 0u != File.sample_begin )
			{
          FPrintf_SHI( FileNum, &File );
			}
			break;

		case K_RIGHT:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;

		case K_LEFT:
			if ( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;

		case K_ESC:
			option = opt_exit;
			break;

		case K_OK_UP:	
			if ( gray < 2050u )
			{
				++gray;
			}
			if( ! releaseKey( K_OK_UP,100 ))
			{
				while( ! releaseKey( K_OK_UP, 3 ))
				{
					++gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;
		case K_OK_DOWN:
			if ( gray >  200u )
			{
				--gray;
			}
			if( ! releaseKey( K_OK_DOWN,100 ))
			{
				while( ! releaseKey( K_OK_DOWN, 1 ))
				{
					--gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;
		case K_OK_RIGHT:
			if ( gray < ( 2000u - 50u ))
			{ 
				gray += 100u;
			}
			graychanged = true;
			break;
		case K_OK_LEFT:	
			if ( gray > ( 200 + 20u ))
			{
				gray -= 20u;
			}
			graychanged = true;
			break;
		default:
			break;
		}
		if( graychanged == true )
		{
			DisplaySetGrayVolt( gray * 0.01f );
			Configure.DisplayGray = gray;
			ConfigureSave();
			graychanged = FALSE;
		}	
	} while ( opt_exit != option );
}
/*************************************************/
void	Query_File_AIR( void )
{
	 uint16_t	FileNum;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
   struct	uFile_AIR	File;
	enum 
	{
		opt_exit,
		opt_air_1, opt_air_2,opt_air_3,opt_air_4,
		opt_max, opt_min = 1u
	};
	uint8_t	option = opt_min;

	FileNum = SampleSet[Q_AIR].FileNum;
	if (( FileNum < 1 ) || ( FileNum > FileNum_Max ))
	{
		beep();
		MsgBox( "大气文件:[空]", vbOKOnly );
		return;	//	中止 查询
	}
	
	File_Load_AIR ( FileNum, &File );

	do {		
		cls();
		if ( 0u == File.sample_begin )
		{
			Lputs( 0x0000u, "大气文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, "[空]" );
		} 
		else
		{
			Lputs( 0x0000u, "大气文件" );	ShowI16U( 0x0008u, FileNum, 0x0300u, NULL );
			switch ( option )
			{
			case opt_air_1:
				Lputs( 0x000Bu, "[1/4]" );
				Lputs( 0x0200u, "开始:" );	    ShowDATE( 0x0205u, File.sample_begin );
				Lputs( 0x0400u, "采样次数:" );	ShowI16U( 0x0409u, File.run_loops,   0x0200u, NULL );
				Lputs( 0x0600u, "设置次数:" );	ShowI16U( 0x0609u, File.set_loops,   0x0200u, NULL );
				break;
			case opt_air_2:
				Lputs( 0x000Bu, "[2/4]" );
				Lputs( 0x0300u, "采样时间:" );	ShowTIME( 0x0309u, File.sum_min  );
				Lputs( 0x0600u, "设置时间:" );	ShowTIME( 0x0609u, File.set_time );
				break;
			case opt_air_3:
				Lputs( 0x000Bu, "[3/4]" );
				Lputs ( 0x0300u, "流 量Ⅰ:" );		ShowFP32 ( 0x0308u, (uint32_t)( Configure.AIRSetFlow[Q_PP1]) * 0.1f,  0x0501u, "L/m" );
				Lputs ( 0x0600u, "体 积Ⅰ:" );		ShowFP32 ( 0x0608u, (uint32_t)( File.sum_min * Configure.AIRSetFlow[Q_PP1] ) * 0.1f,  0x0501u, "L" );
				break;
			case opt_air_4:
				Lputs( 0x000Bu, "[4/4]" );
				Lputs ( 0x0300u, "流 量Ⅱ:" );		ShowFP32 ( 0x0308u, (uint32_t)(Configure.AIRSetFlow[Q_PP2] ) * 0.1f,  0x0501u, "L/m" );
				Lputs ( 0x0600u, "体 积Ⅱ:" );		ShowFP32 ( 0x0608u, (uint32_t)( File.sum_min * Configure.AIRSetFlow[Q_PP2] ) * 0.1f,  0x0501u, "L" );	
				break;
			default:
				break;
			}        
		}
		
		switch ( getKey())
		{
		case K_UP:		//	向前（时间较早的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == 1u )
			{
				fname = 1;
				beep();
				MsgBox( "无更多文件!", vbOKOnly );
			}
			else
			{
				fname = FileNum - 1u;			
			}

			File_Load_AIR ( fname, &File );
			FileNum = fname;
			if ( 0u == File.sample_begin )
			{
				beep();	//	此文件可能无效！
			}
		}	break;

		case K_DOWN:	//	向后（时间较新的文件）查找
		{
			uint16_t	fname;
			
			if ( FileNum == FileNum_Max )
			{
				fname = FileNum_Max;
				beep();
				MsgBox( "文件满！", vbOKOnly );	
				return;
			}
			else
			{
				fname = FileNum + 1u;
			}

			File_Load_AIR ( fname, &File );
			if( fname > SampleSet[Q_AIR].FileNum )
			{
				beep();//	到达最后，"后"面无有效数据
				File_Load_AIR ( FileNum, &File );
			}
			else	
			{
				if ( 0u == File.sample_begin )
					beep();
				FileNum = fname;
			}
		}	break;

		case K_OK:
			if ( 0u != File.sample_begin )
			{
				FPrintf_AIR( FileNum, &File);
			}
			break;

		case K_RIGHT:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;

		case K_LEFT:
			if ( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;

		case K_ESC:
			option = opt_exit;
			break;
				
		case K_OK_UP:	
			if ( gray < 2050u )
			{
				++gray;
			}
			if( ! releaseKey( K_OK_UP,100 ))
			{
				while( ! releaseKey( K_OK_UP, 3 ))
				{
					++gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;
		case K_OK_DOWN:
			if ( gray >  200u )
			{
				--gray;
			}
			if( ! releaseKey( K_OK_DOWN,100 ))
			{
				while( ! releaseKey( K_OK_DOWN, 1 ))
				{
					--gray;
					DisplaySetGrayVolt( gray * 0.01f );
				}
			}
			graychanged = true;
			break;
		case K_OK_RIGHT:
			if ( gray < ( 2000u - 50u ))
			{ 
				gray += 100u;
			}
			graychanged = true;
			break;
		case K_OK_LEFT:	
			if ( gray > ( 200 + 20u ))
			{
				gray -= 20u;
			}
			graychanged = true;
			break;
		default:
			break;
		}
		if( graychanged == true )
		{
			DisplaySetGrayVolt( gray * 0.01f );
			Configure.DisplayGray = gray;
			ConfigureSave();
			graychanged = FALSE;
		}	
	} while ( opt_exit != option );
}



/********************************** 功能说明 ***********************************
: 最大压力值记录的查询
*******************************************************************************/
//	static	void	QueryLog_TSP( void )
//	{
//		enum {
//			opt_exit, opt_standard
//		};
//		uint8_t	option = opt_standard;
//		struct	uFile_TSP	File;
//		uint16_t	FileNum = SampleSet[Q_TSP].FileNum;

//		cls();
//		do {
//			File.Load( FileNum );	// File_Load( FileNum, &File );
//			Lputs( 0x0000u, "粉尘文件记录:" );	ShowI16U( 0x000Cu, FileNum, 0x0300u, NULL );
//			Lputs( 0x0200u, "时间:" );		ShowDATE( 0x0205u, File.sample_begin );
//			Lputs( 0x0400u, "压力:" );		ShowFP32( 0x0407u, File.max_pr, 0x0602u, "kPa" );

//			switch ( getKey())
//			{
//			case K_UP:
//				++FileNum;
//				if ( FileNum >= ( FileNum_Max + 1u ))
//				{
//					FileNum = 1u;
//				}
//				break;
//			case K_DOWN:
//				if ( FileNum <= 1u )
//				{
//					FileNum = ( FileNum_Max + 1u );
//				}
//				--FileNum;;
//				break;

//			case K_ESC:
//				option = opt_exit;
//				break;

//			default:
//				break;
//			}
//		} while( opt_exit != option );
//	}

//	static	void	QueryLog_R24( void )
//	{
//		enum {
//			opt_exit, opt_standard
//		} option = opt_standard;
//		struct	uFile_R24	File;
//		uint16_t	FileNum = SampleSet[Q_R24].FileNum;

//		cls();
//		do {
//			File_Load( FileNum, &File );
//			Lputs( 0x0000u, "日均文件记录:" );	ShowI16U( 0x000Cu, FileNum, 0x0300u, NULL );
//			Lputs( 0x0200u, "时间:" );		ShowDATE( 0x0205u, File.sample_begin );
//			Lputs( 0x0400u, "压力A:" );		ShowFP32( 0x0407u, File.max_pr[Q_PP1], 0x0602u, "kPa" );
//			Lputs( 0x0600u, "压力B:" );		ShowFP32( 0x0607u, File.max_pr[Q_PP2], 0x0602u, "kPa" );

//			switch ( getKey())
//			{
//			case K_UP:
//				++FileNum;
//				if ( FileNum >= ( FileNum_Max + 1u ))
//				{
//					FileNum = 1u;
//				}
//				break;
//			case K_DOWN:
//				if ( FileNum <= 1u )
//				{
//					FileNum = ( FileNum_Max + 1u );
//				}
//				--FileNum;;
//				break;

//			case K_ESC:
//				option = opt_exit;
//				break;

//			default:
//				break;
//			}
//		} while( opt_exit != option );
//	}

//	static	void	QueryLog_SHI( void )
//	{
//		enum {
//			opt_exit, opt_standard
//		} option = opt_standard;
//		struct	uFile_SHI	File;
//		uint16_t	FileNum = SampleSet[Q_SHI].FileNum;

//		cls();
//		do {
//			File_Load( FileNum, &File );
//			Lputs( 0x0000u, "时均文件记录:" );	ShowI16U( 0x000Cu, FileNum, 0x0300u, NULL );
//			Lputs( 0x0200u, "时间:" );		ShowDATE( 0x0205u, File.sample_begin );
//			Lputs( 0x0400u, "压力C:" );		ShowFP32( 0x0407u, File.max_pr[Q_PP1], 0x0602u, "kPa" );
//			Lputs( 0x0600u, "压力D:" );		ShowFP32( 0x0607u, File.max_pr[Q_PP2], 0x0602u, "kPa" );

//			switch ( getKey())
//			{
//			case K_UP:
//				++FileNum;
//				if ( FileNum >= ( FileNum_Max + 1u ))
//				{
//					FileNum = 1u;
//				}
//				break;
//			case K_DOWN:
//				if ( FileNum <= 1u )
//				{
//					FileNum = ( FileNum_Max + 1u );
//				}
//				--FileNum;;
//				break;

//			case K_ESC:
//				option = opt_exit;
//				break;

//			default:
//				break;
//			}
//		} while( opt_exit != option );
//	}

/********************************** 功能说明 ***********************************
: 开关机日志记录的查询
*******************************************************************************/
extern volatile	uint16_t	PowerLogIndex ;

void	PowerLog_Query( void )
{
	struct	uPowerLog log;
	enum
	{
		opt_exit, opt_standard
	};
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint8_t	option = opt_standard;
	uint16_t	PLindex = 0u;	
	CHAR sbuffer[20] = {0};
	do {
		cls();
		Lputs( 0x0000u, "开关机记录" );	
		sprintf( sbuffer, "编号: [%1u/%1u]",PLindex + 1 ,PowerLogIndex + 1 ); 
		Lputs( 0x0200u, sbuffer );	
		PowerLogLoad( PLindex, &log );
		Lputs( 0x0400u, "开机:" );	ShowDATE( 0x0405u, log.boot );
		Lputs( 0x0600u, "关机:" );	if ( PLindex ){  ShowDATE( 0x0605u, log.shut ); }
		if( PowerLogIndex < PowerLogIndex_Max )
		{
			switch ( getKey())
				{
				case K_UP:
					if ( PLindex <  PowerLogIndex )
					{
						++PLindex;
					}
					else
					{
						beep();
						MsgBox( "无更多记录!", vbOKOnly );
					}
					break;

				case K_DOWN:
					if ( PLindex > 0u )
					{
						--PLindex;
					}
					break;

				case K_OK:
					if ( ! releaseKey( K_OK, 200u ))
					{
						beep();
						
						if ( vbYes == MsgBox( "清空开关机记录?", vbYesNo | vbDefaultButton2 ))
						{
							PowerLog_Clean();	
							PLindex = 0u;
						}
					}
					break;

				case K_ESC:
					option = opt_exit;
					break;

						
			case K_OK_UP:	
				if ( gray < 2050u )
				{
					++gray;
				}
				if( ! releaseKey( K_OK_UP,100 ))
				{
					while( ! releaseKey( K_OK_UP, 3 ))
					{
						++gray;
						DisplaySetGrayVolt( gray * 0.01f );
					}
				}
				graychanged = true;
				break;
			case K_OK_DOWN:
				if ( gray >  200u )
				{
					--gray;
				}
				if( ! releaseKey( K_OK_DOWN,100 ))
				{
					while( ! releaseKey( K_OK_DOWN, 1 ))
					{
						--gray;
						DisplaySetGrayVolt( gray * 0.01f );
					}
				}
				graychanged = true;
				break;

			case K_OK_RIGHT:
				if ( gray < ( 2000u - 50u ))
				{ 
					gray += 100u;
				}
				graychanged = true;
				break;
			case K_OK_LEFT:	
				if ( gray > ( 200 + 20u ))
				{
					gray -= 20u;
				}
				graychanged = true;
				break;
			default:
				break;
			}
			if( graychanged == true )
			{
				DisplaySetGrayVolt( gray * 0.01f );
				Configure.DisplayGray = gray;
				ConfigureSave();
				graychanged = FALSE;
			}	
		}	
		else
		{
			beep();
			MsgBox("文件满！",vbOKOnly);
			return;
		}
	} while( opt_exit != option );
}

//	/********************************** 功能说明 ***********************************
//	: 开关机日志记录的查询
//	*******************************************************************************/
//	void	PowerLog_Query0( void )
//	{
//		enum
//		{
//			opt_exit, opt_standard
//		} option = opt_standard;
//		
//		struct	uPowerLog log;
//		uint8_t	index = 0u;

//		cls();
//		Lputs( 0x0000u, "开关机记录" );	//	Menu_Redraw( menu );

//		do {
//			PowerLogLoad( 0u, &log );
//			Lputs( 0x0200u, "编号:" );	ShowI16U( 0x0205u, index, 0x0300u, NULL );
//			Lputs( 0x0400u, "开机:" );	ShowDATE( 0x0405u, log.boot );
//			Lputs( 0x0600u, "关机:" );	ShowDATE( 0x0605u, log.shut );
//			switch ( getKey())
//			{
//			case K_UP:
//				if ( index <  PowerLogIndex_Max )
//				{
//					++index;
//				}
//				break;

//			case K_DOWN:
//				if ( index > 0u )
//				{
//					--index;
//				}
//				break;

//			case K_OK:
//				//$TODO:	RecPower_Print( Log, index );
//				break;

//			case K_ESC:
//				option = opt_exit;
//				break;

//			default:
//				break;
//			}
//		} while( opt_exit != option );
//	}

//	void	menu_QueryLog( void )
//	{
//		static	struct	uMenu  const menu[] =
//		{
//			{ 0x0202u, "记录查询" },
//			{ 0x0203u, "粉尘" }, { 0x0209u, "日均" },
//			{ 0x0403u, "时均" }, { 0x0409u, "开机" }
//		};
//		uint8_t	item = 1u;

//		do {
//			cls();
//			Menu_Redraw( menu );
//			item = Menu_Select( menu, item );
//			switch( item )
//			{
//			case 1:		QueryLog_TSP();	break;
//			case 2:		QueryLog_R24();	break;
//			case 3:		QueryLog_SHI();	break;
//			case 4:		QueryLog_POWER();	break;
//			default:	break;
//			}
//		} while( 0u != item );
//	}

void	menu_SampleQuery( void )	//	enum enumSamplerSelect SamplerSelect )
{
	switch ( SamplerSelect )
	{
	case Q_TSP:	Query_File_TSP();	break;
	case Q_R24:	Query_File_R24();	break;
	case Q_SHI:	Query_File_SHI();	break;
	case Q_AIR:	Query_File_AIR();	break;
	}
}

/********  (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司  **** End Of File ****/
