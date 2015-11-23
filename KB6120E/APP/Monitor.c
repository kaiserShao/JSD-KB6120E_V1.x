/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Monitor.C
* �� �� ��: KB-6120E
* ��  ��  : �����������ʾ���������еĲ���״̬
* ����޸�: 2013��12��17��
*********************************** �޶���¼ ***********************************
* ��  ��:
* �޶���:
*******************************************************************************/
#include "AppDEF.H"

/*
������������Ҫ��ʾ��״̬�ֳ����¼�����
0.��Ӧ����������
1.ʱ�����״̬������������������ʱ�䡢ʣ��ʱ�䡢��ǰ����״̬�ǲ���������ͣ���ǹ��ϵȵ�
2.����״̬�������¶ȡ�ѹ��������ѹ���������������꣩����������Ϣ��
3.ͳ����Ϣ�������ۼ�ʱ�䡢�ۻ�����������꣩��
4.������Ϣ���� �������¶ȼ�����״̬�������¶ȡ�����ѹ�� ��

ԭ���ϣ���س���ֻ��ʾ�������ơ�����ʾ�����ݣ���ȫ�ֱ������ݣ�����Ӧ���Ƴ����������¡�
*/





/********************************** ����˵�� ***********************************
*  ��ʾ���״̬
*******************************************************************************/
FP32	get_Bat_Voltage( void );
FP32	get_Bat_Current( void );
bool	get_Bat_Charging( void );


