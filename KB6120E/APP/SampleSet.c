/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Setup.C
* 创 建 者: Dean
* 描  述  : KB-6120E 采样设置程序
* 最后修改: 2013年12月18日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"

/********************************** 数据定义 **********************************/
struct	uSampleSet	SampleSet[SamplerNum_Max];	//	采样设置

/********************************** 功能说明 ***********************************
*  设置采样任务的控制参数，原则上设置好的参数，每次采样不需要变动。
*******************************************************************************/
void	SetupFlow_AIR( void )
{
	static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "流量设置" },
		{ 0x0300u, "流量Ⅰ" },
		{ 0x0500u, "流量Ⅱ" },
	};
	uint8_t item = 0u;
	
	uint16_t	iflow = Configure.AIRSetFlow[Q_PP1];
	uint16_t	iiflow = Configure.AIRSetFlow[Q_PP2];
	BOOL	changed = FALSE;
	
	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0308u, iflow, 0x0301u, "L/m" );
		ShowI16U( 0x0508u, iiflow, 0x0301u, "L/m" );

		item = Menu_Select2( menu, item + 1 ,FALSE);

		switch ( item )
		{
		case 1:
			if ( EditI16U( 0x0308u, &iflow, 0x0301u ))
			{
				if ( iflow <  1u ) { iflow =  1u; }
				if ( iflow > 10u ) { iflow = 10u; }
				changed = TRUE;
			}
			break;
		case 2:
			if ( EditI16U( 0x0508u, &iiflow, 0x0301u ))
			{	
				if ( iiflow <  1u ) { iiflow =  1u; }
				if ( iiflow > 10u ) { iiflow = 10u; }
				changed = TRUE;
			}
			break;
		default:
			break;
		}
	} while ( enumSelectESC != item );
	
	if ( changed )
	{
		Configure.AIRSetFlow[Q_PP1] = iflow;
		Configure.AIRSetFlow[Q_PP2] = iiflow;
		ConfigureSave();
	}
}

static	BOOL	setup_TSP_AIR( struct uMenu const menu[] )
{
	uint8_t	item = 0u;
	BOOL	changed = FALSE;
	BOOL	need_redraw = TRUE;

	do {
		if ( need_redraw )
		{
			cls();
			Menu_Redraw( menu );
			SamplerTypeShow( 0x000Au );
			
			need_redraw = FALSE;
		}
		
		ShowTIME( 0x0209u, SampleSet[SamplerSelect].sample_1 );
		ShowTIME( 0x0409u, SampleSet[SamplerSelect].suspend_1 );
		ShowI16U( 0x0605u, SampleSet[SamplerSelect].set_loops, 0x0200u, NULL );

		item = Menu_Select2( menu, item + 1u ,FALSE);

		switch( item )
		{
		case 1:	if ( EditTIME( 0x0209u, &( SampleSet[SamplerSelect].sample_1 )))			{ changed = TRUE; }	break;
		case 2:	if ( EditTIME( 0x0409u, &( SampleSet[SamplerSelect].suspend_1 )))			{ changed = TRUE; }	break;
		case 3:	if ( EditI16U( 0x0605u, &( SampleSet[SamplerSelect].set_loops ), 0x0200u ))	{ changed = TRUE; }	break;
		case 4:
			SetupFlow_AIR();
			changed =TRUE;
			need_redraw = TRUE;
			break;
		case enumSelectXCH:		SamplerTypeSwitch();	return	FALSE;	//	返回到上级菜单，切换到其他采样器
		}
	} while ( enumSelectESC != item );

	if ( changed )
	{
		SampleSetSave();
	}
	return	TRUE;
}

