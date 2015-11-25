/**************** (C) COPYRIGHT 2012 青岛金仕达电子科技有限公司 ****************
* 文 件 名: OLED9704.C
* 创 建 者: Dean
* 描  述  : 錸寶OLED9704模块访问程序
*         : 
* 最后修改: 2012年4月5日
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"


//	BIOS.C - OLED9704 access
extern	void	  OLED9704_PortInit( void );
extern	uint8_t	OLED9704_ReadState( void  );
extern	uint8_t	OLED9704_ReadData( void  );
extern	void	  OLED9704_WriteReg( uint8_t OutCommand );
extern	void	  OLED9704_WriteData( uint8_t OutData );
extern	void	  OLED9704_DisplayEnable( void );
extern	void	  OLED9704_DisplayDisable( void );

///////////////////////////////////////////////////////////////////
// 写汉字液晶子程 液晶屏分为(4行 X 8列)汉字，全部使用模拟接口方式
///////////////////////////////////////////////////////////////////
#define  text_width		8U		// 屏幕座标以8点为单位进行计算
#define  max_txt_col	16U		// 列
#define  max_txt_row	8U		// 行(页)

///////////////////////////////////////////////////////////////////
// 根据设定的坐标数据，定位下一个操作单元位置
///////////////////////////////////////////////////////////////////
static  void  move_to( uint8_t yy, uint16_t xx )
{
	xx += 0x02U;					
	OLED9704_WriteReg( 0xB0U + ( yy % 0x08U ));			// Set page
	OLED9704_WriteReg( 0x10U + ((uint8_t)xx / 0x10U ));	// Higher col address
	OLED9704_WriteReg( 0x00U + ((uint8_t)xx % 0x10U ));	// Lower col address
}

///////////////////////////////////////////////////////////////////
//	清屏
///////////////////////////////////////////////////////////////////
void  OLED9704_cls( void  )
{	// 清屏
	uint8_t	row, col;

	for( row = 0U; row < 8u; ++row )
	{
		move_to( row, 0U  );
		for( col = 128U; col != 0U; --col )
		{
			OLED9704_WriteData( 0xffu );
		}
	}

	OLED9704_WriteReg(0xAFU);   		// Display on
}

///////////////////////////////////////////////////////////////////
//	反白
///////////////////////////////////////////////////////////////////
void	OLED9704_mask( uint16_t yx, uint8_t xlen )
{	//	反白
	uint8_t		i, row, col, col_end;
	uint8_t		InData;

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
			move_to( row,      (uint8_t)( col * text_width ) + i );	InData = OLED9704_ReadData();
			move_to( row,      (uint8_t)( col * text_width ) + i );	OLED9704_WriteData((uint8_t)(~InData));
			move_to( row + 1U, (uint8_t)( col * text_width ) + i );	InData = OLED9704_ReadData();
			move_to( row + 1U, (uint8_t)( col * text_width ) + i );	OLED9704_WriteData((uint8_t)(~InData));
		}
	} while ( ++col < col_end );
}

///////////////////////////////////////////////////////////////////
//	输出字符串
///////////////////////////////////////////////////////////////////
// 	static	__inline	uint8_t	invert_byte( uint8_t U8B ){
// 		U8B = (uint8_t)((uint8_t)( U8B << 4 ) & 0x0F0U ) | (uint8_t)((uint8_t)( U8B >> 4 ) & 0x00FU );
// 		U8B = (uint8_t)((uint8_t)( U8B << 2 ) & 0x0CCU ) | (uint8_t)((uint8_t)( U8B >> 2 ) & 0x033U );
// 		U8B = (uint8_t)((uint8_t)( U8B << 1 ) & 0x0AAU ) | (uint8_t)((uint8_t)( U8B >> 1 ) & 0x055U );

// 		return	U8B;
// 	}

void	OLED9704_puts( uint16_t yx, const CHAR * sz )
{	//	输出字符串
	CGROM		pDot;
	CHAR		sDat;
	size_t		slen;
	uint8_t		i, row, col, col_end;

	assert( sz != NULL );
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
			move_to( row, (uint8_t)( col * text_width ) );
			for( i = 8U; i != 0U; --i )	{	OLED9704_WriteData(*pDot++);	}
			move_to( row + 1U, (uint8_t)( col * text_width ) );
			for( i = 8U; i != 0U; --i )	{	OLED9704_WriteData(*pDot++);	}
			col += 1U;
		}
		else
		{	// SBC 全角字
			pDot = DotSeekSBC( sDat, *sz++ );
			move_to( row, (uint8_t)( col * text_width ) );
			for( i = 16U; i != 0U; --i ) {	OLED9704_WriteData(*pDot++);	}
			move_to( row + 1U, (uint8_t)( col * text_width ) );
			for( i = 16U; i != 0U; --i ) {	OLED9704_WriteData(*pDot++);	}
			col += 2U;
		}
	}	while ( col < col_end  );
}


///////////////////////////////////////////////////////////////////
// 设置显示亮度
///////////////////////////////////////////////////////////////////
void	OLED9704_SetLight( uint8_t SetLight )
{
	uint8_t OutValue = (uint16_t)SetLight * 255u / 100u;
  OLED9704_WriteReg(0x81u);
	OLED9704_WriteReg( OutValue );    // Contrast setting: contrast
}

///////////////////////////////////////////////////////////////////
// 液晶初始化
///////////////////////////////////////////////////////////////////
void	OLED9704_Init( void )
{
	//  显示模块初始化
	OLED9704_PortInit();
	OLED9704_WriteReg(0xAEU);   		// Display off
	OLED9704_WriteReg(0xADU);  OLED9704_WriteReg(0x8AU);    // Internal DC-DC off: Second byte
	OLED9704_WriteReg(0xA8U);  OLED9704_WriteReg(0x3FU);    // MUX Ratio: 64 duty
	OLED9704_WriteReg(0xD3U);  OLED9704_WriteReg(0x00U);    // Display offset: Second byte
	OLED9704_WriteReg(0x40U);   		// Start line
//	OLED9704_WriteReg(0xA0U);   		// Set Segment remap (0xA0:normal, 0xA1:remapped)//清达光电需要改的地方a0.正常	a1.反显
	OLED9704_WriteReg(0xA1U);
	OLED9704_WriteReg(0xC8U);   		// Set COM remap (0xC0:normal, 0xC8: enable )
	OLED9704_WriteReg(0xA6U);   		// Set normal/inverse display (0xA6:Normal display)
	OLED9704_WriteReg(0xA4U);   		// Set entire display on/off (0xA4:Normal display)
//    OLED9704_WriteReg(0x81U);  OLED9704_WriteReg(0x5CU);    // Contrast setting: Second byte
	OLED9704_WriteReg(0xD5U);  OLED9704_WriteReg(0x60U);    // Frame rate: 85 Hz
	OLED9704_WriteReg(0xD8U);  OLED9704_WriteReg(0x00U);    // Mode setting: Mono mode
	OLED9704_WriteReg(0xD9U);  OLED9704_WriteReg(0x84U);    // Set Pre-char_tge period: Second byte
	OLED9704_cls();

	OLED9704_DisplayEnable();
}

/********  (C) COPYRIGHT 2012 青岛金仕达电子科技有限公司  **** End Of File ****/
