/**
 * @file	pcm86io.h
 * @brief	Interface of the 86-PCM I/O
 */

#pragma once

/**
 * @brief DSP
 */
struct tagDsp73
{
	unsigned char ctrl;
	unsigned char cmd;
	unsigned char data;
	unsigned char addr;
	unsigned int nIndex;
	unsigned char iwram[256 * 3];
};
typedef struct tagDsp73		DSP73;

#ifdef __cplusplus
extern "C"
{
#endif

extern DSP73 g_dsp73;

void pcm86io_setopt(REG8 cDipSw);
void pcm86io_bind(void);

#ifdef __cplusplus
}
#endif

