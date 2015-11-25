/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: ConfigureEx.C
* 创 建 者: 董峰(2014年4月8日)
* 描  述  : KB-6120E 定义、显示版本信息
* 最后修改: 2014年4月8日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"

void	EditionSelsct( void )
{
// #define	T_KB6120A
#define	T_KB6120AD
// #define	T_KB6120AD2
// 	#define	T_KB2400HL

	#ifdef	T_KB6120A
		eDataValidMask = 0x5A3A;
	#elif	defined	T_KB6120AD
		eDataValidMask = 0x5A3B;
	#elif	defined	T_KB6120AD2
		eDataValidMask = 0x5A3C;
	#elif	defined	T_KB2400HL
		eDataValidMask = 0x5A3D;
	#endif

}


/********************************** 功能说明 ***********************************
*  根据仪器型号显示版本信息
*  根据销售人员要求，可能会改成其他厂家名称，或者干脆不显示。
*******************************************************************************/
CHAR  const * const ExNameIdent1[] =
{
  " 无 厂 家 名 称 ",	//	0
  " 青 岛 金 仕 达 ",	//	1
	
//  ...  							//	...
};

CHAR  const * const ExNameIdent2[] =
{
  " 无 厂 家 名 称 ",	//	0
  "电子科技有限公司",	//	1
	
//  ...  							//	...
};

CHAR  const * const EditionNum[] =
{
  "KB6120E V1.06",	//	内部版本
 __DATE__" V1.00",	//	显示版本
};

static	void	ShowEdition_NoName( void )
{
	cls();
  Lputs( 0x0000u, szNameIdent[Configure.InstrumentName] );
	Lputs( 0x0300u, EditionNum[1] );
	Lputs( 0x0600u, "编号:" );	ShowFP32( 0x0607u, Configure.ExNum, 0x0700u, NULL );
}

static	void	ShowEdition_HasName( void )
{
  cls();
	Lputs( 0x0000u, ExNameIdent1[Configure.ExName] );
	Lputs( 0x0200u, ExNameIdent2[Configure.ExName] );
	Lputs( 0x0400u, EditionNum[1] );
	Lputs( 0x0600u, "编号:" );	ShowFP32( 0x0607u, Configure.ExNum, 0x0700u, NULL );
}

void	ShowEdition( void )
{
  switch( Configure.ExName )
  {
  case 0: 
    ShowEdition_NoName(); 
    break;
  default: 
    ShowEdition_HasName(); 
    break;
  }
}
/********************************** 功能说明 ***********************************
*  显示程序版本 根据仪器型号显示版本信息
*******************************************************************************/
void	ShowEdition_Inner( void )
{	
	cls();
	Lputs( 0x0000u, szNameIdent[Configure.InstrumentName] );
	Lputs( 0x0202u, __DATE__ );
	Lputs( 0x0402u, __TIME__ );
 	Lputs( 0x0602u, EditionNum[0] );	
	getKey();
}



/********************************** 数据定义 ***********************************
*  可用的仪器名称列表
*******************************************************************************/
CHAR  const * const szTypeIdent[] =
{
    "KB-6120A  ",
    "KB-6120AD ",
		"KB-2400HL ",
		"KB-6120AD2",
//    "KB-6120BD ",
//    "KB-6120BH ",
//    "KB-6120C  ",
//    "KB-2400   ",
};

