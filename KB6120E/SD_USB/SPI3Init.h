#ifndef __SPI3INIT_H
#define __SPI3INIT_H
#include "CH376INC.h"

#define		ERR_USB_UNKNOWN		0xFA      		/* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */
#define CLI()      __set_PRIMASK(1)				/* 关闭总中断 */  
#define SEI() __set_PRIMASK(0)						/* 开放总中断 */ 

void   NVI_Init(void);

void	INT_IRQ_Enable( void );

void    SPI3_GPIO_Config( void );        	/*SPI3的硬件接口配置*/

void    CH376_Write( uint8_t CMD_DAT );  	/*SPI3向CH376写一个字节*/

uint8_t CH376_Read( void ); 							/*SPI3从CH376读一个字节*/

void	  CH376_END_CMD( void ); 						/*失能CH376(CH376的结束命令)*/

uint8_t SPI3_RW( uint8_t byte ); 					/*SPI3对从机读写一个字节*/

void	  WriteCH376Cmd( uint8_t mCmd ); 		/*主机对376写一个字节命令*/

void    WriteCH376Data( uint8_t dat ); 		/*主机对376写一个字节数据*/

uint8_t ReadCH376Data( void ); 						/*主机从376读一个字节数据*/

uint8_t CH376QueryInterrupt( void ); 			/*主机查询376的中断状态*/

void    EXTI_PC13_Config( void );					/*中断线配置*/

 extern  uint8_t   CH376_Flag; 						/*中断标志字*/

#endif
