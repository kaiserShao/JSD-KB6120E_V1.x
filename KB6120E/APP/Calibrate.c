/**************** (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Calibrate.C
* 创 建 者: 董峰
* 描  述  : KB6120E 标定程序
* 最后修改: 2015年6月2日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "AppDEF.H"

/********************************** 数据定义 ***********************************
//	标定数据
*******************************************************************************/
struct	uCalibrateLocal		CalibrateLocal;
struct	uCalibrateRemote	CalibrateRemote;//[BS_Max][BC_Max];

void	menu_Calibrate_Battery( void );

/********************************** 功能说明 ***********************************
*  自动计算新的倍率值
*******************************************************************************/
//	uint16_t Calc_New_Scale( FP32 SetFlow, FP32 run_flow )
//	{
//		uint16_t scale = 1000u;
//		uint16_t ruleflow = (uint16_t)( SetFlow * 1.0e3f + .5f );
//		
//		if ( run_flow < 0.001f )
//		{
//			MsgBox( "无法计算零流量!", vbOKOnly );
//			return	0u;
//		}

//		cls();	Lputs( 0x0000u, "自动计算倍率" );
//		Lputs( 0x0300u,"校准流量计读数:" );
//		ShowI16U( 0x0504u, ruleflow, 0x0503u, "L/m" );

//		if ( ! EditI16U( 0x0504u, &ruleflow, 0x0503u ))
//		{
//			return 0u;
//		}
//			
//		scale = (uint16_t)(( ruleflow / ( run_flow * 1000.0f ) + 0.5f ));
//		
//		return	scale;
//	}

/********************************** 功能说明 ***********************************
*  切换传感器
*******************************************************************************/
void	ShowPumpSelect( uint16_t yx, enum enumPumpSelect PumpSelect )
{
	switch ( PumpSelect )
	{
	case PP_TSP  : 	Lputs( yx, "<粉 尘>" );	break;
	case PP_R24_A:	Lputs( yx, "<日均A>" );	break;
	case PP_R24_B:	Lputs( yx, "<日均B>" );	break;
	case PP_SHI_C:	Lputs( yx, "<时均C>" );	break;
	case PP_SHI_D:	Lputs( yx, "<时均D>" );	break;
  case PP_AIR  :	Lputs( yx, "<大 气>" );	break;
	default:		Lputs( yx, " 未知?" );	break;
	}
}

static	void	CalibrateSave_PP( enum enumPumpSelect PumpSelect )
{
	CalibrateSave( );
}

static	void	CalibrateLoad_PP( enum enumPumpSelect PumpSelect )
{
	CalibrateLoad( );
}


