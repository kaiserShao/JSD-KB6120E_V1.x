/**************** (C) COPYRIGHT 2013 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: EE24.C
* �� �� ��: ����
* ��  ��  : ��д24ϵ�е�EEPROM������ֻ�ṩ�����ķ��ʽӿڡ�
* ����޸�: 2013��5��22��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"

////////////////////////////////////////////////////////////////////////////////
//	��ѯ������ȷ��д������ɡ�
////////////////////////////////////////////////////////////////////////////////
static	BOOL	polling( uint8_t SlaveAddress  )
{
	uint16_t	iRetry;
	
	/*	����������400Kʱ, 10msʱ����෢��400����ַ�ֽ� */
	for ( iRetry = 400U; iRetry != 0x00U; --iRetry )
	{
		if ( bus_i2c_start( SlaveAddress, I2C_Write ))
		{
			bus_i2c_stop();
			return	TRUE;
		}
	}
	return	FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//	����ֻ�ṩ�����ķ��ʽӿڡ�
////////////////////////////////////////////////////////////////////////////////
BOOL	EE24C512_Save( uint16_t address, uint8_t const * buffer, uint8_t count )
{
	// send sub address
	if ( ! bus_i2c_start( _SLAVE_24C512, I2C_Write ))   { bus_i2c_stop(); return FALSE; }
	if ( ! bus_i2c_shout((uint8_t)( address / 0x100U ))){ bus_i2c_stop(); return FALSE; }
	if ( ! bus_i2c_shout((uint8_t)( address % 0x100U ))){ bus_i2c_stop(); return FALSE; }

	// continue send write data.
	do
	{
		if ( ! bus_i2c_shout((uint8_t)~(*buffer++))){	bus_i2c_stop(); return FALSE; 	}
	}
	while ( --count );

	bus_i2c_stop();

    // acknowledge polling.
	return	polling( _SLAVE_24C512 );	//	ʹ������洢��ʱ�����Բ�����ѯ, ������Ҳ����ν��
}

BOOL	EE24C512_Load( uint16_t address, uint8_t * buffer, uint8_t count )
{
	// send sub address
	if ( ! bus_i2c_start( _SLAVE_24C512, I2C_Write ))   { bus_i2c_stop(); return FALSE; }
	if ( ! bus_i2c_shout((uint8_t)( address / 0x100U ))){ bus_i2c_stop(); return FALSE; }
	if ( ! bus_i2c_shout((uint8_t)( address % 0x100U ))){ bus_i2c_stop(); return FALSE; }

	// Send read command and receive data.
	if ( ! bus_i2c_start( _SLAVE_24C512, I2C_Read ))	{ bus_i2c_stop(); return FALSE; }
	while ( --count )
	{
		*buffer++ =  (uint8_t)~bus_i2c_shin( I2C_ACK );	// Receive and send ACK
	}
	*buffer =  (uint8_t)~bus_i2c_shin( I2C_NoACK );		// Receive and send NoACK
	bus_i2c_stop();

	return TRUE;
}

/********  (C) COPYRIGHT 2013 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/