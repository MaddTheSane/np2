#include	"compiler.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"


extern	CS4231CFG	cs4231cfg;


// ---- •âŠ®‚­‚ç‚¢‚µ‚æ‚¤‚æc

REG8 cs4231_nodecode(void) {

	return(0);
}

static REG8 cs4231_pcm8s(void) {

	SINT32	leng;
	UINT	indatas;
	UINT32	addr;
	UINT32	ctime;
	UINT	wpos;

	leng = dmac.dmach[0].leng.w;
	if ((leng >= 2) && (cs4231.bufsize < CS4231_BUFFERS)) {
		indatas = cs4231.bufsize;
		ctime = cs4231.curtime;
		wpos = cs4231.writepos;
		addr = dmac.dmach[0].adrs.d;
		do {
			while(ctime < cs4231.step) {
				ctime += cs4231cfg.rate;
				leng -= 2;
				if (leng < 0) {
					leng = 0;
					goto p8s_stop;
				}
				cs4231.pcmdata[0] = (i286_memoryread(addr + 0) ^ 0x80) << 8;
				cs4231.pcmdata[1] = (i286_memoryread(addr + 1) ^ 0x80) << 8;
				addr += 2;
			}
			ctime -= cs4231.step;
			*(UINT32 *)(cs4231.buffer + wpos) = *(UINT32 *)cs4231.pcmdata,
			wpos = (wpos + 4) & (CS4231_BUFBYTES - 1);
			indatas++;
		} while(indatas < CS4231_BUFFERS);

p8s_stop:
		dmac.dmach[0].leng.w = leng;
		dmac.dmach[0].adrs.d = addr;
		cs4231.bufsize = indatas;
		cs4231.curtime = ctime;
		cs4231.writepos = wpos;
	}
	return((leng < 2)?1:0);
}

static REG8 cs4231_pcm8m(void) {

	SINT32	leng;
	UINT	indatas;
	UINT32	addr;
	UINT32	ctime;
	UINT	wpos;
	SINT16	pcmdata;

	leng = dmac.dmach[0].leng.w;
	if ((leng > 0) && (cs4231.bufsize < CS4231_BUFFERS)) {
		indatas = cs4231.bufsize;
		ctime = cs4231.curtime;
		wpos = cs4231.writepos;
		addr = dmac.dmach[0].adrs.d;
		do {
			while(ctime < cs4231.step) {
				ctime += cs4231cfg.rate;
				leng -= 1;
				if (leng < 0) {
					leng = 0;
					goto p8m_stop;
				}
				pcmdata = (i286_memoryread(addr) ^ 0x80) << 8;
				cs4231.pcmdata[0] = pcmdata;
				cs4231.pcmdata[1] = pcmdata;
				addr += 1;
			}
			ctime -= cs4231.step;
			*(UINT32 *)(cs4231.buffer + wpos) = *(UINT32 *)cs4231.pcmdata,
			wpos = (wpos + 4) & (CS4231_BUFBYTES - 1);
			indatas++;
		} while(indatas < CS4231_BUFFERS);

p8m_stop:
		dmac.dmach[0].leng.w = leng;
		dmac.dmach[0].adrs.d = addr;
		cs4231.bufsize = indatas;
		cs4231.curtime = ctime;
		cs4231.writepos = wpos;
	}
	return((leng == 0)?1:0);
}