/********************************** 功能说明 ***********************************
* 传感器标定->大气压力
*******************************************************************************/
static	void	menu_Calibrate_Ba( void )
{
	struct	uMenu const menu[] = 
	{
		{ 0x0201u, "标定大气压" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" }
	};
	enum {
		opt_exit,
		opt_origin, opt_slope, 
		opt_max, opt_min = opt_origin
	};
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint8_t	option = opt_min;
	BOOL	changed = FALSE;

	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0405u, CalibrateLocal.origin_Ba, 0x0502u, NULL );
		ShowI16U( 0x0605u, CalibrateLocal. slope_Ba, 0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "大气压:" );
			ShowFP32( 0x0207u, _sys_get_Ba(), 0x0602u, "kPa" );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, &CalibrateLocal.origin_Ba, 0x0502u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, &CalibrateLocal. slope_Ba, 0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}
			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave();
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad();
					option = opt_exit;
					break;
				case vbCancel:
					cls();
					Menu_Redraw( menu );
					break;
				default:
					break;
				}
			}
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

	} while( opt_exit != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->环境温度
*******************************************************************************/
void	menu_Calibrate_Te( void )
{
	struct	uMenu const menu[] = 
	{
		{ 0x0201u, "标定环境温度" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" }
	};
	enum {
		opt_exit,
		opt_origin, opt_slope, 
		opt_max, opt_min = opt_origin
	};
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint8_t	option = opt_min;
	BOOL	changed = FALSE;

	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0405u, CalibrateRemote.origin_Te, 0x0602u, NULL );
		ShowI16U( 0x0605u, CalibrateRemote. slope_Te, 0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "温度:" );
			ShowFP32( 0x0205u, _sys_get_Te(), 0x0602u, "℃" );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, &CalibrateRemote.origin_Te, 0x0602u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, &CalibrateRemote. slope_Te, 0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}
			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave();
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad();
					option = opt_exit;
					break;
				case vbCancel:
					cls();
					Menu_Redraw( menu );
					break;
				default:
					break;
				}
			}
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

	} while( opt_exit != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->恒温箱温度
*******************************************************************************/
void	menu_Calibrate_HCBox_Temp( void )
{
	struct	uMenu const menu[] = 
	{
		{ 0x0201u, "标定恒温箱温度" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" }
	};
	enum {
		opt_exit,
		opt_origin, opt_slope, 
		opt_max, opt_min = opt_origin
	};
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint8_t	option = opt_min;
	BOOL	changed = FALSE;

	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0405u, CalibrateRemote.origin_HCBox_Temp, 0x0602u, NULL );
		ShowI16U( 0x0605u, CalibrateRemote. slope_HCBox_Temp, 0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "温度:" );
			ShowFP32( 0x0205u, get_HCBoxTemp(), 0x0602u, "℃" );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, &CalibrateRemote.origin_HCBox_Temp, 0x0602u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, &CalibrateRemote. slope_HCBox_Temp, 0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}
			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave();
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad();
					option = opt_exit;
					break;
				case vbCancel:
					cls();
					Menu_Redraw( menu );
					break;
				default:
					break;
				}
			}
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

	} while( opt_exit != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->加热器温度
*******************************************************************************/
void	menu_Calibrate_Heater_Temp( void )
{
	struct	uMenu const menu[] = 
	{
		{ 0x0201u, "标定加热器温度" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" }
	};
	enum {
		opt_exit,
		opt_origin, opt_slope, 
		opt_max, opt_min = opt_origin
	};
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint8_t	option = opt_min;
	BOOL	changed = FALSE;

	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0405u, CalibrateRemote.origin_Heater_Temp, 0x0602u, NULL );
		ShowI16U( 0x0605u, CalibrateRemote. slope_Heater_Temp, 0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "温度:" );
			ShowFP32( 0x0205u, get_HeaterTemp(), 0x0602u, "℃" );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, &CalibrateRemote.origin_Heater_Temp, 0x0602u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, &CalibrateRemote. slope_Heater_Temp, 0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}
			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave();
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad();
					option = opt_exit;
					break;
				case vbCancel:
					cls();
					Menu_Redraw( menu );
					break;
				default:
					break;
				}
			}
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

	} while( opt_exit != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->计前温度
*******************************************************************************/
static	void	Calibrate_Tr ( enum enumPumpSelect PumpSelect )
{
	static	struct  uMenu  const  menu[] = 
	{
		{ 0x0201u, "标定计温" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" }
	};
	enum {
		opt_exit,
		opt_origin, opt_slope, 
		opt_max, opt_min = opt_origin
	};
	uint8_t	option = opt_min;

	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	BOOL	need_redraw = TRUE;

	uint16_t	*p_origin = &CalibrateRemote.origin[esid_tr][PumpSelect];
	uint16_t	*p_slope  = &CalibrateRemote.slope [esid_tr][PumpSelect];
	BOOL		changed = FALSE;

	do {
		if ( need_redraw )
		{
			cls();
			Menu_Item_Mask( menu, option );
			Menu_Redraw( menu );
			ShowPumpSelect( 0x0009u, PumpSelect );

			need_redraw = FALSE;
		}
		
		ShowI16U( 0x0405u, *p_origin, 0x0602u, NULL );
		ShowI16U( 0x0605u, *p_slope , 0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "温度:" );
			ShowFP32( 0x0205u, get_Tr( PumpSelect ), 0x0602u, "℃" );
		} while( ! hitKey( 25u ));
		need_redraw = TRUE;
		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, p_origin, 0x0602u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, p_slope , 0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}

			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave_PP( PumpSelect );
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad_PP( PumpSelect );
					option = opt_exit;
					break;
				default:
					break;
				}
				
				need_redraw = TRUE;
			}
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

	} while( opt_exit != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->计前压力
*******************************************************************************/
static	void	Calibrate_Pr( enum enumPumpSelect PumpSelect )
{
	struct	uMenu const menu[] = 
	{
		{ 0x0201u, "标定计压" },
// 		{ 0x0200u, "计压" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" }
	};
	enum {
		opt_exit,
		 opt_origin, opt_slope, 
		opt_max, opt_min = 1
	};
	uint8_t	option = opt_min;
	BOOL	need_redraw = TRUE;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint16_t	*p_origin = &CalibrateRemote.origin[esid_pr][PumpSelect];
	uint16_t	*p_slope  = &CalibrateRemote.slope [esid_pr][PumpSelect];
	BOOL		changed   = FALSE;

	do {
		if ( need_redraw )
		{
			cls();
			Menu_Redraw( menu );
			ShowPumpSelect( 0x0009u, PumpSelect );

			need_redraw = FALSE;
		}
		
		ShowI16U( 0x0405u, *p_origin, 0x0500u, NULL );
		ShowI16U( 0x0605u, *p_slope,  0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "计压:" );
			ShowFP32( 0x0205, get_Pr( PumpSelect ), 0x0602, "kPa" );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, p_origin, 0x0500u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, p_slope,  0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}
			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave_PP( PumpSelect );
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad_PP( PumpSelect );
					option = opt_exit;
					break;
				default:
					break;
				}
				need_redraw = TRUE;
			}
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

	} while( opt_exit != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->孔板差压
*******************************************************************************/
static	void	Calibrate_pf(  enum enumPumpSelect PumpSelect )
{
	struct	uMenu const	menu[] = {
		{ 0x0201u, "标定差压" },
		{ 0x0400u, "零点" },
		{ 0x0600u, "倍率" },
	};
	enum {
		opt_exit,
		 opt_origin, opt_slope, 
		opt_max, opt_min = 1
	};
	uint8_t option = opt_min;
	BOOL	need_redraw = TRUE;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint16_t	*p_origin = &CalibrateRemote.origin[esid_pf][PumpSelect];
	uint16_t	*p_slope  = &CalibrateRemote.slope [esid_pf][PumpSelect];
	BOOL		changed   = FALSE;

	do {
		if ( need_redraw )
		{
			cls();
			Menu_Redraw( menu );
			ShowPumpSelect( 0x0009u, PumpSelect );

			need_redraw = FALSE;
		}
		ShowI16U( 0x0405u, *p_origin, 0x0500u, NULL );
		ShowI16U( 0x0605u, *p_slope,  0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			Lputs( 0x0200u, "差压:" );
			ShowFP32( 0x0205, get_pf( PumpSelect ), 0x0603, "kPa" );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_origin:
				if ( EditI16U( 0x0405u, p_origin, 0x0500u ))
				{
					changed = TRUE;
				}
				break;
			case opt_slope:
				if ( EditI16U( 0x0605u, p_slope,  0x0503u ))
				{
					changed = TRUE;
				}
				break;
			default:
				break;
			}
			break;

			case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave_PP( PumpSelect );
					option = opt_exit;
					break;
				case vbNo:
					CalibrateLoad_PP( PumpSelect );
					option = opt_exit;
					break;
				default:
					break;
				}
				
				need_redraw = TRUE;
			}
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

	} while( opt_exit != option );
}



