/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: Display.C
* 创 建 者: 董峰
* 描  述  : 使用不同接口形式的模块实现点阵显示功能
* 最后修改: 2014年4月18日
*********************************** 修订记录 ***********************************
* 版  本: V2.0
* 修订人: 董峰 2014年4月18日
* 说  明: 自动检测确定显示屏的类型。
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"

static	enum	enumDisplayModelType
{
	enumOLED9704,
	enumTM12864,
} DisplayModelType;

/********************************** 功能说明 ***********************************
*	液晶(TM12864)的背光、灰度控制
*******************************************************************************/
static	void	TM12864_SetLight( uint8_t setLight )
{	//	调整电流调整背光亮度
	PWM1_SetOutput( setLight * PWM_Output_Max / 100u );
}

FP32	LCDSetGrayVolt;

/********************************** 功能说明 ***********************************
 *	设置显示亮度、灰度
*******************************************************************************/
static	uint8_t	savLight;

void	Backlight_ON( void )
{
	switch ( DisplayModelType )
	{
	default:
	case enumOLED9704:	OLED9704_SetLight( savLight );	break;
	case  enumTM12864:	 TM12864_SetLight( savLight );	break;
	}
}

void	Backlight_OFF( void )
{
	switch ( DisplayModelType )
	{
	default:
	case enumOLED9704:	OLED9704_SetLight( 0u );	break;
	case  enumTM12864:	 TM12864_SetLight( 0u );	break;
	}
}

void	DisplaySetLight( uint8_t ValueOfBrightness )
{	//	背光亮度
	savLight = ValueOfBrightness;

	Backlight_ON();
}

void	DisplaySetGrayVolt( FP32 SetGrayVolt )
{	//	灰度  或 对比度

	switch ( DisplayModelType )
	{
	default:
	case  enumOLED9704:	break;
	case  enumTM12864:	
		LCDSetGrayVolt = SetGrayVolt;
		break;
	}
}

/********************************** 功能说明 ***********************************
 *	点阵显示功能
*******************************************************************************/
void	cls ( void )
{
	switch ( DisplayModelType )
	{
	case enumTM12864:	 TM12864_cls();	break;
	default:
	case enumOLED9704:	OLED9704_cls();	break;
	}
}

void	Lputs( uint16_t yx, const CHAR * sz )
{
	switch ( DisplayModelType )
	{
	case enumTM12864:	 TM12864_puts ( yx, sz );	break;
	default:
	case enumOLED9704:	OLED9704_puts ( yx, sz );	break;
	}
}

void	LcmMask( uint16_t yx, uint8_t xlen )
{
	switch ( DisplayModelType )
	{
	case enumTM12864:	 TM12864_mask ( yx, xlen );	break;
	default:
	case enumOLED9704:	OLED9704_mask ( yx, xlen );	break;
	}
}

/********************************** 功能说明 ***********************************
 *	显示模块初始化
*******************************************************************************/
void	Display_Init( void )
{
	//	自动检测显示屏的类型
	if ( TM12864_Test() )
	{
		DisplayModelType = enumTM12864;
		TM12864_Init();
	}
	else 
	{
		DisplayModelType = enumOLED9704;
		OLED9704_Init();
	}
}

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
