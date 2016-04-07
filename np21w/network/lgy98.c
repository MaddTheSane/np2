/*
 * QEMU NE2000 emulation
 *
 * Copyright (c) 2003-2004 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * ���ӁF���̃t�@�C���̃I���W�i����QEMU��ne2000.c�ł����A�啝�ȉ��ς��s���Ă��܂��B
 */

#include	"compiler.h"

#if defined(SUPPORT_LGY98)

#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"net.h"
#include	"lgy98dev.h"
#include	"lgy98.h"

#define MAX_ETH_FRAME_SIZE 1514

#define E8390_CMD	0x00  /* The command register (for all pages) */
/* Page 0 register offsets. */
#define EN0_CLDALO	0x01	/* Low byte of current local dma addr  RD */
#define EN0_STARTPG	0x01	/* Starting page of ring bfr WR */
#define EN0_CLDAHI	0x02	/* High byte of current local dma addr  RD */
#define EN0_STOPPG	0x02	/* Ending page +1 of ring bfr WR */
#define EN0_BOUNDARY	0x03	/* Boundary page of ring bfr RD WR */
#define EN0_TSR		0x04	/* Transmit status reg RD */
#define EN0_TPSR	0x04	/* Transmit starting page WR */
#define EN0_NCR		0x05	/* Number of collision reg RD */
#define EN0_TCNTLO	0x05	/* Low  byte of tx byte count WR */
#define EN0_FIFO	0x06	/* FIFO RD */
#define EN0_TCNTHI	0x06	/* High byte of tx byte count WR */
#define EN0_ISR		0x07	/* Interrupt status reg RD WR */
#define EN0_CRDALO	0x08	/* low byte of current remote dma address RD */
#define EN0_RSARLO	0x08	/* Remote start address reg 0 */
#define EN0_CRDAHI	0x09	/* high byte, current remote dma address RD */
#define EN0_RSARHI	0x09	/* Remote start address reg 1 */
#define EN0_RCNTLO	0x0a	/* Remote byte count reg WR */
#define EN0_RTL8029ID0	0x0a	/* Realtek ID byte #1 RD */
#define EN0_RCNTHI	0x0b	/* Remote byte count reg WR */
#define EN0_RTL8029ID1	0x0b	/* Realtek ID byte #2 RD */
#define EN0_RSR		0x0c	/* rx status reg RD */
#define EN0_RXCR	0x0c	/* RX configuration reg WR */
#define EN0_TXCR	0x0d	/* TX configuration reg WR */
#define EN0_COUNTER0	0x0d	/* Rcv alignment error counter RD */
#define EN0_DCFG	0x0e	/* Data configuration reg WR */
#define EN0_COUNTER1	0x0e	/* Rcv CRC error counter RD */
#define EN0_IMR		0x0f	/* Interrupt mask reg WR */
#define EN0_COUNTER2	0x0f	/* Rcv missed frame error counter RD */

#define EN1_PHYS        0x11
#define EN1_CURPAG      0x17
#define EN1_MULT        0x18

#define EN2_STARTPG	0x21	/* Starting page of ring bfr RD */
#define EN2_STOPPG	0x22	/* Ending page +1 of ring bfr RD */

#define EN3_CONFIG0	0x33
#define EN3_CONFIG1	0x34
#define EN3_CONFIG2	0x35
#define EN3_CONFIG3	0x36

/*  Register accessed at EN_CMD, the 8390 base addr.  */
#define E8390_STOP	0x01	/* Stop and reset the chip */
#define E8390_START	0x02	/* Start the chip, clear reset */
#define E8390_TRANS	0x04	/* Transmit a frame */
#define E8390_RREAD	0x08	/* Remote read */
#define E8390_RWRITE	0x10	/* Remote write  */
#define E8390_NODMA	0x20	/* Remote DMA */
#define E8390_PAGE0	0x00	/* Select page chip registers */
#define E8390_PAGE1	0x40	/* using the two high-order bits */
#define E8390_PAGE2	0x80	/* Page 3 is invalid. */