/********************************** 功能说明 ***********************************
*  流量调校程序
*******************************************************************************/
BOOL	CalibrateFLOW_4_Point_Debug( enum enumPumpSelect PumpSelect, uint16_t FlowKSet[], FP32 const PointSet[], uint8_t PointSelect )
{
	enum 
	{
		opt_exit, opt_standard,
	};
	uint8_t	option = opt_standard;
	BOOL	changed = FALSE;
	BOOL	need_redraw = TRUE;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	uint16_t FlowKSave[4];
	uint16_t	slope;
	uint32_t Flow32;
	FP32  Flown,Flow = 0.0f;

	//	暂存原始数据，包括所有点的倍率、设定流量。
	slope = FlowKSet[PointSelect];
	FlowKSave[0] = FlowKSet[0];
	FlowKSave[1] = FlowKSet[1];
	FlowKSave[2] = FlowKSet[2];
	FlowKSave[3] = FlowKSet[3];
	// 设置流量
	Configure.SetFlow[PumpSelect] = ( uint16_t)( PointSet[PointSelect] * 1.0e+1f + 0.5f );
	//	开启泵流量控制
	Pump_OutCmd( PumpSelect, TRUE ); 
	do {
		if ( need_redraw )
		{
			cls();	Lputs( 0x0000u, "调试流量");			
			need_redraw = FALSE;
		}		
		//	将所有点的倍率调整到当前倍率
		FlowKSet[0] =
		FlowKSet[1] =
		FlowKSet[2] =
		FlowKSet[3] = slope;
		do {	// 实时显示流量
			Flow = get_fstd( PumpSelect );
			Lputs( 0x0000u, "流量倍率:" );	ShowI16U( 0x000Au, slope, 0x0503u, NULL );
			Lputs( 0x0300u, "流量:" );	ShowFP32( 0x0305u, Flow, 0x0603u, "L/m" );
			Lputs( 0x0600u, "输出:" );	ShowPercent( 0x0605u, Pump_GetOutput( PumpSelect ));		
		} while ( ! hitKey( 100u ));

		switch ( getKey())
		{
		case K_UP:
			if ( slope < 9999u )
			{
				++slope;
			}
			changed = TRUE;
			break;

		case K_DOWN:
			if ( slope > 99u )
			{
				--slope;
			}
			changed = TRUE;
			break;

		case K_OK:
			Flow32 = Configure.SetFlow[PumpSelect] * 100;
			LcmMask( 0x0300u, 4u );
			if ( EditI32U( 0x0305u, &Flow32, 0x0603u ) )
			{
				changed = TRUE;
				Flown = ( FP32 ) Flow32;
				if( slope != 0 )
					Flow  = Flow / ( slope *0.001);
				else
					Flow = Flown / 1000;
				if( Flow != 0 )				
					slope =(uint16_t) ( Flown / Flow ) ; 
				else
					slope = 1000;
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

	
	} while ( opt_exit != option );
	
	Pump_OutCmd( PumpSelect, FALSE ); 
	// 	恢复除当前点之外的其他校准点的倍率。
	FlowKSet[0] = FlowKSave[0];
	FlowKSet[1] = FlowKSave[1];
	FlowKSet[2] = FlowKSave[2];
	FlowKSet[3] = FlowKSave[3];
	FlowKSet[PointSelect] = slope;

	return	changed;
}

BOOL	CalibrateFlow_1_Point_DEBUG( enum enumPumpSelect PumpSelect, uint16_t * const p_FlowK )
{
	BOOL	done = FALSE;
	BOOL	changed = FALSE;
	uint32_t Flow32 = 0;
	FP32  Flown,Flow = 0.0f;
	uint16_t	fmt = 0x0000u;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	const CHAR * szUnit = NULL;
	
	switch ( PumpSelect )
	{
	case PP_TSP  :	fmt = 0x0501u;	szUnit = "L/m";	break;
	case PP_R24_A:	fmt = 0x0503u;	szUnit = "L/m";	break;
	case PP_R24_B:	fmt = 0x0503u;	szUnit = "L/m";	break;
	default:	break;
	}

	cls();
	
	Pump_OutCmd( PumpSelect, TRUE );
	
	while ( ! done )
	{
		
		do {	//	实时显示流量
			FP32	fstd = get_fstd( PumpSelect );
			if( PumpSelect == PP_TSP )
			{						
				FP32	Ba = get_Ba();
				FP32	Te = get_Te();
				FP32	flow = Calc_flow( fstd, Te, 0.0f, Ba, Q_TSP );	
				Flow = flow;
				Lputs( 0x0000u, "流量倍率:" );	ShowI16U( 0x000Au, * p_FlowK, 0x0503u, NULL );	
				Lputs ( 0x0200u, "工况:" );		ShowFP32( 0x0205u, flow, fmt, szUnit );
				Lputs ( 0x0400u, "标况:" );		ShowFP32( 0x0405u, fstd, fmt, szUnit );
				Lputs ( 0x0600u, "输出:" );		ShowPercent( 0x0605u, Pump_GetOutput( PumpSelect ));
			}
			else
			{
 			Flow = fstd;
			Lputs( 0x0000u, "流量倍率:" );	ShowI16U( 0x000Au, * p_FlowK, 0x0503u, NULL );	
			Lputs ( 0x0300u, "标况:" );		ShowFP32( 0x0305u, fstd, fmt, szUnit );
			Lputs ( 0x0600u, "输出:" );		ShowPercent( 0x0605u, Pump_GetOutput( PumpSelect ));
			}
		} while ( ! hitKey( 100u ));
		switch ( getKey())
		{
		case K_UP:
			if ( * p_FlowK < 9999u )
			{
				++ * p_FlowK;
			}
			changed = TRUE;
			break;
		case K_DOWN:
			if ( * p_FlowK > 99u )
			{
				-- * p_FlowK;
			}
			changed = TRUE;
			break;
		case K_OK:

			Flow32 = Configure.SetFlow[PumpSelect];
			if( PumpSelect == PP_TSP )
			{
				LcmMask( 0x0200u, 4u );

				if ( EditI32U( 0x0205u, &Flow32, 0x0501u ) )
				{
					changed = TRUE;
					Flown = ( FP32 ) Flow32 * 0.100;
					if( * p_FlowK != 0 )
						Flow  = Flow / ( * p_FlowK * 0.001 );
					else
						Flow = Flown;
					if(Flow != 0)
						* p_FlowK =(uint16_t) ( Flown * 1000 / Flow ) ;
					else
						* p_FlowK = 1000;
				}
			}
			else
			{
				LcmMask( 0x0300u, 4u );
				if ( EditI32U( 0x0305u, &Flow32, 0x0503u ) )
				{
					changed = TRUE;
					Flown = ( FP32 ) Flow32 * 0.001;
					if( * p_FlowK != 0 )
						Flow  = Flow / ( * p_FlowK * 0.001 );
					else
						Flow = Flown;
					if(Flow != 0)
						* p_FlowK =(uint16_t) ( Flown * 1000 / Flow ) ;
					else
						* p_FlowK = 1000;
				}
			}
			break;
		case K_ESC:
			done = TRUE;
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
	
	Pump_OutCmd( PumpSelect, FALSE );
	
	return	changed;
}

/********************************** 功能说明 ***********************************
* 传感器标定 -> 粉尘流量（单点标定）
*********************************** 功能说明 ***********************************
* 传感器标定 -> 日均流量（单点标定）
*******************************************************************************/
static	void	CalibrateFlow_1_Point( enum enumPumpSelect PumpSelect )
{
	struct	uMenu const	menu[] = 
	{
		{ 0x0301u, "标定流量" },
		{ 0x0200u, "流量" },
		{ 0x0404u, "启动" },
		{ 0x0600u, "倍率" },
	};
	BOOL	need_redraw = TRUE;

	uint8_t	option  = 1u;

	uint16_t	*p_FlowK = CalibrateRemote.slope_flow[PumpSelect];
	BOOL		changed  = FALSE;
	uint16_t	fmt;
	
	switch ( PumpSelect )
	{
	default:
	case PP_TSP:	fmt = 0x0501u; 	break;
	case PP_R24_A:
	case PP_R24_B:	fmt = 0x0503u;	break;
	}
	
	do {
		if ( need_redraw )
		{
			cls();
			Menu_Redraw( menu );
			ShowPumpSelect( 0x0009u, PumpSelect );
			need_redraw = FALSE;
		}

		ShowI16U ( 0x0205u, Configure.SetFlow[PumpSelect], fmt, "L/m" );
		ShowI16U ( 0x0605u, * p_FlowK, 0x0503u, NULL );

		option = Menu_Select( menu, option );
		switch( option )
		{
		case 0:
			if ( changed )
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave();
					break;
				case vbNo:
					CalibrateLoad();
					break;
				default:
				case vbCancel:
					option = 1u;
					break;
				}

				need_redraw = TRUE;
			}
			break;
		case 1:
			if ( EditI16U( 0x0205u, &Configure.SetFlow[PumpSelect], fmt ))
			{
				ConfigureSave();			
			}
			break;
		case 2:
			if ( CalibrateFlow_1_Point_DEBUG( PumpSelect, p_FlowK ))
			{
				changed = TRUE;
			}

			need_redraw = TRUE;
			break;
		case 3:
			if ( EditI16U( 0x0605u, p_FlowK, 0x0503u ))
			{
				changed = TRUE;
			}
			break;

		default:
			break;
		}
	} while( enumSelectESC != option );
}

/********************************** 功能说明 ***********************************
* 传感器标定->时均流量（多点标定，具体的标定点根据设定的孔板的流量范围进行选择）
*******************************************************************************/
static	BOOL	CalibrateFLOW_4_Point_1_Point( enum enumPumpSelect PumpSelect, uint16_t FlowKSet[], FP32 const PointSet[], uint8_t PointSelect )
{
//	extern	BOOL	CalibrateFLOW_4_Point_Debug( enum enumPumpSelect PumpSelect, uint16_t * p_FlowK, FP32 set_flow );

	struct	uMenu  const  menu[] = 
	{
		{ 0x0201u, "标定流量" },
		{ 0x0304u, "启动" },
		{ 0x0600u, "倍率" },
	};
	BOOL	changed = FALSE;
	uint8_t	option  = 1u;
	
	do {
		cls();	Menu_Redraw( menu );

		ShowFP32( 0x000Au, PointSet[PointSelect], 0x0301u, "L/m" );
		ShowI16U( 0x0605u, FlowKSet[PointSelect], 0x0503u, NULL );

		option  = Menu_Select( menu, option );
		
		switch ( option )
		{
		case 1:
			if ( CalibrateFLOW_4_Point_Debug( PumpSelect, FlowKSet, PointSet, PointSelect ))
			{
				changed = TRUE;
			}
			break;

		case 2:
			if ( EditI16U( 0x0605u, &FlowKSet[PointSelect], 0x0503u ))
			{
				changed = TRUE;
			}
			break;

		default:
			break;
		}

	} while ( 0u != option );
	
	return	changed;
}


extern FP32  const  PumpPoints[4][4];

FP32  const  PumpPoints[4][4] = 
{
	{ 0.0f, 0.0f, 0.0f, 0.0f },	//	enumPumpNone
	{ 0.2f, 0.5f, 0.8f, 1.0f },	//	enumOrifice_1
	{ 0.5f, 1.0f, 1.5f, 2.0f },	//	enumOrifice_2
	{ 0.2f, 0.5f, 0.8f, 1.0f },	//	enumMassflow
};

static	void	CalibrateFlow_4_Point( enum enumPumpSelect PumpSelect )
{
	static	struct	uMenu const	menu_1L[] =
	{
		{ 0x0202u, "标定流量" },
		{ 0x0301u, "0.2L/m" },
		{ 0x0309u, "0.5L/m" },
		{ 0x0601u, "0.8L/m" },
		{ 0x0609u, "1.0L/m" }
	};
	static	struct	uMenu const	menu_2L[] =
	{
		{ 0x0202u, "标定流量" },
		{ 0x0301u, "0.5L/m" },
		{ 0x0309u, "1.0L/m" },
		{ 0x0601u, "1.5L/m" },
		{ 0x0609u, "2.0L/m" }
	};
	struct uMenu const * menu;
	uint8_t	option  = 1u;

	uint8_t		PumpType = Configure.PumpType[PumpSelect];
	uint16_t *  p_FlowK4 = CalibrateRemote.slope_flow[PumpSelect];
	BOOL		changed = FALSE;
	
	switch ( PumpType )
	{
	default:	return;
	case enumOrifice_1:	menu = menu_1L;	break;
	case enumOrifice_2:	menu = menu_2L;	break;
	}

	do {
		cls();
		Menu_Redraw( menu );
		
		option = Menu_Select( menu, option );
		switch( option )
		{
		case 0:
			if ( changed )
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					CalibrateSave();
					break;
				case vbNo:
					CalibrateLoad();
					break;
				default:
					break;
				}
			}
			break;
		case 1:	if ( CalibrateFLOW_4_Point_1_Point( PumpSelect, p_FlowK4, PumpPoints[PumpType], 0 )){ changed = TRUE; }	break;
		case 2:	if ( CalibrateFLOW_4_Point_1_Point( PumpSelect, p_FlowK4, PumpPoints[PumpType], 1 )){ changed = TRUE; }	break;
		case 3:	if ( CalibrateFLOW_4_Point_1_Point( PumpSelect, p_FlowK4, PumpPoints[PumpType], 2 )){ changed = TRUE; }	break;
		case 4:	if ( CalibrateFLOW_4_Point_1_Point( PumpSelect, p_FlowK4, PumpPoints[PumpType], 3 )){ changed = TRUE; }	break;
		default:	break;
		}
	} while( 0u != option );
}

