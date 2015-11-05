/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: SD_USBPort.c
* �� �� ��: Kaiser
* ��  ��  : SD����U�̽ӿ�
* ����޸�: 2015/09/10
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/
#include "string.h"
#include "SD_USBPort.h"
#include "AppDEF.H"
#define SD_Card       	0x03
#define USB_Flash_disk	0x06
UINT8			buf[100];	
UINT8			USBbuf[100];
UINT8			bufread[100];
UINT8			TarName[100];
uint8_t 		i,s;
uint16_t xy;

//������Ŀ¼�µ�Ŀ¼���� (Ŀ¼������Ϊ��дӢ�����ֻ�����������Ҳ��ܳ���8���ֽ�)
void Byte_CREAT_CON_DIR(const char * BUF_Name)
{	
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	s = CH376DirCreate( TarName ); /* �ڸ�Ŀ¼���½�Ŀ¼(�ļ���)����,���Ŀ¼�Ѿ�������ôֱ�Ӵ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/
//������Ŀ¼�µ��ļ���д
void Byte_CREAT_WRITE_DIR(const char * BUF_Name, uint8_t * Content, uint32_t offset )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	CH376FileCreate( TarName );
	CH376ByteWrite( Content, offset, NULL );  /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//�򿪸�Ŀ¼�µ��ļ�����
void  Byte_OPEN_READ_DIR (const char * BUF_Name , uint32_t Addr, uint8_t * bufread , uint32_t offset  )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpen( TarName );  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼(�ļ���) */
	CH376ByteLocate( Addr );
//	bufoffset =	CH376GetFileSize( );
	CH376ByteRead( bufread, offset , NULL );/* ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//�򿪸�Ŀ¼�µ��ļ���д
void  Byte_OPEN_WRITE_DIR (const char * BUF_Name, uint8_t * Content , uint32_t Addr, uint32_t offset  )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpen( TarName );  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼(�ļ���) */
//	bufoffset =	CH376GetFileSize( );
	CH376ByteLocate( Addr );
	CH376ByteWrite( Content , offset, NULL );  /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//�����༶Ŀ¼�µ��ļ���д
void Byte_CREAT_WRITE_PATH(const char * BUF_Name, uint8_t * Content , uint32_t offset )
{	//													�ļ���                �ļ�����	
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name ); /* Ŀ���ļ��� */
	s = CH376FileCreatePath( TarName );  	/* �½��༶Ŀ¼�µ��ļ�,֧�ֶ༶Ŀ¼·��,���뻺����������RAM�� */
	s = CH376ByteWrite( Content , offset, NULL ); /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
	s = CH376FileClose( TRUE );   /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//�򿪶༶Ŀ¼�µ��ļ�����
uint8_t  Byte_OPEN_READ_PATH (const char * BUF_Name , uint32_t Addr , uint8_t * bufread , uint32_t offset )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	//bufoffset =	CH376GetFileSize( );
	CH376ByteLocate( Addr );
	s = CH376ByteRead(  bufread , offset ,NULL );/* ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
	return s;
}
/**/

//�򿪶༶Ŀ¼�µ��ļ���д
uint8_t Byte_OPEN_WRITE_PATH (const char * BUF_Name, uint8_t * Content , uint32_t Addr ,uint32_t offset)
{	
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
//	bufoffset =	CH376GetFileSize( );
	CH376ByteLocate( Addr );
	s = CH376ByteWrite(Content, offset, &xy ); /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */	
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();	
	return s;
}
/**/

//�ֽڶ�
BOOL ByteLoad( const char * B_Name , uint32_t Addr, uint8_t * bufread, uint32_t offset  )
{														//�ļ�����       ����ʼ��ַ    Ҫ�������ݳ���
	bus_SPI_mutex_apply();	
	if( Byte_OPEN_READ_PATH ( B_Name  , Addr  ,bufread , offset ) == USB_INT_SUCCESS )
	{ bus_SPI_mutex_release();	return TRUE; }	
	else
	{ bus_SPI_mutex_release();	return FALSE;}
	
}
//�ֽڴ�

BOOL ByteSave( const char * BUF_Name ,  uint8_t * Content , uint32_t Addr, uint32_t offset  )
{												//		�ļ����� 				Ҫд������ 		д��ʼ��ַ		
	bus_SPI_mutex_apply();	
	s = Byte_OPEN_WRITE_PATH ( BUF_Name , Content , Addr, offset );  
	bus_SPI_mutex_release();	
	if( s == USB_INT_SUCCESS )
		return TRUE; 
	else 
	if(s == USB_INT_BUF_OVER)
	{
		beep();
		return FALSE;
	}
	else
		return FALSE;

}
//�ֽ����

BOOL ByteFill( const char * BUF_Name , uint32_t Addr , uint16_t size)
{		
	bus_SPI_mutex_apply();								//		�ļ����� 			д��ʼ��ַ					Ҫд������
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	CH376ByteLocate( Addr );
	
	memset( buf, 0x00, 100 );
	s = CH376ByteWrite( buf, size, NULL ); /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();
	if( s == USB_INT_SUCCESS )
		return TRUE; 
	else 
	if(s == USB_INT_BUF_OVER)
	{
		beep();
		return FALSE;
	}
	else
		return FALSE; 	
}
//���ֽ�Ϊ��λ����ļ�ָ���λ��

uint32_t ByteGetSize(const char * BUF_Name)
{
	uint32_t bufoffset;
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName, BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	bufoffset =	CH376GetFileSize( );
	bus_SPI_mutex_release();	
	return bufoffset;
}
//������Ŀ¼�µ�Ŀ¼����

void Sec_CREAT_CON_DIR(const char * BUF_Name)
{	
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	s=CH376DirCreate( TarName ); /* �ڸ�Ŀ¼���½�Ŀ¼(�ļ���)����,���Ŀ¼�Ѿ�������ôֱ�Ӵ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//������Ŀ¼�µ��ļ���д

void Sec_CREAT_WRITE_DIR(const char * BUF_Name, uint8_t * Content, uint8_t offset )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	CH376FileCreate( TarName );
	CH376SecLocate( 0 );
	CH376SecWrite( Content, offset, NULL );  /* ������Ϊ��λ��ǰλ��д�����ݿ� */	
	CH376FileClose( TRUE ); /* �ر��ļ�,����������д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//�򿪸�Ŀ¼�µ��ļ�����

void  Sec_OPEN_READ_DIR (const char * BUF_Name , uint32_t Addr, uint8_t * bufread , uint8_t offset  )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpen( TarName );  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼(�ļ���) */
	CH376SecLocate( Addr );
//	bufoffset =	CH376GetFileSize( );
	CH376SecRead( bufread, offset , NULL );/* ������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,����������д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	bus_SPI_mutex_release();	
}
/***/

//�򿪸�Ŀ¼�µ��ļ���д

void  Sec_OPEN_WRITE_DIR (const char * BUF_Name, uint8_t * Content , uint32_t Addr, uint8_t offset  )
{	
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpen( TarName );  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼(�ļ���) */
	CH376SecLocate( Addr );	
	CH376SecWrite( Content , offset, NULL );  /* ������Ϊ��λ��ǰλ��д�����ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,����������д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	bus_SPI_mutex_release();	
}
/**/

//�����༶Ŀ¼�µ��ļ���д

void Sec_CREAT_WRITE_PATH(const char * BUF_Name, uint8_t * Content , uint8_t offset )
{	//													�ļ���                �ļ�����               �ļ���С
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name ); /* Ŀ���ļ��� */
	s = CH376FileCreatePath( TarName );  	/* �½��༶Ŀ¼�µ��ļ�,֧�ֶ༶Ŀ¼·��,���뻺����������RAM�� */
// 	CH376SecLocate( 0 );
	s = CH376SecWrite( Content , offset, NULL ); /* ������Ϊ��λ��ǰλ��д�����ݿ� */
	s = CH376FileClose( TRUE );   /* �ر��ļ�,����������д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	bus_SPI_mutex_release();		
}
/**/

//�򿪶༶Ŀ¼�µ��ļ�����

uint8_t  Sec_OPEN_READ_PATH (const char * BUF_Name , uint32_t Addr , uint8_t * bufread , uint8_t offset )
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	//bufoffset =	CH376GetFileSize( );
	CH376SecLocate( Addr );
	s = CH376SecRead(  bufread , offset ,NULL );/* ������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,����������д�����Զ������ļ����� */
	bus_SPI_mutex_release();	
	return s;
}
/**/

//�򿪶༶Ŀ¼�µ��ļ���д
uint8_t Sec_OPEN_WRITE_PATH (const char * BUF_Name, uint8_t * Content , uint32_t Addr ,uint8_t offset)
{
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
//	bufoffset =	CH376GetFileSize( );
	CH376SecLocate( Addr );
	s = CH376SecWrite(Content, offset, NULL ); /* ������Ϊ��λ��ǰλ��д�����ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,����������д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( USBbuf, 0, 100 );
	bus_SPI_mutex_release();	
	return s;
}

/**/

//������
BOOL SecLoad( const char * B_Name , uint32_t Addr, uint8_t * bufread, uint8_t offset  )
{		//�ļ�����       ����ʼ��ַ    									Ҫ������������
	bus_SPI_mutex_apply();	
	if( Sec_OPEN_READ_PATH ( B_Name, Addr, bufread, offset ) == USB_INT_SUCCESS )
	{ bus_SPI_mutex_release();	return TRUE; }	
	else
	{ bus_SPI_mutex_release();	return FALSE; }	
}

//������
BOOL SecSave( const char * BUF_Name,  uint8_t * Content, uint32_t Addr, uint8_t offset  )
{										//		�ļ����� 				Ҫд������ 		д��ʼ��ַ		     Ҫд��������
	bus_SPI_mutex_apply();
	s = Sec_OPEN_WRITE_PATH ( BUF_Name, Content, Addr, offset );
	bus_SPI_mutex_release();
	if( s == USB_INT_SUCCESS )
			return TRUE; 
	else 
	if( s == USB_INT_BUF_OVER )
	{
	//	beep();
		return FALSE;
	}
	else
		return FALSE; 
		
}

//������Ϊ��λ���
BOOL SecFill( const char * BUF_Name , uint32_t Addr , uint8_t size)
{										//		�ļ����� 			д��ʼ��ַ					Ҫд������	
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	CH376ByteLocate( Addr );	
	memset( buf, 0xaa, 100 );
	s = CH376ByteWrite( buf, size, NULL ); /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
	CH376FileClose( TRUE ); /* �ر��ļ�,�����ֽڶ�д�����Զ������ļ����� */
	memset( TarName, 0, 100 );
	memset( buf, 0, 100 );
	bus_SPI_mutex_release();
	if( s == USB_INT_SUCCESS )
		return TRUE;  
	else 
	if(s == USB_INT_BUF_OVER)
	{
		beep();
		return FALSE;
	}
	else
		return FALSE; 	
}

//������Ϊ��λ����ļ�ָ��λ��
uint32_t SecGetSize(const char * BUF_Name)
{
	uint32_t bufoffset;
	bus_SPI_mutex_apply();
	strcpy( (char *)TarName,  BUF_Name );
	CH376FileOpenPath( TarName );/* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
	bufoffset =	CH376GetFileSize( );
	bus_SPI_mutex_release();
	return bufoffset;
}


/***/

#define	FilePageSize_TSP  40u
#define	FilePageSize_AIR  32u
#define	FilePageSize_R24  64u
#define	FilePageSize_SHI  64u
uint8_t  sdinit[16]={"SDINITSUCCESSFUL"};
//SD���ӿڳ�ʼ��
void SD_Init(void)
{
	BOOL SetFlag = FALSE;
	s = mInitCH376Host(SD_Card);       /* ��ʼ��CH376--SD��ģʽ*/
	if( s != 0xFA )
	{
		for ( i = 0; i < 50; i ++ ) 
		{   
			mDelaymS(2);              
			s = CH376DiskMount( );  //��ʼ�����̲����Դ����Ƿ����.   
			if ( s == USB_INT_SUCCESS ) /* ׼���� */
			{
					break;  
			} 
			else if ( s == ERR_DISK_DISCON )/* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */    
			{
				break; 
			}  
			if ( CH376GetDiskStatus( ) >= DEF_DISK_MOUNTED && i >= 5 ) /* �е�U�����Ƿ���δ׼����,�������Ժ���,ֻҪ�佨������MOUNTED�ҳ���5*50mS */
			{
				break; 
			}                                   				
		}
		if( ! ByteLoad( "\\SD_AIR\\AIR.TXT", 0, (uint8_t *) bufread, 15 ) )
		{ 
			Byte_CREAT_CON_DIR("\\SD_AIR");	//��������Ŀ¼	( ����Ѿ����ڣ�ֱ�Ӵ� )
			Byte_CREAT_WRITE_PATH( "\\SD_AIR\\AIR.TXT", sdinit,16 );// ������ ���´���
			ByteFill( "\\SD_AIR\\AIR.TXT", 16, (FilePageSize_AIR - sizeof ( sdinit ) + FilePageSize_AIR ));
			SampleSet[Q_AIR].FileNum = 0u;SetFlag = TRUE;
				
		}
		else 
		if( strncmp( (char*)sdinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
		{
			Byte_CREAT_WRITE_PATH( "\\SD_AIR\\AIR.TXT", sdinit,16 );//���� ������ ���´���
			ByteFill( "\\SD_AIR\\AIR.TXT", 16, ( FilePageSize_AIR - sizeof ( sdinit ) + FilePageSize_AIR ));
			SampleSet[Q_AIR].FileNum = 0u;SetFlag = TRUE;
		}
		
		if( ! ByteLoad( "\\SD_TSP\\TSP.TXT", 0, (uint8_t *) bufread, 15 ) )
		{ 
			Byte_CREAT_CON_DIR("\\SD_TSP");
			Byte_CREAT_WRITE_PATH( "\\SD_TSP\\TSP.TXT", sdinit,16 );// ������ ���´���
			ByteFill( "\\SD_TSP\\TSP.TXT",16, ( FilePageSize_TSP - sizeof ( sdinit ) + FilePageSize_TSP ));
			SampleSet[Q_TSP].FileNum = 0;SetFlag = TRUE;
		}
		else 
		if( strncmp( (char*)sdinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
		{
			Byte_CREAT_WRITE_PATH( "\\SD_TSP\\TSP.TXT", sdinit,16 );//���� ������ ���´���
			ByteFill( "\\SD_TSP\\TSP.TXT", 16, (FilePageSize_TSP - sizeof ( sdinit ) + FilePageSize_TSP ));
			SampleSet[Q_TSP].FileNum = 0;SetFlag = TRUE;
		}
		
		if( ! ByteLoad( "\\SD_R24\\R24.TXT", 0, (uint8_t *) bufread, 15 ) )
		{  
			Byte_CREAT_CON_DIR("\\SD_R24");
			Byte_CREAT_WRITE_PATH( "\\SD_R24\\R24.TXT", sdinit,16 );// ������ ���´���
			ByteFill( "\\SD_R24\\R24.TXT", 16, (FilePageSize_R24 - sizeof ( sdinit ) + FilePageSize_R24 ));
			SampleSet[Q_R24].FileNum = 0;SetFlag = TRUE;
		}
		else 
		if( strncmp( (char*)sdinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
		{
			Byte_CREAT_WRITE_PATH( "\\SD_R24\\R24.TXT", sdinit,16 );//���� ������ ���´���
			ByteFill( "\\SD_R24\\R24.TXT", 16, (FilePageSize_R24 - sizeof ( sdinit ) + FilePageSize_R24 ));
			SampleSet[Q_R24].FileNum = 0;SetFlag = TRUE;
		}
			
		if( ! ByteLoad( "\\SD_SHI\\SHI.TXT", 0, (uint8_t  *) bufread, 15 ) )
		{  
			Byte_CREAT_CON_DIR("\\SD_SHI");
			Byte_CREAT_WRITE_PATH( "\\SD_SHI\\SHI.TXT", sdinit,16 );// ������ ���´���
			ByteFill( "\\SD_SHI\\SHI.TXT", 16, (FilePageSize_SHI - sizeof ( sdinit ) + FilePageSize_SHI ));
			SampleSet[Q_SHI].FileNum = 0;SetFlag = TRUE;
		}
		else 
		if( strncmp( (char*)sdinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
		{
			Byte_CREAT_WRITE_PATH( "\\SD_SHI\\SHI.TXT", sdinit,16 );//���� ������ ���´���
			ByteFill( "\\SD_SHI\\SHI.TXT", 16, (FilePageSize_SHI - sizeof ( sdinit ) + FilePageSize_SHI ));
			SampleSet[Q_SHI].FileNum = 0;SetFlag = TRUE;
		}
		
		if( ! ByteLoad( "\\SD_POW\\POWER.TXT", 0, (uint8_t *) bufread, 15 ) )
		{  
			Byte_CREAT_CON_DIR("\\SD_POW");
			Byte_CREAT_WRITE_PATH( "\\SD_POW\\POWER.TXT", sdinit,16 );// ������ ���´���
			ByteFill( "\\SD_POW\\POWER.TXT", 16, ( 100 + 4 -  sizeof ( sdinit ) ));
		}
		else 
		if( strncmp( (char*)sdinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
		{
			Byte_CREAT_WRITE_PATH( "\\SD_POW\\POWER.TXT", sdinit,16 );//���� ������ ���´���
			ByteFill( "\\SD_POW\\POWER.TXT", 16, (100 + 4 - sizeof ( sdinit ) ));
		}
		if(SetFlag == TRUE )
		{
			SampleSetSave();
			SetFlag = FALSE;
		}
	}	
}
/**/  

void File_Creat(void)
{	
	uint8_t USBinit[512]={"USBINITSUCCESSFUL"};	
	Lputs( 0x0000,"Loading." ); 
	if( ! ByteLoad( "\\USB_AIR\\AIR.TXT", 0, (uint8_t *) bufread, 15 ) )
	{ 
		Byte_CREAT_CON_DIR("\\USB_AIR");	//��������Ŀ¼	( ����Ѿ����ڣ�ֱ�Ӵ� )
		Byte_CREAT_WRITE_PATH( "\\USB_AIR\\AIR.TXT", USBinit,512);// ������ ���´���
	}
	else 
	if( strncmp( (char*)USBinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
	{
		Byte_CREAT_WRITE_PATH( "\\USB_AIR\\AIR.TXT", USBinit,512 );//���� ������ ���´���
	}
	Lputs( 0x0000,"Loading.." ); 
	if( ! ByteLoad( "\\USB_TSP\\TSP.TXT", 0, (uint8_t *) bufread, 15 ) )
	{ 
		Byte_CREAT_CON_DIR("\\USB_TSP");
		Byte_CREAT_WRITE_PATH( "\\USB_TSP\\TSP.TXT", USBinit,512 );// ������ ���´���
	}
	else 
	if( strncmp( (char*)USBinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
	{
		Byte_CREAT_WRITE_PATH( "\\USB_TSP\\TSP.TXT", USBinit,512 );//���� ������ ���´���
	}
	Lputs( 0x0000,"Loading..." ); 
	if( ! ByteLoad( "\\USB_R24\\R24.TXT", 0, (uint8_t *) bufread, 15 ) )
	{  
		Byte_CREAT_CON_DIR("\\USB_R24");
		Byte_CREAT_WRITE_PATH( "\\USB_R24\\R24.TXT", USBinit,512 );// ������ ���´���
	}
	else 
	if( strncmp( (char*)USBinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
	{
		Byte_CREAT_WRITE_PATH( "\\USB_R24\\R24.TXT", USBinit,512 );//���� ������ ���´���
	}
	Lputs( 0x0000,"Loading...." ); 	
	if( ! ByteLoad( "\\USB_SHI\\SHI.TXT", 0, (uint8_t  *) bufread, 15 ) )
	{  
		Byte_CREAT_CON_DIR("\\USB_SHI");
		Byte_CREAT_WRITE_PATH( "\\USB_SHI\\SHI.TXT", USBinit,512 );// ������ ���´���
	}
	else 
	if( strncmp( (char*)USBinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
	{
		Byte_CREAT_WRITE_PATH( "\\USB_SHI\\SHI.TXT", USBinit,512 );//���� ������ ���´���
	}
	Lputs( 0x0000,"Loading....." ); 
	if( ! ByteLoad( "\\USB_POW\\POWER.TXT", 0, (uint8_t *) bufread, 15 ) )
	{  
		Byte_CREAT_CON_DIR("\\USB_POW");
		Byte_CREAT_WRITE_PATH( "\\USB_POW\\POWER.TXT", USBinit,512 );// ������ ���´���
	}
	else 
	if( strncmp( (char*)USBinit, (char*)bufread, 15 ) ) //����ļ��Ƿ����� ������Ϊ����
	{
		Byte_CREAT_WRITE_PATH( "\\USB_POW\\POWER.TXT", USBinit,512 );//���� ������ ���´���
	}
	Lputs( 0x0000,"Loading......" ); 
}
uint8_t USB_CHAK(void)
{
	return CH376DiskMount();
}
uint8_t USB_Init(void)
{
	uint8_t s;
	
	s = mInitCH376Host(USB_Flash_disk);   /* ��ʼ��CH376--USB��ģʽ*/
	if( s != USB_INT_SUCCESS )
		return 1;			
	for ( i = 0; i < 20; i ++ ) 
	{  
		mDelaymS(100);	
		if( USB_INT_SUCCESS == CH376DiskConnect( ) )    /* ���U���Ƿ�����,��֧��SD�� */
		{
			for ( i = 0; i < 10; i ++ ) 
			{   
				delay_us( 50000 );              
				s =	USB_CHAK( );  //��ʼ�����̲����Դ����Ƿ����.   
				if ( s == USB_INT_SUCCESS ) /* ׼���� */
				{
					return 0;  
				}
				else if ( s == ERR_DISK_DISCON )/* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */    
				{
					return 3; 
				}  
				if ( CH376GetDiskStatus( ) >= DEF_DISK_MOUNTED && i >= 5 ) /* �е�U�����Ƿ���δ׼����,�������Ժ���,ֻҪ�佨������MOUNTED�ҳ���5*50mS */
				{
					return 4;  
				}                                   				
			}
			
// 	i = CH376ReadBlock( buf );  /* �����Ҫ,���Զ�ȡ���ݿ�CH376_CMD_DATA.DiskMountInq,���س��� */
// 	if ( i == sizeof( INQUIRY_DATA ) )  /* U�̵ĳ��̺Ͳ�Ʒ��Ϣ */
// 	{  
// 		buf[ i ] = 0;
// 	}
// 	s = CH376DiskCapacity( (PUINT32) buf  ); 
// 	s = CH376DiskQuery ( (PUINT32) buf );	/* ��ѯ����ʣ��ռ���Ϣ,������ */	
			
		}
		else
			return 2;
	}
	return 5;
}	
/**/







// /**************** (C) COPYRIGHT 2015 �ൺ���˴���ӿƼ����޹�˾ ****************
