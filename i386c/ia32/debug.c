/*	$Id: debug.c,v 1.5 2004/01/26 15:23:55 monaka Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "cpu.h"
#include "memory.h"
#ifdef USE_FPU
#include "instructions/fpu/fpu.h"
#endif


char *cpu_reg2str(void)
{
  static char buf[512];

  sprintf(buf,
    "eax=%08x ebx=%08x ecx=%08x edx=%08x esi=%08x edi=%08x\n"
    "eip=%08x esp=%08x ebp=%08x prev_eip=%08x\n"
    "cs=%04x  ss=%04x  ds=%04x  es=%04x  fs=%04x  gs=%04x\n"
    "eflag=%08x "
/* ID VIP VIF AC VM RF NT IOPL OF DF IF TF SF ZF AF PF CF */
      "[ ID=%d VIP=%d VIF=%d AC=%d VM=%d RF=%d NT=%d IOPL=%d%d %s %s %s TF=%d %s %s %s %s %s ]\n"
    "gdtr=%08x:%04x idtr=%08x:%04x\n"
    "ldtr=%04x(%08x:%04x) tr=%04x(%08x:%04x)\n"
    "cr0=%08x cr1=%08x cr2=%08x cr3=%08x cr4=%08x mxcsr=%08x",
    CPU_EAX, CPU_EBX, CPU_ECX, CPU_EDX, CPU_ESI, CPU_EDI,
    CPU_EIP, CPU_ESP, CPU_EBP, CPU_PREV_EIP,
    CPU_CS, CPU_SS, CPU_DS, CPU_ES, CPU_FS, CPU_GS,
    CPU_EFLAG,
      (CPU_EFLAG & ID_FLAG) != 0,
      (CPU_EFLAG & VIP_FLAG) != 0,
      (CPU_EFLAG & VIF_FLAG) != 0,
      (CPU_EFLAG & AC_FLAG) != 0,
      (CPU_EFLAG & VM_FLAG) != 0,
      (CPU_EFLAG & RF_FLAG) != 0,
      (CPU_EFLAG & NT_FLAG) != 0,
      CPU_EFLAG >> 13 & 1,
      CPU_EFLAG >> 12 & 1,
      CPU_EFLAG & O_FLAG ? "OV" : "NV",
      CPU_EFLAG & D_FLAG ? "UP" : "DN",
      CPU_EFLAG & I_FLAG ? "DI" : "EI",
      (CPU_EFLAG & T_FLAG) != 0,
      CPU_EFLAG & S_FLAG ? "NG" : "PL",
      CPU_EFLAG & Z_FLAG ? "ZR" : "NZ",
      CPU_EFLAG & A_FLAG ? "AC" : "NA",
      CPU_EFLAG & P_FLAG ? "PE" : "PO",
      CPU_EFLAG & C_FLAG ? "CY" : "NC",
    CPU_GDTR_BASE, CPU_GDTR_LIMIT, CPU_IDTR_BASE, CPU_IDTR_LIMIT,
    CPU_LDTR, CPU_LDTR_BASE, CPU_LDTR_LIMIT, CPU_TR, CPU_TR_BASE, CPU_TR_LIMIT,
    CPU_CR0, CPU_CR1, CPU_CR2, CPU_CR3, CPU_CR4, CPU_MXCSR);

  return buf;
}

#ifdef USE_FPU
char *fpu_reg2str(void)
{
  static char buf[512];
  char tmp[128];
  int i;
  int no;

  strcpy(buf, "st=\n");
  for(no = 0; no < 8; no++)
  {
    for(i = 9; i >= 0; i--)
    {
      sprintf(tmp, "%02x", FPU_ST[no][i]);
      strcat(buf, tmp);
    }
    strcat(buf, "\n");
  }

  sprintf(tmp,
    "ctrl=%04x  status=%04x  tag=%04x\n"
    "inst=%08x%04x  data=%08x%04x  op=%03x\n",
    FPU_CTRLWORD,
    FPU_STATUSWORD,
    FPU_TAGWORD,
    FPU_INSTPTR_OFFSET, FPU_INSTPTR_SEG,
    FPU_DATAPTR_OFFSET, FPU_DATAPTR_SEG,
    FPU_LASTINSTOP);
  strcat(buf, tmp);

  return buf;
}
#endif