extern	void	Configure_Pr_Protect( void );	//	MsgBox("TODO:压力保护", vbOKOnly );

void	menu_Calibrate_Orifice_KB100( enum enumPumpSelect PumpSelect )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0302u, "孔板标定" },
		{ 0x0201u, "温度" },{ 0x0207u, "自动调零" },
		{ 0x0401u, "计压" },{ 0x0406u, "流量值修正" }, 
		{ 0x0601u, "差压" },{ 0x0607u, "压力保护" }, 
	};
	uint8_t	item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		ShowPumpSelect( 0x0009u, PumpSelect );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:		Calibrate_Tr( PumpSelect );		break;
		case 3:		Calibrate_Pr( PumpSelect );		break;
		case 5:		Calibrate_pf( PumpSelect );		break;
		case 2:
			switch ( PumpSelect )
			{
			default:
			case PP_TSP  : 	CalibrateZero_TSP();	break;
			case PP_R24_A:		
			case PP_R24_B:	CalibrateZero_R24();  	break;
			case PP_SHI_C:		
			case PP_SHI_D:	CalibrateZero_SHI(); 	break;
			}
			break;
		case 4:
			switch ( PumpSelect )
			{
			default:
			case PP_TSP  :
			case PP_R24_A:
			case PP_R24_B:	CalibrateFlow_1_Point( PumpSelect );  	break;
			case PP_SHI_C:
			case PP_SHI_D:	CalibrateFlow_4_Point( PumpSelect ); 	break;
			}
			break;
		case 6:
			Configure_Pr_Protect( );
		
			break;
		default:
			break;
		}
	} while( enumSelectESC != item );
}

