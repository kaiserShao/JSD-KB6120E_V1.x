/**************** (C) COPYRIGHT 2012 青岛金仕达电子科技有限公司 ****************
* 文 件 名: TM12864.C
* 创 建 者: Dean
* 描  述  : 天马128*64液晶读写程序
*         : 
* 最后修改: 2012年4月3日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"

static	uint8_t	invert_byte( uint8_t U8B )
{
	U8B = (( U8B & 0x0Fu ) << 4 ) | (( U8B & 0xF0u ) >> 4 );
	U8B = (( U8B & 0x33u ) << 2 ) | (( U8B & 0xCCu ) >> 2 );
	U8B = (( U8B & 0x55u ) << 1 ) | (( U8B & 0xAAu ) >> 1 );

	return	U8B;
}

//	BIOS.C -  TM12864 access
extern	void	  TM12864_PortInit( void );
extern	void	  TM12864_SelectL( void );
extern	void	  TM12864_SelectR( void );
extern	uint8_t	TM12864_ReadState( void );
extern	uint8_t	TM12864_ReadData( void );
extern	void	  TM12864_WriteCommand( uint8_t OutCommand );
extern	void	  TM12864_WriteData( uint8_t	OutData );

extern	void	  TM12864_GrayInit( void );

///////////////////////////////////////////////////////////////////
#define	 DISPON 		  0x3FU	// 显示 on
#define	 DISPOFF		  0x3EU	// 显示 off
#define	 DISPFIRST		0xC0U	// 显示起始行定义
#define	 SET_COL		  0x40U	// X定位设定指令（列）
#define	 SET_PAG		  0xB8U	// Y定位设定指令（页）

///////////////////////////////////////////////////////////////////
// 写汉字液晶子程 液晶屏分为(4行 X 8列)汉字，全部使用模拟接口方式
///////////////////////////////////////////////////////////////////
#define  text_width		8U		// 屏幕座标以8点为单位进行计算
#define  max_txt_row	8U		// 行(页)
#define  max_txt_col	16U		// 列

///////////////////////////////////////////////////////////////////
// 根据设定的坐标数据，定位LCM上的下一个操作单元位置
///////////////////////////////////////////////////////////////////
static	void	moveto( uint8_t yy, uint8_t xx )
{
#if _Flip
	( xx < 64 ) ? TM12864_SelectL() : TM12864_SelectR();
	TM12864_WriteCommand(((yy) %  8U ) + SET_PAG );
	TM12864_WriteCommand(((xx) % 64U ) + SET_COL );
#else
	( xx < 64 ) ? TM12864_SelectR() : TM12864_SelectL();
	TM12864_WriteCommand(((~yy) %  8U ) + SET_PAG );
	TM12864_WriteCommand(((~xx) % 64U ) + SET_COL );
#endif
}

///////////////////////////////////////////////////////////////////
//	清屏
///////////////////////////////////////////////////////////////////
void	TM12864_cls( void )
{	// 清屏
	uint8_t	row, col;

	TM12864_SelectL(); TM12864_WriteCommand( DISPOFF );
	TM12864_SelectR(); TM12864_WriteCommand( DISPOFF );	// 关闭显示屏

	TM12864_SelectL(); TM12864_WriteCommand(DISPFIRST);
	TM12864_SelectR(); TM12864_WriteCommand(DISPFIRST);	// 定义显示起始行为零

	// 清空显示缓冲区 RAM
	for( row = 0U; row < 8u; ++row )
	{
		// Left  part
		moveto( row, 0U  );
		for( col = 64U; col != 0U; --col )
		{	TM12864_WriteData(0x00U);	}
		// Right part
		moveto( row, 64U );
		for( col = 64U; col != 0U; --col )
		{	TM12864_WriteData(0x00U);	}
	}

	TM12864_SelectL(); TM12864_WriteCommand( DISPON );
	TM12864_SelectR(); TM12864_WriteCommand( DISPON );	// 打开显示屏
}

///////////////////////////////////////////////////////////////////
//	反白
///////////////////////////////////////////////////////////////////
void	TM12864_mask( uint16_t yx, uint8_t xlen )
{	//	反白
	uint8_t 	i, row, col, col_end;
	uint8_t 	InDat;

	assert( 0U != xlen );

	row = ( yx ) / 256u;
	col = ( yx ) % 256u;
	assert( row < max_txt_row );
	assert( col < max_txt_col );

	col_end = col + xlen;
	if ( col_end > max_txt_col )
	{
		col_end = max_txt_col;
	}

	do {   
		for( i = 0u; i < 8u; ++i )
		{
			moveto( row,      (uint8_t)( col * text_width ) + i );	InDat = TM12864_ReadData();
			moveto( row,      (uint8_t)( col * text_width ) + i );	TM12864_WriteData((uint8_t) ~InDat );
		}
		for( i = 0u; i < 8u; ++i )
		{
			moveto( row + 1U, (uint8_t)( col * text_width ) + i );	InDat = TM12864_ReadData();
			moveto( row + 1U, (uint8_t)( col * text_width ) + i );	TM12864_WriteData((uint8_t) ~InDat );
		}
	} while ( ++col < col_end );
}

///////////////////////////////////////////////////////////////////
//	输出字符串
///////////////////////////////////////////////////////////////////
void	TM12864_puts( uint16_t yx, const CHAR * sz )
{	//	输出字符串
	CGROM		pDot;
	CHAR		sDat;
	size_t		slen;
	uint8_t		i, row, col, col_end;

	assert( sz );
	slen = strlen( sz );
	if ( slen > max_txt_col )
	{
		slen = max_txt_col;
	}

	row = ( yx ) / 256u;
	col = ( yx ) % 256u;
	assert( row < max_txt_row );
	assert( col < max_txt_col );

	col_end = col + (uint8_t)slen;
	if ( col_end > max_txt_col )
	{
		col_end = max_txt_col;
	}

	do {
		sDat = *sz++;

		if ( 0U == ((uint8_t)sDat & 0x80U ))
		{	// DBC 半角字
			pDot = DotSeekDBC( sDat );
			for ( i = 0; i < 8u; ++i ){	//	for( i = 8U; i != 0U; --i )	{
			moveto( row, (uint8_t)( col * text_width )+i);
			TM12864_WriteData(invert_byte(*pDot++));	}
			for ( i = 0; i < 8u; ++i ){	//	for( i = 8U; i != 0U; --i ){
			moveto( row + 1U, (uint8_t)( col * text_width )+i);
			TM12864_WriteData(invert_byte(*pDot++));	}
			col += 1U;
		}
		else
		{	// SBC 全角字
			pDot = DotSeekSBC( sDat, *sz++ );
			for ( i = 0; i < 8u; ++i ){	//	for( i = 8U; i != 0U; --i ){
				moveto( row, (uint8_t)( col * text_width )+i);
			TM12864_WriteData(invert_byte(*pDot++));	}
			for ( i = 0; i < 8u; ++i ){	//	for( i = 8U; i != 0U; --i ){
				moveto( row, (uint8_t)(( col + 1U ) * text_width )+i);
			TM12864_WriteData(invert_byte(*pDot++));	}
			for ( i = 0; i < 8u; ++i ){	//	for( i = 8U; i != 0U; --i ){
				moveto( row + 1U, (uint8_t)( col  * text_width )+i);
			TM12864_WriteData(invert_byte(*pDot++));	}
			for ( i = 0; i < 8u; ++i ){	//	for( i = 8U; i != 0U; --i ){
				moveto( row + 1U, (uint8_t)(( col + 1U ) * text_width )+i);
			TM12864_WriteData(invert_byte(*pDot++));	}
			col += 2U;
		}
	}	while ( col < col_end  );
}

/********************************** 功能说明 ***********************************
 *	显示模块初始化
*******************************************************************************/
void	TM12864_Init( void )
{  //  显示模块初始化
	TM12864_PortInit();

	TM12864_cls();                                 		// 清空显示缓冲区 RAM

	TM12864_GrayInit();
}

