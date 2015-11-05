/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Setup.C
* �� �� ��: Dean
* ��  ��  : KB-6120E �������ó���
* ����޸�: 2013��12��18��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"

/********************************** ���ݶ��� **********************************/
struct	uSampleSet	SampleSet[SamplerNum_Max];	//	��������

/********************************** ����˵�� ***********************************
*  ���ò�������Ŀ��Ʋ�����ԭ�������úõĲ�����ÿ�β�������Ҫ�䶯��
*******************************************************************************/
void	SetupFlow_AIR( void )
{
	static	struct uMenu  const  menu[] =
	{
		{ 0x0201u, "��������" },
		{ 0x0300u, "������" },
		{ 0x0500u, "������" },
	};
	uint8_t item = 1u;
	
	uint16_t	iflow = Configure.AIRSetFlow[Q_PP1];
	uint16_t	iiflow = Configure.AIRSetFlow[Q_PP2];
	BOOL	changed = FALSE;
	
	cls();
	Menu_Redraw( menu );
	do {
		ShowI16U( 0x0308u, iflow, 0x0301u, "L/m" );
		ShowI16U( 0x0508u, iiflow, 0x0301u, "L/m" );

		item = Menu_Select2( menu, item ,FALSE);

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
		case enumSelectXCH:		SamplerTypeSwitch();	return	FALSE;	//	���ص��ϼ��˵����л�������������
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
		{ 0x0201u, "��������" },
		{ 0x0300u, "����C" },
		{ 0x0500u, "����D" },
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
					changed = TRUE;
					break;
				case enumOrifice_2:
					if ( cflow <  5u ) { cflow =  5u; }
					if ( cflow > 20u ) { cflow = 20u; }
					changed = TRUE;
					break;
				default:
					break;
				}
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
					changed = TRUE;
					break;
				case enumOrifice_2:
					if ( dflow <  5u ) { dflow =  5u; }
					if ( dflow > 20u ) { dflow = 20u; }
					changed = TRUE;
					break;
				default:
					break;
				}
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
		//	���� 4·С����������������C/D ʱ�� ����������A/B������������Թ̶��ģ�
		//	���� 2·С����������������A/B ��������
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
		{ 0x0401u, "��������" },
		{ 0x0200u, "����ʱ��" },
		{ 0x0400u, "���ʱ��" },
		{ 0x0600u, "����" },
		{ 0x060Au, "����" },	
	};

	static	struct uMenu  const  menu_TSP[] =
	{
		{ 0x0301u, "��������" },
		{ 0x0200u, "����ʱ��" },
		{ 0x0400u, "���ʱ��" },
		{ 0x0600u, "����" },
	};

	static	struct uMenu  const  menu_R24[] =
	{
		{ 0x0401u, "��������" },
		{ 0x0200u, "A��" },
		{ 0x0208u, "B��" },
		{ 0x0400u, "Aͣ" },
		{ 0x0600u, "����"},
	};

	static	struct uMenu  const  menu_SHI[] =
	{
		{ 0x0501u, "��������" },
		{ 0x0200u, "C��" },
		{ 0x0208u, "D��" },
		{ 0x0400u, "Cͣ" },
		{ 0x0600u, "����"},
		{ 0x060Au, "����"}	//	ʱ�����վ������һ����������ѡ��
	};
	BOOL	done = FALSE;

	do {
		monitor();
		
		switch ( SamplerSelect )
		{
		default:
		case Q_TSP:	done = setup_TSP_AIR( menu_TSP );			break;
		case Q_R24:	done = setup_R24_SHI( menu_R24, "Bͣ" );	break;
		case Q_SHI:	done = setup_R24_SHI( menu_SHI, "Dͣ" );	
		break;
		case Q_AIR:	done = setup_TSP_AIR( menu_AIR );			break;
		}
	} while( ! done );
}

/********************************** ����˵�� ***********************************
*  �������������Ŀ��Ʋ�����ԭ����ÿ�β���ǰ��Ҫ�������õĲ���������������
*******************************************************************************/
static	BOOL	SampleStart( enum enumSamplerSelect SamplerSelect )
{
	static	struct uMenu  const  menu_Accurate_withZero[] =
	{
		{ 0x0103u, "��������" },
		{ 0x0400u, "��ʼʱ��" },
		{ 0x0602u, "����" }, 
		{ 0x0609u, "����" },
	};

	static	struct uMenu  const  menu_Delay_withZero[] =
	{
		{ 0x0103u, "��������" },
		{ 0x0400u, "��ʱʱ��" },
		{ 0x0602u, "����" }, 
		{ 0x0609u, "����" },
	};

	static	struct uMenu  const  menu_Accurate_noneZero[] =
	{
		{ 0x0102u, "��������" },
		{ 0x0400u, "��ʼʱ��" },
		{ 0x0609u, "����" },
	};

	static	struct uMenu  const  menu_Delay_noneZero[] =
	{
		{ 0x0102u, "��������" },
		{ 0x0400u, "��ʱʱ��" },
		{ 0x0609u, "����" },
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
			
			Lputs( 0x0200u, "��ǰʱ��=" );
			ShowTIME( 0x0209u, now_hour_min );	//	��ʾ��ǰʱ��
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
		case 3:	//	��������
			if ( changed )
			{
				SampleSetSave();
			}
			Sampler_Start ( SamplerSelect );

			return	FALSE;	//	������ʾ����״̬

		case enumSelectXCH:
			SamplerTypeSwitch();
			return	FALSE;
		}
	} while ( enumSelectESC != item );
	
	return	TRUE;	//	���أ���ʾ�ϼ��˵�
}

/********************************** ����˵�� ***********************************
*  ���˵��������ó���Ľӿ�
*******************************************************************************/
void	menu_SampleStart( void )
{
	BOOL	done = FALSE;
	
	do {
		monitor();
		done = SampleStart( SamplerSelect );
	} while ( ! done );
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
