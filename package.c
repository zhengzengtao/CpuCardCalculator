/* Moudle Name: 	Standard Data Package
 *
 *
 * Version:	1.0
 * Author:	ZZT
 * Date:		2019.01.30
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "package.h"

#define DataBufferSize CmdBufSize

void cal_xor(uint8_t *dSourceData1, uint8_t *dSourceData2, uint8_t *dTargetData, uint16_t Len, uint8_t Mode)
{
	uint16_t i;
	for (i = 0; i < Len; i++)
	{
		if (Mode == 0)
			dTargetData[i] = dSourceData1[i] ^ dSourceData2[0];
		else if (Mode == 1)
			dTargetData[i] = dSourceData1[i] ^ dSourceData2[i];
		else if (Mode == 2)
			dTargetData[0] ^= dSourceData1[i];
	}
}

uint16_t Cal_Crc16(uint16_t dSourceValue, uint16_t basicValue, uint8_t *dSourceData, uint16_t Len)
{
	uint16_t i=0, i1=0;
	uint8_t tmpChar=0;
	uint16_t CRC16=dSourceValue;

    for (i1=0; i1<Len; i1++)
	{
		tmpChar=dSourceData[i1];
		for (i=0; i<8; i++)
		{
			if ((CRC16 & 1) ^ (tmpChar & 1))
			{
				CRC16 >>= 1;
				CRC16 ^= basicValue;
			}
			else
			{
				CRC16 >>= 1;
			}

			tmpChar >>= 1;

		}
	}
    return CRC16;
}

uint16_t InsertDLE(uint8_t *CharBuff, uint16_t Len, uint8_t dDLE)
{
	uint16_t i;
	uint16_t tmpLen;
	/* uint8_t tmpChar[DataBufferSize]; */

	uint8_t *tmpChar = (uint8_t *)malloc(DataBufferSize);
	if (tmpChar==NULL)
	{
		return 0;
	}

#if 0
	for (i=0; i<Len; i++)
	{
		tmpChar[i]=CharBuff[i];
	}
#endif
	
	memcpy (tmpChar, CharBuff, Len);
	tmpLen=0;
	for (i=0; i<Len; i++)
	{
		CharBuff[tmpLen]=tmpChar[i];
		if (tmpChar[i]==dDLE)
		{
			tmpLen++;
			CharBuff[tmpLen]=tmpChar[i];
		}
		tmpLen++;
	}

	free(tmpChar);
	return tmpLen;
}

void InsertSohEot(uint8_t *CharBuff, uint16_t Len, uint8_t dDLE, uint8_t dSOH, uint8_t dEOT)
{
	uint16_t i=0;
	for (i=Len; i>0; i--)
	{
		CharBuff[i+2-1]=CharBuff[i-1];
	}
	CharBuff[0]=dDLE;
	CharBuff[1]=dSOH;
	CharBuff[Len+2]=dDLE;
	CharBuff[Len+3]=dEOT;
}

uint16_t DelDLE(uint8_t *CharBuff, uint16_t Len, uint8_t dDLE)
{
	uint16_t i=0;
	uint16_t tmpLen;
	/* uint8_t tmpChar[DataBufferSize]; */

	uint8_t *tmpChar = (uint8_t *)malloc(DataBufferSize);
	if (tmpChar==NULL)
	{
		return 0;
	}

	memcpy (tmpChar, CharBuff, Len);

	tmpLen=0;
	for (i=0; i<Len; i++)
	{
		CharBuff[tmpLen]=tmpChar[i];
		if (tmpChar[i]==dDLE)
		{
			i++;
		}
		tmpLen++;
	}

	free(tmpChar);
	return tmpLen;
}

uint16_t DeleteSohEot(uint8_t *data, uint16_t len, uint8_t dDLE, uint8_t dSOH, uint8_t dEOT)
{
	if (data[0]==dDLE && data[1]==dSOH && data[len-2]==dDLE && data[len-1]==dEOT)
	{
		memmove (data, &data[2], len-4);
		return (len-4);
	}
	else
		return 0;
}

/* type:		0- Package;  1- Unpack;
 * protocol:	0- private(10 02/10 03);  1- public(98 A0 A1/99 A2 A3;
 */