static REG8 cs4231_pcm16s(void) {

	SINT32	leng;
	UINT	indatas;
	UINT32	addr;
	UINT32	ctime;
	UINT	wpos;
	UINT16	samp;

	leng = dmac.dmach[0].leng.w;
	if ((leng >= 4) && (cs4231.bufsize < CS4231_BUFFERS)) {
		indatas = cs4231.bufsize;
		ctime = cs4231.curtime;
		wpos = cs4231.writepos;
		addr = dmac.dmach[0].adrs.d;
// TRACEOUT(("addr: %x", addr));
		do {
			while(ctime < cs4231.step) {
				ctime += cs4231cfg.rate;
				leng -= 4;
				if (leng < 0) {
					leng = 0;
					goto p16s_stop;
				}
				samp = i286_memoryread_w(addr+0);
				cs4231.pcmdata[0] = (SINT16)((samp << 8) + (samp >> 8));
				samp = i286_memoryread_w(addr+2);
				cs4231.pcmdata[1] = (SINT16)((samp << 8) + (samp >> 8));
				addr += 4;
			}
			ctime -= cs4231.step;
			*(UINT32 *)(cs4231.buffer + wpos) = *(UINT32 *)cs4231.pcmdata,
			wpos = (wpos + 4) & (CS4231_BUFBYTES - 1);
			indatas++;
		} while(indatas < CS4231_BUFFERS);

p16s_stop:
		dmac.dmach[0].leng.w = leng;
		dmac.dmach[0].adrs.d = addr;
		cs4231.bufsize = indatas;
		cs4231.curtime = ctime;
		cs4231.writepos = wpos;
	}
	return((leng < 4)?1:0);
}

static REG8 cs4231_pcm16m(void) {

	SINT32	leng;
	UINT	indatas;
	UINT32	addr;
	UINT32	ctime;
	UINT	wpos;
	UINT16	samp;

	leng = dmac.dmach[0].leng.w;
	if ((leng >= 2) && (cs4231.bufsize < CS4231_BUFFERS)) {
		indatas = cs4231.bufsize;
		ctime = cs4231.curtime;
		wpos = cs4231.writepos;
		addr = dmac.dmach[0].adrs.d;
		do {
			while(ctime < cs4231.step) {
				ctime += cs4231cfg.rate;
				leng -= 2;
				if (leng < 0) {
					leng = 0;
					goto p16m_stop;
				}
				samp = i286_memoryread_w(addr);
				samp = (UINT16)((samp << 8) + (samp >> 8));
				cs4231.pcmdata[0] = (SINT16)samp;
				cs4231.pcmdata[1] = (SINT16)samp;
				addr += 2;
			}
			ctime -= cs4231.step;
			*(UINT32 *)(cs4231.buffer + wpos) = *(UINT32 *)cs4231.pcmdata,
			wpos = (wpos + 4) & (CS4231_BUFBYTES - 1);
			indatas++;
		} while(indatas < CS4231_BUFFERS);

p16m_stop:
		dmac.dmach[0].leng.w = leng;
		dmac.dmach[0].adrs.d = addr;
		cs4231.bufsize = indatas;
		cs4231.curtime = ctime;
		cs4231.writepos = wpos;
	}
	return((leng < 2)?1:0);
}

const CS4231DMA cs4231dec[16] = {
			cs4231_pcm8m,		// 0: 8bit PCM
			cs4231_pcm8s,
			cs4231_nodecode,	// 1: u-Law
			cs4231_nodecode,
			cs4231_nodecode,	// 2:
			cs4231_nodecode,
			cs4231_nodecode,	// 3: A-law
			cs4231_nodecode,
			cs4231_nodecode,	// 4:
			cs4231_nodecode,
			cs4231_nodecode,	// 5: ADPCM
			cs4231_nodecode,
			cs4231_pcm16m,		// 6: 16bit PCM
			cs4231_pcm16s,
			cs4231_nodecode,	// 7: ADPCM
			cs4231_nodecode};


// ----

void SOUNDCALL cs4231_getpcm(CS4231 cs, SINT32 *pcm, UINT count) {

	UINT	pos;

	count = min(count, cs->bufsize);
	if (count) {
		cs->bufsize -= count;
		pos = cs->readpos;
		do {
			pcm[0] += (*(SINT16 *)(cs->buffer + pos + 0)) >> 1;
			pcm[1] += (*(SINT16 *)(cs->buffer + pos + 2)) >> 1;
			pcm += 2;
			pos = (pos + 4) & (CS4231_BUFBYTES - 1);
		} while(--count);
		cs->readpos = pos;
	}
}

