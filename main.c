#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "des.h"
#include "package.h"

const uint8_t external_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

int main(int argc, char *argv[])
{
    uint8_t inbuf[16];
    uint8_t i;
    uint8_t outbuf[5];
    unsigned int tmpvalue;
    uint16_t outlen;

    uint8_t dessource[8];
    uint8_t destarget[8];

    if (argc != 15)
    {
        printf("Usage: ./CpuCardCalculator xx xx xx xx xx xx xx xx xx xx xx xx xx xx\n");
        printf("E.g  : ./CpuCardCalculator F1 00 0A 01 63 C6 4C B7 55 42 17 96 90 00\n\n");
        return 0;
    }

    for (i = 0; i < argc - 1; i++)
    {
        sscanf(argv[i + 1], "%X", &tmpvalue);
        inbuf[i] = (uint8_t)tmpvalue;
    }

    if (inbuf[0] != 0xF1 || inbuf[1] != 0x00 || inbuf[argc - 3] != 0x90 || inbuf[argc - 2] != 0x00)
    {
        printf("Wrong input data, please try again");
        return 0;
    }

    memcpy(dessource, &inbuf[4], 8);
    Encrypt_3Des_Group(dessource, destarget, (uint8_t*)external_key, 1, 0);

    outbuf[0] = 0x00;
    outbuf[1] = 0x82;
    outbuf[2] = 0x00;
    outbuf[3] = 0x00;
    outbuf[4] = 0x08;
    memcpy(&outbuf[5], destarget, 8);
    outlen = 5 + 8;

    memmove(&outbuf[2], outbuf, outlen);
    outbuf[0] = 0xF1;
    outbuf[1] = (uint8_t)outlen;

    outlen += 2;

    for (i = 0; i < outlen; i++)
    {
        printf("%02X ", outbuf[i]);
    }
    putchar('\n');

    return 0;
}
