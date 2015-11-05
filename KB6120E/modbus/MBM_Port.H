/**************** (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司 ****************
* 文 件 名: MBM_Port.H
* 创 建 者: 董峰
* 描  述  : MODBUS 主节点 移植说明文件
* 最后修改: 2013年12月28日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
/* ----------------------- Defines ------------------------------------------*/
#define MB_ADDRESS_BROADCAST	( 0 )   /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN			( 1 )   /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX			( 247 ) /*! Biggest possible slave address. */

#define MB_FUNC_NONE							(  0 )
#define MB_FUNC_READ_COILS						(  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS			(  2 )
#define MB_FUNC_WRITE_SINGLE_COIL				(  5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS			( 15 )
#define MB_FUNC_READ_HOLDING_REGISTER			(  3 )
#define MB_FUNC_READ_INPUT_REGISTER				(  4 )
#define MB_FUNC_WRITE_REGISTER					(  6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS		( 16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS	( 23 )
#define MB_FUNC_DIAG_READ_EXCEPTION				(  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC					(  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT			( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG			( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID			( 17 )
#define MB_FUNC_ERROR							( 128 )

#define MB_PDU_FUNC_READ_COILCNT_MAX			( 0x07D0 )
#define MB_PDU_FUNC_READ_DISCCNT_MAX			( 0x07D0 )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX		( 0x07B0 )
#define MB_PDU_FUNC_READ_REGCNT_MAX		 		( 0x007D )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX		( 0x0078 )

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

#define MB_PDU_FUNC_OFF			0		/*!< Offset of function code in PDU. */
#define MB_PDU_DATA_OFF	 		1		/*!< Offset for response data in PDU. */

#define MB_PDU_RESPONSE_WRITE_SIZE 			( 5 )	//	功能码1字节+写地址2字节+写数据(或写数量)2字节
#define	MB_PDU_RESPONSE_READ_SIZE			( 2 )	//	功能码1字节+字节数1字节+接收数据内容的长度
#define	MB_PDU_RESPONSE_READ_BYTECNT_OFF	( MB_PDU_DATA_OFF )
#define	MB_PDU_RESPONSE_READ_VALUES_OFF		( MB_PDU_DATA_OFF + 1 )

/* ----------------------- Defines ------------------------------------------*/
extern	uint8_t		ucSerialPDU[MB_SER_PDU_SIZE_MAX];
#define	ucModbusPDU	(ucSerialPDU+MB_SER_PDU_PDU_OFF)

#define	MAKE16(_b0,_b1)	((uint16_t)((_b0)+((_b1)*256)))		//	低位在前
#define	LOBYTE(__w)		((uint8_t)((__w)&0x0FF))
#define	HIBYTE(__w)		((uint8_t)(((__w)>>8)&0x0FF))
#define	HowBytes(__b)	(((__b)+7)/8u)

/* ----------------------- Supporting functions -----------------------------*/
extern	void	vMBM_Event_Init( void );
extern	BOOL	xMBM_EventPut_Poll( void );
extern	void	vMBM_EventPut_Post( void );
extern	BOOL	xMBM_EventGet_Poll( void );
extern	void	vMBM_EventGet_Post( void );

/* ----------------------- Serial port functions ----------------------------*/
extern	void	vMBM_Serial_Init( uint32_t ulBaudRate, uint8_t ucDataBits, eMBParity eParity );
extern	void	vMBM_SerialPut_Cmd( BOOL xTxEnable );
extern	void	vMBM_SerialPutByte( uint8_t ucByte );
extern	void	vMBM_SerialGet_Cmd( BOOL xRxEnable );
extern	BOOL	xMBM_SerialGetByte( uint8_t * pucByte );

/* ----------------------- Timers functions ---------------------------------*/
extern	void	vMBM_Timers_Init( uint16_t u_sec );
extern	void	vMBM_Timers_Cmd( BOOL NewState );

/* ----------------------- Tools functions ----------------------------------*/
extern	void	vMBus_Memory_Put( uint8_t * pucMBusPDU, const uint16_t * pRegistersVal, uint8_t usRegCount );
extern	void	vMBus_Memory_Get( uint16_t * pInputRegisters, const uint8_t * pucMBusPDU, uint8_t usRegCount );

extern	void	vMBusLogError( eMBErrorCode	eStatus );

extern	void	vMBus_Mutex_Init( void );
extern	void	vMBus_apply( void );
extern	void	vMBus_release( void );

//	MODBUS 中间层
extern	eMBErrorCode	eMBM_IO( uint8_t SlaveAddress, uint16_t * pusLen );
extern	eMBErrorCode	eMBM_IO_Init( eMBMode eMode, uint32_t ulBaudRate, eMBParity eParity );

//	RTU 模式接口
extern	void			vMBM_RTU_Send_Init( void );
extern	eMBErrorCode	eMBM_RTU_Assemble( uint8_t ucSlaveAddress, uint16_t usLen );
extern	void			vMBM_RTU_Receive_Init( void );
extern	eMBErrorCode	eMBM_RTU_Analyze( uint8_t * pucRcvAddress, uint16_t * pusLen );

extern	void			vMBM_RTU_T35_ISR( void );
extern	void			vMBM_RTU_Send_ISR( void );
extern	void			vMBM_RTU_Receive_ISR( void );

/********  (C) COPYRIGHT 2013 青岛金仕达电子科技有限公司  **** End Of File ****/
