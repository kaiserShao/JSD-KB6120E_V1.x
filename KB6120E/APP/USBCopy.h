#include "stm32f10x.h"
#include "AppDEF.H"
#ifndef	__bool_true_false_are_defined
	#include <stdbool.h>
	typedef	bool	BOOL;
#endif
extern BOOL	USBPrint_TSP( uint16_t FileNum, struct uFile_TSP const * pFile );
extern BOOL USBPrint_R24( uint16_t FileNum, struct uFile_R24 const * pFile );
extern BOOL USBPrint_SHI( uint16_t FileNum, struct uFile_SHI const * pFile );
extern BOOL USBPrint_AIR( uint16_t FileNum, struct uFile_AIR const * pFile );
extern BOOL USB_PrintInit(void);
/***************END************************/
