/* CH376оƬ Ӳ������� V1.0 */
/* �ṩ�����ӳ��� */
#include	"HAL.H"
extern void	delay_us ( uint32_t us );
/* ��ʱָ��΢��ʱ��,���ݵ�Ƭ����Ƶ����,����ȷ */
void	mDelayuS( UINT8 us )
{
	delay_us(us);
}

/* ��ʱָ������ʱ��,���ݵ�Ƭ����Ƶ����,����ȷ */
void	mDelaymS( UINT8 ms )
{
	delay_us(ms*1000);
}