static	void	menu_Select_Calc_Pbv( void )
{
    static	struct  uMenu  const   menu[] =
    {
        { 0x0201u, "计算饱和水汽压" },
        { 0x0303u, "[不 计 算]" },
        { 0x0503u, "[参与计算]" },
    };

    uint8_t	item;

    cls();
    Menu_Redraw( menu );

    if ( Configure .shouldCalcPbv == 0 )
    {
        Lputs( 0x0300u, " ->" );
        item = 1u;
	}
	else
	{
        Lputs( 0x0500u, " ->" );
        item = 2u;
	}

    item = Menu_Select( menu, item );

    switch( item )
    {
    case 1:
	case 2:
		Configure .shouldCalcPbv = ! Configure .shouldCalcPbv ;
		ConfigureSave();
        break;
    default:
        break;
    }
}

void	menu_Calibrate_Orifice_KB4( enum enumPumpSelect PumpSelect )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0302u, "孔板标定" },
		{ 0x0201u, "温度" },{ 0x0207u, "自动调零" },
		{ 0x0401u, "计压" },{ 0x0406u, "流量值修正" }, 
		{ 0x0601u, "差压" },{ 0x0606u, "饱和水汽压" }, 
	};
	uint8_t	item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		ShowPumpSelect( 0x0009u, PumpSelect );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:		Calibrate_Tr( PumpSelect );		break;
		case 3:		Calibrate_Pr( PumpSelect );		break;
		case 5:		Calibrate_pf( PumpSelect );		break;
		case 2:
			switch ( PumpSelect )
			{
			default:
			case PP_TSP  : 	CalibrateZero_TSP();	break;
			case PP_R24_A:		
			case PP_R24_B:	CalibrateZero_R24();  	break;
			case PP_SHI_C:		
			case PP_SHI_D:	CalibrateZero_SHI(); 	break;
			}
			break;
		case 4:
			switch ( PumpSelect )
			{
			default:
			case PP_TSP  :
			case PP_R24_A:
			case PP_R24_B:	CalibrateFlow_1_Point( PumpSelect );  	break;
			case PP_SHI_C:
			case PP_SHI_D:	CalibrateFlow_4_Point( PumpSelect ); 	break;
			}
			break;
		case 6:
			//	MsgBox("TODO:饱和水汽压", vbOKOnly );
			menu_Select_Calc_Pbv();
			break;
		default:
			break;
		}
	} while( enumSelectESC != item );
}