void	show_battery( uint16_t yx )
{
	static const char * BatteryIdentify[]={"���","���","���","���","���","���"};
	
	#define	BatVolStateFULL	5u
	static	uint8_t	BatVolState = BatVolStateFULL;

	uint8_t i;
	bool isCharging;

	FP32 inVoltage = get_Bat_Voltage();
	if( Configure.Battery_SW == TRUE )
	{
    if ( inVoltage < 1.0f )
		{
			return;	//	��ص�ѹ�쳣����Ϊ���ûװ������û�е�ء�
		}
		
		isCharging = get_Bat_Charging();

		if ( ! isCharging )
		{	//	����״̬ �� �ŵ�״̬����ʾ��طֵ�
			FP32	voltage = inVoltage * 0.2f; // 5 �ڵ��
			uint8_t NewState;

			if     ( voltage >= 3.20f )  { NewState = 5; }
			else if( voltage >= 3.10f )  { NewState = 4; }
			else if( voltage >= 3.00f )  { NewState = 3; }
			else if( voltage >= 2.95f )  { NewState = 2; }
			else if( voltage >= 2.85f )  { NewState = 1; }
			else                         { NewState = 0; } // Battery Empty.

			if( NewState < BatVolState )
			{
				BatVolState = NewState;
			}
			i = BatVolState;
		}
		else
		{	//	���״̬������˸����ʾ�����š�

			BatVolState = BatVolStateFULL;
			
			i = get_Now() % BatVolStateFULL + 1;
		}
		
			Lputs( yx, BatteryIdentify[i] );
		}
	
}
/********************************** ����˵�� ***********************************
*  ��ʾ��������������ѹ�������� �� ������ ״̬
*******************************************************************************/
void	show_env_state( void )
{		
	switch ( Configure.Mothed_Ba )
	{
	case enumUserInput:	Lputs( 0x0200u, "����ѹ=" );	break;
	case enumMeasureBa:	Lputs( 0x0200u, "����ѹ:" );	break;
	}
	ShowFP32( 0x0207u, get_Ba(), 0x0602u, "kPa" );

	//	������к�����
	switch ( Configure.HeaterType )
	{
	case enumHCBoxOnly :
		{
			FP32	OutValue = get_HCBoxOutput();
			FP32	RunTemp  = get_HCBoxTemp();
			Lputs( 0x0400u, "������:" );	ShowFP32( 0x0407u, RunTemp, 0x0501u, "��" );

			if      ( OutValue > 0.0f )
			{
				Lputs( 0x0600u, "����: " );	ShowFP32( 0x0606u, OutValue, 0x0601u, "% " );
			}
			else if ( OutValue < 0.0f )
			{
				Lputs( 0x0600u, "����: " );	ShowFP32( 0x0606u, OutValue, 0x0601u, "% " );
			}
			else
			{			
				Lputs( 0x0600u, "��  ��: [�ر�]    " );
			}
		}
		break;
	case enumHeaterOnly:
		{
			FP32	OutValue = get_HeaterOutput();
			FP32	RunTemp  = get_HeaterTemp();
			uint8_t state = Configure.Heater_SW;
			Lputs( 0x0400u, "������:" );	ShowFP32( 0x0407u, RunTemp, 0x0501u, "��" );
			if( state )
			{
				if      ( OutValue > 0.0f )
				{
					Lputs( 0x0600u, "��  ��: " );	ShowFP32( 0x0608u, OutValue, 0x0601u, "% " );
				}
				else
				{			
					Lputs( 0x0600u, "��  ��: " );	Lputs( 0x0608u," [�ر�]    " );
				}
			}
			else
			{
				Lputs( 0x0600u, "��  ��: " );	Lputs( 0x0608u," [ͣ��]    " );
			}		
		}
		break;
	}
}
static	FP32	fstdx[4][PP_Max];
static	FP32	fstd, flow;
void Sampler_TdMonitor( enum enumSamplerSelect SamplerSelect )
{	
  FP32	Te   = get_Te();
  FP32	Ba   = get_Ba();
  switch(  SamplerSelect )
  {	
    case Q_TSP:
      fstdx[0][PP_TSP] = fstdx[1][PP_TSP];
      fstdx[1][PP_TSP] = fstdx[2][PP_TSP];
      fstdx[2][PP_TSP] = fstdx[3][PP_TSP];
      fstdx[3][PP_TSP] = get_fstd( PP_TSP );
      break;
    case Q_SHI:
      fstdx[0][PP_SHI_C] = fstdx[1][PP_SHI_C];
      fstdx[1][PP_SHI_C] = fstdx[2][PP_SHI_C];
      fstdx[2][PP_SHI_C] = fstdx[3][PP_SHI_C];
      fstdx[3][PP_SHI_C] = get_fstd( PP_SHI_C );
      fstdx[0][PP_SHI_D] = fstdx[1][PP_SHI_D];
      fstdx[1][PP_SHI_D] = fstdx[2][PP_SHI_D];
      fstdx[2][PP_SHI_D] = fstdx[3][PP_SHI_D];
      fstdx[3][PP_SHI_D] = get_fstd( PP_SHI_D );
      break;
    case Q_R24:
      fstdx[0][PP_R24_A] = fstdx[1][PP_R24_A];
      fstdx[1][PP_R24_A] = fstdx[2][PP_R24_A];
      fstdx[2][PP_R24_A] = fstdx[3][PP_R24_A];
      fstdx[3][PP_R24_A] = get_fstd( PP_R24_A );
      fstdx[0][PP_R24_B] = fstdx[1][PP_R24_B];
      fstdx[1][PP_R24_B] = fstdx[2][PP_R24_B];
      fstdx[2][PP_R24_B] = fstdx[3][PP_R24_B];
      fstdx[3][PP_R24_B] = get_fstd( PP_R24_B );
      break;
    default:  
      break;
  }
 
}
/********************************** ����˵�� ***********************************
*  ��ʾָ����������ʱ��״̬
*******************************************************************************/
static	void	ShowTimeState ( enum enumSamplerSelect SamplerSelect, enum enumPumpSelect PumpSelect  )
{
	struct uSamplerQuery * p = & Q_Sampler[SamplerSelect];
	FP32	Te   = get_Te();
	FP32	Ba   = get_Ba();  
  Sampler_TdMonitor(  SamplerSelect ); 
	Lputs ( 0x0000u, "ʱ�� " );
	switch ( p->state )
	{
	default:
	case state_ERROR:		Lputs ( 0x0200u, "    !!����!!    " );	break;
	case state_FINISH:		Lputs ( 0x0200u, "    ��ɲ���    " );	break;
	case state_SAMPLE:		Lputs ( 0x0200u, "    ���ڲ���    " );	break;
	case state_SUSPEND:		Lputs ( 0x0200u, "    �ȴ�����    " );	break;
	case state_PAUSE:		Lputs ( 0x0200u, "    ��ͣ����    " );	break;
	}
	//TODO:	2·ͬ����ͬ�ص�ʱ�䲻һ��������
	Lputs ( 0x0400u, "��ǰ����:" );	ShowI16U ( 0x040Bu, p->loops, 0x0500u, NULL );
	Lputs ( 0x0600u, "ʣ��ʱ��:" );	ShowTIME ( 0x060Bu, p->timer );
		
}

