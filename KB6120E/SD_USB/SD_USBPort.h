#ifndef __SDCARD_PORT_H
#define __SDCARD_PORT_H

#include <stdio.h>
#include <string.h>
#include "Pin.H"
#include "time.h"
#include "file_sys.h"
#include "CH376INC.h"
#ifndef	__bool_true_false_are_defined
	#include <stdbool.h>
	typedef	bool	BOOL;

#endif

//根目录操作
void Byte_CREAT_CON_DIR ( const char * BUF_Name );
void Byte_CREAT_WRITE_DIR ( const char * BUF_Name, uint8_t * Content, uint32_t offset );
void Byte_OPEN_READ_DIR ( const char * BUF_Name , uint32_t Addr, uint8_t * bufread, uint32_t offset );
void Byte_OPEN_WRITE_DIR ( const char * BUF_Name , uint8_t * Content , uint32_t Addr, uint32_t offset );
//多层目录操作
void Byte_CREAT_WRITE_PATH ( const char * BUF_Name , uint8_t * Content ,uint32_t offset );
uint8_t Byte_OPEN_READ_PATH ( const char * BUF_Name , uint32_t Addr, uint8_t* bufread, uint32_t offset );
uint8_t Byte_OPEN_WRITE_PATH ( const char * BUF_Name ,   uint8_t * Content , uint32_t Addr, uint32_t offset );
//SD卡初始化
void SD_Init(void);
//SD接口
BOOL ByteLoad( const char * B_Name , uint32_t Addr, uint8_t * bufread, uint32_t offset  );
BOOL ByteSave( const char * BUF_Name ,  uint8_t * Content , uint32_t Addr,uint32_t offset );
BOOL ByteFill( const char * BUF_Name , uint32_t Addr , uint16_t size);
 										//		文件名称 			写起始地址					要写的数量
uint32_t ByteGetSize(const char * BUF_Name);

//根目录操作
void Sec_CREAT_CON_DIR ( const char * BUF_Name );
void Sec_CREAT_WRITE_DIR ( const char * BUF_Name, uint8_t * Content, uint8_t offset );
void Sec_OPEN_READ_DIR ( const char * BUF_Name , uint32_t Addr, uint8_t * bufread, uint8_t offset );
void Sec_OPEN_WRITE_DIR ( const char * BUF_Name , uint8_t * Content , uint32_t Addr, uint8_t offset );
//多层目录操作
void Sec_CREAT_WRITE_PATH ( const char * BUF_Name , uint8_t * Content ,uint8_t offset );
uint8_t Sec_OPEN_READ_PATH ( const char * BUF_Name , uint32_t Addr, uint8_t* bufread, uint8_t offset );
uint8_t Sec_OPEN_WRITE_PATH ( const char * BUF_Name ,   uint8_t * Content , uint32_t Addr, uint8_t offset );
//Sec卡初始化
uint8_t USB_Init(void);
void File_Creat(void);
uint8_t USB_CHAK(void);
//void USBInit_Test(void);
//USB接口
BOOL SecLoad( const char * B_Name , uint32_t Addr, uint8_t * bufread, uint8_t offset  );
BOOL SecSave( const char * BUF_Name ,  uint8_t * Content , uint32_t Addr,uint8_t offset );
BOOL SecFill( const char * BUF_Name , uint32_t Addr , uint8_t size);
 										//		文件名称 			写起始地址					要写的数量
uint32_t SecGetSize(const char * BUF_Name);
#endif
/**/