/********************************** 功能说明 ***********************************
 *	测试显示模块
*******************************************************************************/
BOOL	TM12864_Test( void )
{
	uint8_t state_off_L, state_on_L;
	uint8_t state_off_R, state_on_R;
	
	TM12864_PortInit();

	TM12864_cls();                                 		// 清空显示缓冲区 RAM

	TM12864_SelectL(); TM12864_WriteCommand( DISPOFF );
	TM12864_SelectR(); TM12864_WriteCommand( DISPOFF );	// 关闭显示屏
	TM12864_SelectL(); state_off_L = 0x20 & TM12864_ReadState();
	TM12864_SelectR(); state_off_R = 0x20 & TM12864_ReadState();
	TM12864_SelectL(); TM12864_WriteCommand( DISPON );
	TM12864_SelectR(); TM12864_WriteCommand( DISPON );	// 打开显示屏
	TM12864_SelectL(); state_on_L  = 0x20 & TM12864_ReadState();
	TM12864_SelectR(); state_on_R  = 0x20 & TM12864_ReadState();

	if (( state_off_L == 0x20u ) && ( state_on_L == 0x00 ) 
	 && ( state_off_R == 0x20u ) && ( state_on_R == 0x00 ))
	{
		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}

/********  (C) COPYRIGHT 2012 青岛金仕达电子科技有限公司  **** End Of File ****/