/* Bits in EN0_ISR - Interrupt status register */
#define ENISR_RX	0x01	/* Receiver, no error */
#define ENISR_TX	0x02	/* Transmitter, no error */
#define ENISR_RX_ERR	0x04	/* Receiver, with error */
#define ENISR_TX_ERR	0x08	/* Transmitter, with error */
#define ENISR_OVER	0x10	/* Receiver overwrote the ring */
#define ENISR_COUNTERS	0x20	/* Counters need emptying */
#define ENISR_RDC	0x40	/* remote dma complete */
#define ENISR_RESET	0x80	/* Reset completed */
#define ENISR_ALL	0x3f	/* Interrupts we will enable */

/* Bits in received packet status byte and EN0_RSR*/
#define ENRSR_RXOK	0x01	/* Received a good packet */
#define ENRSR_CRC	0x02	/* CRC error */
#define ENRSR_FAE	0x04	/* frame alignment error */
#define ENRSR_FO	0x08	/* FIFO overrun */
#define ENRSR_MPA	0x10	/* missed pkt */
#define ENRSR_PHY	0x20	/* physical/multicast address */
#define ENRSR_DIS	0x40	/* receiver disable. set in monitor mode */
#define ENRSR_DEF	0x80	/* deferring */

/* Transmitted packet status, EN0_TSR. */
#define ENTSR_PTX 0x01	/* Packet transmitted without error */
#define ENTSR_ND  0x02	/* The transmit wasn't deferred. */
#define ENTSR_COL 0x04	/* The transmit collided at least once. */
#define ENTSR_ABT 0x08  /* The transmit collided 16 times, and was deferred. */
#define ENTSR_CRS 0x10	/* The carrier sense was lost. */
#define ENTSR_FU  0x20  /* A "FIFO underrun" occurred during transmit. */
#define ENTSR_CDH 0x40	/* The collision detect "heartbeat" signal was lost. */
#define ENTSR_OWC 0x80  /* There was an out-of-window collision. */


	LGY98	lgy98;

UINT lgy98_baseaddr = 0x10D0;

static void ne2000_reset(LGY98 *s)
{
    int i;

    s->isr = ENISR_RESET;
    memcpy(s->mem, s->macaddr, 6);
    s->mem[14] = 0x57;
    s->mem[15] = 0x57;

    /* duplicate prom data */
    for(i = 15;i >= 0; i--) {
        s->mem[2 * i] = s->mem[i];
        s->mem[2 * i + 1] = s->mem[i];
    }
}
	
static void ne2000_update_irq(LGY98 *s)
{
    int isr;
    isr = (s->isr & s->imr) & 0x7f;
    //qemu_set_irq(s->irq, (isr != 0));
	if(isr != 0){
		pic_setirq(s->irq);
	}else{
		pic_resetirq(s->irq);
	}
}
	
static void ne2000_mem_writeb(LGY98 *s, UINT32 addr, UINT32 val)
{
    if (addr < 32 ||
        (addr >= NE2000_PMEM_START && addr < NE2000_MEM_SIZE)) {
        s->mem[addr] = val;
    }
}

static void ne2000_mem_writew(LGY98 *s, UINT32 addr, UINT32 val)
{
    addr &= ~1; /* XXX: check exact behaviour if not even */
    if (addr < 32 ||
        (addr >= NE2000_PMEM_START && addr < NE2000_MEM_SIZE)) {
        //*(UINT16 *)(s->mem + addr) = cpu_to_le16(val);
		STOREINTELWORD(s->mem + addr, val);
    }
}

static void ne2000_mem_writel(LGY98 *s, UINT32 addr, UINT32 val)
{
    addr &= ~1; /* XXX: check exact behaviour if not even */
    if (addr < 32 ||
        (addr >= NE2000_PMEM_START && addr < NE2000_MEM_SIZE)) {
        //cpu_to_le32wu((UINT32 *)(s->mem + addr), val);
		STOREINTELDWORD(s->mem + addr, val);
    }
}

static UINT32 ne2000_mem_readb(LGY98 *s, UINT32 addr)
{
    if (addr < 32 ||
        (addr >= NE2000_PMEM_START && addr < NE2000_MEM_SIZE)) {
        return s->mem[addr];
    } else {
        return 0xff;
    }
}

