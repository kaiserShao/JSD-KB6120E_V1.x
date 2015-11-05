/**************** (C) COPYRIGHT 2012 青岛金仕达电子科技有限公司 ****************
* 文 件 名: CGROM.C
* 创 建 者: 董峰
* 描  述  : 显示字模检索
* 最后修改: 
*********************************** 修订记录 ***********************************
* 版  本: 
* 修订人: 
*******************************************************************************/
#include "BSP.H"
#include "BIOS.H"

#define	code	const
#include "DOT_EN.TXT"
#include "DOT_CN.TXT"

/*******************************************************************************
* 函数名称: DotSeekDBC
* 功能说明: 英文半角字库检索
* 输入参数: char_t sc
* 输出参数: None
* 返 回 值: CGROM Dot Address
*******************************************************************************/
CGROM	DotSeekDBC ( int8_t sc )
{
    uint8_t uc = (uint8_t)sc;

	if ( uc < 0x20U ){	uc = 0x7FU;	}
	return	& chardot[( uc - 0x20U ) * 0x10U ];
}

/*******************************************************************************
* 函数名称: DotSeekSBC
* 功能说明: 中文全角字库检索
* 输入参数: char_t cc0, char_t cc1
* 输出参数: None
* 返 回 值: CGROM Dot Address
*******************************************************************************/
CGROM	DotSeekSBC ( int8_t cc0, int8_t cc1 )
{
	uint16_t	cc16, cm16;
	uint16_t	mid, low, high;

	cc16 = ((uint8_t)cc0 * 0x100U ) + (uint8_t)cc1;

	low  = 1U;				/*	从[1]开始搜索	*/
	high = (uint16_t)hzNum - 1U;
	while ( low <= high )
	{
		mid = ( low + high ) / 2U;	/*	使用折半查找以加快检索速度	*/
		cm16 = ( hzIndex[ 2U * mid ] * 0x100U ) + hzIndex[( 2U * mid ) + 1U ];
		if ( cc16 == cm16 )
		{
			return	& hzdot[ mid * 0x20U ];
		}
		if ( cc16 > cm16 )
		{
			low  = mid + 1U;
		} 
		else 
		{
			high = mid - 1U;
		}
	}
	return	& hzdot[0];		/*	[0]存放空白字符	*/
}

/********  (C) COPYRIGHT 2010 青岛金仕达电子科技有限公司  **** End Of File ****/
