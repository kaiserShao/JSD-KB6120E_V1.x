/**************** (C) COPYRIGHT 2013 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Menu.C
* �� �� ��: ����
* ��  ��  : �˵���ʾ/ѡ��
* ����޸�: 2013��8��17��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "AppDEF.H"
#include <stm32f10x.h>
#include <stdio.h>
#define	LOBYTE(__w)		((uint8_t)((__w)&0x0FF))
#define	HIBYTE(__w)		((uint8_t)(((__w)>>8)&0x0FF))

///////////////////////////////////////////////////////////////////
//	�˵�����/ѡ��
///////////////////////////////////////////////////////////////////
void	Menu_Item_Mask ( const struct uMenu * menu, uint8_t item )
{
	LcmMask ( menu[item].yx, strlen ( menu[item].sz ) );
}

void	Menu_Redraw ( const struct uMenu * menu )
{
    uint8_t	mlen, mlen_row, mlen_col;
    uint8_t	i;

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

    if ( menu[0].sz != NULL )
    {
        Lputs ( 0x0000u, menu[0].sz );	// ��ʾ�˵�����
    }

    for ( i = 1u; i <= mlen; ++i )
    {
        Lputs ( menu[i].yx, menu[i].sz );
    }
}

uint8_t	Menu_Select ( const struct uMenu * menu, uint8_t item )
{
	return	Menu_Select_Ex( menu, item, NULL );
}
uint8_t	Menu_Select2 ( const struct uMenu * menu, uint8_t item, BOOL Direction )  //�ɻ���
{
	return	Menu_Select_Ex2( menu, item, NULL, Direction );
}
uint8_t	Menu_Select3 ( const struct uMenu * menu, uint8_t item, uint8_t timeout )         //�ɳ�ʱ�ر�
{
	return	Menu_Select_Ex3( menu, item, NULL, timeout);
}
uint8_t	Menu_Select_Ex ( const struct uMenu * menu, uint8_t item, void ( *pHook ) ( void ) )
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
			if ( NULL != pHook )
			{
				pHook();
			}
		}
		while ( ! hitKey ( 50u ) );

		Lputs ( menu[item].yx, menu[item].sz );

		switch ( getKey() )
		{
		case K_RIGHT:
			if ( 0 == ( item % mlen_col ) )
			{
				item -= mlen_col;
			}
			++item;
			break;

		case K_LEFT:
			--item;
			if ( 0 == ( item % mlen_col ) )
			{
				item += mlen_col;
			}
			break;

		case K_DOWN:
			item += mlen_col;
			if ( item > mlen )
			{
				item -= mlen;
			}
			break;

		case K_UP:
			if ( item <= mlen_col )
			{
				item += mlen;
			}
			item -= mlen_col;
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t	Menu_Select_Ex2 ( const struct uMenu * menu, uint8_t item, void ( *pHook ) ( void ),BOOL Direction ) 
{                                                                                           //TRUE ������  FALSE �ĺ��� 
	uint8_t	mlen, mlen_row, mlen_col;
	BOOL Flag = FALSE;
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
			if ( NULL != pHook )
			{
				pHook();
			}
		}
		while ( ! hitKey ( 50u ) );

		Lputs ( menu[item].yx, menu[item].sz );

		switch ( getKey() )
		{
				
		case K_RIGHT:
		  if((!Direction) && (!Flag))	
			{
				item += mlen_col;
				if ( item > mlen )
				{
					item -= mlen;
				}
			}
			else
			{
				if ( 0 == ( item % mlen_col ) )
				{
					item -= mlen_col;
				}
				++item;
			}
			
			break;

		case K_LEFT:
			if((!Direction) && (!Flag))
			{
				if ( item <= mlen_col )
				{
					item += mlen;
				}
				item -= mlen_col;
			}
			else
			{
				--item;
				if ( 0 == ( item % mlen_col ) )
				{
					item += mlen_col;
				}
			}		
			break;
			
		case K_DOWN:
			if( Direction && (!Flag) )
			{
				 if ( 0 == ( item % mlen_col ) )
				{
					item -= mlen_col;
				}
				++item;
			}		
			else
			{
				item += mlen_col;
				if ( item > mlen )
				{
					item -= mlen;
				}
			}
			break;
		case K_UP:
			if( Direction && (!Flag) )
			{
				--item;
				if ( 0 == ( item % mlen_col ) )
				{
					item += mlen_col;
				}
			}		
			else
			{
				if ( item <= mlen_col )
				{
					item += mlen;
				}
				item -= mlen_col;
			}
			break;

		case K_OK:
			Flag = TRUE;
			return	item;

		case K_ESC:
			Flag = FALSE;
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
uint8_t	Menu_Select_Ex3 ( const struct uMenu * menu, uint8_t item, void ( *pHook ) ( void ),uint8_t timeout )
{  
	uint8_t	mlen, mlen_row, mlen_col;
	uint8_t secnow;
	uint32_t starsec; 
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
	starsec = get_Now();
	for ( ; ; )
	{ 
		LcmMask ( menu[item].yx, strlen ( menu[item].sz ) );   
		do
		{
			CHAR	sbuffer[6];
			if ( NULL != pHook )
			{
				pHook();
			}    
			secnow = get_Now() - starsec;
			sprintf( sbuffer, "(%2us)", timeout - secnow );	Lputs( 0x0404, sbuffer );
			if( secnow >= timeout ) 
				return 1;				
				
		}
		while ( ! hitKey ( 50u ) );
		Lputs ( menu[item].yx, menu[item].sz );
  
		switch ( getKey() )
		{
		case K_RIGHT:
			if ( 0 == ( item % mlen_col ) )
			{
				item -= mlen_col;
			}
			++item;
			break;

		case K_LEFT:
			--item;
			if ( 0 == ( item % mlen_col ) )
			{
				item += mlen_col;
			}
			break;

		case K_DOWN:
			item += mlen_col;
			if ( item > mlen )
			{
				item -= mlen;
			}
			break;

		case K_UP:
			if ( item <= mlen_col )
			{
				item += mlen;
			}
			item -= mlen_col;
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
//		///////////////////////////////////////////////////////////////////
//		//	ģ��VB��MsgBox����
//		///////////////////////////////////////////////////////////////////
//		//	buttons ��������������ֵ��
//		//	vbOKOnly			0 ֻ��ʾ OK ��ť��
//		//	vbOKCancel			1 ��ʾ OK �� Cancel ��ť��
//		//	vbAbortRetryIgnore	2 ��ʾ Abort��Retry �� Ignore ��ť��
//		//	vbYesNoCancel		3 ��ʾ Yes��No �� Cancel ��ť��
//		//	vbYesNo				4 ��ʾ Yes �� No ��ť��
//		//	vbRetryCancel		5 ��ʾ Retry �� Cancel ��ť��
//		//
//		//	vbDefaultButton1	0x00 ��һ����ť��ȱʡֵ��
//		//	vbDefaultButton2	0x10 �ڶ�����ť��ȱʡֵ��
//		//	vbDefaultButton3	0x20 ��������ť��ȱʡֵ��
//		//	vbDefaultButton4	0x30 ���ĸ���ť��ȱʡֵ��

//		//	����ֵ
//		//	0x00	K_ESC press
//		//	vbOK		1 OK
//		//	vbCancel	2 Cancel
//		//	vbAbort		3 Abort
//		//	vbRetry		4 Retry
//		//	vbIgnore	5 Ignore
//		//	vbYes		6 Yes
//		//	vbNo 		7 No

//		//	enum {
//		//		vbOKOnly,
//		//		vbOKCancel,
//		//		vbAbortRetryIgnore,
//		//		vbYesNoCancel,
//		//		vbYesNo,
//		//		vbRetryCancel,
//		//	};
//		//	enum {
//		//		vbDefaultButton1 = 0x00,	//	 ��һ����ť��ȱʡֵ��
//		//		vbDefaultButton2 = 0x10,	//	 �ڶ�����ť��ȱʡֵ��
//		//		vbDefaultButton3 = 0x20,	//	 ��������ť��ȱʡֵ��
//		//		vbDefaultButton4 = 0x30,	//	 ���ĸ���ť��ȱʡֵ��
//		//	};

//		//	MsgBox�ķ���ֵ
//		//	enum {	vbOK, vbCancel, vbAbort, vbRetry, vbIgnore, vbYes, vbNo }
//		uint8_t	MsgBox ( const char * szPrompt, uint16_t buttons )
//		{
//			static	const struct uMenu  menu_OKOnly[]			= { 0x0101u, NULL, 0x050B, "ȷ��" };
//			static	const struct uMenu  menu_OKCancel[]			= { 0x0102u, NULL, 0x0503, "ȷ��", 0x0509, "ȡ��" };
//			static	const struct uMenu  menu_YesNo[]			= { 0x0102u, NULL, 0x0503, " �� ", 0x0509, " �� " };
//			static	const struct uMenu  menu_RetryCancel[]		= { 0x0102u, NULL, 0x0503, "����", 0x0509, "ȡ��" };
//			static	const struct uMenu  menu_YesNoCancel[]		= { 0x0103u, NULL, 0x0501, " �� ", 0x0506, " �� ", 0x050B, "ȡ��" };
//			static	const struct uMenu  menu_AbortRetryIgnore[]	= { 0x0103u, NULL, 0x0501, "��ֹ", 0x0506, "����", 0x050B, "����" };

//			static	const struct uMenu * const menu_lst[] =
//			{
//				menu_OKOnly,
//				menu_OKCancel,
//				menu_AbortRetryIgnore,
//				menu_YesNoCancel,
//				menu_YesNo,
//				menu_RetryCancel
//			};
//			const struct uMenu * menu;
//			uint8_t	Response;

//			cls();
//			//	if( szTitle  != NULL )	Lputs( 0x0000u, szTitle );
//			if ( szPrompt != NULL )
//			{
//				Lputs ( 0x0000u, szPrompt );
//			}

//			menu = menu_lst[buttons & 0x0F];
//			Menu_Redraw ( menu );
//			Response = Menu_Select ( menu, buttons / 0x10 % 0x10 + 1 );	//	Menu_Select������Զ������������Ĭ��ѡ��
//			switch ( buttons & 0x0F )
//			{
//			default:
//			case vbOKOnly:				switch ( Response )
//				{
//				default:	return	vbCancel;
//				case 1:		return	vbOK;
//				}
//				//	break;
//			case vbOKCancel:			switch ( Response )
//				{
//				default:	return	vbCancel;
//				case 1:		return	vbOK;
//					//	case 2:		return	vbCancel;
//				}
//				//	break;
//			case vbAbortRetryIgnore:	switch ( Response )
//				{
//				default:	return	vbRetry;
//				case 1:		return	vbAbort;
//					//	case 2:		return	vbRetry;
//				case 3:		return	vbIgnore;
//				}
//				//	break;
//			case vbYesNoCancel:			switch ( Response )
//				{
//				default:	return	vbCancel;
//				case 1:		return	vbYes;
//				case 2:		return	vbNo;
//					//	case 3:		return	vbCancel;
//				}
//				//	break;
//			case vbYesNo:				switch ( Response )
//				{
//				default:	return	vbCancel;
//				case 1:		return	vbYes;
//				case 2:		return	vbNo;
//				}
//				//	break;
//			case vbRetryCancel:			switch ( Response )
//				{
//				default:	return	vbCancel;
//				case 1:		return	vbRetry;
//					//	case 2:		return	vbCancel;
//				}
//				//	break;
//			}
//		}

//		///////////////////////////////////////////////////////////////////
//		//	uint8_t	Confirm( const char const * prompt ){
//		//	//	struct	uMenu const menu[] = {
//		//	//		0x03, NULL, 0x0501, "��", 0x0505, "��", 0x050A, "ȡ��",
//		//	//	};
//		//	//	cls();	Lputs( 0x0000, prompt );
//		//	//	Menu_Redraw( menu );
//		//	//	return	Menu_Select( menu, 3 );
//		//
//		//		return	MsgBox( prompt, vbYesNoCancel + vbDefaultButton3 );
//		//	}
//		///////////////////////////////////////////////// End of File /////




///////////////////////////////////////////////////////////////////
//	ģ��VB��MsgBox����
///////////////////////////////////////////////////////////////////
uint8_t	MsgBox( const CHAR  * sPrompt, uint16_t buttons )
{
	static	const struct uMenu  menu_OKOnly[]			= { 0x0101u, NULL, 0x050B, "ȷ��" };
	static	const struct uMenu  menu_OKCancel[]			= { 0x0102u, NULL, 0x0503, "ȷ��", 0x0509, "ȡ��" };
	static	const struct uMenu  menu_YesNo[]			= { 0x0102u, NULL, 0x0503, " �� ", 0x0509, " �� " };
	static	const struct uMenu  menu_RetryCancel[]		= { 0x0102u, NULL, 0x0503, "����", 0x0509, "ȡ��" };
	static	const struct uMenu  menu_YesNoCancel[]		= { 0x0103u, NULL, 0x0501, " �� ", 0x0506, " �� ", 0x050B, "ȡ��" };
	static	const struct uMenu  menu_AbortRetryIgnore[]	= { 0x0103u, NULL, 0x0501, "��ֹ", 0x0506, "����", 0x050B, "����" };

	static	const struct uMenu * const menu_list[] = 
	{
		menu_OKOnly,
		menu_OKCancel,
		menu_AbortRetryIgnore,
		menu_YesNoCancel,
		menu_YesNo,
		menu_RetryCancel
	};
	const struct uMenu * menu;
	uint8_t	Response;

	if( sPrompt != NULL )
	{
		cls();
		Lputs( 0x0000, sPrompt );
	}

	menu = menu_list[buttons & 0x0Fu];
	Menu_Redraw( menu );
	Response = Menu_Select( menu, buttons / 0x10 % 0x10 + 1 );	//	Menu_Select������Զ������������Ĭ��ѡ��

	switch( buttons & 0x0Fu )
	{
	default:
	case vbOKOnly:
		switch( Response )
		{
			default:	return	vbCancel;
			case 1:		return	vbOK;
		}
		//	break;
	case vbOKCancel:
		switch( Response )
		{
			default:	return	vbCancel;
			case 1:		return	vbOK;
		//	case 2:		return	vbCancel;
		}
		//	break;
	case vbAbortRetryIgnore:
		switch( Response )
		{
			default:	return	vbRetry;
			case 1:		return	vbAbort;
		//	case 2:		return	vbRetry;
			case 3:		return	vbIgnore; 
		}
		//	break;
	case vbYesNoCancel:
		switch( Response )
		{
			default:	return	vbCancel;
			case 1:		return	vbYes;
			case 2:		return	vbNo;
		//	case 3:		return	vbCancel;
		}
		//	break;
	case vbYesNo:
		switch( Response )
		{
			default:	return	vbCancel;
			case 1:		return	vbYes;
			case 2:		return	vbNo;
		}
		//	break;
	case vbRetryCancel:
		switch( Response )
		{
			default:	return	vbCancel;
			case 1:		return	vbRetry;
		//	case 2:		return	vbCancel;
		}
		//	break;
	}
}
/***************************************/
//�˵�ģ��
// static	struct uMenu  const  menu[] =
// 	{
// 		{ 0x0201u, "" },
// 		{ 0x0300u, "" },
// 		{ 0x0500u, "" },
// 	};
// 	uint8_t item = 1u;	
// 	cls();
// 	Menu_Redraw( menu );
// 	do {
// 		item = Menu_Select( menu, item ,FALSE);

// 		switch ( item )
// 		{
// 		case 1:
// 			break;
// 		case 2:
// 			break;
// 		}
// 	} while ( enumSelectESC != item );
// 	
// 	}
/********  (C) COPYRIGHT 2013 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/