static	void	menu_Calibrate_Other( void )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0202u, "传感器标定" },
		{ 0x0201u, "大气压" }, { 0x0208u, "环境温度" },
		{ 0x0401u, "加热器" },{ 0x0408u, "电池电压" },
	};
	static	struct	uMenu const	menu1[] = 
	{
		{ 0x0202u, "传感器标定" },
		{ 0x0201u, "大气压" }, { 0x0208u, "环境温度" },
		{ 0x0401u, "恒温箱" }, { 0x0408u, "电池电压" }, 
	};
	static	struct	uMenu const	menu2[] = 
	{
		{ 0x0301u, "传感器标定" },
		{ 0x0201u, "大气压" }, 
		{ 0x0401u, "环境温度" },
		{ 0x0601u, "加热器" },
	};
		static	struct	uMenu const	menu3[] = 
	{
		{ 0x0301u, "传感器标定" },
		{ 0x0201u, "大气压" }, 
		{ 0x0401u, "环境温度" },
		{ 0x0601u, "恒温箱" },
	};
	uint8_t	item = 1u;

	do {
		  cls();
			  if( Configure.HeaterType == enumHeaterOnly && Configure.Battery_SW == TRUE )
				{	
					Menu_Redraw( menu );
					item = Menu_Select( menu, item );
				}
				else if( Configure.HeaterType != enumHeaterOnly && Configure.Battery_SW == TRUE )
				{
          Menu_Redraw( menu1 );
					item = Menu_Select( menu1, item );
				}
				else if( Configure.HeaterType == enumHeaterOnly && Configure.Battery_SW == FALSE )
				{
          Menu_Redraw( menu2 );
					item = Menu_Select( menu2, item );
				}
				else
				{
          Menu_Redraw( menu3 );
					item = Menu_Select( menu3, item );
				}
		switch( item )
		{
		case 1:	menu_Calibrate_Ba();	break;
		case 2:	menu_Calibrate_Te();	break;
		case 3:	
			if( Configure.HeaterType == enumHeaterOnly )
				menu_Calibrate_Heater_Temp();	
			else
				menu_Calibrate_HCBox_Temp();
			break;	
		case 4: menu_Calibrate_Battery( );		break;
		default:	
			break;
		}
	} while( enumSelectESC != item );
}


