/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: ConfigureEx.C
* �� �� ��: ����(2014��4��8��)
* ��  ��  : KB-6120E ���塢��ʾ�汾��Ϣ
* ����޸�: 2014��4��8��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"

/********************************** ����˵�� ***********************************
*  ���������ͺ���ʾ�汾��Ϣ
*  ����������ԱҪ�󣬿��ܻ�ĳ������������ƣ����߸ɴ಻��ʾ��
*******************************************************************************/
CHAR  const * const ExNameIdent1[] =
{
  " �� �� �� �� �� ",
  " �� �� �� �� �� ",
//  ...  
};

CHAR  const * const ExNameIdent2[] =
{
  " �� �� �� �� �� ",
  "���ӿƼ����޹�˾",
//  ...  
};

CHAR  const * const EditionNum[] =
{
  "KB6120E V1.04",	//	�ڲ��汾
 __DATE__" V1.00",	//	��ʾ�汾
};

static	void	ShowEdition_NoName( void )
{
	cls();
  Lputs( 0x0000u, szNameIdent[Configure.InstrumentName] );
	Lputs( 0x0300u, EditionNum[1] );
	Lputs( 0x0600u, "���:" );	ShowFP32( 0x0607u, Configure.ExNum, 0x0700u, NULL );
}

static	void	ShowEdition_HasName( void )
{
  cls();
	Lputs( 0x0000u, ExNameIdent1[Configure.ExName] );
	Lputs( 0x0200u, ExNameIdent2[Configure.ExName] );
	Lputs( 0x0400u, EditionNum[1] );
	Lputs( 0x0600u, "���:" );	ShowFP32( 0x0607u, Configure.ExNum, 0x0700u, NULL );
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
/********************************** ����˵�� ***********************************
*  ��ʾ����汾 ���������ͺ���ʾ�汾��Ϣ
*******************************************************************************/
void	ShowEdition_Inner( void )
{	
	cls();
	Lputs( 0x0000u, szNameIdent[Configure.InstrumentName] );
	Lputs( 0x0200u, __DATE__ );
	Lputs( 0x0400u, __TIME__ );
 	Lputs( 0x0600u, EditionNum[0] );	
	getKey();
}



/********************************** ���ݶ��� ***********************************
*  ���õ����������б�
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
	" �����ۺϲ����� ",
	" �ۺϴ��������� ",
	"����������������",
	"���ܴ�����������",
	" ���º��������� ",
	" ���ܺ��������� ",
};
// #define	T_KB6120A
 #define	T_KB6120AD
// #define	T_KB6120AD2
//#define	T_KB2400HL
static	void	ConfigureLoad_KB6120A( void )
{
	#ifdef	T_KB6120A
		Configure.InstrumentType = type_KB6120A;

		Configure.PumpType[PP_TSP  ] = enumOrifice_1;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	�۳� �������� 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumPumpNone;	Configure.SetFlow[PP_R24_A]  =  200u;	//	�վ�1�������� 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumPumpNone;	Configure.SetFlow[PP_R24_B]  =  200u;	//	�վ�2�������� 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumPumpNone;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	ʱ��1�������� 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumPumpNone;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	ʱ��2�������� 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumOrifice_1;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	���� ���� 0.5 L/m
		Configure.AIRSetFlow[Q_PP1] = 5;
		Configure.AIRSetFlow[Q_PP2] = 5;
		Configure.HeaterType = enumHeaterOnly;	//	ֻ�м�����
		Configure.Heater_SetTemp = 300u;		//	�����������¶� 30.0 ��
		Configure.Heater_SW = TRUE;          // ����������
		
		Configure.HCBox_SetMode = MD_Shut;		//	���������ģʽ [�ر�]
		Configure.HCBox_SetTemp = 240u;			//	����������¶� 24.0 ��

		Configure.SetTstd = enum_293K;			//	��������Ķ����¶� 
		
		Configure.Mothed_Delay = enumByDelay;	//	������ʼʱ�䷽ʽ
		Configure.Mothed_Sample = enumBySet;	//	����ʱ����Ʒ�ʽ

		Configure.Mothed_Ba = enumMeasureBa;	//	����ѹ����ȡ��ʽ
		Configure.set_Ba    = 10133u;			//	�����û�������ѹ

		Configure.DisplayGray  = 500u;	//	��ʾ�Ҷ�����
		Configure.DisplayLight = 50u;	//	��ʾ��������
		Configure.TimeoutLight = 2u;	//	������ʱʱ��
		
		Configure.slope_Bat_Voltage = 1000;
		Configure.slope_Bat_Current = 1000;
		Configure.threshold_Current = 50;
		
		Configure.Battery_SW = FALSE;    //����Ƿ����
		Configure.ExName = Name_JSD;
		Configure.Password = 633817;
	
	#endif
}

static	void	ConfigureLoad_KB2400HL( void )
{
	#ifdef	T_KB2400HL
		Configure.InstrumentType = type_KB2400HL;

		Configure.PumpType[PP_TSP  ] = enumPumpNone;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	�۳�  �������� 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumPumpNone;	Configure.SetFlow[PP_R24_A]  =  200u;	//	�վ�1 �������� 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumPumpNone;	Configure.SetFlow[PP_R24_B]  =  200u;	//	�վ�2 �������� 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumOrifice_1;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	ʱ��1 �������� 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumOrifice_1;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	ʱ��2 �������� 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumPumpNone;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	���� ���� 0.5 L/m
		
		Configure.AIRSetFlow[Q_PP1] = 5;
		Configure.AIRSetFlow[Q_PP2] = 5;
		
		Configure.HeaterType = enumHeaterOnly;	//	ֻ�м�����
		Configure.Heater_SetTemp = 300u;		//	�����������¶� 30.0 ��
		Configure.Heater_SW = TRUE;         //����������	
		
		Configure.HCBox_SetMode = MD_Shut;		//	���������ģʽ [�ر�]
		Configure.HCBox_SetTemp = 240u;			//	����������¶� 24.0 ��
				 
		Configure.SetTstd = enum_293K;			//	��������Ķ����¶� 
		
		Configure.Mothed_Delay = enumByDelay;	//	������ʼʱ�䷽ʽ
		Configure.Mothed_Sample = enumBySet;	//	����ʱ����Ʒ�ʽ

		Configure.Mothed_Ba = enumMeasureBa;	//	����ѹ����ȡ��ʽ
		Configure.set_Ba    = 10133u;			//	�����û�������ѹ

		Configure.DisplayGray  = 500u;	//	��ʾ�Ҷ�����
		Configure.DisplayLight = 50u;	//	��ʾ��������
		Configure.TimeoutLight = 2u;	//	������ʱʱ��
		
		Configure.slope_Bat_Voltage = 1000;
		Configure.slope_Bat_Current = 1000;
		Configure.threshold_Current = 50;
		
		Configure.shouldCalcPbv = 0;		//	����ˮ��ѹ�Ƿ������㡣
		
		Configure.Battery_SW = TRUE;
		Configure.ExName = Name_JSD;
		Configure.Password = 633817;
		
	#endif
}

static	void	ConfigureLoad_KB6120AD( void )
{
	#ifdef	T_KB6120AD
		Configure.InstrumentType = type_KB6120AD;

		Configure.PumpType[PP_TSP  ] = enumOrifice_1;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	�۳�  �������� 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumPumpNone;	Configure.SetFlow[PP_R24_A]  =  200u;	//	�վ�1 �������� 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumPumpNone;	Configure.SetFlow[PP_R24_B]  =  200u;	//	�վ�2 �������� 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumOrifice_1;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	ʱ��1 �������� 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumOrifice_1;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	ʱ��2 �������� 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumPumpNone;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	���� ���� 0.5 L/m
		Configure.AIRSetFlow[Q_PP1] = 5;
		Configure.AIRSetFlow[Q_PP2] = 5;
		
		Configure.HeaterType = enumHeaterOnly;	//	ֻ�м�����
		Configure.Heater_SetTemp = 300u;		//	�����������¶� 30.0 ��
		Configure.Heater_SW = TRUE;          // ����������
		
		Configure.HCBox_SetMode = MD_Shut;		//	���������ģʽ [�ر�]
		Configure.HCBox_SetTemp = 240u;			//	����������¶� 24.0 ��
			
		Configure.SetTstd = enum_293K;			//	��������Ķ����¶� 
		
		Configure.Mothed_Delay  = enumByDelay;	//	������ʼʱ�䷽ʽ
		Configure.Mothed_Sample = enumBySet;	//	����ʱ����Ʒ�ʽ
			
		Configure.Mothed_Ba = enumMeasureBa;	//	����ѹ����ȡ��ʽ
		Configure.set_Ba    = 10133u;			//	�����û�������ѹ
			
		Configure.DisplayGray  = 500u;	//	��ʾ�Ҷ�����
		Configure.DisplayLight = 55u;	//	��ʾ��������
		Configure.TimeoutLight = 2u;	//	������ʱʱ��
		
		Configure.slope_Bat_Voltage = 1000;
		Configure.slope_Bat_Current = 1000;
		Configure.threshold_Current = 50;	
		
		Configure.shouldCalcPbv = 0;		//	����ˮ��ѹ�Ƿ������㡣
		
		Configure.Battery_SW = FALSE;
		Configure.ExName = Name_JSD;
		Configure.Password = 633817;
	#endif
}

static	void	ConfigureLoad_KB6120AD2( void )
{
	#ifdef	T_KB6120AD2
		Configure.InstrumentType = type_KB6120AD2;

		Configure.PumpType[PP_TSP  ] = enumOrifice_1;	Configure.SetFlow[PP_TSP  ]  = 1000u;	//	�۳�  �������� 100.0 L/m
		Configure.PumpType[PP_R24_A] = enumOrifice_1;	Configure.SetFlow[PP_R24_A]  =  200u;	//	�վ�1 �������� 0.200 L/m
		Configure.PumpType[PP_R24_B] = enumOrifice_1;	Configure.SetFlow[PP_R24_B]  =  200u;	//	�վ�2 �������� 0.200 L/m
		Configure.PumpType[PP_SHI_C] = enumPumpNone;	Configure.SetFlow[PP_SHI_C]  =    5u;	//	ʱ��1 �������� 0.5 L/m
		Configure.PumpType[PP_SHI_D] = enumPumpNone;	Configure.SetFlow[PP_SHI_D]  =    5u;	//	ʱ��2 �������� 0.5 L/m
		Configure.PumpType[PP_AIR  ] = enumPumpNone;	Configure.SetFlow[PP_AIR  ]  =  500u;	//	���� ���� 0.5 L/m
		Configure.AIRSetFlow[Q_PP1] = 5;
		Configure.AIRSetFlow[Q_PP2] = 5;
		Configure.HeaterType = enumHeaterOnly;	//	ֻ�м�����
		Configure.Heater_SetTemp = 300u;		//	�����������¶� 30.0 ��
		Configure.Heater_SW = TRUE;          // ����������

		Configure.HCBox_SetMode = MD_Shut;		//	���������ģʽ [�ر�]
		Configure.HCBox_SetTemp = 240u;			//	����������¶� 24.0 ��

		Configure.SetTstd = enum_293K;			//	��������Ķ����¶� 
		
		Configure.Mothed_Delay = enumByDelay;	//	������ʼʱ�䷽ʽ
		Configure.Mothed_Sample = enumBySet;	//	����ʱ����Ʒ�ʽ

		Configure.Mothed_Ba = enumMeasureBa;	//	����ѹ����ȡ��ʽ
		Configure.set_Ba    = 10133u;			//	�����û�������ѹ

		Configure.DisplayGray  = 500u;	//	��ʾ�Ҷ�����
		Configure.DisplayLight = 55u;	//	��ʾ��������
		Configure.TimeoutLight = 2u;	//	������ʱʱ��
		
		Configure.slope_Bat_Voltage = 1000;
		Configure.slope_Bat_Current = 1000;
		Configure.threshold_Current = 50;
		
		Configure.shouldCalcPbv = 0;		//	����ˮ��ѹ�Ƿ������㡣
		
		Configure.Battery_SW = FALSE;
		Configure.ExName = Name_JSD;
		Configure.Password = 633817;
		
	#endif
}



/********************************** ����˵�� ***********************************
*	��չ���� -> ѡ��á���������������
*******************************************************************************/
void	Configure_InstrumentName( void )
{
	static  struct  uMenu  const  menu[] = 
	{
		{ 0x0101u, "������������" },
		{ 0x0200u, "����" },
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
			if ( Configure.InstrumentName >= type_Max )
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
		if( vbYes == MsgBox( "��������?",vbYesNo ) )
		{
			ConfigureLoadDefault();
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
		{ 0x0101u, "���������ͺ�" },
		{ 0x0400u, "�ͺ�" },
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
		if( vbYes == MsgBox( "��������?",vbYesNo ) )
		{
			ConfigureLoadDefault( );
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
	ConfigureLoad_KB6120A();

	ConfigureLoad_KB6120AD();

	ConfigureLoad_KB6120AD2();

	ConfigureLoad_KB2400HL();

}
/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
