///////////////////////////////////////////////////////////////////
//	Menu.C
///////////////////////////////////////////////////////////////////
struct	uMenu
{
	uint16_t yx;
	const CHAR * sz;
};

enum
{
	enumSelectESC = 0u,
	enumSelectXCH = 0xFFu,
};

extern	void	Menu_Item_Mask ( const struct uMenu * menu, uint8_t item );
extern	void	Menu_Redraw    ( const struct uMenu * menu );
extern	uint8_t	Menu_Select     ( const struct uMenu * menu, uint8_t item );
extern	uint8_t	Menu_Select2    ( const struct uMenu * menu, uint8_t item, BOOL Direction );
extern  uint8_t	Menu_Select3	  ( const struct uMenu * menu, uint8_t item, uint8_t timeout );
extern	uint8_t	Menu_SelectOnly  ( const struct uMenu * menu, uint8_t item, void ( *pHook ) ( void ));
extern	uint8_t	Menu_Select_Ex  ( const struct uMenu * menu, uint8_t item, void ( *pHook )( void ) );
extern	uint8_t	Menu_Select_Ex2 ( const struct uMenu * menu, uint8_t item, void ( *pHook )( void ), BOOL Direction );
extern	uint8_t	Menu_Select_Ex3 ( const struct uMenu * menu, uint8_t item, void ( *pHook ) ( void ), uint8_t timeout );
extern	uint8_t	Menu_SelectOnlyEx ( const struct uMenu * menu, uint8_t item, void ( *pHook ) ( void )); 
///////////////////////////////////////////////////////////////////
//	MsgBox
///////////////////////////////////////////////////////////////////
//	MsgBox的参数设置值
#define	vbOKOnly			0x00u	//	只显示 OK 按钮。
#define	vbOKCancel			0x01u	//	显示 OK 及 Cancel 按钮。
#define	vbAbortRetryIgnore	0x02u	//	显示 Abort、Retry 及 Ignore 按钮。
#define	vbYesNoCancel		0x03u	//	显示 Yes、No 及 Cancel 按钮。
#define	vbYesNo				0x04u	//	显示 Yes 及 No 按钮。
#define	vbRetryCancel		0x05u	//	显示 Retry 及 Cancel 按钮。
#define	vbDefaultButton1 	0x00u	//	第一个按钮是缺省值。
#define	vbDefaultButton2 	0x10u	//	第二个按钮是缺省值。
#define	vbDefaultButton3 	0x20u	//	第三个按钮是缺省值。
#define	vbDefaultButton4 	0x30u 	//	第四个按钮是缺省值。
//	MsgBox的返回值
#define	vbOK				0x01u
#define	vbCancel			0x02u
#define	vbAbort				0x03u
#define	vbRetry				0x04u
#define	vbIgnore			0x05u
#define	vbYes				0x06u
#define	vbNo				0x07u

extern	uint8_t	MsgBox( const CHAR * sPrompt, uint16_t buttons );

///////////////////////////////////////////////////////////////////
//	Edit.C
///////////////////////////////////////////////////////////////////
extern	void	ShowFP32( uint16_t yx, FP32     NumFP32, uint16_t fmt, const CHAR * szUnit );
extern	void	ShowI16U( uint16_t yx, uint16_t NumI16U, uint16_t fmt, const CHAR * szUnit );
extern	void	ShowTIME( uint16_t yx, uint16_t Time );
extern	void	ShowDATE( uint16_t yx, uint32_t timer ); // const struct uShortDate * pDate );
extern	void	ShowPercent( uint16_t yx, FP32 Percent );
extern	void	ShowHEX16  ( uint16_t yx, uint16_t hex16 );

//	void	Show( uint16_t yx, uint16_t NumI16U, uint16_t fmt, const CHAR * Unit = NULL );
//	void	Show( uint16_t yx, FP32     NumFP32, uint16_t fmt, const CHAR * Unit = NULL );

extern	BOOL	EditI16U( uint16_t yx, uint16_t * pNUM, uint16_t fmt );
extern	BOOL	EditTIME( uint16_t yx, uint16_t * sTime );

//	extern	void	ShowStandard( uint16_t yx );
extern	void	ShowClock(     uint16_t yx, const uClock * pClock );
extern	void	ShowClockDate( uint16_t yx, const uClock * pClock );
extern	void	ShowClockTime( uint16_t yx, const uClock * pClock );
extern	BOOL	EditClockDate( uint16_t yx,       uClock * pClock );
extern	BOOL	EditClockTime( uint16_t yx,       uClock * pClock );

extern	uint32_t	InputPassword( void );

///////////////////////////////////////////////// End of File /////
