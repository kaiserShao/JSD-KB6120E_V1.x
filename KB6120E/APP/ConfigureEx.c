/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: ConfigureEx.C
* 创 建 者: 董峰
* 描  述  : KB-6120E 配置程序（厂家配置部分）
* 最后修改: 2014年3月1日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"
#include "stdio.h" 
/********************************** 功能说明 ***********************************
*	厂家配置 -> 泵类型
*******************************************************************************/
CHAR  const * const Pump[]=
{
	"粉尘泵 ",
	"日均A",
	"日均B",
	"时均C",
	"时均D",
	"大气泵",
	"恒温器"
};

CHAR  const * const Type[]=
{
	"[K-100L]",
	"[K-1.Ｍ]",
	"[1L孔板]",
	"[2L孔板]",
	"[1-单泵]",
	"[2-双泵]",
	"[加热器]",
	"[恒温箱]",
	"[全安装]" 
};
CHAR  PumpC[PP_Max+1];
CHAR  TypeC[PP_Max+1];
static	void	Configure_Pump( void )
{
	enum 
	{
		opt_exit,
		opt_tsp_1,   opt_tsp_2,   opt_tsp_3,
		opt_max, opt_min = opt_tsp_1
	};
	uint8_t	option = opt_min;
	uint8_t opt_maxx = 1;
	uint16_t dispchar[3] ={ 0x0200u, 0x0400u, 0x0600u };
	uint16_t dispchar2[3] ={ 0x0206u, 0x0406u, 0x0606u };
	uint8_t i,imax;	
		do {
		imax = 
		i    = 0;
		ConfigureLoad();
		if( Configure.PumpType[PP_TSP] != enumPumpNone )
		{	
			switch ( Configure.PumpType[PP_TSP] )
			{
			case enumOrifice_1: TypeC[i] = 0;break; 
			case enumOrifice_2: TypeC[i] = 1;break; 
			}
			PumpC[i++] = 0; 		
		}		
		if( Configure.PumpType[PP_R24_A] != enumPumpNone )
		{		
			switch (Configure.PumpType[PP_R24_A] )
			{
			case enumOrifice_1:TypeC[i] = 2;break; 
			case enumOrifice_2:TypeC[i] = 3;break; 
			}
			PumpC[i++] = 1; 	
		}
		if( Configure.PumpType[PP_R24_B] != enumPumpNone )
		{
			switch ( Configure.PumpType[PP_R24_B] )
			{
			case enumOrifice_1:TypeC[i] = 2;break; 
			case enumOrifice_2:TypeC[i] = 3;break; 
			}
			PumpC[i++] = 2; 
		}
		if( Configure.PumpType[PP_SHI_C] != enumPumpNone )
		{
			switch ( Configure.PumpType[PP_SHI_C] )
			{
			case enumOrifice_1:TypeC[i] = 2;break; 
			case enumOrifice_2:TypeC[i] = 3;break; 
			}
			PumpC[i++] = 3; 
		}	 
		if( Configure.PumpType[PP_SHI_D] != enumPumpNone )
		{
			switch ( Configure.PumpType[PP_SHI_D] )
			{
			case enumOrifice_1:TypeC[i] = 2;break; 
			case enumOrifice_2:TypeC[i] = 3;break; 
			}
			PumpC[i++] = 4; 
		}	 
		if( Configure.PumpType[PP_AIR] != enumPumpNone )
		{
			switch ( Configure.PumpType[PP_AIR] )
			{
			case enumOrifice_1:TypeC[i] = 4;break; 
			case enumOrifice_2:TypeC[i] = 5;break; 
			}
			PumpC[i++] = 5; 
		}	 
		if( Configure.HeaterType != enumPumpNone )
		{
			switch ( Configure.HeaterType )
			{ 
			case enumHeaterOnly:	 TypeC[i] = 6;break; 
			case enumHCBoxOnly :	 TypeC[i] = 7;break; 
			case enumHCBoxHeater:  TypeC[i] = 8;break; 

			}
			PumpC[i++] = 6; 
		}
		imax = i;
			switch ( option )
			{
			default:
			case opt_tsp_1:
				cls();
				for( i = 0; i < 3; i++)
				{
					if(i < imax)
					{ 
						if( imax <= 3 )
							Lputs( 0x0000, "泵安装情况  " ); 
						else		
							Lputs( 0x0000, "泵安装情况 1" ); 
            Lputs( dispchar[i%3],   Pump[PumpC[i]] ); 
						Lputs( dispchar2[i%3], Type[TypeC[i]]); 
					}	
				}	
				break;
			case opt_tsp_2:
				cls();
				for( i = 3; i < 6; i++)
				{
					Lputs( 0x0000, "泵安装情况 2" );
					if( i < imax )
					{
						Lputs( dispchar[i%3],   Pump[PumpC[i]] ); 
						Lputs( dispchar2[i%3], Type[TypeC[i]]); 
					}          					
				}	
				break;
			case opt_tsp_3:
				cls();
				for( i = 6; i < 9; i++)
				{	
					Lputs( 0x0000, "泵安装情况 3" );
					if(i < imax)
					{					
						Lputs( dispchar[i%3],   Pump[PumpC[i]] ); 
						Lputs( dispchar2[i%3], Type[TypeC[i]]); 
					}									
				}	
				break;
			}
			opt_maxx = ( imax + 2 ) / 3 ;
			switch ( getKey())
			{
			case K_UP:		//	向前（时间较早的文件）查找
				--option;	
				if ( option < opt_min )
				{
					option = opt_maxx;
				}
				break;
			case K_DOWN:	//	向后（时间较新的文件）查找
				++option;
				if ( option > opt_maxx )
				{
					option = opt_min;
				}
				break;
			case K_RIGHT:
				++option;
				if ( option > opt_maxx )
				{
					option = opt_min;
				}
				break;
			case K_LEFT:
				--option;
				if ( option < opt_min )
				{
					option = opt_maxx;
				}
				break;				
			case K_OK:
				option = opt_exit;
				break;
			case K_SHIFT:
				break;
			case K_ESC:
				option = opt_exit;
				break;
			default:
				break;
			}	
		} while ( opt_exit != option );
}