void	menu_Calibrate_Orifice( enum enumPumpSelect PumpSelect )
{
	switch ( PumpSelect )
	{
	case PP_TSP:	menu_Calibrate_Orifice_KB100( PumpSelect );	break;
	default:		menu_Calibrate_Orifice_KB4  ( PumpSelect );	break;
	}
}

/********************************** 功能说明 ***********************************
* 主菜单 -> 维护菜单 -> 标定菜单
*******************************************************************************/
static	void	menu_Calibrate_PP( enum enumPumpSelect PumpSelect )
{
	switch ( Configure.PumpType[PumpSelect] )
	{
	case enumPumpNone: break;
	case enumOrifice_1:
	case enumOrifice_2:	menu_Calibrate_Orifice( PumpSelect );	break;
	}
}

void	menu_Calibrate_6120C( void )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0203u, "传感器标定" },
		{ 0x0300u, "粉尘" }, { 0x0305u, "日均A" }, { 0x030Bu, "日均B" },
		{ 0x0600u, "时均C"}, { 0x0606u, "时均D" }, { 0x060Cu, "其他" },
	};
	uint8_t item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:	menu_Calibrate_PP( PP_TSP   );	break;
		case 2:	menu_Calibrate_PP( PP_R24_A );	break;
		case 3:	menu_Calibrate_PP( PP_R24_B );	break;
		case 4:	menu_Calibrate_PP( PP_SHI_C );	break;
		case 5:	menu_Calibrate_PP( PP_SHI_D );	break;
		case 6:	menu_Calibrate_Other( );		break;
		default:	break;
		}
	} while( enumSelectESC != item );
}

void	menu_Calibrate_KB6120A( void )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0302u, "标定" },
		{ 0x0201u, "温度" },{ 0x0207u, "自动调零" },
		{ 0x0401u, "计压" },{ 0x0406u, "流量值修正" }, 
		{ 0x0601u, "差压" },{ 0x0607u, "其他参数" }, 
	};
	uint8_t	item = 1u;

	enum	enumPumpSelect	PumpSelect = PP_TSP;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:		Calibrate_Tr( PumpSelect );		break;
		case 3:		Calibrate_Pr( PumpSelect );		break;
		case 5:		Calibrate_pf( PumpSelect );		break;
		case 2:
			switch ( PumpSelect )
			{
			default:
			case PP_TSP  : 	CalibrateZero_TSP();	break;
			case PP_R24_A:		
			case PP_R24_B:	CalibrateZero_R24();  	break;
			case PP_SHI_C:		
			case PP_SHI_D:	CalibrateZero_SHI(); 	break;
			}
			break;
		case 4:
			switch ( PumpSelect )
			{
			default:
			case PP_TSP  :
			case PP_R24_A:
			case PP_R24_B:	CalibrateFlow_1_Point( PumpSelect );  	break;
			case PP_SHI_C:
			case PP_SHI_D:	CalibrateFlow_4_Point( PumpSelect ); 	break;
			}
			break;
		case 6:
			menu_Calibrate_Other();
			break;
		default:
			break;
		}
	} while( enumSelectESC != item );
}

