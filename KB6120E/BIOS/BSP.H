//lint	-w2
//lint	-elib(18)	阻止报告 string.h 中的符号重复定义
//lint	-e578	Declaration of symbol 'Symbol' hides symbol 'Symbol' (Location)
//lint	-e641	Converting enum to int
//lint	-e534	Warning 534: Ignoring return value of function 'Symbol'
//lint	-e524	Loss of precision (Context) (Type to Type)
//lint  -e714 -e765 -e737 -e835
//lint	+d__svc_indirect=_to_brackets

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define	BOOL	bool
#define	TRUE	true
#define	FALSE	false

typedef	float	FP32;
typedef	char	CHAR;

#define __task          // __declspec(noreturn)
#define __used          // __attribute__((used))

//	BIOS.C
extern	void	BIOS_Init( void );

extern	void	beep( void );
extern	void	tick( void );

extern	void	MeasureBattery_OutCmd( BOOL NewState );
extern	void	AIRLightOutCmd( BOOL NewState );
extern	void	delay( uint16_t ms );	//	!!具体实现方式取决于是否使用RTOS!!

extern	void	delay_us ( uint32_t us );	
extern	void	PumpAIR_OutCmd( BOOL NewState );
extern	BOOL	PumpAIR_GetOutState( void );

extern	void	TIM2_Init( void );
extern	void	TIM2_OutCmd( BOOL NewState );
extern	void	TIM2_SetOutput( uint16_t OutValue );

enum	enumADC12_Channel
{
	ADC_Cx_IntTemp,
	ADC_Cx_IntVolt,
	ADC_Cx_LCDVolt,	//	ADC_Cx_NTC10K,
	ADC_Cx_BatCurr,	
	ADC_Cx_BatVolt,
};
extern	void	ADC12_Init( void );
extern	uint16_t	ADC12_Readout( enum enumADC12_Channel ADC_Channel );

// //	BIOS - HCBox
// extern	void	HCBoxCool_OutCmd( BOOL NewState );
// extern	void	HCBoxHeat_OutCmd( BOOL NewState );
// extern	void	HCBoxFan_OutCmd( BOOL NewState );
// extern	void	HCBoxFan_SetOutput( uint16_t NewValue );

extern	void		HCBoxFan_Circle_PortInit( void );
extern	uint16_t	HCBoxFan_Circle_Read( void );

//	Keyboard.C
typedef	uint8_t	uKey;

#define	_Key1	0x01u
#define	_Key2	0x02u
#define	_Key3	0x04u
#define	_Key4	0x08u
#define	_Key5	0x10u
#define	_Key6	0x20u
#define	_Key7	0x40u
#define	_Key8	0x80u

//	#define	K_UP	_Key1
//	#define	K_LEFT	_Key2
//	#define	K_DOWN	_Key3
//	#define	K_RIGHT	_Key4

//	#define	K_OK	_Key5
//	#define	K_ESC	_Key7

//	#define	K_SHIFT	_Key6

#define	K_OK	_Key1
#define	K_SHIFT	_Key2
#define	K_ESC	_Key3

#define	K_RIGHT	_Key4
#define	K_UP	_Key5
#define	K_LEFT	_Key6
#define	K_DOWN	_Key7

#define	K_OK_UP 		( K_OK | K_UP )
#define	K_OK_DOWN 	( K_OK | K_DOWN )
#define	K_OK_LEFT		( K_OK | K_LEFT )
#define	K_OK_RIGHT	( K_OK | K_RIGHT )

#define	K_ESC_SHIFT ( K_ESC | K_SHIFT )

//	Keyboard.C
extern	void	Keyboard_Init( void );

extern	uKey	getKey( void );
extern	BOOL	hitKey( uint16_t iRetry );
extern	BOOL	releaseKey( uKey Key, uint16_t iRetry );

//	Display.C
extern	void	Display_Init( void );

extern	void	cls ( void );
extern	void	Lputs( uint16_t yx, const CHAR * sz );
extern	void	LcmMask( uint16_t yx, uint8_t xlen );
extern	void	Lmask( uint16_t yx, uint8_t xlen );

//	背光开、关、状态切换
extern	void	Backlight_ON( void );
extern	void	Backlight_OFF( void );
extern	void	Backlight_XCH( void );

//	AD7705.C
extern	BOOL		Initialize7705( void );
extern	void		Convert7705( uint8_t chs );
extern	uint16_t	Readout7705( uint8_t chs );

//	CSP120.C EE24.C
extern	void	bus_i2c_mutex_apply( void );
extern	void	bus_i2c_mutex_release( void );
extern	void	bus_SPI_mutex_apply( void );
extern	void	bus_SPI_mutex_release( void );
extern	BOOL	CPS120_Read ( uint16_t * pBa, uint16_t * pTemp );

extern	BOOL	EE24C512_Save( uint16_t address, uint8_t const * ram, uint8_t count );
extern	BOOL	EE24C512_Load( uint16_t address, uint8_t       * ram, uint8_t count );

//	DS18B20.C
extern	struct OW_DRV const DS18B20_TSPTr;
extern	struct OW_DRV const DS18B20_HCBox;

BOOL	DS18B20_Read( struct OW_DRV const * OW, int16_t * pT16S );

//	Clock.C
extern	uint32_t	get_Now( void );

typedef	uint32_t	uClock;

extern	BOOL	RTC_Init( void );
extern	void	RTC_Save( uClock const * pClock );
extern	void	RTC_Load( uClock       * pClock );

//	Printer.C
extern	BOOL	PrinterInit( BOOL (*fCallback)( void ));
extern	void	PrinterPutString( const CHAR * sz );


extern	void	DisplaySetGrayVolt( FP32 SetGrayVolt );
extern	void	DisplaySetLight( uint8_t SetLight );
extern	void	DisplaySetTimeout( uint8_t SetTimeout );

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End Of File ****/