static char *a20str(void)
{
  static char buf[32];

  sprintf(buf, "a20line=%s\n", CPU_STAT_ADRSMASK == 0xffffffff ? "enable" : "disable");
  return buf;
}

static char *mem2str(DWORD cs, DWORD ip)
{
  static char buf[128];
  char tmp[16];
  int i;

  strcpy(buf, "mem=... ");
  for(i = -10; i < 0; i++)
  {
    sprintf(tmp, "%02x ", __i286_memoryread((cs << 4) + ip + i));
    strcat(buf, tmp);
  }
  sprintf(tmp, "<%02x> ", __i286_memoryread((cs << 4) + ip));
  strcat(buf, tmp);
  for(i = 1; i <= 10; i++)
  {
    sprintf(tmp, "%02x ", __i286_memoryread((cs << 4) + ip + i));
    strcat(buf, tmp);
  }
  strcat(buf, "...\n");

  return buf;
}

void FASTCALL msgbox_str(char *msg)
{

  printf(msg);
  fflush(stdout);
  exit(1);
}

void FASTCALL msgbox_mem(DWORD no)
{
  char buf[2048];
  char tmp[16];

  strcpy(buf, cpu_reg2str());
  strcat(buf, "\n");
#ifdef USE_FPU
  strcat(buf, fpu_reg2str());
  strcat(buf, "\n");
#endif
  strcat(buf, mem2str(CPU_CS, CPU_IP));
  strcat(buf, "\n");
  sprintf(tmp, "no=%08x\n", no);
  strcat(buf, tmp);

  msgbox_str(buf);
}

void put_cpuinfo(void)
{
  char buf[2048];

  strcpy(buf, cpu_reg2str());
  strcat(buf, "\n");
#ifdef USE_FPU
  strcat(buf, fpu_reg2str());
  strcat(buf, "\n");
#endif
  strcat(buf, a20str());

  printf(buf);
}

void
memory_dump(int idx, DWORD madr)
{
	DWORD addr;
	size_t size;
	unsigned char buf[16];
	size_t s, i;
	BYTE p;

	if (madr < 0x80) {
		size = madr + 0x80;
		addr = 0;
	} else {
		size = 0x100;
		addr = madr - 0x80;
	}
	VERBOSE(("memory dump\n-- \n"));
	for (s = 0; s < size; s++) {
		if ((s % 16) == 0) {
			VERBOSE(("%08x: ", addr + s));
			memset(buf, '.', sizeof(buf));
		}

		p = cpu_vmemoryread(idx, addr + s);
		VERBOSE(("%02x ", p));
		if (p >= 0x20 && p <= 0x7e)
			buf[s % 16] = p;

		if ((s % 16) == 15) {
			VERBOSE(("| "));
			for (i = 0; i < sizeof(buf); i++)
				VERBOSE(("%c", buf[i]));
			VERBOSE(("\n"));
		}
	}
}

void
gdtr_dump(DWORD base, DWORD limit)
{
	DWORD v[2];
	DWORD i;

	VERBOSE(("GDTR_DUMP: GDTR_BASE = 0x%08x, GDTR_LIMIT = 0x%04x",base,limit));

	for (i = 0; i < limit; i += 8) {
		v[0] = cpu_lmemoryread_d(base + i);
		v[1] = cpu_lmemoryread_d(base + i + 4);
		VERBOSE(("GDTR_DUMP: %08x: %08x%08x", base + i, v[0], v[1]));
	}
}

