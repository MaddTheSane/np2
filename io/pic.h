
typedef struct {
	int		levels;
	BYTE	level[8];

	int		levelsbak;
	BYTE	levelbak[8];

	BYTE	pry[8];
	BYTE	icw[4];

	BYTE	imr;			// ocw1
	BYTE	ocw3;
	BYTE	irr;
	BYTE	ext;

	BYTE	isr;
	BYTE	isrbak;
	BYTE	writeicw;
	BYTE	padding;
} _PICITEM, *PICITEM;


typedef struct {
	_PICITEM	pi[2];
	BYTE		ext_irq;
} _PIC, *PIC;

enum {
	PIC_SYSTEMTIMER		= 0x01,
	PIC_KEYBOARD		= 0x02,
	PIC_CRTV			= 0x04,
	PIC_INT0			= 0x08,
	PIC_RS232C			= 0x10,
	PIC_INT1			= 0x20,
	PIC_INT2			= 0x40,
	PIC_SLAVE			= 0x80,

	PIC_PRINTER			= 0x01,
	PIC_INT3			= 0x02,
	PIC_INT41			= 0x04,
	PIC_INT42			= 0x08,
	PIC_INT5			= 0x10,
	PIC_INT6			= 0x20,
	PIC_NDP				= 0x40,

	IRQ_INT0			= 0x03,
	IRQ_INT1			= 0x05,
	IRQ_INT2			= 0x06,
	IRQ_INT3			= 0x09,
	IRQ_INT41			= 0x0a,
	IRQ_INT42			= 0x0b,
	IRQ_INT5			= 0x0c,
	IRQ_INT6			= 0x0d
};


#ifdef __cplusplus
extern "C" {
#endif

void extirq_push(void);
void extirq_pop(void);

void pic_irq(void);
void pic_setirq(BYTE irq);
void pic_resetirq(BYTE irq);
void pic_registext(BYTE irq);

void picmask(NEVENTITEM item);

void pic_reset(void);
void pic_bind(void);

#ifdef __cplusplus
}
#endif