/********************************** ����˵�� ***********************************
*  ��ʾָ�������õļ�ǰ����״̬
*******************************************************************************/
static	void	ShowPumpBefore( enum enumPumpSelect PumpSelect )
{  
  Sampler_TdMonitor(  SamplerSelect ); 
	Lputs ( 0x0000u, "�� ��" );
	{
		Lputs ( 0x0200u, "��  ��:" );	ShowFP32 ( 0x0207u, get_Tr( PumpSelect ), 0x0602u, "��" );
		Lputs ( 0x0400u, "ѹ  ��:" );	ShowFP32 ( 0x0407u, get_Pr( PumpSelect ), 0x0602u, "kPa" );
		Lputs ( 0x0600u, "����ѹ:" );	ShowFP32 ( 0x0607u, get_Ba(),             0x0602u, "kPa" );
	}
}

/********************************** ����˵�� ***********************************
*  ��ʾָ�������õ���������״̬
*******************************************************************************/
static	void	ShowPumpRunFlow( enum enumPumpSelect PumpSelect )
{
	Lputs ( 0x0000u, "�� ��" );
	Sampler_TdMonitor(  SamplerSelect ); 
	if ( ! Q_Pump[PumpSelect].xp_state )
	{
		Lputs( 0x0200u, "                " );
		Lputs( 0x0400u, "���ѹر�        " );
		Lputs( 0x0600u, "                " );
		return;
	}
	else 
	{
		
		FP32	OutValue = Pump_GetOutput( PumpSelect );
		switch ( PumpSelect )
		{
		case PP_TSP: // TSP_KB120F
			{
				FP32	Te   = get_Te();
				FP32	Ba   = get_Ba();
			  fstd = ( fstdx[0][PumpSelect] + fstdx[1][PumpSelect] + fstdx[2][PumpSelect] + fstdx[3][PumpSelect] ) / 4; 
				flow =  Calc_flow( fstd, Te, 0.0f, Ba, Q_TSP );
			}	
			Lputs ( 0x0200u, "����:" ); 	ShowFP32    ( 0x0205u, flow, 0x0701u, "L/m" );
			Lputs ( 0x0400u, "���:" );		ShowFP32    ( 0x0405u, fstd, 0x0701u, "L/m" );
			Lputs ( 0x0600u, "���:" );		ShowPercent ( 0x0607u, OutValue );
			break;
		case PP_R24_A:
		case PP_R24_B:
		case PP_SHI_C:
		case PP_SHI_D:
			{
				FP32	Te   = get_Te();
				FP32	Ba   = get_Ba();	 
			}
			fstd = ( fstdx[0][PumpSelect] + fstdx[1][PumpSelect] + fstdx[2][PumpSelect] + fstdx[3][PumpSelect] ) / 4; 
			Lputs ( 0x0400u, "                " );	
			Lputs ( 0x0300u, "���:" );		ShowFP32    ( 0x0305u, fstd, 0x0703u, "L/m" );
			Lputs ( 0x0600u, "���:" );		ShowPercent ( 0x0606u, OutValue );
			break;
		default:
		case PP_AIR:
			break;
		}
	}
}