static UINT32 ne2000_mem_readw(LGY98 *s, UINT32 addr)
{
	UINT16 ret = 0xffff;
    addr &= ~1; /* XXX: check exact behaviour if not even */
    if (addr < 32 ||
        (addr >= NE2000_PMEM_START && addr < NE2000_MEM_SIZE)) {
        //return le16_to_cpu(*(UINT16 *)(s->mem + addr));
		ret = LOADINTELWORD(s->mem + addr);
    }
    return (UINT32)ret;
}

static UINT32 ne2000_mem_readl(LGY98 *s, UINT32 addr)
{
	UINT32 ret = 0xffffffff;
    addr &= ~1; /* XXX: check exact behaviour if not even */
    if (addr < 32 ||
        (addr >= NE2000_PMEM_START && addr < NE2000_MEM_SIZE)) {
        //return le32_to_cpupu((UINT32 *)(s->mem + addr));
		ret = LOADINTELDWORD(s->mem + addr);
    }
    return ret;
}

static void ne2000_dma_update(LGY98 *s, int len)
{
    s->rsar += len;
    /* wrap */
    /* XXX: check what to do if rsar > stop */
    if (s->rsar == s->stop)
        s->rsar = s->start;

    if (s->rcnt <= len) {
        s->rcnt = 0;
        /* signal end of transfer */
        s->isr |= ENISR_RDC;
        ne2000_update_irq(s);
		//		TRACEOUT(("LGY-98: DMA_IRQ"));
    } else {
        s->rcnt -= len;
    }
}

void ne2000_send_packet(VLANClientState *vc1, const UINT8 *buf, int size)
{
    VLANState *vlan = vc1->vlan;
    //VLANClientState *vc;

    if (vc1->link_down)
        return;

#ifdef DEBUG_NET
    printf("vlan %d send:\n", vlan->id);
    hex_dump(stdout, buf, size);
#endif
	np2net.send_packet((UCHAR*)buf, size);
    /*for(vc = vlan->first_client; vc != NULL; vc = vc->next) {
        if (vc != vc1 && !vc->link_down) {
            vc->fd_read(vc->opaque, buf, size);
        }
    }*/
}


#define MIN_BUF_SIZE 60

static void pc98_ne2000_cleanup(VLANClientState *vc)
{
    LGY98 *s = (LGY98*)vc->opaque;

    //unregister_savevm("ne2000", s);

    //isa_unassign_ioport(s->isa_io_base, 16);
    //isa_unassign_ioport(s->isa_io_base + 0x200, 2);
    //isa_unassign_ioport(s->isa_io_base + 0x300, 16);
    //isa_unassign_ioport(s->isa_io_base + 0x18, 1);

    //qemu_free(s);
}

static int ne2000_buffer_full(LGY98 *s)
{
    int avail, index, boundary;

    index = s->curpag << 8;
    boundary = s->boundary << 8;
    if (index < boundary)
        avail = boundary - index;
    else
        avail = (s->stop - s->start) - (index - boundary);
    if (avail < (MAX_ETH_FRAME_SIZE + 4))
        return 1;
    return 0;
}

static int ne2000_can_receive(void *opaque)
{
    LGY98 *s = (LGY98*)opaque;

    if (s->cmd & E8390_STOP)
        return 1;
    return !ne2000_buffer_full(s);
}

/* From FreeBSD */
static int compute_mcast_idx(const UINT8 *ep)
{
    UINT32 crc;
    int carry, i, j;
    UINT8 b;

    crc = 0xffffffff;
    for (i = 0; i < 6; i++) {
        b = *ep++;
        for (j = 0; j < 8; j++) {
            carry = ((crc & 0x80000000L) ? 1 : 0) ^ (b & 0x01);
            crc <<= 1;
            b >>= 1;
            if (carry)
                crc = ((crc ^ 0x04c11db6) | carry);
        }
    }
    return (crc >> 26);
}

