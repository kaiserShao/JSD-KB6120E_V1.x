#ifndef __SPI3INIT_H
#define __SPI3INIT_H
#include "CH376INC.h"

#define		ERR_USB_UNKNOWN		0xFA      		/* δ֪����,��Ӧ�÷��������,����Ӳ�����߳������ */
#define CLI()      __set_PRIMASK(1)				/* �ر����ж� */  
#define SEI() __set_PRIMASK(0)						/* �������ж� */ 

void   NVI_Init(void);

void	INT_IRQ_Enable( void );

void    SPI3_GPIO_Config( void );        	/*SPI3��Ӳ���ӿ�����*/

void    CH376_Write( uint8_t CMD_DAT );  	/*SPI3��CH376дһ���ֽ�*/

uint8_t CH376_Read( void ); 							/*SPI3��CH376��һ���ֽ�*/

void	  CH376_END_CMD( void ); 						/*ʧ��CH376(CH376�Ľ�������)*/

uint8_t SPI3_RW( uint8_t byte ); 					/*SPI3�Դӻ���дһ���ֽ�*/

void	  WriteCH376Cmd( uint8_t mCmd ); 		/*������376дһ���ֽ�����*/

void    WriteCH376Data( uint8_t dat ); 		/*������376дһ���ֽ�����*/

uint8_t ReadCH376Data( void ); 						/*������376��һ���ֽ�����*/

uint8_t CH376QueryInterrupt( void ); 			/*������ѯ376���ж�״̬*/

void    EXTI_PC13_Config( void );					/*�ж�������*/

 extern  uint8_t   CH376_Flag; 						/*�жϱ�־��*/

#endif
