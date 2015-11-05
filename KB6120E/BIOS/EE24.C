/**************** (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司 ****************
* 文 件 名: EE24.C
* 创 建 者: 董峰
* 描  述  : 读写24系列的EEPROM器件。只提供基本的访问接口。
* 最后修改: 2013年5月22日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"

////////////////////////////////////////////////////////////////////////////////
//	轮询器件以确定写操作完成。
////////////////////////////////////////////////////////////////////////////////
static	BOOL	polling( uint8_t SlaveAddress  )
{
	uint16_t	iRetry;
	
	/*	在总线速率400K时, 10ms时间最多发送400个地址字节 */
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
//	对外只提供基本的访问接口。
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
	return	polling( _SLAVE_24C512 );	//	使用铁电存储器时，可以不用轮询, 但加上也无所谓。
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

/********  (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司  **** End Of File ****/
