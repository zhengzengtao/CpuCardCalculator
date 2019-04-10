#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#include "main.h"

#define DLE			0x10
#define SOH			0x02
#define EOT			0x03
#define SOH_H		0x98
#define SOH_L		0xA0
#define EOT_1		0xA1
#define SOH_RH		0x99
#define SOH_RL		0xA2
#define EOT_R1		0xA3

#define CrcSource	0xFFFF
#define CrcBasic	0x8408


uint16_t CalCrc (uint8_t *inBuff, uint16_t Len);
uint16_t InsertDLE(uint8_t *CharBuff, uint16_t Len, uint8_t dDLE);
void InsertSohEot(uint8_t *CharBuff, uint16_t Len, uint8_t dDLE, uint8_t dSOH, uint8_t dEOT);

uint16_t Cal_Crc16(uint16_t dSourceValue, uint16_t basicValue, uint8_t *dSourceData, uint16_t Len);

void cal_xor(uint8_t *dSourceData1, uint8_t *dSourceData2, uint8_t *dTargetData, uint16_t Len, uint8_t Mode);
uint8_t DataPackage(uint8_t type, uint8_t protocol, uint8_t dSOH_H, uint8_t dSOH_L, uint8_t dEOT_1, uint8_t dDLE, uint8_t dSOH, uint8_t dEOT, uint8_t *ID, uint8_t *cmd, uint16_t dCrcSource, uint16_t dCrcBasic, uint16_t InputLen, uint8_t *DataInput, uint16_t *OutputLen, uint8_t *DataOutput);

#endif /*__PACKAGE_H__*/