void	SetupFlow_SHI( void )
{
	static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "流量设置" },
		{ 0x0300u, "流量C" },
		{ 0x0500u, "流量D" },
	};
	uint8_t item = 0u;
	uint16_t	cflow = Configure.SetFlow[PP_SHI_C];
	uint16_t	dflow = Configure.SetFlow[PP_SHI_D];
	BOOL	changed = FALSE;
	
	cls();
	
		Menu_Redraw( menu );
	do {
		ShowI16U( 0x0306u, cflow, 0x0301u, "L/m" );
		ShowI16U( 0x0506u, dflow, 0x0301u, "L/m" );

		item = Menu_Select2( menu, item + 1u, FALSE );

		switch ( item )
		{
		case 1:
			if ( EditI16U( 0x0306u, &cflow, 0x0301u ))
			{
				switch ( Configure.PumpType[PP_SHI_C] )
				{
				case enumOrifice_1:
          if ( cflow <  1u ) { cflow =  1u; }
          if ( cflow > 10u ) { cflow = 10u; }
					break;
				case enumOrifice_2:
					if ( cflow <  5u ) { cflow =  5u; }
					if ( cflow > 20u ) { cflow = 20u; }

					break;
				default:
					break;
				}
				changed = TRUE;
			}
			break;
		case 2:
			if ( EditI16U( 0x0506u, &dflow, 0x0301u ))
			{
				switch ( Configure.PumpType[PP_SHI_D] )
				{
				case enumOrifice_1:
					if ( dflow <  1u ) { dflow =  1u; }
					if ( dflow > 10u ) { dflow = 10u; }					
					break;
				case enumOrifice_2:
					if ( dflow <  5u ) { dflow =  5u; }
					if ( dflow > 20u ) { dflow = 20u; }
					break;
				default:
					break;
				}
				changed = TRUE;
			}
			break;
		default:
			break;
		}
	} while ( enumSelectESC != item );
	
	if ( changed )
	{
		Configure.SetFlow[PP_SHI_C] = cflow;
		Configure.SetFlow[PP_SHI_D] = dflow;
		ConfigureSave();
	}
}

#define	LOBYTE(__w)		((uint8_t)((__w)&0x0FF))
#define	HIBYTE(__w)		((uint8_t)(((__w)>>8)&0x0FF))

