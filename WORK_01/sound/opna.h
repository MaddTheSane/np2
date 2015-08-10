/**
 * @file	opna.h
 * @brief	Interface of OPNA
 */

#pragma once

/**
 * @brief opna
 */
struct tagOpna
{
	UINT8	addr1l;
	UINT8	addr1h;
	UINT8	addr2l;
	UINT8	addr2h;
	UINT8	data1;
	UINT8	data2;
	UINT16	base;
	UINT8	adpcmmask;
	UINT8	channels;
	UINT8	extend;
	UINT8	_padding2;
	UINT8	reg[0x400];
};

typedef struct tagOpna OPN_T;
typedef struct tagOpna* POPNA;
