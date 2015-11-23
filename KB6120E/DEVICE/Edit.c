/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Edit.C
* 创 建 者: 董峰
* 描  述  : 数字显示/编辑
* 最后修改: 2014年8月25日
*********************************** 修订记录 ***********************************
* 版  本:
* 修订人:
*******************************************************************************/
#include "AppDEF.H"
#include <time.h>
#include <stdio.h>

#define	K_INC	K_UP
#define	K_DEC	K_DOWN

/********************************** 功能说明 ***********************************
*  显示时间
*******************************************************************************/
void	ShowClock( uint16_t yx, const uClock * pClock )
{
    CHAR	sbuffer[20];
    struct tm t_tm;

    _localtime_r( pClock, &t_tm );
    sprintf( sbuffer, " %02u-%02u %02u:%02u:%02u ",
             t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
    Lputs( yx, sbuffer );
}


void	ShowClockDate( uint16_t yx, const uClock * pClock )
{
    CHAR	sbuffer[20];
    struct tm t_tm;

    _localtime_r( pClock, &t_tm );
    sprintf( sbuffer, "%04u-%02u-%02u",  t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday );
    Lputs( yx, sbuffer );

}

void	ShowClockTime( uint16_t yx, const uClock * pClock )
{
    CHAR	sbuffer[20];
    struct tm t_tm;

    _localtime_r( pClock, &t_tm );
    sprintf( sbuffer, "%02u:%02u:%02u", t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
    Lputs( yx, sbuffer );
}

static	uint8_t	dayOfmonth( uint16_t year, uint8_t month )
{
    switch( month )
    {
    default:
    case  1:
        return 31;
    case  2:
        return (((year%4)==0) ? 29 : 28 );
    case  3:
        return 31;
    case  4:
        return 30;
    case  5:
        return 31;
    case  6:
        return 30;
    case  7:
        return 31;
    case  8:
        return 31;
    case  9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    }
}

BOOL	EditClockDate( uint16_t yx, uClock * pClock )
{
    CHAR  	sbuffer[20];
    struct tm t_tm;
    uint16_t	year;
    uint8_t 	month;
    uint8_t 	day_m;
    uint8_t 	option = 1u;
		uint16_t gray  = Configure.DisplayGray;
		BOOL graychanged = FALSE;	

    _localtime_r( pClock, &t_tm );
    year  = t_tm.tm_year + 1900;
    month = t_tm.tm_mon  + 1;
    day_m = t_tm.tm_mday;

    for(;;)
    {
        sprintf( sbuffer, "%04u-%02u-%02u", year, month, day_m );
        Lputs( yx, sbuffer );

        switch( option )
        {
        case 1:
            LcmMask( yx  , 4 );
            break;	// year
        case 2:
            LcmMask( yx+5, 2 );
            break;	// month
        case 3:
            LcmMask( yx+8, 2 );
            break;	// day_m
        default:
            break;
        }
        switch( getKey() )
        {
        case K_INC:
            switch( option )
            {
            case 1:
                if ( ++year > 2099u )
                {
                    year = 2000u;
                }
                break;
            case 2:
                if ( ++month > 12u )
                {
                    month = 1u;
                }
                break;
            case 3:
                if ( ++day_m > dayOfmonth( year, month ))
                {
                    day_m = 1u;
                }
                break;
            default:
                break;
            }
            break;

        case K_DEC:
            switch(  option )
            {
            case 1:
                if ( --year < 2000u )
                {
                    year = 2099u;
                }
                break;
            case 2:
                if ( --month < 1u )
                {
                    month = 12u;
                }
                break;
            case 3:
                if ( --day_m < 1u )
                {
                    day_m = dayOfmonth( year, month );
                }
                break;
            default:
                break;
            }
            break;

        case K_RIGHT:
            if ( ++option > 3u )
            {
                option = 1u;
            }
            break;

        case K_LEFT:
            if ( --option < 1u )
            {
                option = 3u;
            }
            break;

        case K_OK:
            t_tm.tm_year = year  - 1900;
            t_tm.tm_mon  = month - 1;
            t_tm.tm_mday = day_m;
            * pClock = mktime( &t_tm );
            return	TRUE;

        case K_ESC:
            return	FALSE;
						
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


BOOL	EditClockTime( uint16_t yx, uClock * pClock )
{
    CHAR	sbuffer[20];
    struct tm t;
    uint8_t 	option = 1u;
		uint16_t gray  = Configure.DisplayGray;
		BOOL graychanged = FALSE;	

    _localtime_r( pClock, &t );

    for(;;)
    {
        sprintf( sbuffer, "%02u:%02u:%02u", t.tm_hour, t.tm_min, t.tm_sec );
        Lputs( yx, sbuffer );

        switch( option )
        {
        case 1:
            LcmMask( yx  , 2 );
            break;  // hour
        case 2:
            LcmMask( yx+3, 2 );
            break;  // minute
        case 3:
            LcmMask( yx+6, 2 );
            break;  // second
        default:
            break;
        }
        switch( getKey() )
        {
        case K_INC:
            switch( option )
            {
            case 1:
                t.tm_hour = ( t.tm_hour +  1u ) % 24u;
                break;
            case 2:
                t.tm_min  = ( t.tm_min  +  1u ) % 60u;
                break;
            case 3:
                t.tm_sec  = ( t.tm_sec  +  1u ) % 60u;
                break;
            default:
                break;
            }
            break;

        case K_DEC:
            switch( option )
            {
            case 1:
                t.tm_hour = ( t.tm_hour + 23u ) % 24u;
                break;
            case 2:
                t.tm_min  = ( t.tm_min  + 59u ) % 60u;
                break;
            case 3:
                t.tm_sec  = ( t.tm_sec  + 59u ) % 60u;
                break;
            default:
                break;
            }
            break;

        case K_RIGHT:
            if ( ++option > 3u )
            {
                option = 1;
            }
            break;

        case K_LEFT:
            if ( --option < 1u )
            {
                option = 3u;
            }
            break;

        case K_OK:
            * pClock = mktime( &t );
            return	TRUE;

        case K_ESC:
            return	FALSE;

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

///////////////////////////////////////////////////////////////////
// 设置采样时间参数
///////////////////////////////////////////////////////////////////
void	ShowDATE( uint16_t yx, uint32_t timer )
{
    CHAR	sbuffer[20];
    struct tm t;

    _localtime_r( &timer, &t );
    sprintf( sbuffer, "%2d-%2d %02d:%02d", t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min );
    Lputs( yx, sbuffer );
}

void	ShowTIME( uint16_t yx, uint16_t Time )
{
    CHAR	sbuffer[20];
    uint8_t	hour   = ( Time / 60u );
    uint8_t	minute = ( Time % 60u );

    sprintf( sbuffer, "%02u:%02u", hour, minute );
    Lputs( yx, sbuffer );
}

BOOL	EditTIME( uint16_t yx, uint16_t * pTime )
{
    uint8_t hour   = (( *pTime / 60u ) % 100u );
    uint8_t minute = (  *pTime % 60u );
    BOOL	changed = FALSE;
    uint8_t option = 1u;
		uint16_t gray  = Configure.DisplayGray;
		BOOL graychanged = FALSE;	

    do
    {
        ShowTIME( yx, (( hour * 60u ) + minute ));
        switch( option )
        {
        case 1:
            LcmMask( yx + 0u, 1u );
            break;  // 10 hour
        case 2:
            LcmMask( yx + 1u, 1u );
            break;  //  1 hour
        case 3:
            LcmMask( yx + 3u, 1u );
            break;  // 10 minute
        case 4:
            LcmMask( yx + 4u, 1u );
            break;  //  1 minute
        default:
            break;
        }
        switch( getKey() )
        {
        case K_INC:
            switch( option )
            {
            case 1:
                hour   = (  hour  + 10u ) % 100;
                break;
            case 2:
                hour   = (( hour  / 10u ) * 10u ) + (( hour   + 1u ) % 10u );
                break;
            case 3:
                minute = ( minute + 10u ) % 60u;
                break;
            case 4:
                minute = ((minute / 10u ) * 10u ) + (( minute + 1u ) % 10u );
                break;
            default:
                break;
            }
            break;
        case K_DEC:
            switch( option )
            {
            case 1:
                hour   = (  hour   + 90u ) % 100u;
                break;
            case 2:
                hour   = (( hour   / 10u ) * 10u ) + (( hour   + 9u ) % 10u );
                break;
            case 3:
                minute = (  minute + 50u ) % 60u;
                break;
            case 4:
                minute = (( minute / 10u ) * 10u ) + (( minute + 9u ) % 10u );
                break;
            default:
                break;
            }
            break;

        case K_RIGHT:
            if ( ++ option > 4u )
            {
                option = 1u;
            }
            break;

        case K_LEFT:
            if ( -- option < 1u )
            {
                option = 4u;
            }
            break;

        case K_OK:
            *pTime = ( hour * 60u ) + minute;
            changed = TRUE;
            option = 0u;
            break;

        case K_ESC:
            changed = FALSE;
            option = 0u;
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
    while ( 0u != option );

    return	changed;
}

///////////////////////////////////////////////////////////////////
static	uint32_t const	tbl_pow10_I32U[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
static	FP32	const	tbl_pow10_FP32[] = { 1E0, 1E1, 1E2, 1E3, 1E4, 1E5, 1E6, 1E7, 1E8, 1E9 };

void	ShowFP32( uint16_t yx, FP32   NumFP32, uint16_t fmt, const CHAR * Unit )
{
    CHAR	sbuffer[20];

    sprintf( sbuffer, "%*.*f", fmt / 0x100, fmt % 0x100, NumFP32 );
    Lputs( yx, sbuffer );
    if( Unit )
    {
        Lputs( yx + strlen( sbuffer ), Unit );
    }
}

void	ShowI16U( uint16_t yx, uint16_t NumI16U, uint16_t fmt, const CHAR * Unit )
{
    FP32	NumFP32 = NumI16U / tbl_pow10_FP32[ fmt % 0x100 ];
    CHAR	sbuffer[20];

    sprintf( sbuffer, "%*.*f", fmt / 0x100, fmt % 0x100, NumFP32 );
    Lputs( yx, sbuffer );
    if( Unit )
    {
        Lputs( yx + strlen( sbuffer ), Unit );
    }
}

void	ShowPercent( uint16_t yx, FP32 percent )
{
    ShowFP32( yx, percent * 100.0f, 0x0501u, "%" );
}

void	ShowHEX16( uint16_t yx, uint16_t hex16 )
{
    CHAR	sbuffer[10];

    sprintf( sbuffer, "%04X", hex16 );

    Lputs( yx, sbuffer );
}
//	#if 0
//		uint32_t  PowN = 0;
//		uint32_t  password = 0UL;
//		uint8_t  choice = 0U;						//	用户输入位置从0开始
//		uKey  keyin;
//		uint16_t  yx = 0x0305u;
//		CHAR	sbuffer[20];

//		cls();
//		Lputs( 0x0000U, "输入密码: " );

//		for (;;)
//		{
//			sprintf( sbuffer, "%06u", password );
//			Lputs( yx, sbuffer );

//			LcmMask( yx + choice, 1 );				//	移到光标到输入位置

//			keyin = getKey();

//			PowN = tbl_pow10_I32U[5-choice];

//			switch( keyin )
//			{
//	//			case K_RIGHT:
//	//				choice = ( choice + 1U ) % 6U;
//	//				break;
//	//			case K_LEFT:
//	//				choice = (( choice - 1U ) + 6U ) % 6U;
//	//				break;

//	//			case K_XCH:	 choice = (choice + 1) % 6;	break;

//			case K_INC:
//				if ( password / PowN % 10 == 9 )
//				{
//					password -= PowN * 9;
//				}
//				else
//				{
//					password += PowN;
//				}
//				break;

//			case K_DEC:
//				if ( password / PowN % 10 == 0 )
//				{
//					password += PowN * 9;
//				}
//				else
//				{
//					password -= PowN;
//				}
//				break;

//			case K_RIGHT:
//				choice = ( choice + 1 ) % 6;
//				break;

//			case K_LEFT:
//				choice = ( choice + 6 - 1 ) % 6;
//				break;

//			case K_OK:
//				return  password;

//			case K_ESC:
//				return  0;

//			default:
//				break;
//			}
//		}
//	#endif

	
///////////////////////////////////////////////////////////////////
BOOL	EditI32U( uint16_t yx, uint32_t * pNUM, uint16_t fmt )
{
    uint32_t	PowN;
    uint32_t	Num = *pNUM;
    uint8_t 	M, N, choice = 0u;
    CHAR		sbuffer[20];
		uint16_t gray  = Configure.DisplayGray;
		BOOL graychanged = FALSE;	

    N = fmt % 0x100u;										//	小数点后的有效位数
    M = ( N == 0 ) ? ( fmt / 0x100 ) : ( fmt / 0x100 - 1 );	//	有效位数, 即choice 变化范围
    Num = Num % tbl_pow10_I32U[M];							//	保留低位, 清零高位

    for (;;)
    {
        sprintf( sbuffer, "%0*.*f", ( fmt / 0x100 ), ( fmt % 0x100 ), Num / tbl_pow10_FP32[N] );
        Lputs( yx, sbuffer );

			if ( choice < ( M - N ))
			{
						LcmMask( yx + choice, 1 );		//	输入位置在小数点之前
			}
			else
			{
            LcmMask( yx + choice + 1, 1 );	//	输入位置在小数点之后
			}

        if (  M >= ( choice + 1u ))
        {
            PowN = tbl_pow10_I32U[ M - ( choice + 1u )];
        }
        else
        {
            PowN = 1u;
        }

        switch ( getKey() )
        {
        case K_INC:
            if( Num / PowN % 10 == 9 )
            {
                Num -= PowN * 9;
            }
            else
            {
                Num += PowN;
            }
            break;
        case K_DEC:
            if( Num / PowN % 10 == 0 )
            {
                Num += PowN * 9;
            }
            else
            {
                Num -= PowN;
            }
            break;
        case K_RIGHT:
        //case K_XCH:
            choice = ( choice + 1 ) % M;
            break;

        case K_LEFT:
            choice = ( choice + M - 1 ) % M;
            break;

        case K_OK:
            *pNUM = Num;
            return TRUE;
        case K_ESC:
            return FALSE;
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

BOOL	EditI16U( uint16_t yx, uint16_t * pNUM, uint16_t fmt )
{
	uint32_t	Num = *pNUM;
	
	if ( ! EditI32U( yx, &Num, fmt ))
	{
		return  false;
	}
	
	*pNUM = (uint16_t)Num;
	return  true;	
}

///////////////////////////////////////////////////////////////////
// 输入密码
///////////////////////////////////////////////////////////////////

uint32_t	InputPassword( void )
{
	uint32_t	password = 0u;
	
	cls();
	Lputs( 0x0000u, "输入密码:" );
	
	if ( ! EditI32U( 0x0305u, &password, 0x0600u ))
	{
		return NULL;
	}
	return  password;
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