static void ne2000_receive(void *opaque, const UINT8 *buf, int size)
{
	LGY98 *s = &lgy98;
    UINT8 *p;
    unsigned int total_len, next, avail, len, index, mcast_idx;
    UINT8 buf1[60];
    static const UINT8 broadcast_macaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#if defined(DEBUG_NE2000)
    printf("NE2000: received len=%d\n", size);
#endif

    if (s->cmd & E8390_STOP || ne2000_buffer_full(s))
        return;

    /* XXX: check this */
    if (s->rxcr & 0x10) {
        /* promiscuous: receive all */
    } else {
        if (!memcmp(buf,  broadcast_macaddr, 6)) {
            /* broadcast address */
            if (!(s->rxcr & 0x04))
                return;
        } else if (buf[0] & 0x01) {
            /* multicast */
            if (!(s->rxcr & 0x08))
                return;
            mcast_idx = compute_mcast_idx(buf);
            if (!(s->mult[mcast_idx >> 3] & (1 << (mcast_idx & 7))))
                return;
        } else if (s->mem[0] == buf[0] &&
                   s->mem[2] == buf[1] &&
                   s->mem[4] == buf[2] &&
                   s->mem[6] == buf[3] &&
                   s->mem[8] == buf[4] &&
                   s->mem[10] == buf[5]) {
            /* match */
        } else {
            return;
        }
    }


    /* if too small buffer, then expand it */
    if (size < MIN_BUF_SIZE) {
        memcpy(buf1, buf, size);
        memset(buf1 + size, 0, MIN_BUF_SIZE - size);
        buf = buf1;
        size = MIN_BUF_SIZE;
    }

    index = s->curpag << 8;
    /* 4 bytes for header */
    total_len = size + 4;
    /* address for next packet (4 bytes for CRC) */
    next = index + ((total_len + 4 + 255) & ~0xff);
    if (next >= s->stop)
        next -= (s->stop - s->start);
    /* prepare packet header */
    p = s->mem + index;
    s->rsr = ENRSR_RXOK; /* receive status */
    /* XXX: check this */
    if (buf[0] & 0x01)
        s->rsr |= ENRSR_PHY;
    p[0] = s->rsr;
    p[1] = next >> 8;
    p[2] = total_len;
    p[3] = total_len >> 8;
    index += 4;

    /* write packet data */
    while (size > 0) {
        if (index <= s->stop)
            avail = s->stop - index;
        else
            avail = 0;
        len = size;
        if (len > avail)
            len = avail;
        memcpy(s->mem + index, buf, len);
        buf += len;
        index += len;
        if (index == s->stop)
            index = s->start;
        size -= len;
    }
    s->curpag = next >> 8;

    /* now we can signal we have received something */
    s->isr |= ENISR_RX;
    ne2000_update_irq(s);
	//TRACEOUT(("LGY-98: RCV_IRQ"));
}




// **************************  I/O PORT  **************