/********************************** ����˵�� ***********************************
*  ��ʾָ�������õ���������״̬
*******************************************************************************/
static	void	ShowSumCubage( enum enumPumpSelect PumpSelect )
{
	struct	uPumpQuery * p = &Q_Pump[PumpSelect];
	Sampler_TdMonitor(  SamplerSelect ); 
	Lputs ( 0x0000u, "�� ��" );
	switch (PumpSelect )
	{
	case PP_TSP: // TSP_KB120F
		Lputs ( 0x0200u, "�������:" );	ShowFP32 ( 0x0209u, p->vd,       0x0600u, "L" );
		Lputs ( 0x0400u, "������:" );	ShowFP32 ( 0x0409u, p->vnd,      0x0600u, "L" );
		Lputs ( 0x0600u, "����ʱ��:" );	ShowTIME ( 0x060Bu, p->sum_time );
		break;
	case PP_R24_A:
	case PP_R24_B:
	case PP_SHI_C:
	case PP_SHI_D:
		Lputs ( 0x0300u, "�� ��:" );		ShowFP32 ( 0x0306u, p->vnd,  0x0802u, "L" );
		Lputs ( 0x0600u, "����ʱ��:" );	ShowTIME ( 0x060Bu, p->sum_time );
		break;
	case PP_AIR:
		{
		Lputs ( 0x0200u, "�ۻ�ʱ��:" );	ShowTIME ( 0x020Bu, p->sum_time );
		Lputs ( 0x0400u, "�� ����:" );		ShowFP32 ( 0x0408u, (uint32_t)( p->sum_time * Configure.AIRSetFlow[Q_PP1] ) * 0.1f ,  0x0602u, "L" );
		Lputs ( 0x0600u, "�� ����:" );		ShowFP32 ( 0x0608u, (uint32_t)( p->sum_time * Configure.AIRSetFlow[Q_PP2] ) * 0.1f ,  0x0602u, "L" );
		}
		break;
	}
}

/********************************** ����˵�� ***********************************
*  ��ʾ ������ �� ������ ����״̬
*******************************************************************************/
static	void	ShowHCBox( void )
{   
  Sampler_TdMonitor(  SamplerSelect ); 
	switch ( Configure.HeaterType )
	{
	default:
	case enumHeaterNone:	break;	//	MsgBox( "δ��װ������", vbOKOnly );	break;
	case enumHCBoxOnly:
		Lputs ( 0x0000u, "������");
		Lputs ( 0x0200u, "�¶�:" );		ShowFP32 ( 0x0205u, get_HCBoxTemp(),     0x0602u, "��" );
		Lputs ( 0x0400u, "���:" );		ShowFP32 ( 0x0405u, get_HCBoxOutput(),   0x0501u, "% " );
		Lputs ( 0x0600u, "����:" );		ShowI16U ( 0x0605u, get_HCBoxFanSpeed(), 0x0500u, "RPM" );
		break;
	case enumHeaterOnly:
		Lputs ( 0x0000u, "������");
		Lputs ( 0x0300u, "�¶�:" );		ShowFP32 ( 0x0305u, get_HeaterTemp(),     0x0602u, "��" );
		Lputs ( 0x0600u, "���:" );		ShowFP32 ( 0x0605u, get_HeaterOutput(),   0x0501u, "% " );
		break;
	case enumHCBoxHeater:
		set_HCBoxTemp( Configure.HCBox_SetTemp * 0.1f, Configure.HCBox_SetMode );
		set_HeaterTemp( Configure.Heater_SetTemp*0.1f);
		break;
	}
	
}
/********************************** ����˵�� ***********************************
*  ��ʾ �������ۼ�����
*******************************************************************************/
// static	void	ShowSumAir( void )
// {

