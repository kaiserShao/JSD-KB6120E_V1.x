/**************** (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司 ****************
* 文 件 名: MBM.H
* 创 建 者: 董峰
* 描  述  : MODBUS 主节点说明文件
* 最后修改: 2013年12月28日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
	MB_RTU,					 		/*!< RTU transmission mode. */
	MB_ASCII,				   		/*!< ASCII transmission mode. */
	MB_TCP					  		/*!< TCP mode. */
} eMBMode;

typedef enum
{
	MB_PAR_NONE,					/*!< No parity. */
	MB_PAR_ODD,				 		/*!< Odd parity. */
	MB_PAR_EVEN				 		/*!< Even parity. */
} eMBParity;

typedef	enum
{
	MB_ENOERR,						//  No error.  
	MB_EPORTERR,					//  Porting layer error.  
	MB_EINVAL,						//	Illegal argument.  
	MB_EILLSTATE,					//  Protocol stack in illegal state.  
	MB_ETIMEDOUT,					//  Timeout error occurred.
	MB_EIO,							//  I/O error.（一般指通讯校验错误）

	MB_EX_Response_Unknow = 0x60u,	//	未知的响应信息错误
	MB_EX_Response_IllegalSlave,	//	非期望从机地址响应
	MB_EX_Response_IllegalFunction,	//	非期望请求功能响应
	MB_EX_Response_IllegalLength,	//	应答帧的长度与期望不一致
	MB_EX_Response_IllegalByteCnt,  //	读指令返回的数据量与请求不一致
	MB_EX_Response_IllegalAddress,	//	写指令返回的地址与请求不一致
	MB_EX_Response_IllegalValue,	//	写指令返回的数据值与请求不一致
	MB_EX_Response_IllegalCount,	//	写指令返回的数据量与请求不一致

	MB_EX_Unknow = 0x80u,    		//	未知的异常
	MB_EX_ILLEGAL_FUNCTION,         //  Illegal function exception.              01号异常，非法功能
	MB_EX_ILLEGAL_DATA_ADDRESS,     //  Illegal data address.                    02号异常，非法数据地址
	MB_EX_ILLEGAL_DATA_VALUE,       //  Illegal data value.                      03号异常，非法数据值
	MB_EX_SLAVE_DEVICE_FAILURE,     //  Slave device failure.                    04号异常，从站设备故障
	MB_EX_ACKNOWLEDGE,              //  Slave acknowledge.                       05号异常，确认
	MB_EX_SLAVE_BUSY,               //  Slave device busy.                       06号异常，从属设备忙
	MB_EX_MEMORY_PARITY_ERROR,      //  Memory parity error.                     08号异常，存储奇偶性差错
	MB_EX_GATEWAY_PATH_UNAVAILABLE, //  Gateway path unavailable.                0A号异常，不可用网关路径
	MB_EX_GATEWAY_TARGET_FAILED,   	//  Gateway target device failed to respond. 0B号异常，网关目标设备响应失败
} eMBErrorCode ;

/*	初始化MBUS	*/
eMBErrorCode	eMBMInit( eMBMode eMode, uint32_t ulBaudrate, eMBParity eParity );

/* FC01 Read Coils */
eMBErrorCode	eMBMReadCoils( uint8_t Slave, uint16_t Start, uint16_t Count, uint8_t Coils[] );

/* FC02 Read Discretes */
eMBErrorCode	eMBMReadDiscretes( uint8_t Slave, uint16_t Start, uint16_t Count, uint8_t Discretes[] );

/* FC03 Read holding registers */
eMBErrorCode	eMBMReadHoldingRegisters( uint8_t Slave, uint16_t Start, uint16_t Count, uint16_t HoldingRegisters[] );

/* FC04 Read Input Registers */
eMBErrorCode	eMBMReadInputRegisters( uint8_t Slave, uint16_t Start, uint16_t Count, uint16_t InputRegisters[] );

/* FC05	Write Signle Coil */
eMBErrorCode	eMBMWriteSingleCoil( uint8_t Slave, uint16_t Start, BOOL CoilVal );

/* FC06 Write Signle Register */
eMBErrorCode	eMBMWriteSingleRegister( uint8_t Slave, uint16_t Start, uint16_t RegisterVal );

/* FC15	Write Multiple Coil */
eMBErrorCode	eMBMWriteMultipleCoils( uint8_t Slave, uint16_t Start, uint16_t Count, const uint8_t CoilsVal[] );

/* FC16 Write Multiple Registers */
eMBErrorCode	eMBMWriteMultipleRegisters( uint8_t Slave, uint16_t Start, uint16_t Count, const uint16_t RegistersVal[] );

/*
	FC01/FC02	Read DO/DI
	FC03/FC04	Read AO/AI
*/
eMBErrorCode	eMBMRead ( uint8_t SlaveAddress, uint16_t Register, uint16_t Count,	void * pBuf );
/*
	FC15/FC16	Write DO/AO
*/
eMBErrorCode	eMBMWrite( uint8_t SlaveAddress, uint16_t Register, uint16_t Count, const void * pBuf );

#ifdef	__cplusplus
}
#endif
/********  (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司  **** End Of File ****/
