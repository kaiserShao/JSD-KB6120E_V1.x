/**************** (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司 ****************
* 文 件 名: MBM_IO.C
* 创 建 者: 董峰
* 描  述  : 根据初始化设置，转发MODBUS操作到选定的通讯方式。
* 最后修改: 2013年12月28日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "MBM_BSP.H"
#include "MBM.H"
#include "MBM_Port.H"

static	eMBMode eCurrentMode;

/********************************** 功能说明 ***********************************
*  将接收到的从机异常响应代码转换成错误信息代码
*******************************************************************************/
static	eMBErrorCode	Exception2MBError( uint8_t ExceptionCode )
{
	static	eMBErrorCode const ExceptionList[] = 
	{
		MB_EX_Unknow,
		MB_EX_ILLEGAL_FUNCTION,         //  Illegal function exception.              01号异常，非法功能
		MB_EX_ILLEGAL_DATA_ADDRESS,     //  Illegal data address.                    02号异常，非法数据地址
		MB_EX_ILLEGAL_DATA_VALUE,       //  Illegal data value.                      03号异常，非法数据值
		MB_EX_SLAVE_DEVICE_FAILURE,     //  Slave device failure.                    04号异常，从站设备故障
		MB_EX_ACKNOWLEDGE,              //  Slave acknowledge.                       05号异常，确认
		MB_EX_SLAVE_BUSY,               //  Slave device busy.                       06号异常，从属设备忙
		MB_EX_Unknow,
		MB_EX_MEMORY_PARITY_ERROR,      //  Memory parity error.                     08号异常，存储奇偶性差错
		MB_EX_Unknow,		
		MB_EX_GATEWAY_PATH_UNAVAILABLE, //  Gateway path unavailable.                0A号异常，不可用网关路径
		MB_EX_GATEWAY_TARGET_FAILED,   	//  Gateway target device failed to respond. 0B号异常，网关目标设备响应失败
	};

	eMBErrorCode eStatus = MB_EX_Unknow;
	
	if ( ExceptionCode < sizeof( ExceptionList ) / sizeof(ExceptionList[0]))
	{
		eStatus = ExceptionList[ExceptionCode];
	}
	return	eStatus;
}

/********************************** 功能说明 ***********************************
	MBM 通讯
	主机与从机通讯，返回通讯状态
	正常，指定从机的正常响应，
	异常，指定从机的异常响应，其中包括与请求功能不一致的正常响应
	超时，指定从机在规定的时间内无响应，其他从机的响应全部忽略
*******************************************************************************/
eMBErrorCode	eMBM_IO( uint8_t ucSlaveAddress, uint16_t * pusLen )
{
	const	uint8_t	FunctionCode = ucModbusPDU[MB_PDU_FUNC_OFF];	//	保存请求的功能码
	uint8_t ucRcvSlaveAddress = 0u;
	eMBErrorCode	eStatus;
	uint16_t	usLen = * pusLen;

	switch ( eCurrentMode )
	{
	case MB_RTU:
		eStatus = eMBM_RTU_Assemble( ucSlaveAddress, usLen );		//	装配SerialPDU
		break;
	default:
		eStatus = MB_EINVAL;
	}
	
	if ( MB_ENOERR != eStatus )
	{
		return	eStatus;		//	装配出错，返回装配时检出的错误
	}
	
	//	等待命令发送完成
	vMBM_RTU_Send_Init();										//	允许Serial发送
	if ( ! xMBM_EventPut_Poll())								//	等待Serila发送
	{
		return	MB_EILLSTATE;	//	等待发送时检出错误，返回 移植出错
	}
	
	if ( MB_ADDRESS_BROADCAST == ucSlaveAddress )
	{
		return	MB_ENOERR;
	}
	
	//	等待应答接收完成
	vMBM_RTU_Receive_Init( );									//	允许Serial接收
	if ( ! xMBM_EventGet_Poll())								//	等待Serial接收
	{
		return	MB_ETIMEDOUT;	//	等待接收时检出错误，返回 超时
	}

	switch ( eCurrentMode )
	{
	case MB_RTU:
		eStatus = eMBM_RTU_Analyze( &ucRcvSlaveAddress, &usLen );	//	解析SerialPDU
		break;
	default:
		eStatus = MB_EINVAL;
	}

	if ( MB_ENOERR != eStatus )
	{
		return	eStatus;		//	接收出错，返回接收时检出的错误，如 校验错
	}
	
	//	以下是对接收到的ModbusPDU进行一些解析，以期检出一些公共的错误。

	//	非期望从机响应
	if ( ucRcvSlaveAddress != ucSlaveAddress )
	{
		return	MB_EX_Response_IllegalSlave;
	}

	//	非期望请求响应
	if ( FunctionCode != ( ucModbusPDU[MB_PDU_FUNC_OFF] & 0x7Fu ))
	{
		return	MB_EX_Response_IllegalFunction;
	}

	//	异常响应
	if ( MB_FUNC_ERROR & ucModbusPDU[MB_PDU_FUNC_OFF] )
	{	//	解析异常响应代码
		eStatus = Exception2MBError( ucModbusPDU[MB_PDU_DATA_OFF] );
		return	eStatus;
	}

	* pusLen = usLen;

	return	MB_ENOERR;
}


/********************************** 功能说明 ***********************************
*  MBM 初始化
*******************************************************************************/
eMBErrorCode	eMBM_IO_Init( eMBMode eMode, uint32_t ulBaudRate, eMBParity eParity )
{
	eMBErrorCode    eStatus = MB_ENOERR;
    uint32_t		usTimerT35;

	eCurrentMode = eMode;
	
	switch ( eMode )
	{
	case MB_RTU:
		/* Modbus RTU uses 8 Databits. */
		vMBM_Serial_Init( ulBaudRate, 8u, eParity );
		/* If baudrate > 19200 then we should use the fixed timer values
		 * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
		 */
		if ( ulBaudRate > 19200u )
		{
			usTimerT35 = 1750u;
		}
		else
		{
			usTimerT35 = (uint32_t)( 1000000 * 11 * 3.5f ) / ulBaudRate ;
		}
        vMBM_Timers_Init( ( uint16_t ) usTimerT35 );
		break;

	default:
	case MB_ASCII:
	case MB_TCP:
		eStatus = MB_EINVAL;
		break;
	}

	if ( MB_ENOERR == eStatus )
	{
		vMBM_Event_Init();
	}

	return eStatus;
}

/********  (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司  **** End Of File ****/