// }
/*****************************************
 ********* ֹͣ��ͣ״̬ѡ��˵� **********
******************************************/
void Samplestate_Select( BOOL state )
{
	static	struct  uMenu  const  menu[] =
    {
        { 0x0103u, "���ò���״̬" },
        { 0x0300u, "��ͣ" },{ 0x0305u, "ֹͣ" },{ 0x030au, "ȡ��" }
		
    };
	static	struct  uMenu  const  menu2[] =
    {
        { 0x0103u, "���ò���״̬" },
        { 0x0300u, "�ָ�" },{ 0x0305u, "ֹͣ" },{ 0x030au, "ȡ��" }
		
    };
    uint8_t	item = 0u;
    BOOL	need_redraw = TRUE;
    do
    {   
        if ( need_redraw )
        {
			cls();
			if( state ==TRUE)
				Menu_Redraw( menu2 );
			else
				Menu_Redraw( menu );
			need_redraw = FALSE;
        }
		if( state == TRUE )
			item = Menu_Select2( menu2, item + 1u, TRUE );
		else
			item = Menu_Select2( menu, item + 1u, TRUE );
      switch( item )
      {
      case 1:
          Sampler_PauseKey( SamplerSelect );
          item = enumSelectESC;
          break;
      case 2:
          Sampler_Terminate( SamplerSelect );
          item = enumSelectESC;
          break;
      case 3:
          item = enumSelectESC;					
          break;
      default:
          break;
      }
	}while( enumSelectESC != item );
}
/********************************** ����˵�� ***********************************
*  ������������ͳһ�����������µİ���
*******************************************************************************/
void	disposeKey( const enum enumSamplerSelect SamplerSelect, uint8_t * pOption, const uint8_t opt_max, enum enumPumpSelect * pPumpSelect )
{
	enum {
		opt_exit,
		opt_min = 1u
	};
	static	uint16_t gray  = 500u;
	BOOL graychanged = FALSE;	
	uint8_t option  = * pOption;
	enum enumPumpSelect PumpSelect;

	if ( NULL != pPumpSelect )
	{
		PumpSelect = *pPumpSelect;
	}
	else
	{
		PumpSelect = PP_Max;
	}
	
	if ( hitKey ( 100u ) )
	{
		switch ( getKey() )
		{
		case K_UP:
			switch ( SamplerSelect )
			{
			case Q_R24:	PumpSelect = PP_R24_A;	break;
			case Q_SHI:	PumpSelect = PP_SHI_C;	break;
			default:	break;
			}
			break;
		case K_DOWN:
			switch ( SamplerSelect )
			{
			case Q_R24:	PumpSelect = PP_R24_B;	break;
			case Q_SHI:	PumpSelect = PP_SHI_D;	break;
			default:	break;
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

		case K_SHIFT:

			if( Sampler_isRunning( SamplerSelect ) && ( Configure.InstrumentType == type_KB2400HL ) )
			{
				switch ( SamplerSelect )
				{
				case Q_R24:
				if( PumpSelect == PP_R24_A )
					PumpSelect = PP_R24_B;
				else
					PumpSelect = PP_R24_A;
				break;
				case Q_SHI:
				if( PumpSelect == PP_SHI_C )
					PumpSelect = PP_SHI_D;
				else
					PumpSelect = PP_SHI_C;					
				break;
				}
			 
			} 
			else
			{
				SamplerTypeSwitch();
				option = opt_exit;
			}
				
			break;
		case K_ESC:
		   Samplestate_Select( StateRead(SamplerSelect) );
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

		cls();
		
		*pOption = option;
		if ( NULL != pPumpSelect )
		{
			*pPumpSelect = PumpSelect;
		}
	}
}
void	State_Finish( enum enumSamplerSelect SamplerSelect )
{
	cls();
	SamplerTypeShow( 0x0102u );

	switch( SamplerSelect )
	{
	case	Q_TSP: Lputs( 0x0004, " TSP����");	break;
	case	Q_R24: Lputs( 0x0004, "�վ�����");	break;
	case	Q_SHI: Lputs( 0x0004, "ʱ������");	break;
	case	Q_AIR: Lputs( 0x0004, "��������");	break;
	}
	Lputs( 0x0203,	"�������!");
	Lputs( 0x0401,  "��ѯ�������?");
	do
	{
		show_std_clock();
	}while( !hitKey( 50 ) );

	if( K_OK == getKey() )
		menu_SampleQuery();

}
void	monitor_TSP( void )
{
	enum
	{
	    opt_exit,
	    opt_qt_1, opt_qt_2, opt_qt_3, opt_qt_4,
	    opt_max, opt_min = 1u
	};
	uint8_t option = opt_min;
	uint16_t	iDelay = 1200u;	//	��ʱһ��ʱ�䷵��Ĭ��״̬

	const	enum	enumPumpSelect  PumpSelect = PP_TSP;
	
	while ( ( opt_exit != option ) && Sampler_isRunning( SamplerSelect ))
	{
		if ( 0u != iDelay )
		{
			if ( --iDelay == 0u )
			{
				cls();	
				option = opt_min;
			}
		}

		SamplerTypeShow( 0x000Au );

		switch ( option )
		{
		case opt_qt_1:	ShowTimeState( SamplerSelect, PumpSelect );	break;
		case opt_qt_2:	ShowPumpBefore( PumpSelect );	break;
		case opt_qt_3:	ShowPumpRunFlow( PumpSelect );	break;
		case opt_qt_4:	ShowSumCubage( PumpSelect );	break;
		default:	break;
		}
		
		disposeKey( SamplerSelect, &option, opt_max, NULL );
	}
	State_Finish( SamplerSelect );
}

static	void	monitor_R24 ( void )
{
	enum
	{
	    opt_exit,
	    opt_qt_1, opt_qt_2, opt_qt_3, opt_qt_4,
			opt_qt_5,
	    opt_max, opt_min = 1u
	};
	uint8_t option = opt_min;
	uint16_t	iDelay = 1200u;	//	��ʱһ��ʱ�䷵��Ĭ��״̬

	enum	enumPumpSelect  PumpSelect = PP_R24_A;
	
	while ( ( opt_exit != option ) && Sampler_isRunning( SamplerSelect ))
	{
		if ( 0u != iDelay )
		{
			if ( --iDelay == 0u )
			{
				cls();	
				option = opt_min;
			}
		}

		switch ( PumpSelect )
		{
		case PP_R24_A:		Lputs ( 0x0009u, "<�վ�A>" );	break;
		case PP_R24_B:		Lputs ( 0x0009u, "<�վ�B>" );	break;
		}

		switch ( option )
		{
		case opt_qt_1:	
			ShowTimeState( SamplerSelect, PumpSelect );
			show_battery( 0x0005u );	
			break;
		case opt_qt_2:	ShowPumpBefore( PumpSelect );	break;
		case opt_qt_3:	ShowPumpRunFlow( PumpSelect );	break;
		case opt_qt_4:	ShowSumCubage( PumpSelect );	break;
		case opt_qt_5:	ShowHCBox();					break;
		default:	break;
		}

		disposeKey( SamplerSelect, &option, opt_max, &PumpSelect );
	}
	State_Finish( SamplerSelect );
}

static	void	monitor_SHI ( void )
{
	enum
	{
	    opt_exit,
	    opt_qt_1, opt_qt_2, opt_qt_3, opt_qt_4,
	    opt_qt_5,
	    opt_max, opt_min = 1u
	};

	uint8_t	option = opt_min;
	uint16_t	iDelay = 1200u;	//	��ʱһ��ʱ�䷵��Ĭ��״̬

	enum	enumPumpSelect  PumpSelect = PP_SHI_C;

	while (( opt_exit != option ) && Sampler_isRunning( SamplerSelect ))
	{
		if ( 0u != iDelay )
		{
			if ( --iDelay == 0u )
			{
				cls();	option = opt_min;
			}
		}
    switch ( PumpSelect )
    {
    case PP_SHI_C:		Lputs ( 0x0009u, "<ʱ��C>" );	break;
    case PP_SHI_D:		Lputs ( 0x0009u, "<ʱ��D>" );	break;
    }
		switch ( option )
		{
		case opt_qt_1:	ShowTimeState( SamplerSelect, PumpSelect );
			show_battery( 0x0005u );
			break;
		case opt_qt_2:	ShowPumpBefore( PumpSelect );		break;
		case opt_qt_3:	ShowPumpRunFlow( PumpSelect );	break;
		case opt_qt_4:	ShowSumCubage( PumpSelect );		break;
		case opt_qt_5:	ShowHCBox();										break;
		default:	break;
		}

		disposeKey( SamplerSelect, &option, opt_max, &PumpSelect );
	}
	State_Finish( SamplerSelect );
}

static	void	monitor_AIR ( void )
{
	enum
	{
	    opt_exit,
	    opt_qt_1, opt_qt_2, opt_qt_3,
	    opt_max, opt_min = 1u
	};
	uint8_t	option = opt_min;
	uint16_t	iDelay = 1200u;	//	��ʱһ��ʱ�䷵��Ĭ��״̬

	const enum	enumPumpSelect  PumpSelect = PP_AIR;
	
	while ( ( opt_exit != option ) && Sampler_isRunning( SamplerSelect ))
	{
		if ( 0u != iDelay )
		{
			if ( --iDelay == 0u )
			{
				cls();	option = opt_min;
			}
		}
		SamplerTypeShow( 0x000Au );
		switch ( option )
		{
		case opt_qt_1:	ShowTimeState( SamplerSelect, PumpSelect );
			show_battery( 0x0005u );
			break;
		case opt_qt_2:	ShowSumCubage( PumpSelect );	break;
		case opt_qt_3:	Lputs(0x0000u,"״ ̬");show_env_state();	break;
		default:	break;
		}

		disposeKey( SamplerSelect, &option, opt_max, NULL );
	}
	State_Finish( SamplerSelect );
}

/********************************** ����˵�� ***********************************
*  ������������ʾ����״̬
*******************************************************************************/
void	monitor ( void )
{
	while ( Sampler_isRunning( SamplerSelect ))
	{
		cls();
		switch ( SamplerSelect )
		{
		default:	
		case Q_TSP:	monitor_TSP();	break;
		case Q_R24:	monitor_R24();	break;
		case Q_SHI:	monitor_SHI();	break;
		case Q_AIR:	monitor_AIR();	break;
		}
	}
}

/********************************** ����˵�� ***********************************
*  ���������ͺţ���ʾ���л���ǰ�Ĳ�����
*******************************************************************************/
enum  enumSamplerSelect  SamplerSelect; 	//	= Q_TSP;

void	SamplerTypeShow( uint16_t yx )
{
	if( Configure.InstrumentType != type_KB2400HL )
		switch ( SamplerSelect )
		{
		case Q_TSP: Lputs( yx, "<�۳�>" );	break;
		case Q_R24: Lputs( yx, "<�վ�>" );	break;
		case Q_SHI:	Lputs( yx, "<ʱ��>" );	break;
		case Q_AIR:	Lputs( yx, "<����>" );	break;
		default:		Lputs( yx, "<δ֪>" );	break;
		}
	else
		__nop();
}

void	SamplerTypeSwitch( void )
{
	BOOL	hasPumpTSP =  ( enumPumpNone != Configure.PumpType[PP_TSP] );
	BOOL	hasPumpR24 = (( enumPumpNone != Configure.PumpType[PP_R24_A] ) || ( enumPumpNone != Configure.PumpType[PP_R24_B] ));
	BOOL	hasPumpSHI = (( enumPumpNone != Configure.PumpType[PP_SHI_C] ) || ( enumPumpNone != Configure.PumpType[PP_SHI_D] ));
	BOOL	hasPumpAIR =  ( enumPumpNone != Configure.PumpType[PP_AIR] );

	switch ( SamplerSelect )
	{
	case Q_ALL:	if ( hasPumpTSP ){	SamplerSelect = Q_TSP;	break;	}	//lint	-fallthrough
	case Q_TSP:	if ( hasPumpR24 ){	SamplerSelect = Q_R24;	break;	}	//lint	-fallthrough
	case Q_R24:	if ( hasPumpSHI ){	SamplerSelect = Q_SHI;	break;	}	//lint	-fallthrough
	case Q_SHI:	if ( hasPumpAIR ){	SamplerSelect = Q_AIR;	break;	}	//lint	-fallthrough
	case Q_AIR:	if ( hasPumpTSP ){	SamplerSelect = Q_TSP;	break;	}	//lint	-fallthrough
	default:	break;
	}
}
void	DisplaySetContrast( uint8_t SetContrast );

/********************************** ����˵�� ***********************************
*  ������ʾ���������п����ڿ�������ʾ������½��룩
*******************************************************************************/
void	ModifyLCD( void )
{
	uint16_t gray  = Configure.DisplayGray;

	BOOL	changed = false;
	
	cls();
	Lputs( 0x0000u, "���� Һ�� �Ҷ�" );
	Lputs( 0x0600u, "�÷����������ѹ" );
	for(;;)
	{
		DisplaySetGrayVolt( gray * 0.01f );

		Lputs( 0x0300u, "�Ҷ�" );	ShowI16U( 0x0305u, gray,  0x0502u, " V " );


		switch( getKey())
		{
		case K_UP:	
			if ( gray < 2000u )
			{
				++gray;
			}
			changed = true;
			break;
		case K_DOWN:
			if ( gray >  200u )
			{
				--gray;
			}
			changed = true;
			break;

		case K_RIGHT:
			if ( gray < ( 2000u - 50u ))
			{ 
				gray += 50u;
			}
			changed = true;
			break;
		case K_LEFT:	
			if ( gray > ( 200 + 20u ))
			{
				gray -= 20u;
			}
			changed = true;
			break;

		case K_ESC:
		case K_OK:
			if ( changed )
			{
				Configure.DisplayGray  = gray;
				ConfigureSave();
			}
			return;
		
		}
	}
}

void	menu_show_env_state( void )
{
	BOOL Done = FALSE;
	uint16_t gray  = Configure.DisplayGray;
	BOOL graychanged = FALSE;	
	cls();
	do
	{	
		Lputs( 0x0005u, "״  ̬" );
        
		while( !hitKey( 10 ) )
			show_env_state();

		if ( ! releaseKey( K_OK, 300 ))
		{
			cls();
			getKey();
			beep();
			delay( 100u );
			beep();
			ModifyLCD();
			Done = TRUE; 
		}
		else
		if ( ! releaseKey( K_SHIFT, 300 ))
		{
			cls();
			getKey();
			beep();
			delay( 100u );
			beep();
			Lputs( 0x0201u, "������������:" );
			ConfigureLoad();
			if( EditI32U( 0x0505u, &Configure.ExNum, 0x0700u ))
				if( vbYes == MsgBox("�Ƿ񱣴���?",vbYesNo) )
					ConfigureSave();
				else
					ConfigureLoad();				
			Done = TRUE; 
		}
		else
		{
			switch( getKey() )
			{
				case K_OK:
				case K_ESC:
				case K_SHIFT:
					Done = TRUE;
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
			
		
	}while( Done == FALSE);
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