static	uint8_t	Menu_Select_Start_R24 ( const struct uMenu * menu, uint8_t item )
{
	uint8_t	mlen, mlen_row, mlen_col;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	mlen_row = HIBYTE ( menu[0].yx );
	mlen_col = LOBYTE ( menu[0].yx );

	if ( 0u == mlen_row )
	{
		mlen = mlen_col;
	}
	else
	{
		mlen = mlen_row * mlen_col;
	}

	if ( ( item < 1u ) || ( item > mlen ) )
	{
		item = 1u;
	}

	for ( ; ; )
	{
		LcmMask ( menu[item].yx, strlen ( menu[item].sz ) );
		do
		{
//			if ( NULL != pHook )
//			{
//				pHook();
//			}
		}
		while ( ! hitKey ( 50u ) );

		Lputs ( menu[item].yx, menu[item].sz );

		switch ( getKey() )
		{
		case K_RIGHT:
			switch ( item )
			{
			case 1: item = 2;	break;
			case 2:	item = 3;	break;
			case 3:	item = 3;	break;
			}
			break;

		case K_LEFT:
			switch ( item )
			{
			case 1: item = 1;	break;
			case 2:	item = 1;	break;
			case 3:	item = 2;	break;
			}
			break;

		case K_DOWN:
			switch ( item )
			{
			case 1: item = 2;	break;
			case 2:	item = 3;	break;
			case 3:	item = 3;	break;
			}
			break;

		case K_UP:
			switch ( item )
			{
			case 1: item = 1;	break;
			case 2:	item = 1;	break;
			case 3:	item = 1;	break;
			}
			break;

		case K_OK:
			return	item;

		case K_ESC:
			return	enumSelectESC;

		case K_SHIFT:		
			return	enumSelectXCH;
		
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
}


static	BOOL	setup_R24_SHI( struct uMenu  const  menu[], const CHAR * szPrompt )
{
	uint8_t	item = 0u;
	BOOL	changed = FALSE;
	BOOL	need_redraw = TRUE;
	do {
		if ( need_redraw )
		{
			cls();
			Menu_Redraw( menu );
			SamplerTypeShow( 0x000Au );
			
			need_redraw = FALSE;
		}
		
		ShowTIME( 0x0203u, SampleSet[SamplerSelect].sample_1 );
		ShowTIME( 0x020Bu, SampleSet[SamplerSelect].sample_2 );
		ShowTIME( 0x0403u, SampleSet[SamplerSelect].suspend_1 );
		Lputs(0x0408u, szPrompt );
		{
			uint16_t suspend_2 =  SampleSet[SamplerSelect].sample_1 
                          + SampleSet[SamplerSelect].suspend_1
                          - SampleSet[SamplerSelect].sample_2;
			ShowTIME( 0x040Bu, suspend_2 );
		}
		ShowI16U( 0x0605u, SampleSet[SamplerSelect].set_loops, 0x0200u, NULL );

		item = Menu_Select2( menu, item + 1u ,FALSE);

		switch ( item )
		{
		case 1:
			if ( EditTIME( 0x0203u, &SampleSet[SamplerSelect].sample_1 ))
			{
				if ( SampleSet[SamplerSelect].sample_1 < SampleSet[SamplerSelect].sample_2 )
				{
					SampleSet[SamplerSelect].sample_2 = SampleSet[SamplerSelect].sample_1;
					beep();
				}
				else
				{
					changed = TRUE;
				}
			}
			break;
		case 2:
			if ( EditTIME( 0x020Bu, &SampleSet[SamplerSelect].sample_2 ))
			{
				if ( SampleSet[SamplerSelect].sample_1 < SampleSet[SamplerSelect].sample_2 )
				{
					SampleSet[SamplerSelect].sample_2 = SampleSet[SamplerSelect].sample_1;
					beep();
				}
				else
				{
					changed = TRUE;
				}
			}
			break;
		case 3:
			if ( EditTIME( 0x0403u, &SampleSet[SamplerSelect].suspend_1 ))
			{
				changed = TRUE;
			}
			break;

		case 4:
			if ( EditI16U( 0x0605u, &( SampleSet[SamplerSelect].set_loops ), 0x0200u ))
			{
				changed = TRUE;
			}
			break;
			
		case 5:
		//	对于 4路小流量采样器：设置C/D 时均 采样流量（A/B采样流量是相对固定的）
		//	对于 2路小流量采样器：设置A/B 采样流量
			SetupFlow_SHI();
			need_redraw = TRUE;
			break;
		
		case enumSelectXCH:
			SamplerTypeSwitch();
			return	FALSE;
		}
	} while ( enumSelectESC != item );

	if ( changed )
	{
		SampleSetSave();
	}
	return	TRUE;
}


void	menu_SampleSetup( void )
{
	static	struct uMenu  const  menu_AIR[] =
	{
		{ 0x0401u, "采样设置" },
		{ 0x0200u, "采样时间" },
		{ 0x0400u, "间隔时间" },
		{ 0x0600u, "次数" },
		{ 0x060Au, "流量" },	
	};

	static	struct uMenu  const  menu_TSP[] =
	{
		{ 0x0301u, "采样设置" },
		{ 0x0200u, "采样时间" },
		{ 0x0400u, "间隔时间" },
		{ 0x0600u, "次数" },
	};

	static	struct uMenu  const  menu_R24[] =
	{
		{ 0x0401u, "采样设置" },
		{ 0x0200u, "A采" },
		{ 0x0208u, "B采" },
		{ 0x0400u, "A停" },
		{ 0x0600u, "次数"},
	};

	static	struct uMenu  const  menu_SHI[] =
	{
		{ 0x0501u, "采样设置" },
		{ 0x0200u, "C采" },
		{ 0x0208u, "D采" },
		{ 0x0400u, "C停" },
		{ 0x0600u, "次数"},
		{ 0x060Au, "流量"}	//	时均比日均最后多出一个流量设置选项
	};
	BOOL	done = FALSE;

	do {
		monitor();
		
		switch ( SamplerSelect )
		{
		default:
		case Q_TSP:	done = setup_TSP_AIR( menu_TSP );			break;
		case Q_R24:	done = setup_R24_SHI( menu_R24, "B停" );	break;
		case Q_SHI:	done = setup_R24_SHI( menu_SHI, "D停" );	
		break;
		case Q_AIR:	done = setup_TSP_AIR( menu_AIR );			break;
		}
	} while( ! done );
}
BOOL	SampleFinishFState[SamplerNum_Max];
/********************************** 功能说明 ***********************************
*  设置启动采样的控制参数，原则上每次采样前需要重新设置的参数，并启动采样
*******************************************************************************/
static	BOOL	SampleStart( enum enumSamplerSelect SamplerSelect )
{
	static	struct uMenu  const  menu_Accurate_withZero[] =
	{
		{ 0x0103u, "启动采样" },
		{ 0x0400u, "开始时间" },
		{ 0x0602u, "调零" }, 
		{ 0x0609u, "启动" },
	};

	static	struct uMenu  const  menu_Delay_withZero[] =
	{
		{ 0x0103u, "启动采样" },
		{ 0x0400u, "延时时间" },
		{ 0x0602u, "调零" }, 
		{ 0x0609u, "启动" },
	};

	static	struct uMenu  const  menu_Accurate_noneZero[] =
	{
		{ 0x0102u, "启动采样" },
		{ 0x0400u, "开始时间" },
		{ 0x0609u, "启动" },
	};

	static	struct uMenu  const  menu_Delay_noneZero[] =
	{
		{ 0x0102u, "启动采样" },
		{ 0x0400u, "延时时间" },
		{ 0x0609u, "启动" },
	};

	struct uMenu const * menu;

	uint8_t	item = 1u;
	BOOL	need_redraw = TRUE;
	
	BOOL	changed_Delay = FALSE;
	BOOL	changed = FALSE;

	BOOL	needZero;
	
	switch ( SamplerSelect )
	{
	case Q_TSP:
	case Q_SHI:
	case Q_R24:
		needZero = TRUE;
		break;
	default:
	case Q_AIR:
		needZero = FALSE;
		break;
	}

	switch( Configure.Mothed_Delay )
	{
	case enumByAccurate:	menu = ( needZero ? menu_Accurate_withZero : menu_Accurate_noneZero );	break;
	default:
	case enumByDelay:		menu = ( needZero ? menu_Delay_withZero : menu_Delay_noneZero );		break;
	}
	
	do {
		if ( need_redraw )
		{
			cls();
			Menu_Redraw( menu );
			SamplerTypeShow( 0x000Au );
			need_redraw = FALSE;
		}

		Menu_Item_Mask( menu, item );
		do {
			uint16_t	now_hour_min = (( get_Now() / 60 ) % 1440 );
			
			Lputs( 0x0200u, "当前时间=" );
			ShowTIME( 0x0209u, now_hour_min );	//	显示当前时间
			if ( enumByAccurate == Configure.Mothed_Delay )
			{
				if ( ! changed_Delay )
				{
					SampleSet[SamplerSelect].delay1 = now_hour_min + 1u;
				}
			}
			ShowTIME( 0x0409u, SampleSet[SamplerSelect].delay1 );
		} while ( ! hitKey( 100u ));
		Menu_Item_Mask( menu, item );

		if ( needZero )
		{
			item = Menu_Select_Start_R24( menu, item );
		}
		else
		{
		item = Menu_Select2( menu, item ,TRUE);
		}

		switch( item )
		{
		case 1:
			if ( EditTIME( 0x0409u, &( SampleSet[SamplerSelect].delay1 )))
			{
				changed_Delay = TRUE;
				changed = TRUE;
			}
			
			++item;
			break;

		case 2:
			if ( needZero )
			{
				need_redraw = TRUE;

				switch ( SamplerSelect )
				{
				case Q_TSP:	CalibrateZero_TSP();	break;
				case Q_R24:	CalibrateZero_R24();	break;
				case Q_SHI:	CalibrateZero_SHI();	break;
				}
				++item;
				break;
			}
			//lint -fallthrough
		case 3:	//	启动采样
			if ( changed )
			{
				SampleSetSave();
			}
			Sampler_Start ( SamplerSelect );

			return	FALSE;	//	返回显示采样状态

		case enumSelectXCH:
			SamplerTypeSwitch();
			return	FALSE;
		case	enumSelectESC:
			{
				uint8_t i;
				for( i = 0; i < SamplerNum_Max; i++)
					SampleFinishFState[i] = FALSE;		
			}
			break;
		}
	} while ( enumSelectESC != item );
	
	return	TRUE;	//	返回，显示上级菜单
}

void	State_Finish( enum enumSamplerSelect SamplerSelect )
{
	
	if( (	Q_Sampler[SamplerSelect].state	== state_FINISH ) && SampleFinishFState[SamplerSelect] )
	{
		cls();
		SampleFinishFState[SamplerSelect] = FALSE;
		switch( SamplerSelect )
		{
		case	Q_TSP: Lputs( 0x0004, " TSP采样");	break;
		case	Q_R24: Lputs( 0x0004, "日均采样");	break;
		case	Q_SHI: Lputs( 0x0004, "时均采样");	break;
		case	Q_AIR: Lputs( 0x0004, "大气采样");	break;
		}
		Lputs( 0x0203,	"采样完成!");
		Lputs( 0x0401,  "查询采样结果?");
		do
		{
			show_std_clock();
		}while( !hitKey( 50 ) );

		switch( getKey() )
		{
		case K_OK:	menu_SampleQuery();	break;
		default:	break;
		}
	}		

}
/********************************** 功能说明 ***********************************
*  主菜单访问设置程序的接口
*******************************************************************************/
void	menu_SampleStart( void )
{
	BOOL	done = FALSE;
	
	do {
		monitor();
		State_Finish(	SamplerSelect );
		done = SampleStart( SamplerSelect );
	} while ( ! done );
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
