AS		= masm
AOPT	= /ML /DNP2
OBJ		= ..\obj\romrel

all: $(OBJ) ..\bios\startup.res ..\bios\biosboot.res ..\bios\sxsibios.res ..\fdd\hddboot.res

$(OBJ):
	if not exist $(OBJ) mkdir $(OBJ)


..\bios\startup.res: $(OBJ) $(OBJ)\startup.bin
	bin2txt $(OBJ)\startup.bin nosyscode > ..\bios\startup.res

$(OBJ)\startup.bin: $(OBJ)\startup.exe
	exe2bin $(OBJ)\startup $(OBJ)\startup.bin

$(OBJ)\startup.exe: $(OBJ)\startup.obj
	link $(OBJ)\startup,$(OBJ)\startup;

$(OBJ)\startup.obj: startup.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\bios\biosboot.res: $(OBJ) $(OBJ)\biosboot.bin
	bin2txt $(OBJ)\biosboot.bin biosboot > ..\bios\biosboot.res

$(OBJ)\biosboot.bin: $(OBJ)\biosboot.exe
	exe2bin $(OBJ)\biosboot $(OBJ)\biosboot.bin

$(OBJ)\biosboot.exe: $(OBJ)\biosboot.obj
	link $(OBJ)\biosboot,$(OBJ)\biosboot;

$(OBJ)\biosboot.obj: biosboot.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\bios\sxsibios.res: $(OBJ) $(OBJ)\sxsibios.bin
	bin2txt $(OBJ)\sxsibios.bin sxsibios > ..\bios\sxsibios.res

$(OBJ)\sxsibios.bin: $(OBJ)\sxsibios.exe
	exe2bin $(OBJ)\sxsibios $(OBJ)\sxsibios.bin

$(OBJ)\sxsibios.exe: $(OBJ)\sxsibios.obj
	link $(OBJ)\sxsibios,$(OBJ)\sxsibios;

$(OBJ)\sxsibios.obj: sxsibios.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\fdd\hddboot.res: $(OBJ) $(OBJ)\hddboot.bin
	bin2txt $(OBJ)\hddboot.bin hdddiskboot > ..\fdd\hddboot.res

$(OBJ)\hddboot.bin: $(OBJ)\hddboot.exe
	exe2bin $(OBJ)\hddboot $(OBJ)\hddboot.bin

$(OBJ)\hddboot.exe: $(OBJ)\hddboot.obj
	link $(OBJ)\hddboot,$(OBJ)\hddboot;

$(OBJ)\hddboot.obj: hddboot.asm
	$(AS) $(AOPT) $(*B),$*,$*;

