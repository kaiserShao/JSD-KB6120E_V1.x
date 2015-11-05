/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: ConfigureEx.C
* �� �� ��: ����
* ��  ��  : KB-6120E ���ó��򣨳������ò��֣�
* ����޸�: 2014��3��1��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include "stdio.h" 
/********************************** ����˵�� ***********************************
*	�������� -> ������
*******************************************************************************/
CHAR  const * const Pump[]=
{
	"�۳��� ",
	"�վ�A",
	"�վ�B",
	"ʱ��C",
	"ʱ��D",
	"������",
	"������"
};

CHAR  const * const Type[]=
{
	"[K-100L]",
	"[K-1.��]",
	"[1L�װ�]",
	"[2L�װ�]",
	"[1-����]",
	"[2-˫��]",
	"[������]",
	"[������]",
	"[ȫ��װ]" 
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
							Lputs( 0x0000, "�ð�װ���  " ); 
						else		
							Lputs( 0x0000, "�ð�װ��� 1" ); 
            Lputs( dispchar[i%3],   Pump[PumpC[i]] ); 
						Lputs( dispchar2[i%3], Type[TypeC[i]]); 
					}	
				}	
				break;
			case opt_tsp_2:
				cls();
				for( i = 3; i < 6; i++)
				{
					Lputs( 0x0000, "�ð�װ��� 2" );
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
					Lputs( 0x0000, "�ð�װ��� 3" );
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
			case K_UP:		//	��ǰ��ʱ�������ļ�������
				--option;	
				if ( option < opt_min )
				{
					option = opt_maxx;
				}
				break;
			case K_DOWN:	//	���ʱ����µ��ļ�������
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

/********************************** ����˵�� ***********************************
*	��չ���� -> �����õ�ѹ������
*******************************************************************************/
//	static	void	menu_Configure_PrProtect( void )
//	{
//		MsgBox( "TODO: PrProtect", vbOKOnly );
//	}

/********************************** ����˵�� ***********************************
*	��չ���� -> ���ۼ�����ʱ��
*******************************************************************************/
static	void	menu_Clean_SumTime( void )
{
	BOOL	Done = FALSE;
	do{
		cls();	//	Lputs( 0x0000u, "����ʱ��(Сʱ)" );
		Lputs( 0x0000u, "�۳�:" );	ShowFP32( 0x0005u, PumpSumTimeLoad( PP_TSP ) / 60.0f, 0x0601u, "h" );
		Lputs( 0x0200u, "A:" ); 	ShowFP32( 0x0202u, PumpSumTimeLoad( PP_R24_A ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0208u, "B:" ); 	ShowFP32( 0x020Au, PumpSumTimeLoad( PP_R24_B ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0400u, "C:" ); 	ShowFP32( 0x0402u, PumpSumTimeLoad( PP_SHI_C ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0408u, "D:" ); 	ShowFP32( 0x040Au, PumpSumTimeLoad( PP_SHI_D ) / 60.0f, 0x0601u, NULL );
		Lputs( 0x0600u, "����:" ); 	ShowFP32( 0x0605u, PumpSumTimeLoad( PP_AIR ) / 60.0f, 0x0601u, NULL );

		switch ( getKey() )
		{
		case	K_OK	:						
			if ( ! releaseKey( K_OK, 100u ))
			{
				beep();
				if ( vbYes == MsgBox( "����ۼ�ʱ�� ?", vbYesNo | vbDefaultButton2 ))
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

/********************************** ����˵�� ***********************************
*  ϵͳ���� -> �ļ���
*******************************************************************************/
static	void	menu_Clean_FileNum( void )
{
	BOOL	Done = FALSE;
	do{
		cls();	//	Lputs( 0x0000u, "�ļ���" );
		Lputs( 0x0000u, "�۳�:" );	ShowI16U( 0x0009u, SampleSet[Q_TSP].FileNum, 0x0300u, NULL );
		Lputs( 0x0200u, "�վ�:" );	ShowI16U( 0x0209u, SampleSet[Q_R24].FileNum, 0x0300u, NULL );
		Lputs( 0x0400u, "ʱ��:" );	ShowI16U( 0x0409u, SampleSet[Q_SHI].FileNum, 0x0300u, NULL );
		Lputs( 0x0600u, "����:" );	ShowI16U( 0x0609u, SampleSet[Q_AIR].FileNum, 0x0300u, NULL );
		
		switch ( getKey() )
		{
		case	K_OK	:
			if ( ! releaseKey( K_OK, 100u ))
			{
				beep();
				if ( vbYes == MsgBox( "��������ļ� ?", vbYesNo | vbDefaultButton2 ) )
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
		{ 0x0201u, "�����ۼ�" },
		{ 0x0301u, "�ļ����" },
		{ 0x0601u, "���ۼ�" },

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
				switch( MsgBox("�����޸Ľ��?", vbYesNoCancel|vbDefaultButton3 ) )
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
		{ 0x0301u, "���� ��ʾ" },
		{ 0x0200u, "�Ҷ�" },
		{ 0x0400u, "����" },
		{ 0x0600u, "��ʱ" }
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
		case 0:	Lputs( 0x0605u, "[�ر�]" );	break;
		case 1:	Lputs( 0x0605u, "[15��]" );	break;
		case 2:	Lputs( 0x0605u, "[30��]" );	break;
		case 3:	Lputs( 0x0605u, "[60��]" );	break;
		default:
		case 4:	Lputs( 0x0605u, "[����]" );	break;
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
		{ 0x0301u, "װ������" },
		{ 0x0201u, "�����ͺ�" },
		{ 0x0401u, "��������" },
		{ 0x0601u, "��װ�����" },

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

/********************************** ����˵�� ***********************************
*	��չ���ã�ֻ�ܳ��Ҳ����Ĳ��֣�

���ҵ���
����汾
���ۼ�����ʱ��
�ã������ƣ��ͺ�ѡ��������ʾ����ѡ��ѡ�񹩵��Դ����
��ǰѹ����������ֵ���ã���ֹ�ö���
��ʾ��Ļ����
�ļ���š��ļ��洢��������
*******************************************************************************/

void	menu_ConfigureEx( void )
{
	static	struct  uMenu  const  menu[] =
	{
		{ 0x0302u, "��������" },
		{ 0x0202u,  "����"  }, { 0x0208u, "�����ۼ�" },
		{ 0x0401u, "��װ��" }, { 0x0408u, "��ʾ" },	//	{ 0x0408u, "ѹ������" },
		{ 0x0602u,  "����"  }, { 0x0608u, "����汾" },

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

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
