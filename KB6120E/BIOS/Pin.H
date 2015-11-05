/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
********************************************************************************
* File Name          : Pin.H
* Author             : Tech-01
* Version            : V0.0.1
* Date               : 2012年3月31日
* Description        : 电路板管脚定义
*******************************************************************************/


#include <stm32f10x.h>

// 		#define	Pin_Light	periphBitBand( &GPIOD->ODR, 2U )	//	PB.0 液晶背光
// 		#define	Light_ON	1

// 		#define	Pin_Buzzer	periphBitBand( &GPIOA->ODR, 4U )	//	PA.4 蜂鸣器
// 		#define	Buzzer_ON	1

// 		//	#define	Pin_6963_RST	periphBitBand( &GPIOB->ODR, 1U )
// 		//	#define	Pin_6963_A0 	periphBitBand( &GPIOB->ODR, 0U )
// 		//	#define	Pin_6963_WR 	periphBitBand( &GPIOC->ODR, 4U )
// 		//	#define	Pin_6963_RD 	periphBitBand( &GPIOB->ODR, 5U )
// 		//	#define	Pin_6963_CE 	periphBitBand( &GPIOB->ODR, 2U )

// 		#define	Pin_1608_WR	periphBitBand( &GPIOB->ODR, 0 )
// 		#define	Pin_1608_RD	periphBitBand( &GPIOB->ODR, 1 )
// 		#define	Pin_1608_A0	periphBitBand( &GPIOB->ODR, 2 )
// 		#define	Pin_1608_CS	periphBitBand( &GPIOB->ODR, 5 )

// 		#define	Pin_Key_Test	periphBitBand( &GPIOC->IDR, 12U )

// 		#define	Pin_595_RCLK	periphBitBand( &GPIOC->ODR, 4U )
// 		#define	Pin_595_SRCLK 	periphBitBand( &GPIOC->ODR, 5U )
// 		#define	Pin_595_SER_1	periphBitBand( &GPIOB->ODR, 14U )
// 		#define	Pin_595_SER_2 	periphBitBand( &GPIOB->ODR, 15U )

// 		#define	Port_DatBus8B		GPIOB
// 		#define	DatBus8B_Output()	( Port_DatBus8B->CRH  = 0x22222222U )
// 		#define	DatBus8B_Input()	( Port_DatBus8B->CRH  = 0x44444444U )
// 		#define	DatBus8B_Put(c)		( Port_DatBus8B->BSRR = (( 0x00FF0000UL | (uint8_t)(c)) << 8 ))
// 		#define	DatBus8B_Get()		((uint8_t)( Port_DatBus8B->IDR >> 8 ))

// 		/* I2C */
// 		//	#define	I2C_EEPROM		I2C1		/* PB.6:SCL, PB.7:SDA */

// 		#define	_SLAVE_AT24C512	0xA0U		/* I2C 器件地址: AT24C512 */


// 		#define _SLAVE_ISL1208	0xDEU		/* 定义器件地址 ISL1208  */
// 		#define	_SLAVE_FM24C04A	0xA0U

// 		/* MS5534 SPI */
// 		#define	Pin_5534_SCLK		periphBitBand( &GPIOA->ODR, 5U )
// 		#define	Pin_5534_DOUT		periphBitBand( &GPIOA->IDR, 6U )
// 		#define	Pin_5534_DIN		periphBitBand( &GPIOA->ODR, 7U )

// 		/* Printer */
// 		#define	Pin_Printer_Ready	periphBitBand( &GPIOA->IDR, 10U )

// 		/* Pump */
// 		//	#define	Pin_Pump_A		periphBitBand( &GPIOC->ODR, 0U )
// 		#define	Pin_Pump_B			periphBitBand( &GPIOC->ODR, 1U )
// 		#define	Pin_Pump_C			periphBitBand( &GPIOC->ODR, 2U )
// 		#define	Pin_Pump_D			periphBitBand( &GPIOC->ODR, 3U )

// 		#define	Pin_Valve_P			periphBitBand( &GPIOC->ODR, 0U )
// 		#define	Pump_ON		0

// 		/* SPI */
// 		//	#define	SPIx		SPI1
// 		//	//	#define	spi_ss_low()		SPI_SSOutputCmd( SPIx, ENABLE  )
// 		//	//	#define	spi_ss_high()		SPI_SSOutputCmd( SPIx, DISABLE )
// 		//	//	#define	spi_ss_low()			/*lint	-save	-e960 -e923 */ GPIO_ResetBits( GPIOA, GPIO_Pin_4 )	/*lint	-restore */
// 		//	//	#define	spi_ss_high()			/*lint	-save	-e960 -e923 */ GPIO_SetBits  ( GPIOA, GPIO_Pin_4 )	/*lint	-restore */
// 		//	//	#define	SPI_FLASH_WP_LOW()		/*lint	-save	-e960 -e923 */ GPIO_ResetBits( GPIOB, GPIO_Pin_2 )	/*lint	-restore */
// 		//	//	#define	SPI_FLASH_WP_HIGH()		/*lint	-save	-e960 -e923 */ GPIO_SetBits  ( GPIOB, GPIO_Pin_2 )	/*lint	-restore */
// 		//	//	#define	SPI_FLASH_RST_LOW()		/*lint	-save	-e960 -e923 */ GPIO_ResetBits( GPIOC, GPIO_Pin_13 )	/*lint	-restore */
// 		//	//	#define	SPI_FLASH_RST_HIGH()	/*lint	-save	-e960 -e923 */ GPIO_SetBits  ( GPIOC, GPIO_Pin_13 )	/*lint	-restore */
// 		//	#define	spi_ss_low()			PinClr( GPIOA, GPIO_Pin_4 )
// 		//	#define	spi_ss_high()			PinSet( GPIOA, GPIO_Pin_4 )
// 		//	#define	SPI_FLASH_WP_LOW()		PinClr( GPIOB, GPIO_Pin_2 )
// 		//	#define	SPI_FLASH_WP_HIGH()		PinSet( GPIOB, GPIO_Pin_2 )
// 		//	#define	SPI_FLASH_RST_LOW()		PinClr( GPIOC, GPIO_Pin_13 )
// 		//	#define	SPI_FLASH_RST_HIGH()	PinSet( GPIOC, GPIO_Pin_13 )