CHAR  const * const szNameIdent[] =
{
	" 智能综合采样器 ",
	" 综合大气采样器 ",
	"智能中流量采样器",
	"智能大流量采样器",
	" 恒温恒流采样器 ",
	" 智能恒流采样器 ",
};
void	ConfigureLoadDefault_KB6120E( void )
{
		Configure.Heater_SetTemp = 300u;		//	加热器恒温温度 30.0 ℃
		Configure.Heater_SW = FALSE;         //加热器开关			
		Configure.HCBox_SetMode = MD_Shut;		//	恒温箱控制模式 [关闭]
		Configure.HCBox_SetTemp = 240u;			//	恒温箱恒温温度 24.0 ℃
				 
		Configure.SetTstd = enum_293K;			//	标况流量的定义温度 
		
		Configure.Mothed_Delay = enumByDelay;	//	采样开始时间方式
		Configure.Mothed_Sample = enumBySet;	//	采样时间控制方式

		Configure.Mothed_Ba = enumMeasureBa;	//	大气压力获取方式
		Configure.set_Ba    = 10133u;			//	保存用户输入气压

		Configure.DisplayGray  = 2000u;	//	显示灰度设置
		Configure.DisplayLight = 50u;	//	显示亮度设置
		Configure.TimeoutLight = 2u;	//	背光延时时间
		
		Configure.slope_Bat_Voltage = 1000;
		Configure.slope_Bat_Current = 1000;
		Configure.threshold_Current = 100;
		
		Configure.shouldCalcPbv = 0;		//	饱和水汽压是否参与计算。
		
		Configure.Password = 633817;
		Configure.ExName = Name_JSD;
		
}
void	ConfigureLoad_KB6120A( void )
{
		Configure.InstrumentType = type_KB6120A;

		Configure.PumpType[PP_TSP  ] = enumOrifice_1;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	粉尘 采样流量 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumPumpNone;	Configure.SetFlow[PP_R24_A]  =  200u;	//	日均1采样流量 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumPumpNone;	Configure.SetFlow[PP_R24_B]  =  200u;	//	日均2采样流量 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumPumpNone;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	时均1采样流量 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumPumpNone;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	时均2采样流量 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumOrifice_1;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	大气 流量 0.5 L/m
		Configure.AIRSetFlow[Q_PP1] = 5u;
		Configure.AIRSetFlow[Q_PP2] = 5u;
		
		Configure.HeaterType = enumHeaterOnly;	//	只有加热器
		
		Configure.Battery_SW = FALSE;    //电池是否存在
}

void	ConfigureLoad_KB2400HL( void )
{

		Configure.InstrumentType = type_KB2400HL;

		Configure.PumpType[PP_TSP  ] = enumPumpNone;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	粉尘  采样流量 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumPumpNone;	Configure.SetFlow[PP_R24_A]  =  200u;	//	日均1 采样流量 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumPumpNone;	Configure.SetFlow[PP_R24_B]  =  200u;	//	日均2 采样流量 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumOrifice_1;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	时均1 采样流量 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumOrifice_1;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	时均2 采样流量 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumPumpNone;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	大气 流量 0.5 L/m
		
		Configure.AIRSetFlow[Q_PP1] = 5u;
		Configure.AIRSetFlow[Q_PP2] = 5u;
		
		Configure.HeaterType = enumHeaterOnly;	//	只有加热器
		
		Configure.Battery_SW = TRUE;

}

void	ConfigureLoad_KB6120AD( void )
{
		Configure.InstrumentType = type_KB6120AD;

		Configure.PumpType[PP_TSP  ] = enumOrifice_1;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	粉尘  采样流量 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumPumpNone;	Configure.SetFlow[PP_R24_A]  =  200u;	//	日均1 采样流量 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumPumpNone;	Configure.SetFlow[PP_R24_B]  =  200u;	//	日均2 采样流量 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumOrifice_1;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	时均1 采样流量 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumOrifice_1;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	时均2 采样流量 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumPumpNone;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	大气 流量 0.5 L/m
		Configure.AIRSetFlow[Q_PP1] = 5u;
		Configure.AIRSetFlow[Q_PP2] = 5u;
		
		Configure.HeaterType = enumHeaterOnly;	//	只有加热器

		Configure.Battery_SW = FALSE;
}