static void IOOUTCALL lgy98_ob000(UINT addr, REG8 dat) {
    int offset, page, index;
	UINT32 val = dat;
	LGY98 *s = &lgy98;
	
	//TRACEOUT(("LGY-98: out %04X d=%02X", addr, dat));
    addr &= 0xf;
    if (addr == E8390_CMD) {
        /* control register */
        s->cmd = dat;
        if (!(dat & E8390_STOP)) { /* START bit makes no sense on RTL8029... */
            s->isr &= ~ENISR_RESET;
            /* test specific case: zero length transfer */
            if ((dat & (E8390_RREAD | E8390_RWRITE)) &&
                s->rcnt == 0) {
                s->isr |= ENISR_RDC;
                ne2000_update_irq(s);
				//TRACEOUT(("LGY-98: RRW_IRQ"));
            }
            if (dat & E8390_TRANS) {
                index = (lgy98.tpsr << 8);
                /* XXX: next 2 lines are a hack to make netware 3.11 work */
                if (index >= NE2000_PMEM_END)
                    index -= NE2000_PMEM_SIZE;
                /* fail safe: check range on the transmitted length  */
                if (index + s->tcnt <= NE2000_PMEM_END) {
                    ne2000_send_packet(s->vc, s->mem + index, s->tcnt);
                }
                /* signal end of transfer */
                s->tsr = ENTSR_PTX;
                s->isr |= ENISR_TX;
                s->cmd &= ~E8390_TRANS;
				//TRACEOUT(("LGY-98: SEND_IRQ"));
                ne2000_update_irq(s);
            }
        }
    } else {
        page = s->cmd >> 6;
        offset = addr | (page << 4);
        switch(offset) {
        case EN0_STARTPG:
            s->start = val << 8;
            break;
        case EN0_STOPPG:
            s->stop = val << 8;
            break;
        case EN0_BOUNDARY:
            s->boundary = val;
            break;
        case EN0_IMR:
            s->imr = val;
			//TRACEOUT(("LGY-98: IMR_IRQ"));
			//pic_resetirq(s->irq);
            ne2000_update_irq(s);
            break;
        case EN0_TPSR:
            s->tpsr = val;
            break;
        case EN0_TCNTLO:
            s->tcnt = (s->tcnt & 0xff00) | val;
            break;
        case EN0_TCNTHI:
            s->tcnt = (s->tcnt & 0x00ff) | (val << 8);
            break;
        case EN0_RSARLO:
            s->rsar = (s->rsar & 0xff00) | val;
            break;
        case EN0_RSARHI:
            s->rsar = (s->rsar & 0x00ff) | (val << 8);
            break;
        case EN0_RCNTLO:
            s->rcnt = (s->rcnt & 0xff00) | val;
            break;
        case EN0_RCNTHI:
            s->rcnt = (s->rcnt & 0x00ff) | (val << 8);
            break;
        case EN0_RXCR:
            s->rxcr = val;
            break;
        case EN0_DCFG:
            s->dcfg = val;
            break;
        case EN0_ISR:
            s->isr &= ~(val & 0x7f);
			//TRACEOUT(("LGY-98: ISR_IRQ"));
			//pic_resetirq(s->irq);
            ne2000_update_irq(s);
            break;
		case EN1_PHYS:
		case EN1_PHYS + 1:
		case EN1_PHYS + 2:
		case EN1_PHYS + 3:
		case EN1_PHYS + 4:
		case EN1_PHYS + 5:
            s->phys[offset - EN1_PHYS] = val;
            break;
        case EN1_CURPAG:
            s->curpag = val;
            break;
		case EN1_MULT:
		case EN1_MULT + 1:
		case EN1_MULT + 2:
		case EN1_MULT + 3:
		case EN1_MULT + 4:
		case EN1_MULT + 5:
		case EN1_MULT + 6:
		case EN1_MULT + 7:
            s->mult[offset - EN1_MULT] = val;
            break;
        }
    }
	(void)addr;
	(void)dat;
}
static REG8 IOINPCALL lgy98_ib000(UINT addr) {
	int ret = 0;
    int offset, page;
	LGY98 *s = &lgy98;
	
	//pic_resetirq(s->irq);
	//TRACEOUT(("LGY-98: inp %04X", addr));
    addr = (addr & 0xf);
    if (addr == E8390_CMD) {
        ret = s->cmd;
    } else {
        page = s->cmd >> 6;
        offset = addr | (page << 4);
        switch(offset) {
        case EN0_TSR:
            ret = s->tsr;
            break;
        case EN0_BOUNDARY:
            ret = s->boundary;
            break;
        case EN0_ISR:
            ret = s->isr;
            break;
		case EN0_RSARLO:
			ret = s->rsar & 0x00ff;
			break;
		case EN0_RSARHI:
			ret = s->rsar >> 8;
			break;
		case EN1_PHYS:
		case EN1_PHYS + 1:
		case EN1_PHYS + 2:
		case EN1_PHYS + 3:
		case EN1_PHYS + 4:
		case EN1_PHYS + 5:
            ret = s->phys[offset - EN1_PHYS];
            break;
        case EN1_CURPAG:
            ret = s->curpag;
            break;
		case EN1_MULT:
		case EN1_MULT + 1:
		case EN1_MULT + 2:
		case EN1_MULT + 3:
		case EN1_MULT + 4:
		case EN1_MULT + 5:
		case EN1_MULT + 6:
		case EN1_MULT + 7:
            ret = s->mult[offset - EN1_MULT];
            break;
        case EN0_RSR:
            ret = s->rsr;
            break;
        case EN2_STARTPG:
            ret = s->start >> 8;
            break;
        case EN2_STOPPG:
            ret = s->stop >> 8;
            break;
	case EN0_RTL8029ID0:
	    ret = 0x50;
	    break;
	case EN0_RTL8029ID1:
	    ret = 0x43;
	    break;
	case EN3_CONFIG0:
	    ret = 0;		/* 10baseT media */
	    break;
	case EN3_CONFIG2:
	    ret = 0x40;		/* 10baseT active */
	    break;
	case EN3_CONFIG3:
	    ret = 0x40;		/* Full duplex */
	    break;
        default:
            ret = 0x00;
            break;
        }
    }
    return (REG8)ret;
}
void IOOUTCALL lgy98_ob200_8(UINT addr, REG8 dat) {
	UINT32 val = dat;
	LGY98 *s = &lgy98;
	
	//pic_resetirq(s->irq);
    if (s->rcnt == 0)
        return;
	if(s->dcfg & 0x01){
		//TRACEOUT(("LGY-98: Illigal 16 bit write!!!"));
        ne2000_mem_writew(s, s->rsar, val);
        ne2000_dma_update(s, 2);
        return;
	}else{
		//if (s->dcfg & 0x01) {
		//    /* 16 bit access */
		//    ne2000_mem_writew(s, s->rsar, val);
		//    ne2000_dma_update(s, 2);
		//} else {
        /* 8 bit access */
		//TRACEOUT(("LGY-98: 8 bit write"));
        ne2000_mem_writeb(s, s->rsar, val);
        ne2000_dma_update(s, 1);
		//}
	}
	(void)addr;
	(void)dat;
}
REG8 IOINPCALL lgy98_ib200_8(UINT addr) {
	int ret = 0;
	LGY98 *s = &lgy98;
	
	//pic_resetirq(s->irq);
	if(s->dcfg & 0x01){
		//TRACEOUT(("LGY-98: Illigal 16 bit read!!!"));
		ret = ne2000_mem_readw(s, s->rsar);
		ne2000_dma_update(s, 2);
        return (REG8)ret;
	}else{
		//TRACEOUT(("LGY-98: 8 bit read"));
		/* 8 bit access */
		ret = ne2000_mem_readb(s, s->rsar);
		ne2000_dma_update(s, 1);
#ifdef DEBUG_NE2000
		printf("NE2000: asic read val=0x%04x\n", ret);
#endif
		return (REG8)ret;
	}
}
void IOOUTCALL lgy98_ob200_16(UINT addr, REG16 dat) {
	UINT32 val = dat;
	LGY98 *s = &lgy98;
	
	//pic_resetirq(s->irq);
    if (s->rcnt == 0)
        return;
	if(s->dcfg & 0x01){
		/* 16 bit access */
		//TRACEOUT(("LGY-98: 16 bit write"));
		ne2000_mem_writew(s, s->rsar, val);
		ne2000_dma_update(s, 2);
	}else{
		//TRACEOUT(("LGY-98: Illigal 8 bit write!!!"));
        ne2000_mem_writeb(s, s->rsar, val);
        ne2000_dma_update(s, 1);
	}
	(void)addr;
	(void)dat;
}
REG16 IOINPCALL lgy98_ib200_16(UINT addr) {
	int ret = 0;
	LGY98 *s = &lgy98;
	
	//pic_resetirq(s->irq);
	if(s->dcfg & 0x01){
		/* 16 bit access */
		//TRACEOUT(("LGY-98: 16 bit read"));
		ret = ne2000_mem_readw(s, s->rsar);
		ne2000_dma_update(s, 2);
#ifdef DEBUG_NE2000
		printf("NE2000: asic read val=0x%04x\n", ret);
#endif
	}else{
		//TRACEOUT(("LGY-98: Illigal 8 bit read!!!"));
		ret = ne2000_mem_readb(s, s->rsar);
		ne2000_dma_update(s, 1);
	}
    return (REG16)ret;
}