// 		/* Exported types ------------------------------------------------------------*/
// 		//	#define	STM32_F_SIZE_in_KB		(*(uint16_t *)0x1FFFF7E0U)	/*	Flash Size in KB  */
// 		//	#define	STM32_R_SIZE_in_KB		(*(uint16_t *)0x1FFFF7E2U)	/*	RAM size in KB  */
// 		//	#define	STM32_Unique_96_Bits	(*(uint8_t *)0x1FFFF7E8U)	/*	Unique device ID register (96 bits) */
// 		/* Exported constants --------------------------------------------------------*/


// 		//	extern	void	wait1us ( void );

// 		/* Buzzer */
// 		//	#define	Port_Beep		GPIOC
// 		//	#define	Pin_Beep		GPIO_Pin_6
// 		//	#define	Beep_ON			0

// 		/* LCM  Port & Pin */
// 		//	#define	Port_LCD_DATA	GPIOB
// 		//	#define	 Shift_LCD_DATA	8
// 		//	#define	 Mask_LCD_DATA	((uint16_t)((uint16_t)0xFFU << Shift_LCD_DATA ))

// 		// #define	Port_LCD_A0 	GPIOB
// 		// #define	Port_LCD_RST	GPIOB
// 		// #define	Port_LCD_WR 	GPIOC
// 		// #define	Port_LCD_RD 	GPIOC
// 		// #define	Port_LCD_CE 	GPIOC
// 		// //	
// 		// #define	 Pin_LCD_A0 	GPIO_Pin_0
// 		// #define	 Pin_LCD_RST	GPIO_Pin_1
// 		// #define	 Pin_LCD_WR 	GPIO_Pin_4
// 		// #define	 Pin_LCD_RD 	GPIO_Pin_5
// 		#define	 Pin_LCD_CE 	GPIO_Pin_12
// 		//	
// 		//	/* Keyboard */
// 		//	#define	Port_Key_OUT	GPIOC
// 		//	#define	 Pin_Key_OUT0	GPIO_Pin_0
// 		//	#define	 Pin_Key_OUT1	GPIO_Pin_1
// 		//	#define	 Pin_Key_OUT2	GPIO_Pin_2
// 		//	#define	 Pin_Key_OUT3	GPIO_Pin_3
// 		//	#define	Port_Key_IN		GPIOC
// 		//	#define	 Pin_Key_IN0	GPIO_Pin_10
// 		//	#define	 Pin_Key_IN1	GPIO_Pin_11



// 		/* USART */
// 		//	#define	USART_SENSOR	USART2
// 		//	#define	 Pin_SENSOR_TX	GPIO_Pin_2
// 		//	#define	 Pin_SENSOR_RX	GPIO_Pin_3
// 		//	
// 		//	#define	USART_PRINTER	USART1
// 		//	#define	 Pin_PRINTER_TX	GPIO_Pin_9
// 		//	#define	 Pin_PRINTER_RX	GPIO_Pin_10
// 		//	#define	Port_Prt_Ready	GPIOA
// 		//	#define	 Pin_Prt_Ready	GPIO_Pin_10
// 		//	#define	Prt_Ready		0


// 		//	#define	Pin_PWM_T1CH1	GPIO_Pin_8	/*	PA.08	*/
// 		//	#define	Pin_PWM_T3CH1	GPIO_Pin_6	/*	PC.06	*/
// 		//	#define	Pin_PWM_T3CH2	GPIO_Pin_7	/*	PC.07	*/
// 		//	#define	Pin_PWM_T3CH3	GPIO_Pin_8	/*	PC.08	*/
// 		//	#define	Pin_PWM_T3CH4	GPIO_Pin_9	/*	PC.09	*/

// 		//	void	__nop( void );		//lint	-esym( 526, __nop )
// 		//	#define	PinSet( Port, Pin )	/*lint -save -e960 -e923 */ GPIO_SetBits((Port),(Pin))	/*lint	-restore */
// 		//	#define	PinClr( Port, Pin )	/*lint -save -e960 -e923 */	GPIO_ResetBits((Port),(Pin))/*lint	-restore */
// 		//	#define	PinTST( Port, Pin )	/*lint -save -e960 -e923 */	GPIO_ReadInputDataBit((Port),(Pin))/*lint	-restore */
// 		//	

/********  (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司  **** End of File ****/
