/*	$Id: debug.c,v 1.3 2004/01/14 16:14:49 monaka Exp $	*/

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
#ifdef USE_FPU
#include "instructions/fpu/fpu.h"
#endif

#if 0
extern BOOL is_a20(void);	/* in ../cpuio.c */
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
    "cr0=%08x cr1=%08x cr2=%08x cr3=%08x cr4=%08x mxcsr=%08x\n",
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

#if 0
static char *a20str(void)
{
  static char buf[32];

  sprintf(buf, "a20line=%s\n", is_a20() ? "enable" : "disable");
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
#endif