static void IOOUTCALL lgy98_ob300_16(UINT port, REG8 dat) {
	//TRACEOUT(("LGY-98: out %04X d=%02X", port, dat));
	//pic_resetirq(lgy98.irq);
	/* Nothing to do */
	(void)port;
	(void)dat;
}
static REG8 IOINPCALL lgy98_ib300_16(UINT port) {
	//TRACEOUT(("LGY-98: inp %04X", port));
	//pic_resetirq(lgy98.irq);
    switch (port & 0x0f) {
    case 0x0a:
        return 0x00;
    case 0x0b:
        return 0x40;
    case 0x0c:
        return 0x26;
    case 0x0d:
        return 0x0b;
    }
    return 0xff;
}
static void IOOUTCALL lgy98_ob018(UINT port, REG8 dat) {
	//TRACEOUT(("LGY-98: out %04X d=%02X", port, dat));
	//pic_resetirq(lgy98.irq);
	/* Nothing to do */
	(void)port;
	(void)dat;
}
static REG8 IOINPCALL lgy98_ib018(UINT port) {
	LGY98 *s = &lgy98;
	//TRACEOUT(("LGY-98: inp %04X reset", port));
	//pic_resetirq(s->irq);
    ne2000_reset(s);
	return 0;
}

static VLANState np2net_vlan;
/* find or alloc a new VLAN */
VLANState *np2net_find_vlan(int id)
{
    np2net_vlan.id = id;
    np2net_vlan.next = NULL;
    return &np2net_vlan;
}

