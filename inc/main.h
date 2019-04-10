#ifndef __MAIN_H__
#define __MAIN_H__

//此宏展开后，类似于printf("123"),printf("456");
#define TRACE_CMH_1 (printf("%s(%d)-<%s>: ",__FILE__, __LINE__, __FUNCTION__), printf)

//此宏展开后，类似于printf("%d""%d", 1, 2);
#define TRACE_CMH_2(fmt,...) \
	printf("%s(%d)-<%s>: "##fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define PrintDebugInfo TRACE_CMH_1("debug information\n")

//TRACE_CMH_2("BASE: [%d]\n", count++);
//TRACE_CMH_1("SUB: [%d]\n", count++);

#define CmdBufSize      512
#define MAX_CONNECT     256
#define BUFFER_SIZE     512
//#define LISTENING_PORT  1234

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


typedef struct
{
	char address[128];
	int  port;
	int  localport;
	int  connecttimeout;
	char updateserver_address[128];
	int  updateserver_port;
}configfile;



//typedef char int8_t;
//typedef short int16_t;
//typedef int int32_t;





















#endif /*__MAIN_H__*/
