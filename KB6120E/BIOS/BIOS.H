//	BIOS.C - Keyboard Port
extern	void	Keyboard_PortInit( void );
extern	uint8_t	Keyboard_PortRead( void );

//	BIOS.C - Keyboard ISR
extern	void	Keyboard_IRQ_Enable( void );
extern	void	Keyboard_IRQ_Disable( void );

//	BIOS.C - SPI1x
extern	uint8_t	bus_SPI1xShift( uint8_t OutByte );
extern	void	bus_SPI1xPortInit( void );

//	BIOS.C - I2C
#define	_SLAVE_24C512	0xA0u
#define	_SLAVE_CPS120	0x50u

enum	I2C_DirectSet { I2C_Write, I2C_Read };
enum	I2C_AcknowlegeSet { I2C_ACK, I2C_NoACK };
extern	BOOL	bus_i2c_start( uint8_t Address8Bit, enum I2C_DirectSet DirectSet );
extern	void	bus_i2c_stop( void );
extern	BOOL	bus_i2c_shout( uint8_t OutByte );
extern	uint8_t	bus_i2c_shin( enum I2C_AcknowlegeSet AcknowlegeSet );

//	BIOS.C - OneWire ( DS18B20 )
extern	BOOL	OW_0_Init( void );
extern	BOOL	OW_0_Reset( void );
extern	BOOL	OW_0_Slot( BOOL bitOut );

extern	BOOL	OW_1_Init( void );
extern	BOOL	OW_1_Reset( void );
extern	BOOL	OW_1_Slot( BOOL bitOut );

//	BIOS.C - Backup Registers
#define BKP_DR1 	((uint16_t)0x0004)
#define BKP_DR2 	((uint16_t)0x0008)
#define BKP_DR3 	((uint16_t)0x000C)
#define BKP_DR4 	((uint16_t)0x0010)
#define BKP_DR5 	((uint16_t)0x0014)
#define BKP_DR6 	((uint16_t)0x0018)
#define BKP_DR7 	((uint16_t)0x001C)
#define BKP_DR8 	((uint16_t)0x0020)
#define BKP_DR9 	((uint16_t)0x0024)
#define BKP_DR10	((uint16_t)0x0028)

extern	void		BackupRegister_Write( uint16_t BKP_DR, uint16_t uiData );
extern	uint16_t	BackupRegister_Read ( uint16_t BKP_DR );

//	BIOS.C - USART1
extern	BOOL	UART1_RX_Pin_State( void );
extern	void	UART1_Send( uint8_t OutChar );
extern	void	UART1_Init( void );

extern	void	TRIAC_Init( void );
extern	void	TRIAC_Shut( void );
extern	void	TRIAC_SetTriggerOutput( uint16_t TriggerDelay );

//	BIOS.C - PWM ( use TIM3 )
#define	PWM_Output_Max	27648u
extern	void	PWM1_SetOutput( uint16_t OutValue );
extern	void	PWM2_SetOutput( uint16_t OutValue );
extern	void	PWM3_SetOutput( uint16_t OutValue );
extern	void	PWM4_SetOutput( uint16_t OutValue );

//	BIOS.C - GPIO
extern	void	HCBoxFan_ON( void );
extern	void	HCBoxFan_OFF( void );
extern	void	HCBoxHeat_ON( void );
extern	void	HCBoxHeat_OFF( void );
extern	void	HCBoxCool_ON( void );
extern	void	HCBoxCool_OFF( void );
extern	BOOL	bus_SPI1xDRDY( void );
extern	void	bus_SPI1xNSS( BOOL NewState );

//	CGROM.C
typedef	uint8_t const * CGROM;
extern	CGROM	DotSeekDBC( int8_t cc );
extern	CGROM	DotSeekSBC( int8_t cc0, int8_t cc1 );

//	OLED9704.C
extern	void	OLED9704_Init( void );
extern	void	OLED9704_cls( void );
extern	void	OLED9704_puts( uint16_t yx, const CHAR * sz );
extern	void	OLED9704_mask( uint16_t yx, uint8_t xlen );
extern	void	OLED9704_SetLight( uint8_t SetLight );

//	 TM12864.C
extern	void	TM12864_Init( void );
extern	void	TM12864_cls( void );
extern	void	TM12864_puts( uint16_t yx, const CHAR * sz );
extern	void	TM12864_mask( uint16_t yx, uint8_t xlen );
extern	BOOL	TM12864_Test( void );