VLANClientState *np2net_new_vlan_client(VLANState *vlan,
                                      const char *model,
                                      const char *name,
                                      IOReadHandler *fd_read,
                                      IOCanRWHandler *fd_can_read,
                                      NetCleanup *cleanup,
                                      void *opaque)
{
    VLANClientState *vc, **pvc;
    vc = (VLANClientState*)calloc(1, sizeof(VLANClientState));
    vc->model = strdup(model);
    //if (name)
        vc->name = strdup(name);
    //else
    //    vc->name = assign_name(vc, model);
    vc->fd_read = fd_read;
    vc->fd_can_read = fd_can_read;
    vc->cleanup = cleanup;
    vc->opaque = opaque;
    vc->vlan = vlan;

    vc->next = NULL;
    pvc = &vlan->first_client;
    while (*pvc != NULL)
        pvc = &(*pvc)->next;
    *pvc = vc;
    return vc;
}


void lgy98_reset(const NP2CFG *pConfig){

}
// �p�P�b�g��M���ɌĂ΂��
static void lgy98_recieve_packet(const UINT8 *buf, int size)
{
	lgy98.vc->fd_read(&lgy98, buf, size);
}

void lgy98_bind(void){
	int i;
	UINT base = 0x10D0;
	REG8 irq = 5;
	REG8 macaddr[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
    VLANState *vlan;
	//NICInfo *nd;

	if(!np2cfg.uselgy98) return;
	if(np2cfg.lgy98io) base = np2cfg.lgy98io;
	if(np2cfg.lgy98irq) irq = np2cfg.lgy98irq;
	
	TRACEOUT(("LGY-98: I/O:%04X, IRQ:%d, TAP:%s", base, irq, np2cfg.np2nettap));

    vlan = np2net_find_vlan(0);

	lgy98_baseaddr = base;
	//
	//if(np2net_reset(np2cfg.lgy98tap)){
	//	TRACEOUT(("LGY-98: reset falied"));
	//	return;
	//}

    //np2net_check_nic_model(nd, "ne2k_isa");

	memset(&lgy98, 0, sizeof(lgy98));
	
	lgy98.base = base;
	lgy98.irq = irq;
	memcpy(lgy98.macaddr, macaddr, 6);
	for(i=0;i<16;i++){
		iocore_attachout(base + i, lgy98_ob000);
		iocore_attachinp(base + i, lgy98_ib000);
	}
	
	iocore_attachout(base + 0x200, lgy98_ob200_8);
	iocore_attachinp(base + 0x200, lgy98_ib200_8);
	//iocore_attachout(base + 0x200, lgy98_ob200_16); // in iocore.c iocore_out16()
	//iocore_attachinp(base + 0x200, lgy98_ib200_16); // in iocore.c iocore_inp16()
	
	for(i=0;i<16;i++){
		iocore_attachout(base + 0x300 + i, lgy98_ob300_16);
		iocore_attachinp(base + 0x300 + i, lgy98_ib300_16);
	}

	iocore_attachout(base + 0x018, lgy98_ob018);
	iocore_attachinp(base + 0x018, lgy98_ib018);
	
	ne2000_reset(&lgy98);
	
    lgy98.vc = np2net_new_vlan_client(vlan, "ne2k_isa", "ne2k_isa.1",
                                          ne2000_receive, ne2000_can_receive,
                                          pc98_ne2000_cleanup, &lgy98);

	np2net.recieve_packet = lgy98_recieve_packet;
	
}

#endif	/* SUPPORT_LGY98 */