void
ldtr_dump(DWORD base, DWORD limit)
{
	DWORD v[2];
	DWORD i;

	VERBOSE(("LDTR_DUMP: LDTR_BASE = 0x%08x, LDTR_LIMIT = 0x%04x",base,limit));

	for (i = 0; i < limit; i += 8) {
		v[0] = cpu_lmemoryread_d(base + i);
		v[1] = cpu_lmemoryread_d(base + i + 4);
		VERBOSE(("LDTR_DUMP: %08x: %08x%08x", base + i, v[0], v[1]));
	}
}

void
idtr_dump(DWORD base, DWORD limit)
{
	DWORD v[2];
	DWORD i;

	VERBOSE(("IDTR_DUMP: IDTR_BASE = 0x%08x, IDTR_LIMIT = 0x%04x",base,limit));

	for (i = 0; i < limit; i += 8) {
		v[0] = cpu_lmemoryread_d(base + i);
		v[1] = cpu_lmemoryread_d(base + i + 4);
		VERBOSE(("IDTR_DUMP: %08x: %08x%08x", base + i, v[0], v[1]));
	}
}

void
tr_dump(WORD selector, DWORD base, DWORD limit)
{
	DWORD v;
	DWORD i;

	VERBOSE(("TR_DUMP: selector = %04x", selector));

	for (i = 0; i < limit; i += 4) {
		v = cpu_lmemoryread_d(base + i);
		VERBOSE(("TR_DUMP: %08x: %08x", base + i, v));
	}
}

DWORD
pde_dump(DWORD base, int idx)
{
	DWORD paddr;
	DWORD v;
	int i;

	if (idx < 0 && idx > -8192) {
		idx = -idx;
		VERBOSE(("PDE_DUMP: address = 0x%08x, num = %d", base, idx));
		for (i = 0; i < idx; i++) {
			paddr = (base & CPU_CR3_PD_MASK) | (idx << 2);
			v = cpu_memoryread_d(paddr);
			VERBOSE(("PDE_DUMP: 0x%08x: %08x", paddr, v));
		}
		paddr = 0;
	} else if (idx < 8192) {
		VERBOSE(("PDE_DUMP: address = 0x%08x", base));
		paddr = (base & CPU_CR3_PD_MASK) | (idx << 2);
		v = cpu_memoryread_d(paddr);
		VERBOSE(("PDE_DUMP: 0x%08x: %08x", paddr, v));
	} else {
		paddr = 0;
	}

	return paddr;
}

DWORD
convert_laddr_to_paddr(DWORD laddr)
{
	DWORD paddr;	/* physical address */
	DWORD pde_addr;	/* page directory entry address */
	DWORD pde;	/* page directory entry */
	DWORD pte_addr;	/* page table entry address */
	DWORD pte;	/* page table entry */

	pde_addr = (CPU_CR3 & CPU_CR3_PD_MASK) | ((laddr >> 20) & 0xffc);
	pde = cpu_memoryread_d(pde_addr);

	if ((CPU_CR4 & CPU_CR4_PSE) && (pde & CPU_PDE_PAGE_SIZE)) {
		/* 4MB page size */
		paddr = (pde & CPU_PDE_4M_BASEADDR_MASK) | (laddr & 0x003fffff);
	} else {
		/* 4KB page size */
		pte_addr = (pde & CPU_PDE_BASEADDR_MASK) | ((laddr >> 10) & 0xffc);
		pte = cpu_memoryread_d(pte_addr);
		paddr = (pte & CPU_PTE_BASEADDR_MASK) | (laddr & 0x00000fff);
	}
	return paddr;
}

DWORD
convert_vaddr_to_paddr(unsigned int idx, DWORD offset)
{
	descriptor_t *sdp;
	DWORD laddr;

	if (idx < CPU_SEGREG_NUM) {
		sdp = &CPU_STAT_SREG(idx);
		if (sdp->valid) {
			laddr = CPU_STAT_SREGBASE(idx) + offset;
			return convert_laddr_to_paddr(laddr);
		}
	}
	return 0;
}
