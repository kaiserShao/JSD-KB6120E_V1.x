/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: Display.C
* �� �� ��: ����
* ��  ��  : ʹ�ò�ͬ�ӿ���ʽ��ģ��ʵ�ֵ�����ʾ����
* ����޸�: 2014��4��18��
*********************************** �޶���¼ ***********************************
* ��  ��: V2.0
* �޶���: ���� 2014��4��18��
* ˵  ��: �Զ����ȷ����ʾ�������͡�
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"

static	enum	enumDisplayModelType
{
	enumOLED9704,
	enumTM12864,
} DisplayModelType;

/********************************** ����˵�� ***********************************
*	Һ��(TM12864)�ı��⡢�Ҷȿ���
*******************************************************************************/
static	void	TM12864_SetLight( uint8_t setLight )
{	//	��������������������
	PWM1_SetOutput( setLight * PWM_Output_Max / 100u );
}

FP32	LCDSetGrayVolt;

/********************************** ����˵�� ***********************************
 *	������ʾ���ȡ��Ҷ�
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
{	//	��������
	savLight = ValueOfBrightness;

	Backlight_ON();
}

void	DisplaySetGrayVolt( FP32 SetGrayVolt )
{	//	�Ҷ�  �� �Աȶ�

	switch ( DisplayModelType )
	{
	default:
	case  enumOLED9704:	break;
	case  enumTM12864:	
		LCDSetGrayVolt = SetGrayVolt;
		break;
	}
}

/********************************** ����˵�� ***********************************
 *	������ʾ����
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

/********************************** ����˵�� ***********************************
 *	��ʾģ���ʼ��
*******************************************************************************/
void	Display_Init( void )
{
	//	�Զ������ʾ��������
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

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/