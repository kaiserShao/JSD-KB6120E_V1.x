/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Configure.C
* �� �� ��: ����
* ��  ��  : KB-6120E ���ó���(�����û����õĲ���)
* ����޸�: 2014��8��25��
*********************************** �޶���¼ ***********************************
* ��  ��:
* �޶���:
*******************************************************************************/
#include "AppDEF.H"

/********************************** ���ݶ��� ***********************************
*  ϵͳ����
*******************************************************************************/
struct	uConfigure	Configure;

/********************************** ����˵�� ***********************************
*  ����ʱ�䡢����
*******************************************************************************/
static	void	menu_SetupClock( void )
{
    static  struct  uMenu  const  menu[] =
    {
        { 0x0201u, "����ʱ��" },
        { 0x0300u, "����" },
        { 0x0500u, "ʱ��" },
    };
    enum
    {
        opt_exit, opt_date, opt_time,
        opt_max, opt_min = 1
    };
    uint8_t	option = opt_min;
		uint16_t gray  = Configure.DisplayGray;
		BOOL graychanged = FALSE;	
    uClock standard;

    cls();
    Menu_Redraw( menu );
    do
    {
        Menu_Item_Mask( menu, option );
        do
        {
            RTC_Load( &standard );
            ShowClockDate(0x0305u, &standard );
            ShowClockTime(0x0505u, &standard );
        }
        while ( ! hitKey( 25u ));
        Menu_Item_Mask( menu, option );

        switch( getKey() )
        {
        case K_RIGHT:
            break;

        case K_LEFT:
            break;

        case K_DOWN:
            ++option;
            if ( option >= opt_max )
            {
                option = opt_min;
            }
            break;

        case K_UP:
            if ( option <= opt_min )
            {
                option = opt_max;
            }
            --option;
            break;

        case K_ESC:
            option = opt_exit;
            break;

        case K_OK:
            switch( option )
            {
            case opt_date:
                if ( EditClockDate(0x0305u, &standard ))
                {
                    RTC_Save( &standard );
                }
                break;
            case opt_time:
                if ( EditClockTime(0x0505u, &standard ))
                {
                    RTC_Save( &standard );
                }
                break;
            default:
                break;
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
    }
    while( opt_exit != option );
}

/********************************** ����˵�� ***********************************
*	ϵͳ���� -> ����ѹ
*******************************************************************************/
static	void	menu_Configure_Ba( void )
{
    static const struct uMenu menu[] =
    {
				{ 0x0301u, "���ô���ѹ" },
				{ 0x0200u, "��ʽ" },
        { 0x0400u, "Ԥ�����ѹ" },
        { 0x0600u, "����" },
        
    };
		 static const struct uMenu menu1[] =
    {
				{ 0x0101u, "���ô���ѹ" },
				{ 0x0200u, "��ʽ" },            
    };
    uint8_t item = 1u;
    BOOL	need_redraw = TRUE;

    uint8_t 	Mothed_Ba = Configure.Mothed_Ba;
    uint16_t	set_Ba    = Configure.set_Ba;
    BOOL	changed = FALSE;

    do
    {
        if ( need_redraw )
        {
            cls();
					if( Mothed_Ba == enumUserInput )
            Menu_Redraw( menu );
					else
						Menu_Redraw( menu1 );
            need_redraw = FALSE;
        }
				if( Mothed_Ba == enumUserInput )
					ShowI16U( 0x0605u, set_Ba, 0x0602u, "kPa" );

        switch ( Mothed_Ba )
        {
        default:
        case enumUserInput:
					Lputs( 0x0205u, "[Ԥ������]" );
					break;
        case enumMeasureBa:
					Lputs( 0x0205u, "[ʵʱ����]" );
					break;
        }
				if( Mothed_Ba == enumUserInput )
					item = Menu_Select( menu, item );
				else
					item = Menu_Select( menu1, item );
        switch ( item )
        {
        case 0:
            if ( changed )
            {
                need_redraw = TRUE;
                switch( MsgBox( "�����޸Ľ�� ?", vbYesNoCancel + vbDefaultButton3 ))
                {
                case vbYes:
                    Configure.Mothed_Ba = Mothed_Ba;
                    Configure.set_Ba    = set_Ba;
                    ConfigureSave();
                    break;
                case vbNo:
                    break;
                case vbCancel:
                    item = 1u;
                    break;
                default:
                    break;
                }
            }
            break;
        case 1:
            switch ( Mothed_Ba )
            {
            default:
            case enumUserInput:
                Mothed_Ba = enumMeasureBa;
                break;
            case enumMeasureBa:
                Mothed_Ba = enumUserInput;
                break;
            }
            changed = TRUE;
						need_redraw = TRUE;	 
            break;

        case 2:
					if( Mothed_Ba == enumUserInput )
          {
						cls();
            Lputs( 0x0000u, "��������ѹ" );

            do
            {
                FP32 Ba = get_Ba();
                set_Ba = Ba * 100.0f + 0.5f;
                if ( set_Ba > 12000u )
                {
                    set_Ba = 12000u;
                }
                if ( set_Ba <  3000u )
                {
                    set_Ba =  3000u;
                }
                ShowI16U( 0x0302u, set_Ba, 0x0602u, "kPa" );
            }
            while ( ! hitKey( 100u ));
            ( void ) getKey();
            changed = TRUE;
            
					}  
					need_redraw = TRUE;	 
          break;

        case 3:
					if( Mothed_Ba == enumUserInput )
            if ( EditI16U( 0x0605u, &set_Ba, 0x0602u ))
            {
                changed = TRUE;
            }
            if ( set_Ba > 12000u )
            {
                set_Ba = 12000u;
            }
            if ( set_Ba <  3000u )
            {
                set_Ba =  3000u;
            }
          break;

        default:
					break;
        }
    }
    while ( 0u != item );
}

/********************************** ����˵�� ***********************************
*  ϵͳ���� -> ������
*******************************************************************************/
static	void	Configure_HCBox( void )
{
    static  CHAR  const  MDS[][6] =
    {
        { "S�ر�" }, { "H����" }, { "C����" } , { "A�Զ�" }
    };
    static	struct	uMenu	const	menu[] =
    {
        {	0x0201u, "���ú�����"	},
        {	0x0300u, "���Ʒ�ʽ"	},
        {	0x0500u, "�����¶�"	},
    };
    uint8_t 	item = 1u;

    uint8_t		SetMode = Configure.HCBox_SetMode;
    uint16_t	SetTemp = Configure.HCBox_SetTemp;
    BOOL		changed = FALSE;
    BOOL		need_redraw = TRUE;

    do
    {
        if ( need_redraw )
        {
            cls();
            Menu_Redraw( menu );
            need_redraw = FALSE;
        }

        Lputs( 0x0309U, MDS[SetMode] );
        ShowI16U( 0x0509U, SetTemp, 0x0501U, "��" );

        item = Menu_Select( menu, item );
        switch( item )
        {
        case 1u:	/*	EditBoxMode	*/
            SetMode = ( SetMode + 1u) % 4u;
            changed = TRUE;
            break;

        case 2u:
            /* ע��: ��Ϊ������������¶ȷ�Χ������, ���Բſ���ʹ���޷������༭������б༭ */
            if ( EditI16U( 0x0509U, &SetTemp, 0x0501U ))
            {
                if ( SetTemp <  150u )
                {
                    SetTemp = 150u;
                    beep();
                }
                if ( SetTemp >  300u )
                {
                    SetTemp = 300u;
                    beep();
                }
                changed = TRUE;
            }
            break;

        case 0u:
            if ( changed )
            {
                need_redraw = TRUE;
                switch ( MsgBox( "������������ ?", vbYesNoCancel + vbDefaultButton3 ))
                {
                case vbYes:
                    Configure.HCBox_SetMode = SetMode;
                    Configure.HCBox_SetTemp = SetTemp;
                    ConfigureSave();
                    set_HCBoxTemp( Configure.HCBox_SetTemp * 0.1f, Configure.HCBox_SetMode );
                    break;
                case vbNo:
                    ConfigureLoad();
                    break;
                case vbCancel:
                    item = 1u;
                    break;
                default:
                    break;
                }
            }
            break;

        default:
            break;
        }
    }
    while ( enumSelectESC != item );
}
static	void	Configure_Heater( void )
{
    static	struct	uMenu	const	menu[] =
    {
        {	0x0201u, "���ü�����"	},
        {	0x0300u, "��  ��"	},
				{	0x0600u, "�����¶�"	},
    };
		static	struct	uMenu	const	menu1[] =
    {
        {	0x0101u, "���ü�����"	},
        {	0x0300u, "��  ��"	},
    };
static	CHAR  const * const HeaterSW[] = {"[ͣ��]", "[����]"};
			
    uint8_t 	item = 1u;
		uint8_t 		state  = Configure.Heater_SW;          //	����������
    uint16_t	SetTemp = Configure.Heater_SetTemp;			//	�����������¶�
    BOOL		changed = FALSE;
    BOOL		need_redraw = TRUE;
   
    do {
        if ( need_redraw )
        {
            cls();
           
					if( state == FALSE )
						Menu_Redraw( menu1 );
					else
						Menu_Redraw( menu );
					
            need_redraw = FALSE;
        }
				Lputs(0x0309,HeaterSW[state]);
				if( state == TRUE )
					ShowI16U( 0x0609U, SetTemp, 0x0501U, "��" );
			
				if( state == FALSE )
					item = Menu_Select( menu1, item );
				else
					item = Menu_Select( menu, item );
        switch( item )
        {
         case 1u:	 
							need_redraw = TRUE;
							state = ! state; 
							changed = TRUE;
					break;
				 case 2u:
            /* ע��: ��Ϊ������������¶ȷ�Χ������, ���Բſ���ʹ���޷������༭������б༭ */
				 if( state == TRUE )
            if ( EditI16U( 0x0609U, &SetTemp, 0x0501U ))
            {
							if ( SetTemp <  150u )
							{
									SetTemp = 150u;
									beep();
							}
							if ( SetTemp >  350u )
							{
									SetTemp = 350u;
									beep();
							}
							changed = TRUE;
            }
						need_redraw = TRUE;
            break;
        case 0u:
            if ( changed )
            {
                need_redraw = TRUE;
                switch ( MsgBox( "������������ ?", vbYesNoCancel + vbDefaultButton3 ))
                {
                case vbYes:
                    Configure.Heater_SetTemp = SetTemp;
										Configure.Heater_SW = state;
                    ConfigureSave();
                    set_HeaterTemp( Configure.Heater_SetTemp * 0.1f );
                    break;
                case vbNo:
                    ConfigureLoad();
                    break;
                case vbCancel:
                    item = 1u;
                    break;
                default:
                    break;
                }
            }
            break;

        default:
            break;
        }
    }
    while ( enumSelectESC != item );
}

static	void	menu_ConfigureHCBox( void )
{
    switch ( Configure.HeaterType )
    {
    default:
    case enumHeaterNone:
        MsgBox( "δ��װ������", vbOKOnly );
        break;
    case enumHCBoxOnly:
        Configure_HCBox();
        break;
    case enumHeaterOnly:
        Configure_Heater();
        break;
    case enumHCBoxHeater:
        MsgBox( "Ӳ������֧��", vbOKOnly );
        break;
    }
}

/********************************** ����˵�� ***********************************
*  ϵͳ���� -> ������������ʱ��ʽ
*******************************************************************************/
static	void	menu_SelectDelayMode( void )
{
    static	struct  uMenu  const   menu[] =
    {
        { 0x0201u, "ѡ�������ʼ��ʽ" },
        { 0x0303u, "[��ʱ����]" },
        { 0x0503u, "[��ʱ����]" },
    };

    uint8_t	item;

    cls();
    Menu_Redraw( menu );
    switch ( Configure.Mothed_Delay )
    {
    default:
        Lputs( 0x0300u, " >>" );
        item = 1u;
        break;
    case enumByAccurate:
        Lputs( 0x0300u, " ->" );
        item = 1u;
        break;
    case enumByDelay:
        Lputs( 0x0500u, " ->" );
        item = 2u;
        break;
    }

    item = Menu_Select( menu, item );

    switch( item )
    {
    case 1:
        if ( Configure.Mothed_Delay != enumByAccurate )
        {
            Configure.Mothed_Delay = enumByAccurate;
            ConfigureSave();
        }
        break;
    case 2:
        if ( Configure.Mothed_Delay != enumByDelay )
        {
            SampleSet[Q_TSP].delay1 = 1u;
            SampleSetSave();
            Configure.Mothed_Delay = enumByDelay;
            ConfigureSave();
        }
        break;
    default:
        break;
    }
}

/********************************** ����˵�� ***********************************
*  ϵͳ���� -> ������ʱ��ʽ
*******************************************************************************/
static	void	menu_SelectTimeMode( void )
{
    static	struct uMenu  const	menu[] =
    {
        { 0x0201u, "ѡ�������ʱ��ʽ" },
        { 0x0303u, "[ͣ��۳�]" },
        { 0x0503u, "[ͣ�粹��]" },
    };

    uint8_t item;

    cls();
    Menu_Redraw( menu );
    switch ( Configure.Mothed_Sample )
    {
    default:
        Lputs( 0x0300u, " >>" );
        item = 1u;
        break;
    case enumBySet:
        Lputs( 0x0300u, " ->" );
        item = 1u;
        break;
    case enumBySum:
        Lputs( 0x0500u, " ->" );
        item = 2u;
        break;
    }

    item = Menu_Select( menu, item );

    switch ( item )
    {
    case 1:
        Configure.Mothed_Sample = enumBySet;
        ConfigureSave();
        break;
    case 2:
        Configure.Mothed_Sample = enumBySum;
        ConfigureSave();
        break;
    default:
        break;
    }
}

static	void	menu_ConfigureTime( void )
{
    static	struct uMenu  const	menu[] =
    {
        { 0x0201u, "ѡ��ʱ����Ʒ�ʽ" },
        { 0x0301u, "������ʱ��ʽ" },
        { 0x0501u, "������ʱ��ʽ" },
    };

    uint8_t item = 1;

    do
    {
        cls();
        Menu_Redraw( menu );
        item = Menu_Select( menu, item );
        switch ( item )
        {
        case 1:
            menu_SelectDelayMode();
            break;
        case 2:
            menu_SelectTimeMode();
            break;
        default:
            break;
        }
    }
    while ( enumSelectESC != item );
}

/********************************** ����˵�� ***********************************
*  ϵͳ���� -> ��������
* �û�����ʹ�ù����в���Ҫ�޸ģ�ԭ�����޸��������������Ҫ���±궨��
* ʱ�����������ڲ���ǰ���û����ã����ڴ˴����ã���Ӧ����Ҫ�ֶα궨��
* �������м��ֿ��ܵ���ϣ�
	�۳� 1.05m3��KB-1000��
	�۳� 100L��KB-120F��KB6120A��KB6120B��
	�۳� 100L + 0.2L ��·��KB-6120C��
	�վ� 0.2L ��·�����º���ϵ�У�

*******************************************************************************/
static	void	Configure_Flow_KB1000( void )
{
    MsgBox( "TODO KB-1000", vbOKOnly );
}

static	void	Configure_Flow_KB2400( void )
{
    static	struct  uMenu  const  menu[] =
    {
        { 0x0201u, "���ò�������" },
        { 0x0300u, "�վ�A" },
        { 0x0500u, "�վ�B" },
    };
    uint8_t	item = 0u;
    BOOL	changed = FALSE;
    BOOL	need_redraw = TRUE;

    do
    {
        if ( need_redraw )
        {
            cls();
            Menu_Redraw( menu );
            need_redraw = FALSE;
        }

        ShowI16U( 0x0306u, Configure.SetFlow[PP_R24_A], 0x0503u, " L/m" );
        ShowI16U( 0x0506u, Configure.SetFlow[PP_R24_B], 0x0503u, " L/m" );

        item = Menu_Select( menu, item + 1u );
        switch( item )
        {
        case 0:
            if ( changed )
            {
                need_redraw = TRUE;
                switch( MsgBox( "�����޸Ľ�� ?", vbYesNoCancel + vbDefaultButton3 ))
                {
                case vbYes:
                    ConfigureSave();
                    break;
                case vbNo:
                    ConfigureLoad();
                    break;
                case vbCancel:
                    item = 1u;
                    break;
                default:
                    break;
                }
            }
            break;
        case 1:
            if( EditI16U( 0x0306u, & Configure.SetFlow[PP_R24_A], 0x0503u ))
            {
                if ( Configure.SetFlow[PP_R24_A] >  300u )
                {
                    Configure.SetFlow[PP_R24_A] =  300u;
                }
                if ( Configure.SetFlow[PP_R24_A] <  100u )
                {
                    Configure.SetFlow[PP_R24_A] =  100u;
                }
                changed = TRUE;
            }
            break;
        case 2:
            if( EditI16U( 0x0506u, & Configure.SetFlow[PP_R24_B], 0x0503u ))
            {
                if ( Configure.SetFlow[PP_R24_B] >  300u )
                {
                    Configure.SetFlow[PP_R24_B] =  300u;
                }
                if ( Configure.SetFlow[PP_R24_B] <  100u )
                {
                    Configure.SetFlow[PP_R24_B] =  100u;
                }
                changed = TRUE;
            }
            break;
        default:
            break;
        }
    }
    while( enumSelectESC != item );
}


void	Configure_Pr_Protect( void )
{
    static	struct  uMenu  const  menu[] =
    {
        { 0x0101u, "����ѹ������" },
		{ 0x0300u, "�� ѹ" }
    };
    uint8_t	item = 0u;
    BOOL	changed = FALSE;
    BOOL	need_redraw = TRUE;

    do {
        if ( need_redraw )
        {
            cls();
            Menu_Redraw( menu );
            need_redraw = FALSE;
        }

        if ( Configure.TSP_Pr_Portect != 0u )
		{
			ShowI16U( 0x0307u, Configure.TSP_Pr_Portect , 0x0502u, " kPa" );
		}
		else
		{
			Lputs( 0x0307, " [ͣ��]  " );
		}

        item = Menu_Select( menu, item + 1u );
        switch( item )
        {
        case 0:
            if ( changed )
            {
                need_redraw = TRUE;
                switch( MsgBox( "�����޸Ľ�� ?", vbYesNoCancel + vbDefaultButton3 ))
                {
                case vbYes:
                    ConfigureSave();
                    break;
                case vbNo:
                    ConfigureLoad();
                    break;
                case vbCancel:
                    item = 1u;
                    break;
                default:
                    break;
                }
            }
            break;
        case 1:
			ShowI16U( 0x0307u, Configure.TSP_Pr_Portect , 0x0502u, " kPa" );

			if( EditI16U( 0x0307u, & Configure.TSP_Pr_Portect , 0x0502u ))
            {
                if ( Configure.TSP_Pr_Portect > 2000u )
                {
                    Configure.TSP_Pr_Portect = 2000u;
                }
                changed = TRUE;
            }
            break;
        default:
            break;
        }
    }
    while( enumSelectESC != item );
}


static	void	Configure_Flow_KB120F( void )
{
    //	for kb120f, kb6120, kb6120b
    static	struct  uMenu  const  menu[] =
    {
				{ 0x0201u, "���÷۳�����" },
				{ 0x0300u, "�� ��" },
				{ 0x0500u, "�� ѹ" }
    };
    uint8_t	item = 0u;
    BOOL	changed = FALSE;
    BOOL	need_redraw = TRUE;

    do
    {
        if ( need_redraw )
        {
            cls();
            Menu_Redraw( menu );
            need_redraw = FALSE;
        }

        ShowI16U( 0x0307u, Configure.SetFlow[PP_TSP], 0x0501u, " L/m" );
        if ( Configure.TSP_Pr_Portect != 0u )
		{
			ShowI16U( 0x0507u, Configure.TSP_Pr_Portect , 0x0502u, " kPa" );
		}
		else
		{
			Lputs( 0x0507, " [ͣ��]  " );
		}

		item = Menu_Select( menu, item + 1u );
		switch( item )
		{
		case 0:
				if ( changed )
				{
						need_redraw = TRUE;
						switch( MsgBox( "�����޸Ľ�� ?", vbYesNoCancel + vbDefaultButton3 ))
						{
						case vbYes:
								ConfigureSave();
								break;
						case vbNo:
								ConfigureLoad();
								break;
						case vbCancel:
								item = 1u;
								break;
						default:
								break;
						}
				}
				break;
		case 1:
				if( EditI16U( 0x0307u, & Configure.SetFlow[PP_TSP  ], 0x0501u ))
				{
						if ( Configure.SetFlow[PP_TSP  ] > 1400u )
						{
								Configure.SetFlow[PP_TSP  ] = 1400u;
						}
						if ( Configure.SetFlow[PP_TSP  ] <  600u )
						{
								Configure.SetFlow[PP_TSP  ] =  600u;
						}
						changed = TRUE;
				}
				break;

		case 2:
                ShowI16U( 0x0507u, Configure.TSP_Pr_Portect , 0x0502u, " kPa" );

                if( EditI16U( 0x0507u, & Configure.TSP_Pr_Portect , 0x0502u ))
				{
						if ( Configure.TSP_Pr_Portect > 2000u )
						{
								Configure.TSP_Pr_Portect = 2000u;
						}
						changed = TRUE;
				}
				break;
		default:
				break;
		}
}while( enumSelectESC != item );

}

static	void	Configure_Flow_KB6120C( void )
{
    static	struct  uMenu  const  menu[] =
    {
        { 0x0301u, "���ò�������" },
        { 0x0200u, "�� ��" },
        { 0x0400u, "�վ�A" },
        { 0x0600u, "�վ�B" },
    };
    uint8_t	item = 0u;
    BOOL	changed = FALSE;
    BOOL	need_redraw = TRUE;

    do
    {
        if ( need_redraw )
        {
            cls();
            Menu_Redraw( menu );
            need_redraw = FALSE;
        }

        ShowI16U( 0x0206u, Configure.SetFlow[PP_TSP  ], 0x0501u, " L/m" );
        ShowI16U( 0x0406u, Configure.SetFlow[PP_R24_A], 0x0503u, " L/m" );
        ShowI16U( 0x0606u, Configure.SetFlow[PP_R24_B], 0x0503u, " L/m" );

        item = Menu_Select( menu, item + 1u );
        switch( item )
        {
        case 0:
            if ( changed )
            {
                need_redraw = TRUE;
                switch( MsgBox( "�����޸Ľ�� ?", vbYesNoCancel + vbDefaultButton3 ))
                {
                case vbYes:
                    ConfigureSave();
                    break;
                case vbNo:
                    ConfigureLoad();
                    break;
                case vbCancel:
                    item = 1u;
                    break;
                default:
                    break;
                }
            }
            break;
        case 1:
            if( EditI16U( 0x0206u, & Configure.SetFlow[PP_TSP  ], 0x0501u ))
            {
                if ( Configure.SetFlow[PP_TSP  ] > 1400u )
                {
                    Configure.SetFlow[PP_TSP  ] = 1400u;
                }
                if ( Configure.SetFlow[PP_TSP  ] <  600u )
                {
                    Configure.SetFlow[PP_TSP  ] =  600u;
                }
                changed = TRUE;
            }
            break;
        case 2:
            if( EditI16U( 0x0406u, & Configure.SetFlow[PP_R24_A], 0x0503u ))
            {
                if ( Configure.SetFlow[PP_R24_A] >  300u )
                {
                    Configure.SetFlow[PP_R24_A] =  300u;
                }
                if ( Configure.SetFlow[PP_R24_A] <  100u )
                {
                    Configure.SetFlow[PP_R24_A] =  100u;
                }
                changed = TRUE;
            }
            break;
        case 3:
            if( EditI16U( 0x0606u, & Configure.SetFlow[PP_R24_B], 0x0503u ))
            {
                if ( Configure.SetFlow[PP_R24_B] >  300u )
                {
                    Configure.SetFlow[PP_R24_B] =  300u;
                }
                if ( Configure.SetFlow[PP_R24_B] <  100u )
                {
                    Configure.SetFlow[PP_R24_B] =  100u;
                }
                changed = TRUE;
            }
            break;
        default:
            break;
        }
    }
    while( enumSelectESC != item );
}

static	void	menu_ConfigureFlow( void )
{
    BOOL	hasPumpTSP = ( enumPumpNone != Configure.PumpType[PP_TSP ] );
    BOOL	hasPumpR24 = (( enumPumpNone != Configure.PumpType[PP_R24_A] ) || ( enumPumpNone != Configure.PumpType[PP_R24_B] ));

    if ( enumOrifice_2 == Configure.PumpType[PP_TSP ] )
    {
        Configure_Flow_KB1000();	//	������
    }
    else if (( ! hasPumpR24 ) && (   hasPumpTSP ))
    {
        Configure_Flow_KB120F();	//	������
    }
    else if (( hasPumpR24 ) && ( ! hasPumpTSP ))
    {
        Configure_Flow_KB2400();	//	���º���
    }
    else if ((   hasPumpR24 ) && (   hasPumpTSP ))
    {
        Configure_Flow_KB6120C();	//	KB-6120C��TSP��+�վ���
    }
    else // (  ! hasPumpR24 ) && ( ! hasPumpTSP ))
    {
        //	ʲô��ûװ������������á����߿�������򵥵�KB-6E������Ҫ�����κ�������
        MsgBox( "��������", vbOKOnly );
    }
}

/********************************** ����˵�� ***********************************
*  ���ñ�������Ķ����¶�
*******************************************************************************/
void	menu_SelectTstd( void )
{
    static  struct  uMenu  const  menu[] =
    {
			{ 0x0301u, "ѡ�����¶�" },
			{ 0x0204u, " 0��(273K)" },
			{ 0x0404u, "20��(293K)" },
			{ 0x0604u, "25��(298K)" }
    };
    uint8_t	item;

    uint8_t	SetTstd = Configure.SetTstd;
    BOOL	changed = FALSE;

    cls();
    Menu_Redraw( menu );

    do
    {
			switch ( SetTstd )
			{
			default:
			case enum_273K:
					Lputs( 0x0200u,"->"	);
					Lputs( 0x0400u,"  "	);
					Lputs( 0x0600u,"  "	);
					item = 1;
					break;
			case enum_293K:
					Lputs( 0x0200u,"  "	);
					Lputs( 0x0400u,"->"	);
					Lputs( 0x0600u,"  "	);
					item = 2;
					break;
			case enum_298K:
					Lputs( 0x0200u,"  "	);
					Lputs( 0x0400u,"  "	);
					Lputs( 0x0600u,"->"	);
					item = 3;
					break;
			}
			item = Menu_Select( menu, item );
			switch ( item )
			{
			case 1:
					SetTstd = enum_273K;
					changed = TRUE;
					break;
			case 2:
					SetTstd = enum_293K;
					changed = TRUE;
					break;
			case 3:
					SetTstd = enum_298K;
					changed = TRUE;
					break;
			default:
					break;
			}
    }
    while( enumSelectESC != item );

    if ( changed )
    {
        Configure.SetTstd = SetTstd;
        ConfigureSave();
    }
}
void menu_ChangePassword( void )
{
 uint32_t  passwordi,passwordii;
 BOOL Done = FALSE;
 do{
		cls();
		passwordi = passwordii =0u;
		Lputs( 0x0202u, "������������:" );
		if ( ! EditI32U( 0x0405u, &passwordi, 0x0600u ))
		{
			 Done = TRUE;
		}
		if ( Done == FALSE )
		{
			Lputs( 0x0202u, "��ȷ��������:" );
			if ( ! EditI32U( 0x0405u, &passwordii, 0x0600u ))
			{
				Done = TRUE;
			}
		}
		if( ( passwordi == passwordii ) && ( Done == FALSE ))
		{
			cls();
			switch( MsgBox( "�Ƿ񱣴�������?", vbYesNoCancel | vbDefaultButton3 ) )
			{
			case	vbYes:
				Configure.Password = passwordii;
				ConfigureSave();
				Done = TRUE;
				break;
			case	vbNo:
				Done = TRUE;
				break;
			case	vbCancel:
				break;
			}		
		}
		else	if( Done == FALSE )
		{
			cls();
			MsgBox( "���벻һ��!", vbOKOnly );
			if( vbYes != MsgBox( "�Ƿ�����޸�?", vbYesNo ) )
				Done = TRUE;
		}
			
 }while( !Done );
}
void	menu_MoreSet( void )
{
	static	struct	uMenu  const menu[] =
	{
		{ 0x0201u,  "��������"  },
		{ 0x0301u,  "����¶�"  },
		{ 0x0601u,  "�����޸�"  },
	};
	uint8_t	item = 1u;
	
	do
	{
		cls();
		Menu_Redraw( menu );
		item = Menu_Select( menu, item );
		switch( item )
		{
		case 1:
			menu_SelectTstd();
			break;
		case 2:
			menu_ChangePassword();
			break;
		default:
				break;
		}
	}while ( enumSelectESC != item );    
}
/********************************** ����˵�� ***********************************
*  ϵͳ���ã������û������Ĳ��֣�

ʱ�䣬���ڡ�ʱ�䡢ʵʱ�� ���Է���ά�����˵�
����ѹ��Ԥ��/ʵ�⣩�������¶�
�����䣨���·�ʽ�������¶ȣ�δ��װ ���� ������δ��װ
���ļ��ţ��ļ�ʹ�����
��ʱ��ʽ����ʱ��ʽ�����߷�������ʱ��ѡ�
����ʾ���Աȶȣ��Ҷȣ�
*******************************************************************************/
static	void	menu_Configure( void )
{
	static	struct	uMenu  const menu[] =
	{
			{ 0x0302u,  "����"  },
			{ 0x0202u,  "ʱ��"  }, { 0x0208u, "��������" },
			{ 0x0401u, "������" }, { 0x0408u, "��ʱ��ʽ" },
			{ 0x0601u, "����ѹ" }, { 0x0608u, "��������" }// { 0x0608u, "��ʱ��ʽ" }
	};
	static	struct	uMenu  const menu1[] =
	{
			{ 0x0302u,  "����"  },
			{ 0x0202u,  "ʱ��"  }, { 0x0208u, "��������" },
			{ 0x0401u, "������" }, { 0x0408u, "��ʱ��ʽ" },
			{ 0x0601u, "����ѹ" }, { 0x0608u, "��������" }// { 0x0608u, "��ʱ��ʽ" }
	};
	static	struct	uMenu  const menu2[] =
	{
			{ 0x0302u,  "����"  },
			{ 0x0202u,  "ʱ��"  }, { 0x0208u, "������ʱ" },
			{ 0x0401u, "������" }, { 0x0408u, "������ʱ" },
			{ 0x0601u, "����ѹ" }, { 0x0608u, "��������" }// { 0x0608u, "��ʱ��ʽ" }
	};
	static	struct	uMenu  const menu3[] =
	{
			{ 0x0302u,  "����"  },
			{ 0x0202u,  "ʱ��"  }, { 0x0208u, "������ʱ" },
			{ 0x0401u, "������" }, { 0x0408u, "������ʱ" },
			{ 0x0601u, "����ѹ" }, { 0x0608u, "��������" }// { 0x0608u, "��ʱ��ʽ" }
	};
	uint8_t	item = 1u;

	do
	{
			cls();
			if(( Configure.HeaterType == enumHCBoxOnly ) && ( Configure.InstrumentType != type_KB2400HL ))
			{
				Menu_Redraw( menu );
				item = Menu_Select( menu, item );
			}
			else if(  ( Configure.HeaterType == enumHeaterOnly ) && ( Configure.InstrumentType != type_KB2400HL ) )
			{	
				Menu_Redraw( menu1 );
				item = Menu_Select( menu1, item );
			} 
			else if(( Configure.HeaterType == enumHCBoxOnly ) && ( Configure.InstrumentType == type_KB2400HL ))
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
			case 1:
					menu_SetupClock();
					break;
			case 3:
					menu_ConfigureHCBox();
					break;
			case 5:
					menu_Configure_Ba();
					break;
			case 2:
				if( Configure.InstrumentType == type_KB2400HL )
					menu_SelectDelayMode();
				else
					menu_ConfigureFlow();
				 break;
			case 4:
				if( Configure.InstrumentType == type_KB2400HL )
					menu_SelectTimeMode();
				else
					menu_ConfigureTime();       
				 
				break;
			case 6:
					menu_MoreSet();
					break;
			default:
					break;
			}
	}
	while ( enumSelectESC != item );
}

/********************************** ����˵�� ***********************************
*	���˵� -> ά���˵�
*******************************************************************************/
void	menu_Maintenance( void )
{
    extern	void	menu_ConfigureEx( void );

    static	struct uMenu  const  menu[] =
    {
        { 0x0202u, "ά��" },
        { 0x0303u, "����" }, { 0x030Au, "�궨" },
        { 0x0603u, "��¼" }, { 0x060Au, "�汾" }

    };
    uint8_t	item = 1u;

    uint32_t  password = InputPassword();

    if (( Configure.Password != password ) && ( SysPassword1a != password ))
    {
        return;
    }

    do
    {
        cls();
        Menu_Redraw( menu );

        item = Menu_Select( menu, item );
        switch( item )
        {
        case 1:
					if ( ! Sampler_isRunning( Q_ALL ))
					{
							menu_Configure();
					}
					break;
        case 2:
					if ( ! Sampler_isRunning( Q_ALL ))
					{
							menu_Calibrate();
					}
					break;
        case 3:
            PowerLog_Query();
            break;
        case 4:
					ShowEdition();
					if ( K_RIGHT == getKey())
					{
							if ( ! releaseKey( K_RIGHT, 100u ))
							{
									beep();
									menu_ConfigureEx();
							}
					}
					break;
        default:
          break;
        }
    }
    while( enumSelectESC != item );
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
