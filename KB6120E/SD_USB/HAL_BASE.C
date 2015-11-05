/* CH376芯片 硬件抽象层 V1.0 */
/* 提供基本子程序 */
#include	"HAL.H"
extern void	delay_us ( uint32_t us );
/* 延时指定微秒时间,根据单片机主频调整,不精确 */
void	mDelayuS( UINT8 us )
{
	delay_us(us);
}

/* 延时指定毫秒时间,根据单片机主频调整,不精确 */
void	mDelaymS( UINT8 ms )
{
	delay_us(ms*1000);
}