void	ConfigureLoad_KB6120AD2( void )
{
		Configure.InstrumentType = type_KB6120AD2;

		Configure.PumpType[PP_TSP  ] = enumOrifice_1;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	粉尘  采样流量 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumOrifice_1;	Configure.SetFlow[PP_R24_A]  =  200u;	//	日均1 采样流量 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumOrifice_1;	Configure.SetFlow[PP_R24_B]  =  200u;	//	日均2 采样流量 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumPumpNone;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	时均1 采样流量 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumPumpNone;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	时均2 采样流量 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumPumpNone;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	大气 流量 0.5 L/m
		Configure.AIRSetFlow[Q_PP1] = 5u;
		Configure.AIRSetFlow[Q_PP2] = 5u;
		
		Configure.HeaterType = enumHeaterOnly;	//	只有加热器

		Configure.Battery_SW = FALSE;	
}



/********************************** 功能说明 ***********************************
*	扩展配置 -> 选择泵、流量、仪器名称
*******************************************************************************/
void	Configure_InstrumentName( void )
{
	static  struct  uMenu  const  menu[] = 
	{
		{ 0x0101u, "配置仪器名称" },
		{ 0x0200u, "名称" },
	};
	uint8_t item = 1u;
	BOOL	changed = FALSE;

	cls();
	
	do {
		cls();
		Menu_Redraw( menu );		
		Lputs( 0x0400u, szNameIdent[Configure.InstrumentName] );
		
		item = Menu_Select( menu, item );
		switch ( item )
		{
		case 1:
			cls();			
			++Configure.InstrumentName;
			if ( Configure.InstrumentName >= (sizeof(szNameIdent) / sizeof(szNameIdent[0] )) )	//	type_Max
			{
				Configure.InstrumentName = 0u;
			}
			changed = TRUE;
			break;
		default:
			break;
		}
	} while ( enumSelectESC != item );
	
	if ( changed )
	{
		if( vbYes == MsgBox( "保存配置?",vbYesNo ) )
		{
// 			ConfigureLoadDefault();
			ConfigureSave();
		}
		else
			ConfigureLoad();
	}
	
}

void	Configure_InstrumentType( void )
{
	static  struct  uMenu  const  menu[] = 
	{
		{ 0x0101u, "配置仪器型号" },
		{ 0x0400u, "型号" },
	};
	uint8_t item = 1u;
	BOOL	changed = FALSE;

	do {
		cls();
		Menu_Redraw( menu );
		Lputs( 0x0405u, szTypeIdent[Configure.InstrumentType] );
		
		item = Menu_Select( menu, item );
		switch ( item )
		{
		case 1:	
			changed = TRUE;
			++Configure.InstrumentType;
			if ( Configure.InstrumentType >= ( sizeof( szTypeIdent ) / sizeof( szTypeIdent[0] )))
			{
				Configure.InstrumentType = 0u;
			}		
			break;
		default:
			break;
		}
	} while ( enumSelectESC != item );
	
	if ( changed )
	{
		if( vbYes == MsgBox( "保存配置?",vbYesNo ) )
		{
// 			ConfigureLoadDefault( );
			ConfigureSave();
		}
		else
			ConfigureLoad();
	}
	
}

// void	ConfigureLoadDefault( void )
// {
// 	switch ( Configure.InstrumentType )
// 	{
// 	case type_KB6120A:		ConfigureLoad_KB6120A();	break;
// 	case type_KB6120AD:		ConfigureLoad_KB6120AD();	break;
// 	case type_KB2400HL:		ConfigureLoad_KB2400HL();	break;
// 	case type_KB6120AD2:	ConfigureLoad_KB6120AD2();break;
// 	}
// }

void	ConfigureLoadDefault( void )
{
	#ifdef	T_KB6120A
		ConfigureLoad_KB6120A();
	#elif	defined	T_KB6120AD
		ConfigureLoad_KB6120AD();
	#elif	defined	T_KB6120AD2
		ConfigureLoad_KB6120AD2();
	#elif	defined	T_KB2400HL
		ConfigureLoad_KB2400HL();
	#endif	
	if ( Configure.DataValidMask != eDataValidMask )
	{
		ConfigureLoadDefault_KB6120E();
		Configure.DataValidMask = eDataValidMask;
	}
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/