void	menu_Calibrate_KB6120AD2( void )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0202u, "传感器标定" },
		{ 0x0303u, "粉尘" }, { 0x0309u, "日均A" },
		{ 0x0603u, "其他" }, { 0x0609u, "日均B" },
	};
	uint8_t item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:	menu_Calibrate_PP( PP_TSP   );	break;
		case 2:	menu_Calibrate_PP( PP_R24_A );	break;
		case 4:	menu_Calibrate_PP( PP_R24_B );	break;
		case 3:	menu_Calibrate_Other( );		break;
		default:	break;
		}
	} while( enumSelectESC != item );
}

void	menu_Calibrate_KB2400HL( void )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0103u, "传感器标定" },
		{ 0x0300u, "时均C" },
		{ 0x0306u, "时均D" },
		{ 0x030Cu, "其他" }, 
	};
	uint8_t item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:	menu_Calibrate_PP( PP_SHI_C );	break;
		case 2:	menu_Calibrate_PP( PP_SHI_D );	break;
		case 3:	menu_Calibrate_Other( );		break;
		default:	break;
		}
	} while( enumSelectESC != item );
}
void	menu_Calibrate_KB6120AD( void )
{
	static	struct	uMenu const	menu[] = 
	{
		{ 0x0202u, "传感器标定" },
		{ 0x0303u, "粉尘" }, { 0x0309u, "时均C" },
		{ 0x0603u, "其他" }, { 0x0609u, "时均D" },
	};
	uint8_t item = 1u;

	do {
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:	menu_Calibrate_PP( PP_TSP   );	break;
		case 2:	menu_Calibrate_PP( PP_SHI_C );	break;
		case 4:	menu_Calibrate_PP( PP_SHI_D );	break;
		case 3:	menu_Calibrate_Other( );		    break;
		default:	break;
		}
	} while( enumSelectESC != item );
}
void	menu_Calibrate( void )
{
	uint8_t TO = Configure.TimeoutLight;
	Configure.TimeoutLight = 4;
	DisplaySetTimeout( Configure.TimeoutLight );
	switch ( Configure.InstrumentType )
	{
	case type_KB6120A:	 menu_Calibrate_KB6120A();	  break;
	case type_KB6120AD:	 menu_Calibrate_KB6120AD();	  break;
	case type_KB2400HL:	 menu_Calibrate_KB2400HL();	  break;
	case type_KB6120AD2: menu_Calibrate_KB6120AD2();	break;
	}
	Configure.TimeoutLight = TO;
	DisplaySetTimeout( Configure.TimeoutLight );
}

/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File ****/

/********************************** 功能说明 ***********************************
* 传感器标定->电池电压、电流
*******************************************************************************/
extern	FP32	get_Bat_Voltage( void );
extern	FP32	get_Bat_Current( void );

void	menu_Calibrate_Battery( void )
{
	struct	uMenu const menu[] = 
	{
		{ 0x0301u, NULL }, // "电池" },
		{ 0x0200u, "电压" },
		{ 0x0400u, "电流" },
		{ 0x0600u, "门限" },
		
	};
	enum {
		opt_exit,
		opt_voltage, opt_current, opt_threshold,
		opt_max, opt_min = opt_voltage
	};
	uint8_t	option = opt_min;
	BOOL	changed = FALSE;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0205u, Configure.slope_Bat_Voltage, 0x0503u, NULL );
		ShowI16U( 0x0405u, Configure.slope_Bat_Current, 0x0503u, NULL );
		ShowI16U( 0x0605u, Configure.threshold_Current, 0x0503u, NULL );

		Menu_Item_Mask( menu, option );
		do{
			ShowFP32( 0x0000u, get_Bat_Voltage(), 0x0603u, "V," );
			ShowFP32( 0x0008u, get_Bat_Current(), 0x0603u, "A " );
		} while( ! hitKey( 25u ));
		Menu_Item_Mask( menu, option );

		switch( getKey())
		{
		case K_DOWN:
			++option;
			if ( option >= opt_max )
			{
				option = opt_min;
			}
			break;
		case K_UP:
			if( option <= opt_min )
			{
				option = opt_max;
			}
			--option;
			break;
		
		case K_OK:
			switch( option )
			{
			case opt_voltage:
				if ( EditI16U( 0x0205u, &Configure.slope_Bat_Voltage, 0x0503u ))
				{
					changed = TRUE;
				}
				break;
			case opt_current:
				if ( EditI16U( 0x0405u, &Configure.slope_Bat_Current, 0x0503u ))
				{
					changed = TRUE;
				}
				break;
				
			case opt_threshold:
				if ( EditI16U( 0x0605u, &Configure.threshold_Current, 0x0503u ))
				{
					changed = true;
				}
				break;
				
			default:
				break;
			}
			break;

		case K_ESC:
			if( ! changed )
			{
				option = opt_exit;
			}
			else
			{
				switch( MsgBox( "保存标定数据 ?", vbYesNoCancel + vbDefaultButton3 ))
				{
				case vbYes:
					ConfigureSave();	//	CalibrateSave();
					option = opt_exit;
					break;
				case vbNo:
					ConfigureLoad();	//	CalibrateLoad();
					option = opt_exit;
					break;
				case vbCancel:
					cls();
					Menu_Redraw( menu );
					break;
				default:
					break;
				}
			}
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

	} while( opt_exit != option );
}