/********************************** 功能说明 ***********************************
*	扩展配置 -> 采样泵的压力保护
*******************************************************************************/
//	static	void	menu_Configure_PrProtect( void )
//	{
//		MsgBox( "TODO: PrProtect", vbOKOnly );
//	}

/********************************** 功能说明 ***********************************
*	扩展配置 -> 泵累计运行时间
*******************************************************************************/
static	void	menu_Clean_SumTime( void )
{
	BOOL	Done = FALSE;
	do{
		cls();	//	Lputs( 0x0000u, "运行时间(小时)" );
		Lputs( 0x0000u, "粉尘:" );	ShowFP32( 0x0005u, PumpSumTimeLoad( PP_TSP ) / 60.0f, 0x0601u, "h" );
		Lputs( 0x0200u, "A:" ); 	ShowFP32( 0x0202u, PumpSumTimeLoad( PP_R24_A ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0208u, "B:" ); 	ShowFP32( 0x020Au, PumpSumTimeLoad( PP_R24_B ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0400u, "C:" ); 	ShowFP32( 0x0402u, PumpSumTimeLoad( PP_SHI_C ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0408u, "D:" ); 	ShowFP32( 0x040Au, PumpSumTimeLoad( PP_SHI_D ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0600u, "大气:" ); 	ShowFP32( 0x0605u, PumpSumTimeLoad( PP_AIR ) / 60.0f, 0x0601u, NULL );

		switch ( getKey() )
		{
		case	K_OK	:						
			if ( ! releaseKey( K_OK, 100u ))
			{
				beep();
				if ( vbYes == MsgBox( "清除累计时间 ?", vbYesNo | vbDefaultButton2 ))
				{
					PumpSumTimeSave( PP_TSP,   0u );
					PumpSumTimeSave( PP_R24_A, 0u );
					PumpSumTimeSave( PP_R24_B, 0u );
					PumpSumTimeSave( PP_SHI_C, 0u );
					PumpSumTimeSave( PP_SHI_D, 0u );
					PumpSumTimeSave( PP_AIR,   0u );
				}
			}
			break;
		case	K_ESC	:
			Done = TRUE;
			break;
		default	:
			break;
		}
	}while( !Done );
	
}

/********************************** 功能说明 ***********************************
*  系统配置 -> 文件号
*******************************************************************************/
static	void	menu_Clean_FileNum( void )
{
	BOOL	Done = FALSE;
	do{
		cls();	//	Lputs( 0x0000u, "文件号" );
		Lputs( 0x0000u, "粉尘:" );	ShowI16U( 0x0009u, SampleSet[Q_TSP].FileNum, 0x0300u, NULL );
		Lputs( 0x0200u, "日均:" );	ShowI16U( 0x0209u, SampleSet[Q_R24].FileNum, 0x0300u, NULL );
		Lputs( 0x0400u, "时均:" );	ShowI16U( 0x0409u, SampleSet[Q_SHI].FileNum, 0x0300u, NULL );
		Lputs( 0x0600u, "大气:" );	ShowI16U( 0x0609u, SampleSet[Q_AIR].FileNum, 0x0300u, NULL );
		
		switch ( getKey() )
		{
		case	K_OK	:
			if ( ! releaseKey( K_OK, 100u ))
			{
				beep();
				if ( vbYes == MsgBox( "清除所有文件 ?", vbYesNo | vbDefaultButton2 ) )
				{
					File_Clean();
					
					SampleSet[Q_TSP].FileNum =
					SampleSet[Q_R24].FileNum =
					SampleSet[Q_SHI].FileNum =
					SampleSet[Q_AIR].FileNum = 0u;
					SampleSetSave();
				}
			}
			break;
		case	K_ESC	:
			Done = TRUE;
			break;
		default	:
			break;
		}		
	}while( !Done );
	
}
void  menu_Sample_Sum( void )
{
  static	struct  uMenu  const  menu[] =
	{
		{ 0x0201u, "采样累计" },
		{ 0x0301u, "文件编号" },
		{ 0x0601u, "泵累计" },

	};
	uint8_t item = 1u;
	do {
		cls();
		Menu_Redraw( menu );
    item = Menu_Select( menu, item );
    switch( item )
    {
		case 1:		menu_Clean_FileNum();	break;
		case 2:		menu_Clean_SumTime();	break;
		default:	break;			
    }
  }while( enumSelectESC != item );
}


void menu_ExName( void )
{
	BOOL changed = FALSE;
	BOOL Done = FALSE;
	
	do{
		cls();
		ShowEdition();
				
		switch( getKey() )
		{
		case	K_OK:	
			++ Configure.ExName;
			if ( Configure.ExName >= Name_Max )
			{
				Configure.ExName = 0u;
			}
			changed = TRUE;	
			break;
		case	K_ESC:
			Done = TRUE;
			if( changed )
			{
				switch( MsgBox("保存修改结果?", vbYesNoCancel|vbDefaultButton3 ) )
				{
				case	vbYes		:	ConfigureSave();	break;
				case	vbNo		:	ConfigureLoad();	break;
				case	vbCancel:	Done = FALSE;			break;
				}				
			}			
			break;
		default:
			break;
		}
		
	}while( !Done );
}


void	menu_ConfigureDisplay( void )
{
	static	struct  uMenu  const  menu[] =
	{
		{ 0x0301u, "配置 显示" },
		{ 0x0200u, "灰度" },
		{ 0x0400u, "亮度" },
		{ 0x0600u, "定时" }
	};
	uint8_t item = 1u;
	
	uint16_t gray  = Configure.DisplayGray;
	uint16_t light = Configure.DisplayLight;
	uint8_t  ltime = Configure.TimeoutLight;
	BOOL	changed = FALSE;
	
	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0205u, gray,  0x0502u, " V " );
		ShowI16U( 0x0405u, light, 0x0300u, " % " );
		switch ( ltime )
		{
		case 0:	Lputs( 0x0605u, "[关闭]" );	break;
		case 1:	Lputs( 0x0605u, "[15秒]" );	break;
		case 2:	Lputs( 0x0605u, "[30秒]" );	break;
		case 3:	Lputs( 0x0605u, "[60秒]" );	break;
		default:
		case 4:	Lputs( 0x0605u, "[常亮]" );	break;
		}
		item = Menu_Select( menu, item );

		switch( item )
		{
		case 1:	
			if ( EditI16U( 0x0205u, &gray, 0x0502u ))
			{
				if ( gray > 2000u ){ gray = 2000u; }
				if ( gray <  200u ){ gray =  200u; }
				DisplaySetGrayVolt( gray * 0.01f );
				changed = TRUE;
			}
			break;
		case 2:
			if ( EditI16U( 0x0405u, &light, 0x0300u ))
			{
				if ( light > 100u ){ light = 100u; }
				DisplaySetLight( light );
				changed = TRUE;
			}
			break;
		case 3:	
			if ( ++ltime > 4 ){  ltime = 0u; }
			DisplaySetTimeout( ltime );
			changed = TRUE;
			break;
		}
		
	} while ( enumSelectESC != item );
	
	if ( changed )
	{
		Configure.DisplayGray  = gray;
		Configure.DisplayLight = light;
		Configure.TimeoutLight = ltime;
		ConfigureSave();
	}
}



extern	void	Configure_InstrumentType( void );
extern	void	Configure_InstrumentName( void );

static	void	menu_Configure_ExPump( void )
{
	static	struct  uMenu  const  menu[] =
	{
		{ 0x0301u, "装配配置" },
		{ 0x0201u, "仪器型号" },
		{ 0x0401u, "仪器名称" },
		{ 0x0601u, "泵装配情况" },

	};
	uint8_t	item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:	Configure_InstrumentType();	break;	
	 	case 2:	Configure_InstrumentName();	break;
		case 3:	Configure_Pump();						break;
		default:	
			break;
		}
	} while( enumSelectESC != item );
} 	 

/********************************** 功能说明 ***********************************
*	扩展配置（只能厂家操作的部分）

厂家调试
程序版本
泵累计运行时间
泵（流量计）型号选择，仪器显示名称选择，选择供电电源类型
计前压力保护限制值设置，防止泵堵塞
显示屏幕调整
文件编号、文件存储内容清零
*******************************************************************************/

void	menu_ConfigureEx( void )
{
	static	struct  uMenu  const  menu[] =
	{
		{ 0x0302u, "厂家配置" },
		{ 0x0202u,  "调试"  }, { 0x0208u, "采样累计" },
		{ 0x0401u, "泵装配" }, { 0x0408u, "显示" },	//	{ 0x0408u, "压力保护" },
		{ 0x0602u,  "厂家"  }, { 0x0608u, "程序版本" },

	};
	uint8_t	item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:	menu_FactoryDebug();			break;	
	 	case 3:	menu_Configure_ExPump();	break;
		case 5:	menu_ExName();						break;
		case 2:	menu_Sample_Sum();				break;
		case 4:	menu_ConfigureDisplay();	break;
		case 6:	ShowEdition_Inner(); 			break;
		default:	break;
		}
	} while( enumSelectESC != item );
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
