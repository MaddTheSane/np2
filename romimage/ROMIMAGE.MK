AS		= masm
AOPT	= /ML /DNP2
OBJ		= ..\obj\romrel

all: $(OBJ) ..\bios\startup.res ..\bios\biosboot.res ..\cbus\sasibios.res ..\cbus\scsibios.res ..\fdd\hddboot.res

$(OBJ):
	if not exist $(OBJ) mkdir $(OBJ)


..\bios\startup.res: $(OBJ)\startup.bin
	bin2txt $(OBJ)\startup.bin nosyscode > ..\bios\startup.res

$(OBJ)\startup.bin: $(OBJ)\startup.exe
	exe2bin $(OBJ)\startup $(OBJ)\startup.bin

$(OBJ)\startup.exe: $(OBJ)\startup.obj
	link $(OBJ)\startup,$(OBJ)\startup;

$(OBJ)\startup.obj: startup.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\bios\biosboot.res: $(OBJ)\biosboot.bin
	bin2txt $(OBJ)\biosboot.bin biosboot > ..\bios\biosboot.res

$(OBJ)\biosboot.bin: $(OBJ)\biosboot.exe
	exe2bin $(OBJ)\biosboot $(OBJ)\biosboot.bin

$(OBJ)\biosboot.exe: $(OBJ)\biosboot.obj
	link $(OBJ)\biosboot,$(OBJ)\biosboot;

$(OBJ)\biosboot.obj: biosboot.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\cbus\sasibios.res: $(OBJ)\sasibios.bin
	bin2txt $(OBJ)\sasibios.bin sasibios > ..\cbus\sasibios.res

$(OBJ)\sasibios.bin: $(OBJ)\sasibios.exe
	exe2bin $(OBJ)\sasibios $(OBJ)\sasibios.bin

$(OBJ)\sasibios.exe: $(OBJ)\sasibios.obj
	link $(OBJ)\sasibios,$(OBJ)\sasibios;

$(OBJ)\sasibios.obj: sasibios.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\cbus\scsibios.res: $(OBJ)\scsibios.bin
	bin2txt $(OBJ)\scsibios.bin scsibios > ..\cbus\scsibios.res

$(OBJ)\scsibios.bin: $(OBJ)\scsibios.exe
	exe2bin $(OBJ)\scsibios $(OBJ)\scsibios.bin

$(OBJ)\scsibios.exe: $(OBJ)\scsibios.obj
	link $(OBJ)\scsibios,$(OBJ)\scsibios;

$(OBJ)\scsibios.obj: scsibios.asm
	$(AS) $(AOPT) $(*B),$*,$*;



..\fdd\hddboot.res: $(OBJ)\hddboot.bin
	bin2txt $(OBJ)\hddboot.bin hdddiskboot > ..\fdd\hddboot.res

$(OBJ)\hddboot.bin: $(OBJ)\hddboot.exe
	exe2bin $(OBJ)\hddboot $(OBJ)\hddboot.bin

$(OBJ)\hddboot.exe: $(OBJ)\hddboot.obj
	link $(OBJ)\hddboot,$(OBJ)\hddboot;

$(OBJ)\hddboot.obj: hddboot.asm
	$(AS) $(AOPT) $(*B),$*,$*;

