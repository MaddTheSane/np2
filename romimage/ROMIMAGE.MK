AS		= masm
AOPT	= /ML /DNP2
OBJ		= ..\obj\romrel

all: $(OBJ) startup.res hddboot.res

$(OBJ):
	if not exist $(OBJ) mkdir $(OBJ)


startup.res: $(OBJ) $(OBJ)\startup.bin
	bin2txt $(OBJ)\startup.bin startup > startup.res

$(OBJ)\startup.bin: $(OBJ)\startup.exe
	exe2bin $(OBJ)\startup $(OBJ)\startup.bin

$(OBJ)\startup.exe: $(OBJ)\startup.obj
	link $(OBJ)\startup,$(OBJ)\startup;

$(OBJ)\startup.obj: startup.asm
	$(AS) $(AOPT) $(*B),$*,$*;


hddboot.res: $(OBJ) $(OBJ)\hddboot.bin
	bin2txt $(OBJ)\hddboot.bin hddboot > hddboot.res

$(OBJ)\hddboot.bin: $(OBJ)\hddboot.exe
	exe2bin $(OBJ)\hddboot $(OBJ)\hddboot.bin

$(OBJ)\hddboot.exe: $(OBJ)\hddboot.obj
	link $(OBJ)\hddboot,$(OBJ)\hddboot;

$(OBJ)\hddboot.obj: hddboot.asm
	$(AS) $(AOPT) $(*B),$*,$*;