uint8_t DataPackage(uint8_t type, uint8_t protocol, uint8_t dSOH_H, uint8_t dSOH_L, uint8_t dEOT_1, uint8_t dDLE, uint8_t dSOH, uint8_t dEOT, uint8_t *ID, uint8_t *cmd, uint16_t dCrcSource, uint16_t dCrcBasic, uint16_t InputLen, uint8_t *DataInput, uint16_t *OutputLen, uint8_t *DataOutput)
{
	uint16_t tmpLen;
	/* uint8_t tmpBuf[DataBufferSize]; */

	uint16_t tmpCrc;
	uint8_t tmpLrc;
	
	uint16_t i, i1, i2;

	uint8_t *tmpBuf = (uint8_t *)malloc(DataBufferSize);
	if (tmpBuf == NULL)
	{
		return 10;
	}
	

	if (protocol == 0)
	{
		if (type == 0)	/* package */
		{
			memcpy (&tmpBuf[1], DataInput, InputLen);
			tmpLen = InputLen + 1;
			tmpBuf[0] = *cmd;
	
			tmpCrc = Cal_Crc16(dCrcSource, dCrcBasic, tmpBuf, tmpLen);
			tmpBuf[tmpLen] = (uint8_t)tmpCrc;
			tmpBuf[tmpLen + 1] = (uint8_t)(tmpCrc >> 8);
			tmpLen = InsertDLE(tmpBuf, tmpLen + 2, dDLE);	/* Len+2 bytes Crc, return new len */
			InsertSohEot(tmpBuf, tmpLen, dDLE, dSOH, dEOT);
			*OutputLen = tmpLen + 4;
			memcpy (DataOutput, tmpBuf, *OutputLen);
	
		}
		else if (type == 1)	/* unpackage */
		{
			if (InputLen < 7)	/* DLE, SOH, CMD, CRC_H, CRC_L DLE, EOT 必须有的7个字节 */
			{
				free(tmpBuf);
				return 1;	/* Wrong SOH */
			}
	
			i1 = 0;
			for (i = 1; i < InputLen - (7 - 2); i++)
			{
				if (DataInput[i - 1] == dDLE && DataInput[i] == dSOH)
				{
					i1 = i;
					i = InputLen;
				}
			}
	
			for (i = i1 + 5; i < InputLen; i++)
			{
				if (DataInput[i - 1] == dDLE && DataInput[i] == dEOT)
				{
					i2 = i;
					i = InputLen;
				}
			}
	
			tmpLen = i2 - i1 + 2;
			memcpy (tmpBuf, &DataInput[i1 - 1], tmpLen);
	
			i = DeleteSohEot(tmpBuf, tmpLen, dDLE, dSOH, dEOT);
			if (i == 0)
			{
				free(tmpBuf);
				return 6;
			}
			tmpLen = i;
	
			i = DelDLE(tmpBuf, tmpLen, dDLE);
			if (i == 0)
			{
				free(tmpBuf);
				return 7;
			}
			tmpLen = i;
	
			tmpCrc=Cal_Crc16(dCrcSource, dCrcBasic, tmpBuf, tmpLen - 2);
			if (tmpBuf[tmpLen - 2] == (uint8_t)tmpCrc && tmpBuf[tmpLen - 1] == (uint8_t)(tmpCrc >> 8))
				tmpLen -= 2;
			else
			{
				free(tmpBuf);
				return 8;
			}
	
			tmpLen--;
			*cmd = tmpBuf[0];
	
			*OutputLen = tmpLen;
			memcpy(DataOutput, &tmpBuf[1], tmpLen);
		}
		else
		{
			free(tmpBuf);
			return 3;
		}
	
	}
	else if (protocol == 1)
	{
		if (type == 0)	/* package */
		{
			DataOutput[0] = dSOH_H;
			DataOutput[1] = dSOH_L;
			DataOutput[2] = *ID;
			DataOutput[3] = *cmd;
			DataOutput[4] = (uint8_t)(InputLen & 0xFF);
			if (InputLen)
				memcpy(&DataOutput[5], DataInput, InputLen);
			*OutputLen = InputLen + 7;
			tmpLrc = 0;
			for (i = 0; i < InputLen+5; i++)
				tmpLrc ^= DataOutput[i];
			DataOutput[5 + InputLen] = tmpLrc;
			DataOutput[6 + InputLen] = dEOT_1;
		}
		else if (type == 1)	/* unpackage */
		{
			if (InputLen < 7)	/* SOH_H, SOH_L, ID, CMD, LEN, LRC, EOT  必须有的7个字节 */
			{
				free(tmpBuf);
				return 1;	/* Wrong SOH */
			}
		
			i1 = 0;
			for (i = 1; i < InputLen - (7 - 2); i++)
			{
				if (DataInput[i - 1] == dSOH_H && DataInput[i] == dSOH_L)
				{
					i1 = i;
					i = (uint8_t)(InputLen & 0xFF);
				}
			}
		
			tmpLen = DataInput[i1 + 3] + 7;
	
			if (tmpLen + (i1 - 1) > InputLen)
			{
				free(tmpBuf);
				return 2;	/* Wrong Lenght */
			}
		
		
			if (DataInput[tmpLen - 1 + (i1 - 1)] != dEOT_1)
			{
				free(tmpBuf);
				return 3;	/* Wrong EOT */
			}
	
	
			tmpLrc = 0;
			for (i = i1-1; i < tmpLen - 1; i++)
				tmpLrc ^= DataInput[i];
	
			if (tmpLrc)
			{
				free(tmpBuf);
				return 4;	/* Wrong lrc */
			}
	
			*ID = DataInput[1 + i1];
			*cmd = DataInput[2 + i1];
			*OutputLen = DataInput[3 + i1];
			
			if (*OutputLen)
				memcpy(DataOutput, &DataInput[4 + i1], *OutputLen);
		}
		else
		{
			free(tmpBuf);
			return 3;
		}
	}
	else
	{
		free(tmpBuf);
		return 3;
	}

	free(tmpBuf);
	return 0;
}
