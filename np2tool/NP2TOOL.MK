AS		= masm
AOPT	= /ML /DNP2
OBJ		= ..\obj\np2tool

INCS	= np2tool.inc np2tool.x86

all: $(OBJ) poweroff.com hostdrv.com

$(OBJ):
	if not exist $(OBJ) mkdir $(OBJ)


poweroff.com: $(OBJ)\poweroff.exe
	exe2bin $(OBJ)\poweroff poweroff.com

$(OBJ)\poweroff.exe: $(OBJ)\poweroff.obj
	link $(OBJ)\poweroff,$(OBJ)\poweroff;

$(OBJ)\poweroff.obj: poweroff.asm $(INCS)
	$(AS) $(AOPT) $(*B),$*,$*;


hostdrv.com: $(OBJ)\hostdrv.exe
	exe2bin $(OBJ)\hostdrv hostdrv.com

$(OBJ)\hostdrv.exe: $(OBJ)\hostdrv.obj
	link $(OBJ)\hostdrv,$(OBJ)\hostdrv;

$(OBJ)\hostdrv.obj: hostdrv.asm $(INCS) hostdrv.inc
	$(AS) $(AOPT) $(*B),$*,$*;

